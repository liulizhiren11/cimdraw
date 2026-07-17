#include "CimdrawSceneTopologyQueryController.h"

#include "CimdrawScene.h"
#include "topology/PowerTopologyProjection.h"

namespace {

QString powerTopologyRoleName(PowerTopologyRole role)
{
    switch (role)
    {
    case PowerTopologyRole::Busbar:
        return QStringLiteral("Busbar");
    case PowerTopologyRole::Breaker:
        return QStringLiteral("Breaker");
    case PowerTopologyRole::Disconnector:
        return QStringLiteral("Disconnector");
    case PowerTopologyRole::GroundSwitch:
        return QStringLiteral("GroundSwitch");
    case PowerTopologyRole::Ground:
        return QStringLiteral("Ground");
    case PowerTopologyRole::Transformer:
        return QStringLiteral("Transformer");
    case PowerTopologyRole::Reactor:
        return QStringLiteral("Reactor");
    case PowerTopologyRole::Compensation:
        return QStringLiteral("Compensation");
    case PowerTopologyRole::SurgeArrester:
        return QStringLiteral("SurgeArrester");
    case PowerTopologyRole::Measurement:
        return QStringLiteral("Measurement");
    case PowerTopologyRole::Feeder:
        return QStringLiteral("Feeder");
    case PowerTopologyRole::Conductor:
        return QStringLiteral("Conductor");
    case PowerTopologyRole::GenericEquipment:
        return QStringLiteral("GenericEquipment");
    }
    return QStringLiteral("Unknown");
}

QVariantList pointListToVariant(const QVector<QPointF>& points)
{
    QVariantList list;
    list.reserve(points.size());
    for (const QPointF& point : points)
    {
        QVariantMap entry;
        entry.insert(QStringLiteral("x"), point.x());
        entry.insert(QStringLiteral("y"), point.y());
        list.push_back(entry);
    }
    return list;
}

QVariantList terminalRefsToVariant(const QVector<CimdrawTopologyTerminalRef>& terminals)
{
    QVariantList list;
    list.reserve(terminals.size());
    for (const CimdrawTopologyTerminalRef& terminal : terminals)
    {
        QVariantMap entry;
        entry.insert(QStringLiteral("componentId"), terminal.componentId);
        entry.insert(QStringLiteral("terminalId"), terminal.terminalId);
        list.push_back(entry);
    }
    return list;
}

QVariantMap topologyQueryError(const QString& domain,
                               const QString& op,
                               const QString& errorCode,
                               const QString& message)
{
    QVariantMap result;
    result.insert(QStringLiteral("ok"), false);
    result.insert(QStringLiteral("domain"), domain);
    result.insert(QStringLiteral("op"), op);
    result.insert(QStringLiteral("errorCode"), errorCode);
    result.insert(QStringLiteral("message"), message);
    return result;
}

QVariantMap topologyQuerySuccess(const QString& domain,
                                 const QString& op,
                                 const QVariant& data)
{
    QVariantMap result;
    result.insert(QStringLiteral("ok"), true);
    result.insert(QStringLiteral("domain"), domain);
    result.insert(QStringLiteral("op"), op);
    result.insert(QStringLiteral("data"), data);
    return result;
}

QVariantMap powerProjectionInfoToVariant(const PowerTopologyProjection& projection)
{
    QVariantMap map;
    map.insert(QStringLiteral("sourceKind"), projection.sourceKindKey());
    map.insert(QStringLiteral("sourceLabel"), projection.sourceLabel());
    map.insert(QStringLiteral("truthBoundary"), projection.truthBoundaryText());
    map.insert(QStringLiteral("deviceCount"), projection.snapshot().devices.size());
    map.insert(QStringLiteral("nodeCount"), projection.snapshot().nodes.size());
    map.insert(QStringLiteral("conductorCount"), projection.snapshot().conductors.size());
    return map;
}

QVariantMap relationNodeInfoToVariant(const TopologyNodeRelationInfo& info)
{
    QVariantMap map;
    map.insert(QStringLiteral("nodeId"), info.nodeStableId);
    map.insert(QStringLiteral("incidentEdgeIds"), info.incidentEdgeIds);
    map.insert(QStringLiteral("adjacentNodeIds"), info.adjacentNodeIds);
    map.insert(QStringLiteral("incidentEdgeCount"), info.incidentEdgeCount());
    map.insert(QStringLiteral("adjacentNodeCount"), info.adjacentNodeCount());
    return map;
}

QVariantMap relationEdgeInfoToVariant(const TopologyEdgeMeta& meta)
{
    QVariantMap map;
    map.insert(QStringLiteral("edgeId"), meta.edgeStableId);
    map.insert(QStringLiteral("relationType"), static_cast<int>(meta.relationType));
    map.insert(QStringLiteral("startNodeId"), meta.startNodeStableId);
    map.insert(QStringLiteral("endNodeId"), meta.endNodeStableId);
    map.insert(QStringLiteral("startPortKey"), meta.startPortKey);
    map.insert(QStringLiteral("endPortKey"), meta.endPortKey);
    return map;
}

TopologyEdgeLookupHint relationEdgeLookupHintFromRequest(const QVariantMap& request)
{
    TopologyEdgeLookupHint hint;
    hint.edgeStableId = request.value(QStringLiteral("edgeId")).toString().trimmed();
    hint.startNodeStableId = request.value(QStringLiteral("startNodeId")).toString().trimmed();
    hint.endNodeStableId = request.value(QStringLiteral("endNodeId")).toString().trimmed();
    hint.startPortKey = request.value(QStringLiteral("startPortKey")).toString().trimmed();
    hint.endPortKey = request.value(QStringLiteral("endPortKey")).toString().trimmed();
    const QVariant relationTypeValue = request.value(QStringLiteral("relationType"));
    hint.relationTypeSpecified = relationTypeValue.isValid();
    if (hint.relationTypeSpecified)
        hint.relationType = static_cast<TopologyRelationType>(relationTypeValue.toInt());
    return hint;
}

QVariantMap relationBindingSnapshotToVariant(const QVector<TopologyEdgeMeta>& edges)
{
    QVariantList edgeList;
    edgeList.reserve(edges.size());
    for (const TopologyEdgeMeta& edge : edges)
        edgeList.push_back(relationEdgeInfoToVariant(edge));

    QVariantMap map;
    map.insert(QStringLiteral("edgeCount"), edges.size());
    map.insert(QStringLiteral("edges"), edgeList);
    return map;
}

QVariantMap powerIslandToVariant(const PowerTopologyIsland& island)
{
    QVariantMap map;
    map.insert(QStringLiteral("nodeIds"), QVariant::fromValue(island.nodeIds));
    map.insert(QStringLiteral("deviceIds"), island.deviceIds);
    map.insert(QStringLiteral("conductorIds"), island.conductorIds);
    return map;
}

QVariantMap powerIslandAnalysisToVariant(const PowerTopologyIslandAnalysis& analysis)
{
    QVariantList islands;
    islands.reserve(analysis.islands.size());
    for (const auto& island : analysis.islands)
        islands.push_back(powerIslandToVariant(island));

    QVariantMap map;
    map.insert(QStringLiteral("islands"), islands);
    map.insert(QStringLiteral("isolatedDeviceIds"), analysis.isolatedDeviceIds);
    return map;
}

QVariantMap powerPathToVariant(const PowerTopologyPath& path)
{
    QVariantMap map;
    map.insert(QStringLiteral("sourceDeviceId"), path.sourceDeviceId);
    map.insert(QStringLiteral("targetDeviceId"), path.targetDeviceId);
    map.insert(QStringLiteral("deviceIds"), path.deviceIds);
    map.insert(QStringLiteral("conductorIds"), path.conductorIds);
    map.insert(QStringLiteral("found"), path.found);
    return map;
}

QVariantMap powerGroundPathToVariant(const PowerTopologyGroundPath& path)
{
    QVariantMap map;
    map.insert(QStringLiteral("sourceDeviceId"), path.sourceDeviceId);
    map.insert(QStringLiteral("groundDeviceId"), path.groundDeviceId);
    map.insert(QStringLiteral("deviceIds"), path.deviceIds);
    map.insert(QStringLiteral("conductorIds"), path.conductorIds);
    map.insert(QStringLiteral("found"), path.found);
    return map;
}

QVariantMap powerBranchToVariant(const PowerTopologyBranch& branch)
{
    QVariantMap map;
    map.insert(QStringLiteral("sourceDeviceId"), branch.sourceDeviceId);
    map.insert(QStringLiteral("leafDeviceId"), branch.leafDeviceId);
    map.insert(QStringLiteral("deviceIds"), branch.deviceIds);
    map.insert(QStringLiteral("conductorIds"), branch.conductorIds);
    return map;
}

QVariantMap powerBranchAnalysisToVariant(const PowerTopologyBranchAnalysis& analysis)
{
    QVariantList branches;
    branches.reserve(analysis.branches.size());
    for (const auto& branch : analysis.branches)
        branches.push_back(powerBranchToVariant(branch));

    QVariantMap map;
    map.insert(QStringLiteral("sourceDeviceId"), analysis.sourceDeviceId);
    map.insert(QStringLiteral("branches"), branches);
    return map;
}

QVariantMap powerLoopToVariant(const PowerTopologyLoop& loop)
{
    QVariantMap map;
    map.insert(QStringLiteral("deviceIds"), loop.deviceIds);
    map.insert(QStringLiteral("conductorIds"), loop.conductorIds);
    return map;
}

QVariantMap powerLoopAnalysisToVariant(const PowerTopologyLoopAnalysis& analysis)
{
    QVariantList loops;
    loops.reserve(analysis.loops.size());
    for (const auto& loop : analysis.loops)
        loops.push_back(powerLoopToVariant(loop));

    QVariantMap map;
    map.insert(QStringLiteral("loops"), loops);
    return map;
}

QVariantMap powerProtectionRangeToVariant(const PowerTopologyProtectionRangePreview& preview)
{
    QVariantMap map;
    map.insert(QStringLiteral("protectiveDeviceId"), preview.protectiveDeviceId);
    map.insert(QStringLiteral("sourceDeviceId"), preview.sourceDeviceId);
    map.insert(QStringLiteral("fromSwitchPosition"), preview.fromSwitchPosition);
    map.insert(QStringLiteral("toSwitchPosition"), preview.toSwitchPosition);
    map.insert(QStringLiteral("beforeDeviceIds"), preview.beforeDeviceIds);
    map.insert(QStringLiteral("afterDeviceIds"), preview.afterDeviceIds);
    map.insert(QStringLiteral("removedDeviceIds"), preview.removedDeviceIds);
    map.insert(QStringLiteral("addedDeviceIds"), preview.addedDeviceIds);
    map.insert(QStringLiteral("topologyChanged"), preview.topologyChanged);
    return map;
}

QVariantMap powerSwitchChangePreviewToVariant(const PowerTopologySwitchChangePreview& preview)
{
    QVariantMap map;
    map.insert(QStringLiteral("deviceId"), preview.deviceId);
    map.insert(QStringLiteral("fromSwitchPosition"), preview.fromSwitchPosition);
    map.insert(QStringLiteral("toSwitchPosition"), preview.toSwitchPosition);
    map.insert(QStringLiteral("beforeNodeIds"), QVariant::fromValue(preview.beforeNodeIds));
    map.insert(QStringLiteral("afterNodeIds"), QVariant::fromValue(preview.afterNodeIds));
    map.insert(QStringLiteral("beforeConnectedDevices"), preview.beforeConnectedDevices);
    map.insert(QStringLiteral("afterConnectedDevices"), preview.afterConnectedDevices);
    map.insert(QStringLiteral("addedDevices"), preview.addedDevices);
    map.insert(QStringLiteral("removedDevices"), preview.removedDevices);
    map.insert(QStringLiteral("topologyChanged"), preview.topologyChanged);
    return map;
}

QVariantMap powerOperationPreviewToVariant(const PowerTopologyOperationPreview& preview)
{
    QVariantMap map;
    map.insert(QStringLiteral("switchChange"), powerSwitchChangePreviewToVariant(preview.switchChange));
    map.insert(QStringLiteral("beforeIslands"), powerIslandAnalysisToVariant(preview.beforeIslands));
    map.insert(QStringLiteral("afterIslands"), powerIslandAnalysisToVariant(preview.afterIslands));
    map.insert(QStringLiteral("beforeBranches"), powerBranchAnalysisToVariant(preview.beforeBranches));
    map.insert(QStringLiteral("afterBranches"), powerBranchAnalysisToVariant(preview.afterBranches));
    map.insert(QStringLiteral("beforeLoops"), powerLoopAnalysisToVariant(preview.beforeLoops));
    map.insert(QStringLiteral("afterLoops"), powerLoopAnalysisToVariant(preview.afterLoops));
    map.insert(QStringLiteral("beforeSupplyPath"), powerPathToVariant(preview.beforeSupplyPath));
    map.insert(QStringLiteral("afterSupplyPath"), powerPathToVariant(preview.afterSupplyPath));
    map.insert(QStringLiteral("beforeGroundPath"), powerGroundPathToVariant(preview.beforeGroundPath));
    map.insert(QStringLiteral("afterGroundPath"), powerGroundPathToVariant(preview.afterGroundPath));
    map.insert(QStringLiteral("protectionRange"), powerProtectionRangeToVariant(preview.protectionRange));
    return map;
}

} // namespace

QVariantMap CimdrawSceneTopologyQueryController::buildPowerTopologyBindingSnapshot(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    const PowerTopologyProjection projection = PowerTopologyProjection::fromRuntimeScene(scene);
    const PowerTopologyAnalysisSnapshot& snapshot = projection.snapshot();

    QVariantList devices;
    devices.reserve(snapshot.devices.size());
    for (const PowerTopologyDeviceRecord& device : snapshot.devices)
    {
        QVariantMap connectivity;
        for (auto it = device.internalConnectivity.constBegin(); it != device.internalConnectivity.constEnd(); ++it)
            connectivity.insert(it.key(), it.value());

        QVariantList terminals;
        terminals.reserve(device.terminals.size());
        for (const PowerTopologyTerminalRecord& terminal : device.terminals)
        {
            QVariantMap terminalMap;
            terminalMap.insert(QStringLiteral("terminalId"), terminal.terminalId);
            terminalMap.insert(QStringLiteral("name"), terminal.name);
            terminalMap.insert(QStringLiteral("directionKey"), terminal.directionKey);
            terminalMap.insert(QStringLiteral("localX"), terminal.localPos.x());
            terminalMap.insert(QStringLiteral("localY"), terminal.localPos.y());
            terminals.push_back(terminalMap);
        }

        QVariantMap deviceMap;
        deviceMap.insert(QStringLiteral("deviceId"), device.deviceId);
        deviceMap.insert(QStringLiteral("className"), device.className);
        deviceMap.insert(QStringLiteral("displayName"), device.displayName);
        deviceMap.insert(QStringLiteral("role"), powerTopologyRoleName(device.role));
        deviceMap.insert(QStringLiteral("flowSign"), device.flowSign);
        deviceMap.insert(QStringLiteral("supportsDirectedFlow"), device.supportsDirectedFlow);
        deviceMap.insert(QStringLiteral("terminals"), terminals);
        deviceMap.insert(QStringLiteral("terminalIds"), device.terminalIds);
        deviceMap.insert(QStringLiteral("internalConnectivity"), connectivity);
        deviceMap.insert(QStringLiteral("nodeIds"), QVariant::fromValue(snapshot.deviceToNodeIds.value(device.deviceId)));
        deviceMap.insert(QStringLiteral("conductorIds"), snapshot.deviceToConductorIds.value(device.deviceId));
        devices.push_back(deviceMap);
    }

    QVariantList conductors;
    conductors.reserve(snapshot.conductors.size());
    for (const PowerTopologyConductorRecord& conductor : snapshot.conductors)
    {
        QVariantMap conductorMap;
        conductorMap.insert(QStringLiteral("wireId"), conductor.wireId);
        conductorMap.insert(QStringLiteral("fromDeviceId"), conductor.fromDeviceId);
        conductorMap.insert(QStringLiteral("fromTerminalId"), conductor.fromTerminalId);
        conductorMap.insert(QStringLiteral("toDeviceId"), conductor.toDeviceId);
        conductorMap.insert(QStringLiteral("toTerminalId"), conductor.toTerminalId);
        conductorMap.insert(QStringLiteral("routePoints"), pointListToVariant(conductor.routePoints));
        conductors.push_back(conductorMap);
    }

    QVariantList nodes;
    nodes.reserve(snapshot.nodes.size());
    for (const PowerTopologyNodeRecord& node : snapshot.nodes)
    {
        QVariantMap nodeMap;
        nodeMap.insert(QStringLiteral("nodeId"), node.nodeId);
        nodeMap.insert(QStringLiteral("terminals"), terminalRefsToVariant(node.terminals));
        nodeMap.insert(QStringLiteral("deviceIds"), node.deviceIds);
        nodes.push_back(nodeMap);
    }

    QVariantMap result;
    result.insert(QStringLiteral("domain"), QStringLiteral("PowerSystem"));
    result.insert(QStringLiteral("deviceCount"), snapshot.devices.size());
    result.insert(QStringLiteral("conductorCount"), snapshot.conductors.size());
    result.insert(QStringLiteral("nodeCount"), snapshot.nodes.size());
    result.insert(QStringLiteral("projection"), powerProjectionInfoToVariant(projection));
    result.insert(QStringLiteral("devices"), devices);
    result.insert(QStringLiteral("conductors"), conductors);
    result.insert(QStringLiteral("nodes"), nodes);
    return result;
}

QVariantMap CimdrawSceneTopologyQueryController::buildTopologyBindingSnapshot(CimdrawScene* scene) const
{
    QVariantMap result;
    result.insert(QStringLiteral("power"), buildPowerTopologyBindingSnapshot(scene));
    return result;
}

QVariantMap CimdrawSceneTopologyQueryController::queryTopology(CimdrawScene* scene, const QVariantMap& request) const
{
    if (!scene)
        return topologyQueryError(QString(), QString(), QStringLiteral("missing_scene"), QStringLiteral("scene is required"));

    const QString domain = request.value(QStringLiteral("domain")).toString().trimmed();
    const QString op = request.value(QStringLiteral("op")).toString().trimmed();
    if (domain.isEmpty())
        return topologyQueryError(domain, op, QStringLiteral("missing_domain"), QStringLiteral("domain is required"));
    if (op.isEmpty())
        return topologyQueryError(domain, op, QStringLiteral("missing_op"), QStringLiteral("op is required"));

    if (domain == QStringLiteral("all"))
    {
        if (op == QStringLiteral("bindingSnapshot"))
            return topologyQuerySuccess(domain, op, buildTopologyBindingSnapshot(scene));
        return topologyQueryError(domain, op, QStringLiteral("unsupported_op"), QStringLiteral("unsupported topology query op"));
    }

    if (domain == QStringLiteral("power"))
    {
        const PowerTopologyProjection projection = PowerTopologyProjection::fromRuntimeScene(scene);
        if (op == QStringLiteral("bindingSnapshot"))
            return topologyQuerySuccess(domain, op, buildPowerTopologyBindingSnapshot(scene));
        if (op == QStringLiteral("projectionInfo"))
            return topologyQuerySuccess(domain, op, powerProjectionInfoToVariant(projection));
        if (op == QStringLiteral("reachableDevices"))
            return topologyQuerySuccess(domain, op, projection.reachableDevices(request.value(QStringLiteral("deviceId")).toString()));
        if (op == QStringLiteral("directedReachableDevices"))
            return topologyQuerySuccess(domain, op, projection.directedReachableDevices(request.value(QStringLiteral("sourceDeviceId")).toString()));
        if (op == QStringLiteral("deviceNodeIds"))
            return topologyQuerySuccess(domain, op, QVariant::fromValue(projection.deviceNodeIds(request.value(QStringLiteral("deviceId")).toString())));
        if (op == QStringLiteral("deviceConductorIds"))
            return topologyQuerySuccess(domain, op, projection.deviceConductorIds(request.value(QStringLiteral("deviceId")).toString()));
        if (op == QStringLiteral("connectedDevices"))
            return topologyQuerySuccess(domain, op, projection.connectedComponentDeviceIds(request.value(QStringLiteral("deviceId")).toString()));
        if (op == QStringLiteral("busbarAttachedDevices"))
            return topologyQuerySuccess(domain, op, projection.busbarAttachedDevices(request.value(QStringLiteral("deviceId")).toString()));
        if (op == QStringLiteral("islandAnalysis"))
            return topologyQuerySuccess(domain, op, powerIslandAnalysisToVariant(projection.islandAnalysis()));
        if (op == QStringLiteral("branchAnalysis"))
            return topologyQuerySuccess(domain, op, powerBranchAnalysisToVariant(
                projection.branchAnalysis(request.value(QStringLiteral("sourceDeviceId")).toString())));
        if (op == QStringLiteral("loopAnalysis"))
            return topologyQuerySuccess(domain, op, powerLoopAnalysisToVariant(projection.loopAnalysis()));
        if (op == QStringLiteral("shortestSupplyPath"))
            return topologyQuerySuccess(domain, op, powerPathToVariant(projection.shortestSupplyPath(
                request.value(QStringLiteral("sourceDeviceId")).toString(),
                request.value(QStringLiteral("targetDeviceId")).toString())));
        if (op == QStringLiteral("shortestDirectedSupplyPath"))
            return topologyQuerySuccess(domain, op, powerPathToVariant(projection.shortestDirectedSupplyPath(
                request.value(QStringLiteral("sourceDeviceId")).toString(),
                request.value(QStringLiteral("targetDeviceId")).toString())));
        if (op == QStringLiteral("shortestGroundPath"))
            return topologyQuerySuccess(domain, op, powerGroundPathToVariant(projection.shortestGroundPath(
                request.value(QStringLiteral("sourceDeviceId")).toString())));
        if (op == QStringLiteral("previewSwitchTopologyChange"))
            return topologyQuerySuccess(domain, op, powerSwitchChangePreviewToVariant(scene->previewPowerSwitchTopologyChange(
                request.value(QStringLiteral("deviceId")).toString(),
                request.value(QStringLiteral("toSwitchPosition")).toInt())));
        if (op == QStringLiteral("previewProtectionRange"))
            return topologyQuerySuccess(domain, op, powerProtectionRangeToVariant(scene->previewPowerProtectionRange(
                request.value(QStringLiteral("deviceId")).toString(),
                request.value(QStringLiteral("toSwitchPosition")).toInt(),
                request.value(QStringLiteral("sourceDeviceId")).toString())));
        if (op == QStringLiteral("previewSwitchOperation"))
            return topologyQuerySuccess(domain, op, powerOperationPreviewToVariant(scene->previewPowerSwitchOperation(
                request.value(QStringLiteral("deviceId")).toString(),
                request.value(QStringLiteral("toSwitchPosition")).toInt(),
                request.value(QStringLiteral("supplySourceDeviceId")).toString(),
                request.value(QStringLiteral("supplyTargetDeviceId")).toString(),
                request.value(QStringLiteral("groundSourceDeviceId")).toString())));
        return topologyQueryError(domain, op, QStringLiteral("unsupported_op"), QStringLiteral("unsupported topology query op"));
    }

    if (domain == QStringLiteral("relation"))
    {
        if (op == QStringLiteral("nodeInfo"))
        {
            const QString nodeId = request.value(QStringLiteral("nodeId")).toString().trimmed();
            if (nodeId.isEmpty())
                return topologyQueryError(domain, op, QStringLiteral("missing_node_id"), QStringLiteral("nodeId is required"));

            const TopologyNodeRelationInfo info = scene->relationNodeInfoValue(nodeId);
            return topologyQuerySuccess(domain, op, relationNodeInfoToVariant(info));
        }
        if (op == QStringLiteral("edgeInfo"))
        {
            const QString edgeId = request.value(QStringLiteral("edgeId")).toString().trimmed();
            if (edgeId.isEmpty())
                return topologyQueryError(domain, op, QStringLiteral("missing_edge_id"), QStringLiteral("edgeId is required"));

            const TopologyEdgeMeta meta = scene->relationEdgeMeta(edgeId);
            return topologyQuerySuccess(domain, op, relationEdgeInfoToVariant(meta));
        }
        if (op == QStringLiteral("bindingSnapshot"))
        {
            const QVector<TopologyEdgeMeta> edges = scene->relationEdgeMetas();
            return topologyQuerySuccess(domain, op, relationBindingSnapshotToVariant(edges));
        }
        if (op == QStringLiteral("resolveEdgeInfo"))
        {
            const TopologyEdgeMeta meta = scene->resolveRelationEdgeMeta(relationEdgeLookupHintFromRequest(request));
            if (!meta.edgeStableId.isEmpty())
                return topologyQuerySuccess(domain, op, relationEdgeInfoToVariant(meta));
            return topologyQueryError(domain, op, QStringLiteral("edge_not_found"), QStringLiteral("relation edge not found"));
        }
        return topologyQueryError(domain, op, QStringLiteral("unsupported_op"), QStringLiteral("unsupported topology query op"));
    }

    return topologyQueryError(domain, op, QStringLiteral("unsupported_domain"), QStringLiteral("unsupported topology query domain"));
}
