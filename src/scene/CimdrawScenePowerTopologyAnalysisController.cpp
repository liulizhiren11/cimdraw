#include "CimdrawScenePowerTopologyAnalysisController.h"

#include "CimdrawScene.h"

QVector<int> CimdrawScenePowerTopologyAnalysisController::powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->powerDeviceNodeIds(deviceId);
}

QStringList CimdrawScenePowerTopologyAnalysisController::powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->powerDeviceConductorIds(deviceId);
}

QStringList CimdrawScenePowerTopologyAnalysisController::connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->connectedPowerDevices(deviceId);
}

QStringList CimdrawScenePowerTopologyAnalysisController::busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->busbarAttachedPowerDevices(deviceId);
}

QStringList CimdrawScenePowerTopologyAnalysisController::reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    if (!scene)
        return {};

    return scene->reachablePowerDevices(deviceId);
}

QStringList CimdrawScenePowerTopologyAnalysisController::directedReachablePowerDevices(CimdrawScene* scene,
                                                                                   const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->directedReachablePowerDevices(sourceDeviceId);
}

PowerTopologyIslandAnalysis CimdrawScenePowerTopologyAnalysisController::analyzePowerIslands(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    return scene->analyzePowerIslands();
}

PowerTopologyBranchAnalysis CimdrawScenePowerTopologyAnalysisController::analyzePowerBranches(CimdrawScene* scene,
                                                                                          const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->analyzePowerBranches(sourceDeviceId);
}

PowerTopologyLoopAnalysis CimdrawScenePowerTopologyAnalysisController::analyzePowerLoops(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    return scene->analyzePowerLoops();
}

PowerTopologyPath CimdrawScenePowerTopologyAnalysisController::shortestPowerSupplyPath(CimdrawScene* scene,
                                                                                   const QString& sourceDeviceId,
                                                                                   const QString& targetDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestPowerSupplyPath(sourceDeviceId, targetDeviceId);
}

PowerTopologyPath CimdrawScenePowerTopologyAnalysisController::shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                                                           const QString& sourceDeviceId,
                                                                                           const QString& targetDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestDirectedPowerSupplyPath(sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath CimdrawScenePowerTopologyAnalysisController::shortestPowerGroundPath(CimdrawScene* scene,
                                                                                         const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->shortestPowerGroundPath(sourceDeviceId);
}

PowerTopologySwitchChangePreview CimdrawScenePowerTopologyAnalysisController::previewPowerSwitchTopologyChange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    if (!scene)
        return {};

    return scene->previewPowerSwitchTopologyChange(deviceId, toSwitchPosition);
}

PowerTopologyProtectionRangePreview CimdrawScenePowerTopologyAnalysisController::previewPowerProtectionRange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    if (!scene)
        return {};

    return scene->previewPowerProtectionRange(deviceId, toSwitchPosition, sourceDeviceId);
}

PowerTopologyOperationPreview CimdrawScenePowerTopologyAnalysisController::previewPowerSwitchOperation(
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
