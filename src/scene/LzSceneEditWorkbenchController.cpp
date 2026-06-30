#include "scene/LzSceneEditWorkbenchController.h"

#include "LzScene.h"
#include "LzView.h"
#include "scene/LzSceneEditController.h"

QUndoStack* LzSceneEditWorkbenchController::stackFromScene(LzScene* scene) const
{
    if (!scene)
        return nullptr;
    LzView* view = scene->getView();
    return view ? view->getStack() : nullptr;
}

void LzSceneEditWorkbenchController::alignLeft(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignLeft(selection, scene, stack);
}

void LzSceneEditWorkbenchController::alignRight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignRight(selection, scene, stack);
}

void LzSceneEditWorkbenchController::alignTop(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignTop(selection, scene, stack);
}

void LzSceneEditWorkbenchController::alignBottom(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignBottom(selection, scene, stack);
}

void LzSceneEditWorkbenchController::alignVCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignVCenter(selection, scene, stack);
}

void LzSceneEditWorkbenchController::alignHCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.alignHCenter(selection, scene, stack);
}

void LzSceneEditWorkbenchController::autoCol(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.autoCol(selection, scene, stack);
}

void LzSceneEditWorkbenchController::autoRow(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.autoRow(selection, scene, stack);
}

void LzSceneEditWorkbenchController::sameWidth(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.sameWidth(selection, scene, stack);
}

void LzSceneEditWorkbenchController::sameHeight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.sameHeight(selection, scene, stack);
}

void LzSceneEditWorkbenchController::sameSize(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.sameSize(selection, scene, stack);
}

void LzSceneEditWorkbenchController::shortcutLeft(const QList<QGraphicsItem*>& selection,
                                                  LzScene* scene,
                                                  QUndoStack* stack,
                                                  const QPointF& delta) const
{
    LzSceneEditController controller;
    controller.shortcutLeft(selection, scene, stack, delta);
}

void LzSceneEditWorkbenchController::shortcutRight(const QList<QGraphicsItem*>& selection,
                                                   LzScene* scene,
                                                   QUndoStack* stack,
                                                   const QPointF& delta) const
{
    LzSceneEditController controller;
    controller.shortcutRight(selection, scene, stack, delta);
}

void LzSceneEditWorkbenchController::shortcutUp(const QList<QGraphicsItem*>& selection,
                                                LzScene* scene,
                                                QUndoStack* stack,
                                                const QPointF& delta) const
{
    LzSceneEditController controller;
    controller.shortcutUp(selection, scene, stack, delta);
}

void LzSceneEditWorkbenchController::shortcutDown(const QList<QGraphicsItem*>& selection,
                                                  LzScene* scene,
                                                  QUndoStack* stack,
                                                  const QPointF& delta) const
{
    LzSceneEditController controller;
    controller.shortcutDown(selection, scene, stack, delta);
}

void LzSceneEditWorkbenchController::cut(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.cut(selection, scene, stack);
}

void LzSceneEditWorkbenchController::copy(const QList<QGraphicsItem*>& selection) const
{
    LzSceneEditController controller;
    controller.copy(selection);
}

void LzSceneEditWorkbenchController::paste(LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.paste(scene, stack);
}

void LzSceneEditWorkbenchController::remove(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.remove(selection, scene, stack);
}

void LzSceneEditWorkbenchController::levelUp(const QList<QGraphicsItem*>& selection,
                                             LzScene* scene,
                                             const QGraphicsScene* graphicsScene) const
{
    LzSceneEditController controller;
    controller.levelUp(selection, scene, graphicsScene);
}

void LzSceneEditWorkbenchController::levelDown(const QList<QGraphicsItem*>& selection,
                                               LzScene* scene,
                                               const QGraphicsScene* graphicsScene) const
{
    LzSceneEditController controller;
    controller.levelDown(selection, scene, graphicsScene);
}

void LzSceneEditWorkbenchController::toTop(const QList<QGraphicsItem*>& selection,
                                           LzScene* scene,
                                           const QGraphicsScene* graphicsScene) const
{
    LzSceneEditController controller;
    controller.toTop(selection, scene, graphicsScene);
}

void LzSceneEditWorkbenchController::toBottom(const QList<QGraphicsItem*>& selection,
                                              LzScene* scene,
                                              const QGraphicsScene* graphicsScene) const
{
    LzSceneEditController controller;
    controller.toBottom(selection, scene, graphicsScene);
}

void LzSceneEditWorkbenchController::rotate(const QList<QGraphicsItem*>& selection,
                                            LzScene* scene,
                                            QUndoStack* stack,
                                            qreal angleDegrees) const
{
    LzSceneEditController controller;
    controller.rotate(selection, scene, stack, angleDegrees);
}

void LzSceneEditWorkbenchController::group(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.group(selection, scene, stack);
}

void LzSceneEditWorkbenchController::ungroup(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    LzSceneEditController controller;
    controller.ungroup(selection, scene, stack);
}
