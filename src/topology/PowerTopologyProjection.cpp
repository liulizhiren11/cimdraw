#include "PowerTopologyProjection.h"

#include "CimdrawScene.h"
#include "cim/model/CimModel.h"
#include "cim/topology/CimTopologyMapper.h"
#include "topology/PowerTopologyBuilder.h"
#include "topology/PowerTopologyService.h"

namespace {

PowerTopologyAnalysisSnapshot withImportedNodeConductors(const PowerTopologyAnalysisSnapshot& snapshot)
{
    PowerTopologyAnalysisSnapshot enriched = snapshot;
    if (!enriched.conductors.isEmpty())
        return enriched;

    for (const PowerTopologyNodeRecord& node : enriched.nodes)
    {
        for (int i = 0; i < node.terminals.size(); ++i)
        {
            const CimdrawTopologyTerminalRef& lhs = node.terminals.at(i);
            for (int j = i + 1; j < node.terminals.size(); ++j)
            {
                const CimdrawTopologyTerminalRef& rhs = node.terminals.at(j);
                if (lhs.componentId == rhs.componentId)
                    continue;

                PowerTopologyConductorRecord conductor;
                conductor.wireId = QStringLiteral("imported-node-link:%1:%2:%3")
                                       .arg(node.nodeId)
                                       .arg(i)
                                       .arg(j);
                conductor.fromDeviceId = lhs.componentId;
                conductor.fromTerminalId = lhs.terminalId;
                conductor.toDeviceId = rhs.componentId;
                conductor.toTerminalId = rhs.terminalId;

                enriched.conductorIndex.insert(conductor.wireId, enriched.conductors.size());
                enriched.conductors.push_back(conductor);
                if (!enriched.deviceToConductorIds[conductor.fromDeviceId].contains(conductor.wireId))
                    enriched.deviceToConductorIds[conductor.fromDeviceId].append(conductor.wireId);
                if (!enriched.deviceToConductorIds[conductor.toDeviceId].contains(conductor.wireId))
                    enriched.deviceToConductorIds[conductor.toDeviceId].append(conductor.wireId);
            }
        }
    }

    return enriched;
}

} // namespace

PowerTopologyProjection::PowerTopologyProjection(PowerTopologyProjectionSourceKind sourceKind,
                                                 const PowerTopologyAnalysisSnapshot& snapshot)
    : sourceKind_(sourceKind)
    , snapshot_(snapshot)
{
}

PowerTopologyProjection PowerTopologyProjection::fromImportedModel(const CimModel& model)
{
    CimTopologyMapper mapper;
    return fromImportedSnapshot(mapper.build(model));
}

PowerTopologyProjection PowerTopologyProjection::fromImportedSnapshot(const PowerTopologyAnalysisSnapshot& snapshot)
{
    return PowerTopologyProjection(PowerTopologyProjectionSourceKind::ImportedModel,
                                   withImportedNodeConductors(snapshot));
}

PowerTopologyProjection PowerTopologyProjection::fromRuntimeScene(CimdrawScene* scene)
{
    return fromRuntimeSnapshot(PowerTopologyBuilder::build(scene));
}

PowerTopologyProjection PowerTopologyProjection::fromRuntimeSnapshot(const PowerTopologyAnalysisSnapshot& snapshot)
{
    return PowerTopologyProjection(PowerTopologyProjectionSourceKind::RuntimeScene, snapshot);
}

bool PowerTopologyProjection::isValid() const
{
    return !snapshot_.devices.isEmpty() || !snapshot_.conductors.isEmpty() || !snapshot_.nodes.isEmpty();
}

PowerTopologyProjectionSourceKind PowerTopologyProjection::sourceKind() const
{
    return sourceKind_;
}

QString PowerTopologyProjection::sourceKindKey() const
{
    switch (sourceKind_)
    {
    case PowerTopologyProjectionSourceKind::ImportedModel:
        return QStringLiteral("importedModel");
    case PowerTopologyProjectionSourceKind::RuntimeScene:
        return QStringLiteral("runtimeScene");
    }
    return QStringLiteral("unknown");
}

QString PowerTopologyProjection::sourceLabel() const
{
    switch (sourceKind_)
    {
    case PowerTopologyProjectionSourceKind::ImportedModel:
        return QStringLiteral("导入模型语义快照");
    case PowerTopologyProjectionSourceKind::RuntimeScene:
        return QStringLiteral("运行期场景拓扑快照");
    }
    return QStringLiteral("未知拓扑来源");
}

QString PowerTopologyProjection::truthBoundaryText() const
{
    switch (sourceKind_)
    {
    case PowerTopologyProjectionSourceKind::ImportedModel:
        return QStringLiteral("CimModel/CimObject 是导入数据真源；当前拓扑分析基于导入语义快照，不替代运行期拓扑真源。");
    case PowerTopologyProjectionSourceKind::RuntimeScene:
        return QStringLiteral("运行期连接关系以当前场景/拓扑图为真源；CimModel/CimObject 仍只承担导入数据真源职责。");
    }
    return QStringLiteral("未定义拓扑真源边界。");
}

const PowerTopologyAnalysisSnapshot& PowerTopologyProjection::snapshot() const
{
    return snapshot_;
}

QVector<int> PowerTopologyProjection::deviceNodeIds(const QString& deviceId) const
{
    return PowerTopologyService::deviceNodeIds(snapshot_, deviceId);
}

QStringList PowerTopologyProjection::deviceConductorIds(const QString& deviceId) const
{
    return PowerTopologyService::deviceConductorIds(snapshot_, deviceId);
}

QStringList PowerTopologyProjection::connectedComponentDeviceIds(const QString& deviceId) const
{
    return PowerTopologyService::connectedComponentDeviceIds(snapshot_, deviceId);
}

QStringList PowerTopologyProjection::busbarAttachedDevices(const QString& deviceId) const
{
    return PowerTopologyService::busbarAttachedDevices(snapshot_, deviceId);
}

QStringList PowerTopologyProjection::reachableDevices(const QString& deviceId) const
{
    return PowerTopologyService::reachableDevices(snapshot_, deviceId);
}

QStringList PowerTopologyProjection::directedReachableDevices(const QString& sourceDeviceId) const
{
    return PowerTopologyService::directedReachableDevices(snapshot_, sourceDeviceId);
}

PowerTopologyIslandAnalysis PowerTopologyProjection::islandAnalysis() const
{
    return PowerTopologyService::islandAnalysis(snapshot_);
}

PowerTopologyBranchAnalysis PowerTopologyProjection::branchAnalysis(const QString& sourceDeviceId) const
{
    return PowerTopologyService::branchAnalysis(snapshot_, sourceDeviceId);
}

PowerTopologyLoopAnalysis PowerTopologyProjection::loopAnalysis() const
{
    return PowerTopologyService::loopAnalysis(snapshot_);
}

PowerTopologyPath PowerTopologyProjection::shortestSupplyPath(const QString& sourceDeviceId,
                                                              const QString& targetDeviceId) const
{
    return PowerTopologyService::shortestSupplyPath(snapshot_, sourceDeviceId, targetDeviceId);
}

PowerTopologyPath PowerTopologyProjection::shortestDirectedSupplyPath(const QString& sourceDeviceId,
                                                                      const QString& targetDeviceId) const
{
    return PowerTopologyService::shortestDirectedSupplyPath(snapshot_, sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath PowerTopologyProjection::shortestGroundPath(const QString& sourceDeviceId) const
{
    return PowerTopologyService::shortestGroundPath(snapshot_, sourceDeviceId);
}
