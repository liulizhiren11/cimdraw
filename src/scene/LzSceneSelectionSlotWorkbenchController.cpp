#include "scene/LzSceneSelectionSlotWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneSelectionWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneSelection(LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

QGraphicsItem* LzSceneSelectionSlotWorkbenchController::firstSelection(LzScene* scene) const
{
    return runSceneSelection(scene, [](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.firstSelection(currentScene->selectionStorage(), currentScene);
    }, []() -> QGraphicsItem*
    {
        return nullptr;
    });
}

QList<QGraphicsItem*> LzSceneSelectionSlotWorkbenchController::selections(LzScene* scene) const
{
    return runSceneSelection(scene, [](LzScene* currentScene)
    {
        currentScene->pruneSelectionList();
        return currentScene->selectionStorage();
    }, []()
    {
        return QList<QGraphicsItem*>();
    });
}

QGraphicsItem* LzSceneSelectionSlotWorkbenchController::dispatch(LzScene* scene, QueryItemAction action) const
{
    switch (action)
    {
    case QueryItemAction::FirstSelection:
        return firstSelection(scene);
    }

    return nullptr;
}

QList<QGraphicsItem*> LzSceneSelectionSlotWorkbenchController::dispatch(LzScene* scene,
                                                                        QueryItemsAction action) const
{
    switch (action)
    {
    case QueryItemsAction::Selections:
        return selections(scene);
    }

    return QList<QGraphicsItem*>();
}

bool LzSceneSelectionSlotWorkbenchController::dispatch(LzScene* scene, SelectionAction action) const
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

bool LzSceneSelectionSlotWorkbenchController::dispatch(LzScene* scene,
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

int LzSceneSelectionSlotWorkbenchController::dispatch(LzScene* scene,
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

void LzSceneSelectionSlotWorkbenchController::pruneSelectionList(LzScene* scene) const
{
    runSceneSelection(scene, [](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        QList<QGraphicsItem*>& selection = currentScene->selectionStorage();
        selection = controller.pruned(selection, currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::replaceSelection(LzScene* scene,
                                                               const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        controller.replaceSelection(currentScene->selectionStorage(), items, currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::clearSelection(LzScene* scene) const
{
    return runSceneSelection(scene, [](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        controller.clearSelection(currentScene->selectionStorage(), currentScene);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::addSelection(LzScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.addSelection(currentScene->selectionStorage(), item, currentScene);
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::removeSelection(LzScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.removeSelection(currentScene->selectionStorage(), item, currentScene);
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::removeSceneItem(LzScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.removeSceneItem(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::insertSelection(LzScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.insertSelection(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

bool LzSceneSelectionSlotWorkbenchController::deleteSelection(LzScene* scene, QGraphicsItem* item) const
{
    return runSceneSelection(scene, [item](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.deleteSelection(currentScene, currentScene->selectionStorage(), item);
    }, []()
    {
        return false;
    });
}

int LzSceneSelectionSlotWorkbenchController::addSelections(LzScene* scene,
                                                           const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.addSelections(currentScene, currentScene->selectionStorage(), items);
    }, []()
    {
        return 0;
    });
}

int LzSceneSelectionSlotWorkbenchController::removeSelections(LzScene* scene,
                                                              const QList<QGraphicsItem*>& items) const
{
    return runSceneSelection(scene, [&items](LzScene* currentScene)
    {
        LzSceneSelectionWorkbenchController controller;
        return controller.removeSelections(currentScene, currentScene->selectionStorage(), items);
    }, []()
    {
        return 0;
    });
}
