#include "LzScenePowerTopologyAnalysisController.h"

#include "LzScene.h"
#include "topology/PowerTopologyBuilder.h"
#include "topology/PowerTopologyService.h"

namespace {

bool supportsSwitchPreviewRole(PowerTopologyRole role)
{
    return role == PowerTopologyRole::Breaker
        || role == PowerTopologyRole::Disconnector
        || role == PowerTopologyRole::GroundSwitch;
}

bool resolveSwitchPreviewContext(LzScene* scene,
                                 const QString& deviceId,
                                 int requestedTarget,
                                 PowerTopologyAnalysisSnapshot* beforeSnapshot,
                                 int* boundedTarget,
                                 int* fromSwitchPosition,
                                 int* deviceIndex)
{
    if (!scene || !beforeSnapshot || !boundedTarget || !fromSwitchPosition || !deviceIndex)
        return false;

    *beforeSnapshot = scene->buildPowerTopologySnapshot();
    *deviceIndex = beforeSnapshot->deviceIndex.value(deviceId, -1);
    if (*deviceIndex < 0 || *deviceIndex >= beforeSnapshot->devices.size())
        return false;

    const auto role = beforeSnapshot->devices.at(*deviceIndex).role;
    if (!supportsSwitchPreviewRole(role))
        return false;

    *boundedTarget = qBound(0, requestedTarget, 1);
    *fromSwitchPosition = beforeSnapshot->devices.at(*deviceIndex).internalConnectivity.isEmpty() ? 0 : 1;
    return true;
}

} // namespace

QVector<int> LzScenePowerTopologyAnalysisController::powerDeviceNodeIds(LzScene* scene, const QString& deviceId) const
{
    return scene ? PowerTopologyService::deviceNodeIds(scene->buildPowerTopologySnapshot(), deviceId) : QVector<int>{};
}

QStringList LzScenePowerTopologyAnalysisController::powerDeviceConductorIds(LzScene* scene, const QString& deviceId) const
{
    return scene ? PowerTopologyService::deviceConductorIds(scene->buildPowerTopologySnapshot(), deviceId) : QStringList{};
}

QStringList LzScenePowerTopologyAnalysisController::connectedPowerDevices(LzScene* scene, const QString& deviceId) const
{
    return scene ? PowerTopologyService::connectedComponentDeviceIds(scene->buildPowerTopologySnapshot(), deviceId)
                 : QStringList{};
}

QStringList LzScenePowerTopologyAnalysisController::busbarAttachedPowerDevices(LzScene* scene, const QString& deviceId) const
{
    return scene ? PowerTopologyService::busbarAttachedDevices(scene->buildPowerTopologySnapshot(), deviceId)
                 : QStringList{};
}

QStringList LzScenePowerTopologyAnalysisController::reachablePowerDevices(LzScene* scene, const QString& deviceId) const
{
    return scene ? PowerTopologyService::reachableDevices(scene->buildPowerTopologySnapshot(), deviceId)
                 : QStringList{};
}

QStringList LzScenePowerTopologyAnalysisController::directedReachablePowerDevices(LzScene* scene,
                                                                                  const QString& sourceDeviceId) const
{
    return scene ? PowerTopologyService::directedReachableDevices(scene->buildPowerTopologySnapshot(), sourceDeviceId)
                 : QStringList{};
}

PowerTopologyIslandAnalysis LzScenePowerTopologyAnalysisController::analyzePowerIslands(LzScene* scene) const
{
    return scene ? PowerTopologyService::islandAnalysis(scene->buildPowerTopologySnapshot())
                 : PowerTopologyIslandAnalysis{};
}

PowerTopologyBranchAnalysis LzScenePowerTopologyAnalysisController::analyzePowerBranches(LzScene* scene,
                                                                                         const QString& sourceDeviceId) const
{
    return scene ? PowerTopologyService::branchAnalysis(scene->buildPowerTopologySnapshot(), sourceDeviceId)
                 : PowerTopologyBranchAnalysis{};
}

PowerTopologyLoopAnalysis LzScenePowerTopologyAnalysisController::analyzePowerLoops(LzScene* scene) const
{
    return scene ? PowerTopologyService::loopAnalysis(scene->buildPowerTopologySnapshot())
                 : PowerTopologyLoopAnalysis{};
}

PowerTopologyPath LzScenePowerTopologyAnalysisController::shortestPowerSupplyPath(LzScene* scene,
                                                                                  const QString& sourceDeviceId,
                                                                                  const QString& targetDeviceId) const
{
    return scene ? PowerTopologyService::shortestSupplyPath(scene->buildPowerTopologySnapshot(),
                                                            sourceDeviceId,
                                                            targetDeviceId)
                 : PowerTopologyPath{};
}

PowerTopologyPath LzScenePowerTopologyAnalysisController::shortestDirectedPowerSupplyPath(LzScene* scene,
                                                                                          const QString& sourceDeviceId,
                                                                                          const QString& targetDeviceId) const
{
    return scene ? PowerTopologyService::shortestDirectedSupplyPath(scene->buildPowerTopologySnapshot(),
                                                                    sourceDeviceId,
                                                                    targetDeviceId)
                 : PowerTopologyPath{};
}

PowerTopologyGroundPath LzScenePowerTopologyAnalysisController::shortestPowerGroundPath(LzScene* scene,
                                                                                        const QString& sourceDeviceId) const
{
    return scene ? PowerTopologyService::shortestGroundPath(scene->buildPowerTopologySnapshot(), sourceDeviceId)
                 : PowerTopologyGroundPath{};
}

PowerTopologySwitchChangePreview LzScenePowerTopologyAnalysisController::previewPowerSwitchTopologyChange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    PowerTopologyAnalysisSnapshot beforeSnapshot;
    int boundedTarget = 0;
    int fromSwitchPosition = 0;
    int deviceIndex = -1;
    if (!resolveSwitchPreviewContext(scene,
                                     deviceId,
                                     toSwitchPosition,
                                     &beforeSnapshot,
                                     &boundedTarget,
                                     &fromSwitchPosition,
                                     &deviceIndex))
    {
        return {};
    }

    const PowerTopologyAnalysisSnapshot afterSnapshot =
        PowerTopologyBuilder::buildWithSwitchOverride(scene, deviceId, boundedTarget);
    return PowerTopologyService::compareSwitchConnectivity(
        beforeSnapshot, afterSnapshot, deviceId, fromSwitchPosition, boundedTarget);
}

PowerTopologyProtectionRangePreview LzScenePowerTopologyAnalysisController::previewPowerProtectionRange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    PowerTopologyAnalysisSnapshot beforeSnapshot;
    int boundedTarget = 0;
    int fromSwitchPosition = 0;
    int deviceIndex = -1;
    if (!resolveSwitchPreviewContext(scene,
                                     deviceId,
                                     toSwitchPosition,
                                     &beforeSnapshot,
                                     &boundedTarget,
                                     &fromSwitchPosition,
                                     &deviceIndex))
    {
        return {};
    }

    const PowerTopologyAnalysisSnapshot afterSnapshot =
        PowerTopologyBuilder::buildWithSwitchOverride(scene, deviceId, boundedTarget);
    return PowerTopologyService::previewProtectionRange(beforeSnapshot,
                                                        afterSnapshot,
                                                        deviceId,
                                                        fromSwitchPosition,
                                                        boundedTarget,
                                                        sourceDeviceId);
}

PowerTopologyOperationPreview LzScenePowerTopologyAnalysisController::previewPowerSwitchOperation(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    PowerTopologyAnalysisSnapshot beforeSnapshot;
    int boundedTarget = 0;
    int fromSwitchPosition = 0;
    int deviceIndex = -1;
    if (!resolveSwitchPreviewContext(scene,
                                     deviceId,
                                     toSwitchPosition,
                                     &beforeSnapshot,
                                     &boundedTarget,
                                     &fromSwitchPosition,
                                     &deviceIndex))
    {
        return {};
    }

    const PowerTopologyAnalysisSnapshot afterSnapshot =
        PowerTopologyBuilder::buildWithSwitchOverride(scene, deviceId, boundedTarget);
    return PowerTopologyService::previewSwitchOperation(beforeSnapshot,
                                                        afterSnapshot,
                                                        deviceId,
                                                        fromSwitchPosition,
                                                        boundedTarget,
                                                        supplySourceDeviceId,
                                                        supplyTargetDeviceId,
                                                        groundSourceDeviceId);
}
