#include "scene/CimdrawSceneSelectionSlotWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneSelectionWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneSelection(CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

QGraphicsItem* CimdrawSceneSelectionSlotWorkbenchController::firstSelection(CimdrawScene* scene) const
{
    return runSceneSelection(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.firstSelection(currentScene->selectionStorage(), currentScene);
    }, []() -> QGraphicsItem*
    {
        return nullptr;
    });
}

QList<QGraphicsItem*> CimdrawSceneSelectionSlotWorkbenchController::selections(CimdrawScene* scene) const
{
    return runSceneSelection(scene, [](CimdrawScene* currentScene)
    {
        currentScene->pruneSelectionList();
        return currentScene->selectionStorage();
    }, []()
    {
        return QList<QGraphicsItem*>();
    });
}

QGraphicsItem* CimdrawSceneSelectionSlotWorkbenchController::dispatch(CimdrawScene* scene, QueryItemAction action) const
{
    switch (action)
    {
    case QueryItemAction::FirstSelection:
        return firstSelection(scene);
    }

    return nullptr;
}

QList<QGraphicsItem*> CimdrawSceneSelectionSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                                        QueryItemsAction action) const
{
    switch (action)
    {
    case QueryItemsAction::Selections:
        return selections(scene);
    }

    return QList<QGraphicsItem*>();
}

bool CimdrawSceneSelectionSlotWorkbenchController::dispatch(CimdrawScene* scene, SelectionAction action) const
{
    switch (action)
    {
    case SelectionAction::Prune:
        pruneSelectionList(scene);
        return scene != nullptr;
    case SelectionAction::Clear:
        return clearSelection(scene);
    }

    return false;
}

bool CimdrawSceneSelectionSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                       ItemSelectionAction action,
                                                       QGraphicsItem* item) const
{
    switch (action)
    {
    case ItemSelectionAction::AddSelection:
        return addSelection(scene, item);
    case ItemSelectionAction::RemoveSelection:
        return removeSelection(scene, item);
    case ItemSelectionAction::RemoveSceneItem:
        return removeSceneItem(scene, item);
    case ItemSelectionAction::InsertSelection:
        return insertSelection(scene, item);
    case ItemSelectionAction::DeleteSelection:
        return deleteSelection(scene, item);
    }

    return false;
}

int CimdrawSceneSelectionSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                      ItemsSelectionAction action,
                                                      const QList<QGraphicsItem*>& items) const
{
    switch (action)
    {
    case ItemsSelectionAction::ReplaceSelection:
        return replaceSelection(scene, items) ? 1 : 0;
    case ItemsSelectionAction::AddSelections:
        return addSelections(scene, items);
    case ItemsSelectionAction::RemoveSelections:
        return removeSelections(scene, items);
    }

    return 0;
}

void CimdrawSceneSelectionSlotWorkbenchController::pruneSelectionList(CimdrawScene* scene) const
{
    runSceneSelection(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        QList<QGraphicsItem*>& selection = currentScene->selectionStorage();
        selection = controller.pruned(selection, currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::replaceSelection(CimdrawScene* scene,
                                                               const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        controller.replaceSelection(currentScene->selectionStorage(), items, currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::clearSelection(CimdrawScene* scene) const
{
    return runSceneSelection(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        controller.clearSelection(currentScene->selectionStorage(), currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::addSelection(CimdrawScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.addSelection(currentScene->selectionStorage(), item, currentScene);
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::removeSelection(CimdrawScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.removeSelection(currentScene->selectionStorage(), item, currentScene);
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::removeSceneItem(CimdrawScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.removeSceneItem(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::insertSelection(CimdrawScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.insertSelection(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneSelectionSlotWorkbenchController::deleteSelection(CimdrawScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.deleteSelection(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

int CimdrawSceneSelectionSlotWorkbenchController::addSelections(CimdrawScene* scene,
                                                           const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.addSelections(currentScene, currentScene->selectionStorage(), items);
    }, []()
    {
        return 0;
    });
}

int CimdrawSceneSelectionSlotWorkbenchController::removeSelections(CimdrawScene* scene,
                                                              const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](CimdrawScene* currentScene)
    {
        CimdrawSceneSelectionWorkbenchController controller;
        return controller.removeSelections(currentScene, currentScene->selectionStorage(), items);
    }, []()
    {
        return 0;
    });
}
