#include "scene/CimdrawSceneSelectionWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneItemLifecycleController.h"
#include "scene/CimdrawSceneSelectionManager.h"

QList<QGraphicsItem*> CimdrawSceneSelectionWorkbenchController::pruned(const QList<QGraphicsItem*>& selection,
                                                                  const QGraphicsScene* scene) const
{
    CimdrawSceneSelectionManager manager;
    return manager.pruned(selection, scene);
}

QGraphicsItem* CimdrawSceneSelectionWorkbenchController::firstSelection(QList<QGraphicsItem*>& selection,
                                                                   const QGraphicsScene* scene) const
{
    selection = pruned(selection, scene);
    return selection.isEmpty() ? nullptr : selection.first();
}

void CimdrawSceneSelectionWorkbenchController::replaceSelection(QList<QGraphicsItem*>& selection,
                                                           const QList<QGraphicsItem*>& items,
                                                           const QGraphicsScene* scene) const
{
    clearSelection(selection, scene);
    CimdrawSceneSelectionManager manager;
    selection = manager.replaceSelection(items, scene);
}

void CimdrawSceneSelectionWorkbenchController::clearSelection(QList<QGraphicsItem*>& selection,
                                                         const QGraphicsScene* scene) const
{
    const QList<QGraphicsItem*> pending = selection;
    selection.clear();
    CimdrawSceneSelectionManager manager;
    manager.clearSelectionState(pending, scene);
}

bool CimdrawSceneSelectionWorkbenchController::addSelection(QList<QGraphicsItem*>& selection,
                                                       QGraphicsItem* item,
                                                       const QGraphicsScene* scene) const
{
    CimdrawSceneSelectionManager manager;
    return manager.addSelection(selection, item, scene);
}

bool CimdrawSceneSelectionWorkbenchController::removeSelection(QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item,
                                                          const QGraphicsScene* scene) const
{
    CimdrawSceneSelectionManager manager;
    return manager.removeSelection(selection, item, scene);
}

bool CimdrawSceneSelectionWorkbenchController::removeSceneItem(CimdrawScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    CimdrawSceneItemLifecycleController controller;
    return controller.removeSceneItem(scene, selection, item);
}

bool CimdrawSceneSelectionWorkbenchController::insertSelection(CimdrawScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    CimdrawSceneItemLifecycleController controller;
    return controller.insertSelection(scene, selection, item);
}

bool CimdrawSceneSelectionWorkbenchController::deleteSelection(CimdrawScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          QGraphicsItem* item) const
{
    CimdrawSceneItemLifecycleController controller;
    return controller.deleteSelection(scene, selection, item);
}

int CimdrawSceneSelectionWorkbenchController::addSelections(CimdrawScene* scene,
                                                       QList<QGraphicsItem*>& selection,
                                                       const QList<QGraphicsItem*>& items) const
{
    CimdrawSceneItemLifecycleController controller;
    return controller.addSelections(scene, selection, items);
}

int CimdrawSceneSelectionWorkbenchController::removeSelections(CimdrawScene* scene,
                                                          QList<QGraphicsItem*>& selection,
                                                          const QList<QGraphicsItem*>& items) const
{
    CimdrawSceneItemLifecycleController controller;
    return controller.removeSelections(scene, selection, items);
}
