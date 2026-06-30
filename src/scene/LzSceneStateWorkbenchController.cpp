#include "scene/LzSceneStateWorkbenchController.h"

#include "scene/LzSceneDeferredUpdateController.h"
#include "scene/LzSceneStateController.h"
#include "scene/LzSceneTopologyStateController.h"

LzView* LzSceneStateWorkbenchController::view(LzView* currentView) const
{
    LzSceneStateController controller;
    return controller.view(currentView);
}

void LzSceneStateWorkbenchController::setView(LzView*& currentView, LzView* nextView) const
{
    LzSceneStateController controller;
    controller.setView(currentView, nextView);
}

bool LzSceneStateWorkbenchController::paintState(bool currentState) const
{
    LzSceneStateController controller;
    return controller.paintState(currentState);
}

void LzSceneStateWorkbenchController::setPaintState(bool& currentState, bool nextState) const
{
    LzSceneStateController controller;
    controller.setPaintState(currentState, nextState);
}

bool LzSceneStateWorkbenchController::interactiveTransformActive(bool currentState) const
{
    LzSceneStateController controller;
    return controller.interactiveTransformActive(currentState);
}

void LzSceneStateWorkbenchController::setInteractiveTransformActive(bool& currentState, bool nextState) const
{
    LzSceneStateController controller;
    controller.setInteractiveTransformActive(currentState, nextState);
}

void LzSceneStateWorkbenchController::invalidateTopologySnapshots(
    LzSceneTopologyStateController& topologyStateController) const
{
    topologyStateController.invalidateSnapshots();
}

void LzSceneStateWorkbenchController::scheduleTopologyRebuild(
    LzScene* scene,
    LzSceneDeferredUpdateController& deferredUpdateController,
    LzSceneTopologyStateController& topologyStateController) const
{
    invalidateTopologySnapshots(topologyStateController);
    deferredUpdateController.scheduleTopologyRebuild(scene);
}

void LzSceneStateWorkbenchController::cancelConnectLinePathRecompute(
    LzSceneDeferredUpdateController& deferredUpdateController,
    LzConnectLine* line) const
{
    deferredUpdateController.cancelConnectLinePathRecompute(line);
}

bool LzSceneStateWorkbenchController::isConnectLinePathUpdatePending(
    const LzSceneDeferredUpdateController& deferredUpdateController) const
{
    return deferredUpdateController.isConnectLinePathUpdatePending();
}

void LzSceneStateWorkbenchController::requestDeferredItemPropertyPanel(
    LzSceneDeferredUpdateController& deferredUpdateController) const
{
    deferredUpdateController.requestDeferredItemPropertyPanel();
}

void LzSceneStateWorkbenchController::flushDeferredItemPropertyPanel(
    LzScene* scene,
    LzSceneDeferredUpdateController& deferredUpdateController) const
{
    deferredUpdateController.flushDeferredItemPropertyPanel(scene);
}

void LzSceneStateWorkbenchController::scheduleConnectLinesPathRecompute(
    LzScene* scene,
    LzSceneDeferredUpdateController& deferredUpdateController,
    const QList<LzConnectLine*>& lines) const
{
    deferredUpdateController.scheduleConnectLinesPathRecompute(scene, lines);
}

void LzSceneStateWorkbenchController::scheduleConnectorPostprocess(
    LzScene* scene,
    LzSceneDeferredUpdateController& deferredUpdateController,
    const QList<LzConnectLine*>& affectedLines) const
{
    deferredUpdateController.scheduleConnectorPostprocess(scene, affectedLines);
}

bool LzSceneStateWorkbenchController::setWiringData(
    QHash<QString, QVariant>& wiringData,
    LzSceneTopologyStateController& topologyStateController,
    const QString& key,
    const QVariant& value) const
{
    LzSceneStateController controller;
    if (!controller.setWiringData(wiringData, key, value))
        return false;
    invalidateTopologySnapshots(topologyStateController);
    return true;
}

QVariant LzSceneStateWorkbenchController::wiringData(const QHash<QString, QVariant>& wiringData,
                                                     const QString& key) const
{
    LzSceneStateController controller;
    return controller.wiringData(wiringData, key);
}
