#include "scene/LzSceneInteractionWorkbenchController.h"

#include "LzScene.h"
#include "Tool/LzTool.h"
#include "scene/LzSceneContextMenuController.h"
#include "scene/LzSceneInteractionController.h"

LzTool* LzSceneInteractionWorkbenchController::currentTool() const
{
    return LzToolManager::getInstance()->getCurrentTool();
}

bool LzSceneInteractionWorkbenchController::dispatchMouse(LzScene* scene,
                                                          MouseAction action,
                                                          QGraphicsSceneMouseEvent* event) const
{
    switch (action)
    {
    case MouseAction::Press:
        return dispatchMousePress(scene, event);
    case MouseAction::Move:
        return dispatchMouseMove(scene, event);
    case MouseAction::Release:
        return dispatchMouseRelease(scene, event);
    case MouseAction::DoubleClick:
        return dispatchMouseDoubleClick(scene, event);
    }

    return false;
}

bool LzSceneInteractionWorkbenchController::dispatchMousePress(LzScene* scene,
                                                               QGraphicsSceneMouseEvent* event) const
{
    LzSceneInteractionController controller;
    return controller.dispatchMousePress(scene, currentTool(), event);
}

bool LzSceneInteractionWorkbenchController::dispatchMouseMove(LzScene* scene,
                                                              QGraphicsSceneMouseEvent* event) const
{
    LzSceneInteractionController controller;
    return controller.dispatchMouseMove(scene, currentTool(), event);
}

bool LzSceneInteractionWorkbenchController::dispatchMouseRelease(LzScene* scene,
                                                                 QGraphicsSceneMouseEvent* event) const
{
    LzSceneInteractionController controller;
    return controller.dispatchMouseRelease(scene, currentTool(), event);
}

bool LzSceneInteractionWorkbenchController::dispatchMouseDoubleClick(LzScene* scene,
                                                                     QGraphicsSceneMouseEvent* event) const
{
    LzSceneInteractionController controller;
    return controller.dispatchMouseDoubleClick(scene, currentTool(), event);
}

bool LzSceneInteractionWorkbenchController::handleMenuAction(LzScene* scene, QAction* action) const
{
    LzSceneInteractionController controller;
    return controller.handleMenuAction(scene, action);
}

bool LzSceneInteractionWorkbenchController::dispatchMenuAction(LzScene* scene, QAction* action) const
{
    return handleMenuAction(scene, action);
}

QAction* LzSceneInteractionWorkbenchController::execContextMenu(LzScene* scene,
                                                                QMenu*& menu,
                                                                const QPoint& screenPos) const
{
    LzSceneContextMenuController controller;
    menu = controller.ensureMenu(menu, scene);
    return controller.execMenu(menu, screenPos);
}

QAction* LzSceneInteractionWorkbenchController::dispatchContextMenu(LzScene* scene,
                                                                    QMenu*& menu,
                                                                    const QPoint& screenPos) const
{
    return execContextMenu(scene, menu, screenPos);
}

bool LzSceneInteractionWorkbenchController::handleDragEnter(LzScene* scene,
                                                            const QMimeData* mimeData) const
{
    LzSceneInteractionController controller;
    return controller.handleDragEnter(scene, mimeData);
}

bool LzSceneInteractionWorkbenchController::handleDragMove(const QMimeData* mimeData) const
{
    LzSceneInteractionController controller;
    return controller.handleDragMove(mimeData);
}

bool LzSceneInteractionWorkbenchController::handleDrop(LzScene* scene,
                                                       const QMimeData* mimeData,
                                                       const QPointF& scenePos) const
{
    LzSceneInteractionController controller;
    return controller.handleDrop(scene, mimeData, scenePos);
}

bool LzSceneInteractionWorkbenchController::dispatchDrag(LzScene* scene,
                                                         DragAction action,
                                                         const QMimeData* mimeData,
                                                         const QPointF& scenePos) const
{
    switch (action)
    {
    case DragAction::Enter:
        return handleDragEnter(scene, mimeData);
    case DragAction::Move:
        return handleDragMove(mimeData);
    case DragAction::Drop:
        return handleDrop(scene, mimeData, scenePos);
    }

    return false;
}
