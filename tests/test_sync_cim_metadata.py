import importlib.util
import sys
import tempfile
import textwrap
import unittest
from pathlib import Path


def load_sync_module():
    repo_root = Path(__file__).resolve().parents[1]
    module_path = repo_root / "tools" / "sync_cim_metadata.py"
    spec = importlib.util.spec_from_file_location("sync_cim_metadata", module_path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def write_file(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(textwrap.dedent(content).lstrip(), encoding="utf-8", newline="\n")


class SyncCimMetadataTests(unittest.TestCase):
    def test_auto_source_merges_cimpy_and_gridcal_for_complete_lineage(self):
        module = load_sync_module()
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            cimpy_dir = root / "cimpy" / "cgmes_v2_4_15"
            gridcal_dir = root / "GridCal" / "devices"

            write_file(
                cimpy_dir / "Terminal.py",
                """
                from .ACDCTerminal import ACDCTerminal

                class Terminal(ACDCTerminal):
                    pass
                """,
            )
            write_file(
                cimpy_dir / "ACDCTerminal.py",
                """
                from .IdentifiedObject import IdentifiedObject

                class ACDCTerminal(IdentifiedObject):
                    pass
                """,
            )
            write_file(
                cimpy_dir / "IdentifiedObject.py",
                """
                from .Base import Base

                class IdentifiedObject(Base):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "CurrentTransformer.py",
                """
                class CurrentTransformer(Sensor):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "Sensor.py",
                """
                class Sensor(AuxiliaryEquipment):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "AuxiliaryEquipment.py",
                """
                class AuxiliaryEquipment(Equipment):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "Equipment.py",
                """
                class Equipment(PowerSystemResource):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "PowerSystemResource.py",
                """
                class PowerSystemResource(IdentifiedObject):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "IdentifiedObject.py",
                """
                class IdentifiedObject(Base):
                    pass
                """,
            )

            snapshot = module.build_snapshot(
                source_name="auto",
                cgmes_version="cgmes_v2_4_15",
                local_cimpy_package_dir=str(cimpy_dir),
                local_gridcal_devices_dir=str(gridcal_dir),
                allow_remote=False,
            )

            self.assertEqual(snapshot.source_name, "merged")
            self.assertEqual(snapshot.source_mode, "local-cache")
            self.assertEqual(snapshot.parent_map["Terminal"], "ACDCTerminal")
            self.assertEqual(snapshot.parent_map["ACDCTerminal"], "IdentifiedObject")
            self.assertEqual(snapshot.parent_map["CurrentTransformer"], "Sensor")
            self.assertEqual(snapshot.parent_map["Sensor"], "AuxiliaryEquipment")
            self.assertEqual(snapshot.parent_map["AuxiliaryEquipment"], "Equipment")
            self.assertEqual(snapshot.parent_map["Equipment"], "PowerSystemResource")
            self.assertEqual(snapshot.parent_map["PowerSystemResource"], "IdentifiedObject")

    def test_auto_source_prefers_cimpy_and_extracts_profiles(self):
        module = load_sync_module()
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            cimpy_dir = root / "cimpy" / "cgmes_v2_4_15"

            write_file(
                cimpy_dir / "Terminal.py",
                """
                from .ACDCTerminal import ACDCTerminal
                from .CGMESProfile import Profile

                class Terminal(ACDCTerminal):
                    possibleProfileList = {
                        "class": [Profile.DY.value, Profile.EQ.value, Profile.SSH.value],
                        "TopologicalNode": [Profile.TP.value],
                    }
                    recommendedClassProfile = Profile.EQ.value
                """,
            )
            write_file(
                cimpy_dir / "ACDCTerminal.py",
                """
                from .IdentifiedObject import IdentifiedObject
                from .CGMESProfile import Profile

                class ACDCTerminal(IdentifiedObject):
                    possibleProfileList = {
                        "class": [Profile.EQ.value, Profile.SSH.value],
                    }
                    recommendedClassProfile = Profile.EQ.value
                """,
            )
            write_file(
                cimpy_dir / "IdentifiedObject.py",
                """
                from .Base import Base

                class IdentifiedObject(Base):
                    pass
                """,
            )
            snapshot = module.build_snapshot(
                source_name="auto",
                cgmes_version="cgmes_v2_4_15",
                local_cimpy_package_dir=str(cimpy_dir),
                local_gridcal_devices_dir=str(root / "missing-gridcal"),
                allow_remote=False,
            )

            self.assertEqual(snapshot.source_name, "merged")
            self.assertEqual(snapshot.source_mode, "local-cache")
            self.assertEqual(snapshot.parent_map["Terminal"], "ACDCTerminal")
            self.assertEqual(snapshot.parent_map["ACDCTerminal"], "IdentifiedObject")
            self.assertEqual(
                snapshot.class_profiles["Terminal"].recommended_class_profile,
                "EQ",
            )
            self.assertEqual(
                snapshot.class_profiles["Terminal"].possible_profiles_by_member["class"],
                ["DY", "EQ", "SSH"],
            )
            self.assertEqual(
                snapshot.class_profiles["Terminal"].possible_profiles_by_member["TopologicalNode"],
                ["TP"],
            )

    def test_auto_source_does_not_fall_back_to_removed_veragrid_source(self):
        module = load_sync_module()
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)

            with self.assertRaisesRegex(RuntimeError, "no parent relations extracted"):
                module.build_snapshot(
                    source_name="auto",
                    cgmes_version="cgmes_v2_4_15",
                    local_cimpy_package_dir=str(root / "missing-cimpy"),
                    local_gridcal_devices_dir=str(root / "missing-gridcal"),
                    allow_remote=False,
                )

    def test_gridcal_source_extracts_parent_relations_directly(self):
        module = load_sync_module()
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            gridcal_dir = root / "GridCal" / "devices"

            write_file(
                gridcal_dir / "Terminal.py",
                """
                class Terminal(ACDCTerminal):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "ACDCTerminal.py",
                """
                class ACDCTerminal(IdentifiedObject):
                    pass
                """,
            )
            write_file(
                gridcal_dir / "IdentifiedObject.py",
                """
                class IdentifiedObject(Base):
                    pass
                """,
            )
            snapshot = module.build_snapshot(
                source_name="gridcal",
                cgmes_version="cgmes_v2_4_15",
                local_cimpy_package_dir=str(root / "missing-cimpy"),
                local_gridcal_devices_dir=str(gridcal_dir),
                allow_remote=False,
            )

            self.assertEqual(snapshot.source_name, "gridcal")
            self.assertEqual(snapshot.source_mode, "local-cache")
            self.assertEqual(snapshot.parent_map["Terminal"], "ACDCTerminal")
            self.assertEqual(snapshot.parent_map["ACDCTerminal"], "IdentifiedObject")


if __name__ == "__main__":
    unittest.main()
