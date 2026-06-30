#include "scene/LzSceneSelectionWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneItemLifecycleController.h"
#include "scene/LzSceneSelectionManager.h"

QList<QGraphicsItem*> LzSceneSelectionWorkbenchController::pruned(const QList<QGraphicsItem*>& selection,
                                                                  const QGraphicsScene* scene) const
{
    LzSceneSelectionManager manager;
    return manager.pruned(selection, scene);
}

QGraphicsItem* LzSceneSelectionWorkbenchController::firstSelection(QList<QGraphicsItem*>& selection,
                                                                   const QGraphicsScene* scene) const
{
    selection = pruned(selection, scene);
    return selection.isEmpty() ? nullptr : selection.first();
}

void LzSceneSelectionWorkbenchController::replaceSelection(QList<QGraphicsItem*>& selection,
                                                           const QList<QGraphicsItem*>& items,
                                                           const QGraphicsScene* scene) const
{
    clearSelection(selection, scene);
    LzSceneSelectionManager manager;
    selection = manager.replaceSelection(items, scene);
}

void LzSceneSelectionWorkbenchController::clearSelection(QList<QGraphicsItem*>& selection,
                                                         const QGraphicsScene* scene) const
{
    const QList<QGraphicsItem*> pending = selection;
    selection.clear();
    LzSceneSelectionManager manager;
    manager.clearSelectionState(pending, scene);
}

bool LzSceneSelectionWorkbenchController::addSelection(QList<QGraphicsItem*>& selection,
                                                       QGraphicsItem* item,
                                                       const QGraphicsScene* scene) const
{
    LzSceneSelectionManager manager;
    return manager.addSelection(selection, item, scene);
}

bool LzSceneSelectionWorkbenchController::removeSelection(QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item,
                                                          const QGraphicsScene* scene) const
{
    LzSceneSelectionManager manager;
    return manager.removeSelection(selection, item, scene);
}

bool LzSceneSelectionWorkbenchController::removeSceneItem(LzScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    LzSceneItemLifecycleController controller;
    return controller.removeSceneItem(scene, selection, item);
}

bool LzSceneSelectionWorkbenchController::insertSelection(LzScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    LzSceneItemLifecycleController controller;
    return controller.insertSelection(scene, selection, item);
}

bool LzSceneSelectionWorkbenchController::deleteSelection(LzScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    LzSceneItemLifecycleController controller;
    return controller.deleteSelection(scene, selection, item);
}

int LzSceneSelectionWorkbenchController::addSelections(LzScene* scene,
                                                       QList<QGraphicsItem*>& selection,
                                                       const QList<QGraphicsItem*>& items) const
{
    LzSceneItemLifecycleController controller;
    return controller.addSelections(scene, selection, items);
}

int LzSceneSelectionWorkbenchController::removeSelections(LzScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          const QList<QGraphicsItem*>& items) const
{
    LzSceneItemLifecycleController controller;
    return controller.removeSelections(scene, selection, items);
}
