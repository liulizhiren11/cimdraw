#include "scene/CimdrawSceneStateWorkbenchController.h"

#include "scene/CimdrawSceneDeferredUpdateController.h"
#include "scene/CimdrawSceneStateController.h"
#include "scene/CimdrawSceneTopologyStateController.h"

CimdrawView* CimdrawSceneStateWorkbenchController::view(CimdrawView* currentView) const
{
    CimdrawSceneStateController controller;
    return controller.view(currentView);
}

void CimdrawSceneStateWorkbenchController::setView(CimdrawView*& currentView, CimdrawView* nextView) const
{
    CimdrawSceneStateController controller;
    controller.setView(currentView, nextView);
}

bool CimdrawSceneStateWorkbenchController::paintState(bool currentState) const
{
    CimdrawSceneStateController controller;
    return controller.paintState(currentState);
}

void CimdrawSceneStateWorkbenchController::setPaintState(bool& currentState, bool nextState) const
{
    CimdrawSceneStateController controller;
    controller.setPaintState(currentState, nextState);
}

bool CimdrawSceneStateWorkbenchController::interactiveTransformActive(bool currentState) const
{
    CimdrawSceneStateController controller;
    return controller.interactiveTransformActive(currentState);
}

void CimdrawSceneStateWorkbenchController::setInteractiveTransformActive(bool& currentState, bool nextState) const
{
    CimdrawSceneStateController controller;
    controller.setInteractiveTransformActive(currentState, nextState);
}

void CimdrawSceneStateWorkbenchController::invalidateTopologySnapshots(
    CimdrawSceneTopologyStateController& topologyStateController) const
{
    topologyStateController.invalidateSnapshots();
}

void CimdrawSceneStateWorkbenchController::scheduleTopologyRebuild(
    CimdrawScene* scene,
    CimdrawSceneDeferredUpdateController& deferredUpdateController,
    CimdrawSceneTopologyStateController& topologyStateController) const
{
    invalidateTopologySnapshots(topologyStateController);
    deferredUpdateController.scheduleTopologyRebuild(scene);
}

void CimdrawSceneStateWorkbenchController::cancelConnectLinePathRecompute(
    CimdrawSceneDeferredUpdateController& deferredUpdateController,
    CimdrawConnectLine* line) const
{
    deferredUpdateController.cancelConnectLinePathRecompute(line);
}

bool CimdrawSceneStateWorkbenchController::isConnectLinePathUpdatePending(
    const CimdrawSceneDeferredUpdateController& deferredUpdateController) const
{
    return deferredUpdateController.isConnectLinePathUpdatePending();
}

void CimdrawSceneStateWorkbenchController::requestDeferredItemPropertyPanel(
    CimdrawSceneDeferredUpdateController& deferredUpdateController) const
{
    deferredUpdateController.requestDeferredItemPropertyPanel();
}

void CimdrawSceneStateWorkbenchController::flushDeferredItemPropertyPanel(
    CimdrawScene* scene,
    CimdrawSceneDeferredUpdateController& deferredUpdateController) const
{
    deferredUpdateController.flushDeferredItemPropertyPanel(scene);
}

void CimdrawSceneStateWorkbenchController::scheduleConnectLinesPathRecompute(
    CimdrawScene* scene,
    CimdrawSceneDeferredUpdateController& deferredUpdateController,
    const QList<CimdrawConnectLine*>& lines) const
{
    deferredUpdateController.scheduleConnectLinesPathRecompute(scene, lines);
}

void CimdrawSceneStateWorkbenchController::scheduleConnectorPostprocess(
    CimdrawScene* scene,
    CimdrawSceneDeferredUpdateController& deferredUpdateController,
    const QList<CimdrawConnectLine*>& affectedLines) const
{
    deferredUpdateController.scheduleConnectorPostprocess(scene, affectedLines);
}

bool CimdrawSceneStateWorkbenchController::setWiringData(
    QHash<QString, QVariant>& wiringData,
    CimdrawSceneTopologyStateController& topologyStateController,
    const QString& key,
    const QVariant& value) const
{
    CimdrawSceneStateController controller;
    if (!controller.setWiringData(wiringData, key, value))
        return false;
    invalidateTopologySnapshots(topologyStateController);
    return true;
}

QVariant CimdrawSceneStateWorkbenchController::wiringData(const QHash<QString, QVariant>& wiringData,
                                                     const QString& key) const
{
    CimdrawSceneStateController controller;
    return controller.wiringData(wiringData, key);
}
