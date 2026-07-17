#include "scene/CimdrawSceneTopologyWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneStateController.h"
#include "scene/CimdrawSceneTopologyStateController.h"

void CimdrawSceneTopologyWorkbenchController::rebuildTopologyIndex(CimdrawScene* scene,
                                                               CimdrawSceneTopologyStateController& stateController) const
{
    stateController.rebuildTopologyIndex(scene);
}

TopologyNodeRelationInfo CimdrawSceneTopologyWorkbenchController::relationNodeInfo(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController,
    const QString& nodeStableId) const
{
    return stateController.relationNodeInfo(scene, nodeStableId);
}

TopologyEdgeMeta CimdrawSceneTopologyWorkbenchController::relationEdgeMeta(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController,
    const QString& edgeStableId) const
{
    return stateController.relationEdgeMeta(scene, edgeStableId);
}

TopologyEdgeMeta CimdrawSceneTopologyWorkbenchController::resolveRelationEdgeMeta(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController,
    const TopologyEdgeLookupHint& lookupHint) const
{
    return stateController.resolveRelationEdgeMeta(scene, lookupHint);
}

QVector<TopologyEdgeMeta> CimdrawSceneTopologyWorkbenchController::relationEdgeMetas(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController) const
{
    return stateController.relationEdgeMetas(scene);
}

void CimdrawSceneTopologyWorkbenchController::highlightTopologyNode(CimdrawSceneTopologyStateController& stateController,
                                                               const QString& nodeStableId,
                                                               bool on,
                                                               const QColor& c) const
{
    stateController.highlightTopologyNode(nodeStableId, on, c);
}

void CimdrawSceneTopologyWorkbenchController::highlightTopologyEdge(CimdrawSceneTopologyStateController& stateController,
                                                               const QString& edgeStableId,
                                                               bool on,
                                                               const QColor& c) const
{
    stateController.highlightTopologyEdge(edgeStableId, on, c);
}

void CimdrawSceneTopologyWorkbenchController::highlightTopologyEdges(CimdrawSceneTopologyStateController& stateController,
                                                                const QStringList& edgeStableIds,
                                                                bool on,
                                                                const QColor& c) const
{
    stateController.highlightTopologyEdges(edgeStableIds, on, c);
}

void CimdrawSceneTopologyWorkbenchController::clearTopologyHighlights(CimdrawSceneTopologyStateController& stateController) const
{
    stateController.clearTopologyHighlights();
}

PowerTopologyAnalysisSnapshot CimdrawSceneTopologyWorkbenchController::buildPowerTopologySnapshot(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController) const
{
    Q_UNUSED(stateController);
    if (!scene)
        return {};

    return scene->buildPowerTopologySnapshot();
}

PowerTopologyDocumentExport CimdrawSceneTopologyWorkbenchController::buildPowerTopologyDocumentExport(
    CimdrawScene* scene,
    CimdrawSceneTopologyStateController& stateController) const
{
    Q_UNUSED(stateController);
    if (!scene)
        return {};

    return scene->buildPowerTopologyDocumentExport();
}

QVector<int> CimdrawSceneTopologyWorkbenchController::powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->powerDeviceNodeIds(deviceId);
}

QStringList CimdrawSceneTopologyWorkbenchController::powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->powerDeviceConductorIds(deviceId);
}

QStringList CimdrawSceneTopologyWorkbenchController::connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->connectedPowerDevices(deviceId);
}

QStringList CimdrawSceneTopologyWorkbenchController::busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->busbarAttachedPowerDevices(deviceId);
}

QStringList CimdrawSceneTopologyWorkbenchController::reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->reachablePowerDevices(deviceId);
}

QStringList CimdrawSceneTopologyWorkbenchController::directedReachablePowerDevices(CimdrawScene* scene,
                                                                               const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->directedReachablePowerDevices(sourceDeviceId);
}

PowerTopologyIslandAnalysis CimdrawSceneTopologyWorkbenchController::analyzePowerIslands(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    return scene->analyzePowerIslands();
}

PowerTopologyBranchAnalysis CimdrawSceneTopologyWorkbenchController::analyzePowerBranches(CimdrawScene* scene,
                                                                                      const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->analyzePowerBranches(sourceDeviceId);
}

PowerTopologyLoopAnalysis CimdrawSceneTopologyWorkbenchController::analyzePowerLoops(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    return scene->analyzePowerLoops();
}

PowerTopologyPath CimdrawSceneTopologyWorkbenchController::shortestPowerSupplyPath(CimdrawScene* scene,
                                                                               const QString& sourceDeviceId,
                                                                               const QString& targetDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestPowerSupplyPath(sourceDeviceId, targetDeviceId);
}

PowerTopologyPath CimdrawSceneTopologyWorkbenchController::shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                                                       const QString& sourceDeviceId,
                                                                                       const QString& targetDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestDirectedPowerSupplyPath(sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath CimdrawSceneTopologyWorkbenchController::shortestPowerGroundPath(CimdrawScene* scene,
                                                                                     const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestPowerGroundPath(sourceDeviceId);
}

PowerTopologySwitchChangePreview CimdrawSceneTopologyWorkbenchController::previewPowerSwitchTopologyChange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    if (!scene)
        return {};

    return scene->previewPowerSwitchTopologyChange(deviceId, toSwitchPosition);
}

PowerTopologyProtectionRangePreview CimdrawSceneTopologyWorkbenchController::previewPowerProtectionRange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->previewPowerProtectionRange(deviceId, toSwitchPosition, sourceDeviceId);
}

PowerTopologyOperationPreview CimdrawSceneTopologyWorkbenchController::previewPowerSwitchOperation(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->previewPowerSwitchOperation(deviceId,
                                              toSwitchPosition,
                                              supplySourceDeviceId,
                                              supplyTargetDeviceId,
                                              groundSourceDeviceId);
}

bool CimdrawSceneTopologyWorkbenchController::setWiringData(QHash<QString, QVariant>& wiringData,
                                                       const QString& key,
                                                       const QVariant& value) const
{
    CimdrawSceneStateController controller;
    return controller.setWiringData(wiringData, key, value);
}

QVariant CimdrawSceneTopologyWorkbenchController::wiringData(const QHash<QString, QVariant>& wiringData,
                                                        const QString& key) const
{
    CimdrawSceneStateController controller;
    return controller.wiringData(wiringData, key);
}
