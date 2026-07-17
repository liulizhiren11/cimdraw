#include "cim/query/CimPowerTopologyPreviewQuery.h"

#include "CimdrawScene.h"
#include "topology/PowerTopologyBuilder.h"
#include "topology/PowerTopologyProjection.h"
#include "topology/PowerTopologyService.h"

namespace {

bool supportsSwitchPreviewRole(PowerTopologyRole role)
{
    return role == PowerTopologyRole::Breaker
        || role == PowerTopologyRole::Disconnector
        || role == PowerTopologyRole::GroundSwitch;
}

bool resolveSwitchPreviewContext(CimdrawScene* scene,
                                 const QString& deviceId,
                                 int requestedTarget,
                                 PowerTopologyAnalysisSnapshot* beforeSnapshot,
                                 int* boundedTarget,
                                 int* fromSwitchPosition,
                                 int* deviceIndex)
{
    if (!scene || !beforeSnapshot || !boundedTarget || !fromSwitchPosition || !deviceIndex)
        return false;

    *beforeSnapshot = PowerTopologyProjection::fromRuntimeScene(scene).snapshot();
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

PowerTopologySwitchChangePreview
CimPowerTopologyPreviewQuery::previewPowerSwitchTopologyChange(CimdrawScene* scene,
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

PowerTopologyProtectionRangePreview
CimPowerTopologyPreviewQuery::previewPowerProtectionRange(CimdrawScene* scene,
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

PowerTopologyOperationPreview
CimPowerTopologyPreviewQuery::previewPowerSwitchOperation(CimdrawScene* scene,
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
