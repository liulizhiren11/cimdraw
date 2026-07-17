#!/usr/bin/env python3
"""
Generate local CIM class metadata from reusable open-source upstream sources.

This tool currently supports two source adapters:
1. CIMpy generated Python classes, which expose both inheritance and profile
   metadata in class bodies.
2. GridCal generated CGMES device classes, which expose inheritance in Python
   class declarations and can complement missing lineage from CIMpy.

The generated outputs are:
1. a JSON snapshot for inspection and provenance
2. a C++ include fragment consumed by CimClassLineage.h
"""

from __future__ import annotations

import argparse
import ast
import base64
import json
import os
import re
import sys
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable


CLASS_PATTERN = re.compile(r"^class\s+([A-Za-z_][A-Za-z0-9_]*)\(([^)]+)\):", re.MULTILINE)
EXCLUDED_PARENTS = {"Base", "object"}
SUPPORTED_SOURCES = ("auto", "cimpy", "gridcal")


@dataclass(frozen=True)
class UpstreamSource:
    name: str
    owner: str
    repo: str
    ref: str
    cgmes_version: str
    package_path: str
    local_cache_dir: str

    @property
    def api_url(self) -> str:
        return (
            f"https://api.github.com/repos/{self.owner}/{self.repo}/contents/"
            f"{self.package_path}?ref={self.ref}"
        )

    @property
    def github_url(self) -> str:
        return f"https://github.com/{self.owner}/{self.repo}"


@dataclass(frozen=True)
class ClassProfileMetadata:
    possible_profiles_by_member: dict[str, list[str]] = field(default_factory=dict)
    recommended_class_profile: str | None = None


@dataclass(frozen=True)
class LocalExtractionResult:
    parent_map: dict[str, str]
    class_profiles: dict[str, ClassProfileMetadata] = field(default_factory=dict)
    skipped_files: tuple[str, ...] = ()


@dataclass(frozen=True)
class MetadataSnapshot:
    source_name: str
    source_mode: str
    source: UpstreamSource
    parent_map: dict[str, str]
    class_profiles: dict[str, ClassProfileMetadata] = field(default_factory=dict)
    local_cache_dir: str | None = None
    skipped_files: tuple[str, ...] = ()
    merged_sources: tuple[UpstreamSource, ...] = ()


def merge_parent_maps(primary_map: dict[str, str], secondary_map: dict[str, str]) -> dict[str, str]:
    merged = dict(primary_map)
    for class_name, parent_name in secondary_map.items():
        merged.setdefault(class_name, parent_name)
    return dict(sorted(merged.items()))


def snapshot_source_summary(snapshot: MetadataSnapshot) -> dict[str, object]:
    if snapshot.merged_sources:
        sources = [
            {
                "name": source.name,
                "owner": source.owner,
                "repo": source.repo,
                "ref": source.ref,
                "package_path": source.package_path,
                "github_url": source.github_url,
                "api_url": source.api_url,
                "local_cache_dir": source.local_cache_dir.replace("\\", "/"),
            }
            for source in snapshot.merged_sources
        ]
        return {
            "name": snapshot.source_name,
            "cgmes_version": snapshot.source.cgmes_version,
            "source_mode": snapshot.source_mode,
            "merged_sources": sources,
        }

    payload = {
        "name": snapshot.source.name,
        "owner": snapshot.source.owner,
        "repo": snapshot.source.repo,
        "ref": snapshot.source.ref,
        "cgmes_version": snapshot.source.cgmes_version,
        "package_path": snapshot.source.package_path,
        "api_url": snapshot.source.api_url,
        "github_url": snapshot.source.github_url,
        "source_mode": snapshot.source_mode,
    }
    if snapshot.local_cache_dir:
        payload["local_cache_dir"] = snapshot.local_cache_dir.replace("\\", "/")
    return payload


def fetch_text(url: str) -> str:
    request = urllib.request.Request(
        url,
        headers={
            "Accept": "application/vnd.github+json, application/json, text/plain, */*",
            "User-Agent": "cimdraw-cim-metadata-sync",
        },
    )
    try:
        with urllib.request.urlopen(request, timeout=60) as response:
            return response.read().decode("utf-8")
    except Exception as exc:
        raise RuntimeError(f"failed to fetch {url}: {exc}") from exc


def load_directory_listing(source: UpstreamSource) -> list[dict]:
    payload = fetch_text(source.api_url)
    data = json.loads(payload)
    if not isinstance(data, list):
        raise RuntimeError(f"unexpected GitHub directory payload: {type(data)!r}")
    return data


def extract_parent_relations(file_contents: str) -> dict[str, str]:
    relations: dict[str, str] = {}
    for class_name, parent_list in CLASS_PATTERN.findall(file_contents):
        parent_name = parent_list.split(",", 1)[0].strip()
        if "." in parent_name:
            parent_name = parent_name.rsplit(".", 1)[-1]
        if parent_name in EXCLUDED_PARENTS:
            continue
        relations[class_name] = parent_name
    return relations


def parse_source_args(args: argparse.Namespace) -> dict[str, UpstreamSource]:
    return {
        "cimpy": UpstreamSource(
            name="cimpy",
            owner=args.cimpy_owner,
            repo=args.cimpy_repo,
            ref=args.cimpy_ref,
            cgmes_version=args.cgmes_version,
            package_path=f"cimpy/{args.cgmes_version}",
            local_cache_dir=args.local_cimpy_package_dir,
        ),
        "gridcal": UpstreamSource(
            name="gridcal",
            owner=args.gridcal_owner,
            repo=args.gridcal_repo,
            ref=args.gridcal_ref,
            cgmes_version=args.cgmes_version,
            package_path=f"src/VeraGridEngine/IO/cim/cgmes/{args.cgmes_version}/devices",
            local_cache_dir=args.local_gridcal_devices_dir,
        ),
    }


def parse_profile_value(node: ast.AST) -> str | None:
    if isinstance(node, ast.Attribute) and node.attr == "value":
        owner = node.value
        if isinstance(owner, ast.Attribute):
            return owner.attr
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return node.value
    return None


def parse_possible_profiles(node: ast.AST) -> dict[str, list[str]]:
    if not isinstance(node, ast.Dict):
        return {}

    profiles: dict[str, list[str]] = {}
    for key_node, value_node in zip(node.keys, node.values):
        if not isinstance(key_node, ast.Constant) or not isinstance(key_node.value, str):
            continue
        key = key_node.value
        if not isinstance(value_node, (ast.List, ast.Tuple)):
            continue
        values: list[str] = []
        for item in value_node.elts:
            profile_name = parse_profile_value(item)
            if profile_name:
                values.append(profile_name)
        profiles[key] = values
    return profiles


def extract_cimpy_class_metadata(file_contents: str) -> tuple[dict[str, str], dict[str, ClassProfileMetadata]]:
    tree = ast.parse(file_contents)
    parent_map: dict[str, str] = {}
    class_profiles: dict[str, ClassProfileMetadata] = {}

    for node in tree.body:
        if not isinstance(node, ast.ClassDef):
            continue
        if not node.bases:
            continue

        parent_name: str | None = None
        base = node.bases[0]
        if isinstance(base, ast.Name):
            parent_name = base.id
        elif isinstance(base, ast.Attribute):
            parent_name = base.attr

        if parent_name and parent_name not in EXCLUDED_PARENTS:
            parent_map[node.name] = parent_name

        possible_profiles_by_member: dict[str, list[str]] = {}
        recommended_class_profile: str | None = None
        for statement in node.body:
            if not isinstance(statement, ast.Assign):
                continue
            for target in statement.targets:
                if not isinstance(target, ast.Name):
                    continue
                if target.id == "possibleProfileList":
                    possible_profiles_by_member = parse_possible_profiles(statement.value)
                elif target.id == "recommendedClassProfile":
                    recommended_class_profile = parse_profile_value(statement.value)

        if possible_profiles_by_member or recommended_class_profile:
            class_profiles[node.name] = ClassProfileMetadata(
                possible_profiles_by_member=possible_profiles_by_member,
                recommended_class_profile=recommended_class_profile,
            )

    return dict(sorted(parent_map.items())), class_profiles


def build_cimpy_metadata_from_local_dir(package_dir: str) -> LocalExtractionResult:
    if not os.path.isdir(package_dir):
        return LocalExtractionResult(parent_map={})

    parent_map: dict[str, str] = {}
    class_profiles: dict[str, ClassProfileMetadata] = {}
    skipped_files: list[str] = []
    for entry in sorted(os.listdir(package_dir)):
        if not entry.endswith(".py"):
            continue
        if entry == "CGMESProfile.py" or entry.startswith("__"):
            continue
        path = os.path.join(package_dir, entry)
        with open(path, "r", encoding="utf-8") as handle:
            file_contents = handle.read()
        if file_contents.lstrip().startswith("404: Not Found"):
            skipped_files.append(entry)
            continue
        file_parent_map, file_profiles = extract_cimpy_class_metadata(file_contents)
        parent_map.update(file_parent_map)
        class_profiles.update(file_profiles)

    return LocalExtractionResult(
        parent_map=dict(sorted(parent_map.items())),
        class_profiles=dict(sorted(class_profiles.items())),
        skipped_files=tuple(sorted(skipped_files)),
    )


def build_gridcal_metadata_from_local_dir(devices_dir: str) -> LocalExtractionResult:
    if not os.path.isdir(devices_dir):
        return LocalExtractionResult(parent_map={})

    parent_map: dict[str, str] = {}
    skipped_files: list[str] = []
    for entry in sorted(os.listdir(devices_dir)):
        if not entry.endswith(".py"):
            continue
        path = os.path.join(devices_dir, entry)
        with open(path, "r", encoding="utf-8") as handle:
            file_contents = handle.read()
        if file_contents.lstrip().startswith("404: Not Found"):
            skipped_files.append(entry)
            continue
        parent_map.update(extract_parent_relations(file_contents))
    return LocalExtractionResult(
        parent_map=dict(sorted(parent_map.items())),
        skipped_files=tuple(sorted(skipped_files)),
    )


def build_gridcal_metadata_from_github(source: UpstreamSource) -> LocalExtractionResult:
    listing = load_directory_listing(source)
    parent_map: dict[str, str] = {}
    for item in listing:
        if item.get("type") != "file":
            continue
        if not str(item.get("name", "")).endswith(".py"):
            continue
        api_url = item.get("url")
        if not api_url:
            continue
        payload = fetch_text(api_url)
        file_data = json.loads(payload)
        encoded_content = file_data.get("content", "")
        if not encoded_content:
            continue
        file_contents = base64.b64decode(encoded_content).decode("utf-8")
        parent_map.update(extract_parent_relations(file_contents))
    return LocalExtractionResult(parent_map=dict(sorted(parent_map.items())))


def build_cimpy_metadata_from_github(source: UpstreamSource) -> LocalExtractionResult:
    listing = load_directory_listing(source)
    parent_map: dict[str, str] = {}
    class_profiles: dict[str, ClassProfileMetadata] = {}
    for item in listing:
        if item.get("type") != "file":
            continue
        name = str(item.get("name", ""))
        if not name.endswith(".py"):
            continue
        if name == "CGMESProfile.py" or name.startswith("__"):
            continue
        api_url = item.get("url")
        if not api_url:
            continue
        payload = fetch_text(api_url)
        file_data = json.loads(payload)
        encoded_content = file_data.get("content", "")
        if not encoded_content:
            continue
        file_contents = base64.b64decode(encoded_content).decode("utf-8")
        file_parent_map, file_profiles = extract_cimpy_class_metadata(file_contents)
        parent_map.update(file_parent_map)
        class_profiles.update(file_profiles)
    return LocalExtractionResult(
        parent_map=dict(sorted(parent_map.items())),
        class_profiles=dict(sorted(class_profiles.items())),
    )


def build_snapshot(
    source_name: str,
    cgmes_version: str,
    local_cimpy_package_dir: str,
    local_gridcal_devices_dir: str,
    allow_remote: bool = True,
    cimpy_owner: str = "sogno-platform",
    cimpy_repo: str = "cimpy",
    cimpy_ref: str = "master",
    gridcal_owner: str = "SanPen",
    gridcal_repo: str = "GridCal",
    gridcal_ref: str = "master",
) -> MetadataSnapshot:
    args = argparse.Namespace(
        source=source_name,
        cgmes_version=cgmes_version,
        local_cimpy_package_dir=local_cimpy_package_dir,
        local_gridcal_devices_dir=local_gridcal_devices_dir,
        cimpy_owner=cimpy_owner,
        cimpy_repo=cimpy_repo,
        cimpy_ref=cimpy_ref,
        gridcal_owner=gridcal_owner,
        gridcal_repo=gridcal_repo,
        gridcal_ref=gridcal_ref,
    )
    sources = parse_source_args(args)

    if source_name not in SUPPORTED_SOURCES:
        raise ValueError(f"unsupported source {source_name!r}")

    if source_name == "auto":
        cimpy_source = sources["cimpy"]
        gridcal_source = sources["gridcal"]
        cimpy_result = build_cimpy_metadata_from_local_dir(cimpy_source.local_cache_dir)
        if not cimpy_result.parent_map and allow_remote:
            cimpy_result = build_cimpy_metadata_from_github(cimpy_source)
        gridcal_result = build_gridcal_metadata_from_local_dir(gridcal_source.local_cache_dir)
        if not gridcal_result.parent_map and allow_remote:
            gridcal_result = build_gridcal_metadata_from_github(gridcal_source)

        merged_parent_map = merge_parent_maps(cimpy_result.parent_map, gridcal_result.parent_map)
        if merged_parent_map:
            source_mode = "local-cache"
            if not cimpy_result.parent_map or (gridcal_result.parent_map == {} and not gridcal_result.skipped_files):
                source_mode = "github-api" if allow_remote and not cimpy_result.parent_map else source_mode
            if cimpy_result.parent_map and gridcal_result.parent_map:
                source_mode = "local-cache"
            elif allow_remote and (not cimpy_result.parent_map or not gridcal_result.parent_map):
                source_mode = "mixed"

            return MetadataSnapshot(
                source_name="merged",
                source_mode=source_mode,
                source=UpstreamSource(
                    name="merged",
                    owner="multiple",
                    repo="multiple",
                    ref="multiple",
                    cgmes_version=cgmes_version,
                    package_path="merged",
                    local_cache_dir=f"{cimpy_source.local_cache_dir};{gridcal_source.local_cache_dir}",
                ),
                parent_map=merged_parent_map,
                class_profiles=cimpy_result.class_profiles,
                local_cache_dir=f"{cimpy_source.local_cache_dir};{gridcal_source.local_cache_dir}",
                skipped_files=tuple(sorted(set(cimpy_result.skipped_files + gridcal_result.skipped_files))),
                merged_sources=(cimpy_source, gridcal_source),
            )

        raise RuntimeError("no parent relations extracted from upstream sources")

    for candidate_name in [source_name]:
        source = sources[candidate_name]
        if candidate_name == "cimpy":
            local_result = build_cimpy_metadata_from_local_dir(source.local_cache_dir)
            if local_result.parent_map:
                return MetadataSnapshot(
                    source_name=candidate_name,
                    source_mode="local-cache",
                    source=source,
                    parent_map=local_result.parent_map,
                    class_profiles=local_result.class_profiles,
                    local_cache_dir=source.local_cache_dir,
                    skipped_files=local_result.skipped_files,
                )
            if not allow_remote:
                continue
            remote_result = build_cimpy_metadata_from_github(source)
        else:
            local_result = build_gridcal_metadata_from_local_dir(source.local_cache_dir)
            if local_result.parent_map:
                return MetadataSnapshot(
                    source_name=candidate_name,
                    source_mode="local-cache",
                    source=source,
                    parent_map=local_result.parent_map,
                    class_profiles=local_result.class_profiles,
                    local_cache_dir=source.local_cache_dir,
                    skipped_files=local_result.skipped_files,
                )
            if not allow_remote:
                continue
            remote_result = build_gridcal_metadata_from_github(source)

        if remote_result.parent_map:
            return MetadataSnapshot(
                source_name=candidate_name,
                source_mode="github-api",
                source=source,
                parent_map=remote_result.parent_map,
                class_profiles=remote_result.class_profiles,
            )

    raise RuntimeError("no parent relations extracted from upstream sources")


def write_json_snapshot(output_path: str, snapshot: MetadataSnapshot) -> None:
    profile_summary = {
        class_name: {
            "recommended_class_profile": metadata.recommended_class_profile,
            "class_profiles": metadata.possible_profiles_by_member.get("class", []),
        }
        for class_name, metadata in snapshot.class_profiles.items()
        if class_name in snapshot.parent_map
    }
    payload = {
        "source": snapshot_source_summary(snapshot),
        "class_count": len(snapshot.parent_map),
        "parents": snapshot.parent_map,
    }
    if snapshot.skipped_files:
        payload["source"]["skipped_local_files"] = list(snapshot.skipped_files)
    if profile_summary:
        payload["profile_class_count"] = len(profile_summary)
        payload["profiles"] = profile_summary

    with open(output_path, "w", encoding="utf-8", newline="\n") as handle:
        json.dump(payload, handle, ensure_ascii=True, indent=2, sort_keys=False)
        handle.write("\n")


def iter_inc_lines(snapshot: MetadataSnapshot) -> Iterable[str]:
    profile_class_count = sum(1 for class_name in snapshot.class_profiles if class_name in snapshot.parent_map)
    yield "// Generated by tools/sync_cim_metadata.py"
    if snapshot.merged_sources:
        yield "// Source mode: merged"
        for source in snapshot.merged_sources:
            yield f"//  - {source.github_url} @ {source.ref} ({source.name})"
            yield f"//    Package path: {source.package_path}"
    else:
        yield (
            f"// Source: {snapshot.source.github_url} @ {snapshot.source.ref} "
            f"({snapshot.source.name})"
        )
        yield f"// Package path: {snapshot.source.package_path}"
    yield f"// Extracted direct parent relations: {len(snapshot.parent_map)}"
    if profile_class_count:
        yield f"// Extracted class profile metadata: {profile_class_count}"
    for class_name, parent_name in snapshot.parent_map.items():
        yield (
            f'parents.insert(QStringLiteral("{class_name}"), '
            f'QStringLiteral("{parent_name}"));'
        )


def write_inc_fragment(output_path: str, snapshot: MetadataSnapshot) -> None:
    with open(output_path, "w", encoding="utf-8", newline="\n") as handle:
        handle.write("\n".join(iter_inc_lines(snapshot)))
        handle.write("\n")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", choices=SUPPORTED_SOURCES, default="auto")
    parser.add_argument("--cgmes-version", default="cgmes_v2_4_15")
    parser.add_argument(
        "--output-json",
        default="src/cim/model/cim_class_metadata_cgmes_v2_4_15.json",
    )
    parser.add_argument(
        "--output-inc",
        default="src/cim/model/CimClassLineageGenerated.inc",
    )
    parser.add_argument("--cimpy-owner", default="sogno-platform")
    parser.add_argument("--cimpy-repo", default="cimpy")
    parser.add_argument("--cimpy-ref", default="master")
    parser.add_argument("--gridcal-owner", default="SanPen")
    parser.add_argument("--gridcal-repo", default="GridCal")
    parser.add_argument("--gridcal-ref", default="master")
    parser.add_argument(
        "--local-cimpy-package-dir",
        default=str(Path.home() / "AppData" / "Local" / "Temp" / "cim-upstream-scout" / "cimpy" / "cimpy" / "cgmes_v2_4_15"),
        help="Prefer reading an already cloned local CIMpy package directory.",
    )
    parser.add_argument(
        "--local-gridcal-devices-dir",
        default=str(Path.home() / "AppData" / "Local" / "Temp" / "cim-upstream-scout" / "GridCal" / "src" / "VeraGridEngine" / "IO" / "cim" / "cgmes" / "cgmes_v2_4_15" / "devices"),
        help="Prefer reading an already cloned local GridCal device directory.",
    )
    parser.add_argument(
        "--no-remote",
        action="store_true",
        help="Disable GitHub API fallback and only use local cached upstream files.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    snapshot = build_snapshot(
        source_name=args.source,
        cgmes_version=args.cgmes_version,
        local_cimpy_package_dir=args.local_cimpy_package_dir,
        local_gridcal_devices_dir=args.local_gridcal_devices_dir,
        allow_remote=not args.no_remote,
        cimpy_owner=args.cimpy_owner,
        cimpy_repo=args.cimpy_repo,
        cimpy_ref=args.cimpy_ref,
        gridcal_owner=args.gridcal_owner,
        gridcal_repo=args.gridcal_repo,
        gridcal_ref=args.gridcal_ref,
    )
    write_json_snapshot(args.output_json, snapshot)
    write_inc_fragment(args.output_inc, snapshot)
    print(
        f"Generated {len(snapshot.parent_map)} parent relations from "
        f"{snapshot.source.owner}/{snapshot.source.repo}:{snapshot.source.cgmes_version} "
        f"via {snapshot.source_name} ({snapshot.source_mode})"
    )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # pragma: no cover - command-line diagnostics
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)
