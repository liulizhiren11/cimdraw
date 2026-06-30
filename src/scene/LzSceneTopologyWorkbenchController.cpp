#include "scene/LzSceneTopologyWorkbenchController.h"

#include "scene/LzScenePowerTopologyAnalysisController.h"
#include "scene/LzSceneStateController.h"
#include "scene/LzSceneTopologyQueryController.h"
#include "scene/LzSceneTopologyStateController.h"

void LzSceneTopologyWorkbenchController::rebuildTopologyIndex(LzScene* scene,
                                                              LzSceneTopologyStateController& stateController) const
{
    stateController.rebuildTopologyIndex(scene);
}

const TopologyGraphIndex* LzSceneTopologyWorkbenchController::topologyIndex(
    const LzSceneTopologyStateController& stateController) const
{
    return stateController.topologyIndex();
}

TopologyGraphIndex* LzSceneTopologyWorkbenchController::topologyIndex(
    LzSceneTopologyStateController& stateController) const
{
    return stateController.topologyIndex();
}

void LzSceneTopologyWorkbenchController::highlightTopologyNode(LzSceneTopologyStateController& stateController,
                                                               const QString& nodeStableId,
                                                               bool on,
                                                               const QColor& c) const
{
    stateController.highlightTopologyNode(nodeStableId, on, c);
}

void LzSceneTopologyWorkbenchController::highlightTopologyEdge(LzSceneTopologyStateController& stateController,
                                                               const QString& edgeStableId,
                                                               bool on,
                                                               const QColor& c) const
{
    stateController.highlightTopologyEdge(edgeStableId, on, c);
}

void LzSceneTopologyWorkbenchController::highlightTopologyEdges(LzSceneTopologyStateController& stateController,
                                                                const QStringList& edgeStableIds,
                                                                bool on,
                                                                const QColor& c) const
{
    stateController.highlightTopologyEdges(edgeStableIds, on, c);
}

void LzSceneTopologyWorkbenchController::clearTopologyHighlights(LzSceneTopologyStateController& stateController) const
{
    stateController.clearTopologyHighlights();
}

PowerTopologyAnalysisSnapshot LzSceneTopologyWorkbenchController::buildPowerTopologySnapshot(
    LzScene* scene,
    LzSceneTopologyStateController& stateController) const
{
    return stateController.buildPowerTopologySnapshot(scene);
}

PowerTopologyDocumentExport LzSceneTopologyWorkbenchController::buildPowerTopologyDocumentExport(
    LzScene* scene,
    LzSceneTopologyStateController& stateController) const
{
    return stateController.buildPowerTopologyDocumentExport(scene);
}

QVector<int> LzSceneTopologyWorkbenchController::powerDeviceNodeIds(LzScene* scene, const QString& deviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.powerDeviceNodeIds(scene, deviceId);
}

QStringList LzSceneTopologyWorkbenchController::powerDeviceConductorIds(LzScene* scene, const QString& deviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.powerDeviceConductorIds(scene, deviceId);
}

QStringList LzSceneTopologyWorkbenchController::connectedPowerDevices(LzScene* scene, const QString& deviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.connectedPowerDevices(scene, deviceId);
}

QStringList LzSceneTopologyWorkbenchController::busbarAttachedPowerDevices(LzScene* scene, const QString& deviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.busbarAttachedPowerDevices(scene, deviceId);
}

QStringList LzSceneTopologyWorkbenchController::reachablePowerDevices(LzScene* scene, const QString& deviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.reachablePowerDevices(scene, deviceId);
}

QStringList LzSceneTopologyWorkbenchController::directedReachablePowerDevices(LzScene* scene,
                                                                              const QString& sourceDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.directedReachablePowerDevices(scene, sourceDeviceId);
}

PowerTopologyIslandAnalysis LzSceneTopologyWorkbenchController::analyzePowerIslands(LzScene* scene) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.analyzePowerIslands(scene);
}

PowerTopologyBranchAnalysis LzSceneTopologyWorkbenchController::analyzePowerBranches(LzScene* scene,
                                                                                     const QString& sourceDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.analyzePowerBranches(scene, sourceDeviceId);
}

PowerTopologyLoopAnalysis LzSceneTopologyWorkbenchController::analyzePowerLoops(LzScene* scene) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.analyzePowerLoops(scene);
}

PowerTopologyPath LzSceneTopologyWorkbenchController::shortestPowerSupplyPath(LzScene* scene,
                                                                              const QString& sourceDeviceId,
                                                                              const QString& targetDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.shortestPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
}

PowerTopologyPath LzSceneTopologyWorkbenchController::shortestDirectedPowerSupplyPath(LzScene* scene,
                                                                                      const QString& sourceDeviceId,
                                                                                      const QString& targetDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.shortestDirectedPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath LzSceneTopologyWorkbenchController::shortestPowerGroundPath(LzScene* scene,
                                                                                    const QString& sourceDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.shortestPowerGroundPath(scene, sourceDeviceId);
}

PowerTopologySwitchChangePreview LzSceneTopologyWorkbenchController::previewPowerSwitchTopologyChange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.previewPowerSwitchTopologyChange(scene, deviceId, toSwitchPosition);
}

PowerTopologyProtectionRangePreview LzSceneTopologyWorkbenchController::previewPowerProtectionRange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.previewPowerProtectionRange(scene, deviceId, toSwitchPosition, sourceDeviceId);
}

PowerTopologyOperationPreview LzSceneTopologyWorkbenchController::previewPowerSwitchOperation(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    LzScenePowerTopologyAnalysisController controller;
    return controller.previewPowerSwitchOperation(scene,
                                                  deviceId,
                                                  toSwitchPosition,
                                                  supplySourceDeviceId,
                                                  supplyTargetDeviceId,
                                                  groundSourceDeviceId);
}

QVariantMap LzSceneTopologyWorkbenchController::buildPowerTopologyBindingSnapshot(LzScene* scene) const
{
    LzSceneTopologyQueryController controller;
    return controller.buildPowerTopologyBindingSnapshot(scene);
}

QVariantMap LzSceneTopologyWorkbenchController::buildTopologyBindingSnapshot(LzScene* scene) const
{
    LzSceneTopologyQueryController controller;
    return controller.buildTopologyBindingSnapshot(scene);
}

QVariantMap LzSceneTopologyWorkbenchController::queryTopology(LzScene* scene, const QVariantMap& request) const
{
    LzSceneTopologyQueryController controller;
    return controller.queryTopology(scene, request);
}

bool LzSceneTopologyWorkbenchController::setWiringData(QHash<QString, QVariant>& wiringData,
                                                       const QString& key,
                                                       const QVariant& value) const
{
    LzSceneStateController controller;
    return controller.setWiringData(wiringData, key, value);
}

QVariant LzSceneTopologyWorkbenchController::wiringData(const QHash<QString, QVariant>& wiringData,
                                                        const QString& key) const
{
    LzSceneStateController controller;
    return controller.wiringData(wiringData, key);
}
