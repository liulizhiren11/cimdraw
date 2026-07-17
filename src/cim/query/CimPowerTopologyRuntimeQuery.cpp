#include "cim/query/CimPowerTopologyRuntimeQuery.h"

#include "CimdrawScene.h"
#include "CimdrawView.h"

PowerTopologyProjection CimPowerTopologyRuntimeQuery::runtimePowerTopologyProjection(
    CimdrawScene* scene) const
{
    return PowerTopologyProjection::fromRuntimeScene(scene);
}

PowerTopologyAnalysisSnapshot CimPowerTopologyRuntimeQuery::runtimePowerTopologySnapshot(
    CimdrawScene* scene) const
{
    return runtimePowerTopologyProjection(scene).snapshot();
}

PowerTopologyDocumentExport CimPowerTopologyRuntimeQuery::runtimePowerTopologyDocumentExport(
    CimdrawScene* scene) const
{
    PowerTopologyDocumentExport document;
    if (!scene)
        return document;

    const PowerTopologyAnalysisSnapshot snapshot = runtimePowerTopologySnapshot(scene);
    document.devices = snapshot.devices;
    document.conductors = snapshot.conductors;
    document.nodes = snapshot.nodes;

    if (CimdrawView* view = scene->getView())
    {
        const QPoint viewportCenter(view->viewport()->width() / 2, view->viewport()->height() / 2);
        document.viewCenter = view->mapToScene(viewportCenter);
        document.viewScale = view->transform().m11();
        if (qFuzzyIsNull(document.viewScale))
            document.viewScale = 1.0;
    }
    else
    {
        document.viewCenter = scene->getContentsRect().center();
        document.viewScale = 1.0;
    }

    return document;
}

QVector<int> CimPowerTopologyRuntimeQuery::powerDeviceNodeIds(CimdrawScene* scene,
                                                              const QString& deviceId) const
{
    if (!scene || deviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).deviceNodeIds(deviceId);
}

QStringList CimPowerTopologyRuntimeQuery::powerDeviceConductorIds(CimdrawScene* scene,
                                                                  const QString& deviceId) const
{
    if (!scene || deviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).deviceConductorIds(deviceId);
}

QStringList CimPowerTopologyRuntimeQuery::connectedPowerDevices(CimdrawScene* scene,
                                                                const QString& deviceId) const
{
    if (!scene || deviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).connectedComponentDeviceIds(deviceId);
}

QStringList CimPowerTopologyRuntimeQuery::busbarAttachedPowerDevices(CimdrawScene* scene,
                                                                     const QString& deviceId) const
{
    if (!scene || deviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).busbarAttachedDevices(deviceId);
}

QStringList CimPowerTopologyRuntimeQuery::reachablePowerDevices(CimdrawScene* scene,
                                                                const QString& deviceId) const
{
    if (!scene || deviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).reachableDevices(deviceId);
}

QStringList CimPowerTopologyRuntimeQuery::directedReachablePowerDevices(
    CimdrawScene* scene,
    const QString& sourceDeviceId) const
{
    if (!scene || sourceDeviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).directedReachableDevices(sourceDeviceId);
}

PowerTopologyIslandAnalysis CimPowerTopologyRuntimeQuery::analyzePowerIslands(CimdrawScene* scene) const
{
    if (!scene)
        return {};
    return runtimePowerTopologyProjection(scene).islandAnalysis();
}

PowerTopologyBranchAnalysis CimPowerTopologyRuntimeQuery::analyzePowerBranches(
    CimdrawScene* scene,
    const QString& sourceDeviceId) const
{
    if (!scene || sourceDeviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).branchAnalysis(sourceDeviceId);
}

PowerTopologyLoopAnalysis CimPowerTopologyRuntimeQuery::analyzePowerLoops(CimdrawScene* scene) const
{
    if (!scene)
        return {};
    return runtimePowerTopologyProjection(scene).loopAnalysis();
}

PowerTopologyPath CimPowerTopologyRuntimeQuery::shortestPowerSupplyPath(CimdrawScene* scene,
                                                                        const QString& sourceDeviceId,
                                                                        const QString& targetDeviceId) const
{
    if (!scene || sourceDeviceId.trimmed().isEmpty() || targetDeviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).shortestSupplyPath(sourceDeviceId, targetDeviceId);
}

PowerTopologyPath CimPowerTopologyRuntimeQuery::shortestDirectedPowerSupplyPath(
    CimdrawScene* scene,
    const QString& sourceDeviceId,
    const QString& targetDeviceId) const
{
    if (!scene || sourceDeviceId.trimmed().isEmpty() || targetDeviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).shortestDirectedSupplyPath(sourceDeviceId,
                                                                            targetDeviceId);
}

PowerTopologyGroundPath CimPowerTopologyRuntimeQuery::shortestPowerGroundPath(
    CimdrawScene* scene,
    const QString& sourceDeviceId) const
{
    if (!scene || sourceDeviceId.trimmed().isEmpty())
        return {};
    return runtimePowerTopologyProjection(scene).shortestGroundPath(sourceDeviceId);
}
