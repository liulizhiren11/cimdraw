#include "scene/CimdrawSceneEditWorkbenchController.h"

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "scene/CimdrawSceneEditController.h"

QUndoStack* CimdrawSceneEditWorkbenchController::stackFromScene(CimdrawScene* scene) const
{
    if (!scene)
        return nullptr;
    CimdrawView* view = scene->getView();
    return view ? view->getStack() : nullptr;
}

void CimdrawSceneEditWorkbenchController::alignLeft(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignLeft(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::alignRight(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignRight(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::alignTop(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignTop(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::alignBottom(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignBottom(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::alignVCenter(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignVCenter(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::alignHCenter(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.alignHCenter(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::autoCol(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.autoCol(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::autoRow(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.autoRow(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::sameWidth(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.sameWidth(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::sameHeight(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.sameHeight(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::sameSize(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.sameSize(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::shortcutLeft(const QList<QGraphicsItem*>& selection,
                                                  CimdrawScene* scene,
                                                  QUndoStack* stack,
                                                  const QPointF& delta) const
{
    CimdrawSceneEditController controller;
    controller.shortcutLeft(selection, scene, stack, delta);
}

void CimdrawSceneEditWorkbenchController::shortcutRight(const QList<QGraphicsItem*>& selection,
                                                   CimdrawScene* scene,
                                                   QUndoStack* stack,
                                                   const QPointF& delta) const
{
    CimdrawSceneEditController controller;
    controller.shortcutRight(selection, scene, stack, delta);
}

void CimdrawSceneEditWorkbenchController::shortcutUp(const QList<QGraphicsItem*>& selection,
                                                CimdrawScene* scene,
                                                QUndoStack* stack,
                                                const QPointF& delta) const
{
    CimdrawSceneEditController controller;
    controller.shortcutUp(selection, scene, stack, delta);
}

void CimdrawSceneEditWorkbenchController::shortcutDown(const QList<QGraphicsItem*>& selection,
                                                  CimdrawScene* scene,
                                                  QUndoStack* stack,
                                                  const QPointF& delta) const
{
    CimdrawSceneEditController controller;
    controller.shortcutDown(selection, scene, stack, delta);
}

void CimdrawSceneEditWorkbenchController::cut(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.cut(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::copy(const QList<QGraphicsItem*>& selection) const
{
    CimdrawSceneEditController controller;
    controller.copy(selection);
}

void CimdrawSceneEditWorkbenchController::paste(CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.paste(scene, stack);
}

void CimdrawSceneEditWorkbenchController::remove(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.remove(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::levelUp(const QList<QGraphicsItem*>& selection,
                                             CimdrawScene* scene,
                                             const QGraphicsScene* graphicsScene) const
{
    CimdrawSceneEditController controller;
    controller.levelUp(selection, scene, graphicsScene);
}

void CimdrawSceneEditWorkbenchController::levelDown(const QList<QGraphicsItem*>& selection,
                                               CimdrawScene* scene,
                                               const QGraphicsScene* graphicsScene) const
{
    CimdrawSceneEditController controller;
    controller.levelDown(selection, scene, graphicsScene);
}

void CimdrawSceneEditWorkbenchController::toTop(const QList<QGraphicsItem*>& selection,
                                           CimdrawScene* scene,
                                           const QGraphicsScene* graphicsScene) const
{
    CimdrawSceneEditController controller;
    controller.toTop(selection, scene, graphicsScene);
}

void CimdrawSceneEditWorkbenchController::toBottom(const QList<QGraphicsItem*>& selection,
                                              CimdrawScene* scene,
                                              const QGraphicsScene* graphicsScene) const
{
    CimdrawSceneEditController controller;
    controller.toBottom(selection, scene, graphicsScene);
}

void CimdrawSceneEditWorkbenchController::rotate(const QList<QGraphicsItem*>& selection,
                                            CimdrawScene* scene,
                                            QUndoStack* stack,
                                            qreal angleDegrees) const
{
    CimdrawSceneEditController controller;
    controller.rotate(selection, scene, stack, angleDegrees);
}

void CimdrawSceneEditWorkbenchController::group(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.group(selection, scene, stack);
}

void CimdrawSceneEditWorkbenchController::ungroup(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    CimdrawSceneEditController controller;
    controller.ungroup(selection, scene, stack);
}
