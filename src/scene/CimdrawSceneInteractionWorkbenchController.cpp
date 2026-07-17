#include "scene/CimdrawSceneInteractionWorkbenchController.h"

#include "CimdrawScene.h"
#include "Tool/CimdrawTool.h"
#include "scene/CimdrawSceneContextMenuController.h"
#include "scene/CimdrawSceneInteractionController.h"

CimdrawTool* CimdrawSceneInteractionWorkbenchController::currentTool() const
{
    return CimdrawToolManager::getInstance()->getCurrentTool();
}

bool CimdrawSceneInteractionWorkbenchController::dispatchMouse(CimdrawScene* scene,
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

bool CimdrawSceneInteractionWorkbenchController::dispatchMousePress(CimdrawScene* scene,
                                                               QGraphicsSceneMouseEvent* event) const
{
    CimdrawSceneInteractionController controller;
    return controller.dispatchMousePress(scene, currentTool(), event);
}

bool CimdrawSceneInteractionWorkbenchController::dispatchMouseMove(CimdrawScene* scene,
                                                              QGraphicsSceneMouseEvent* event) const
{
    CimdrawSceneInteractionController controller;
    return controller.dispatchMouseMove(scene, currentTool(), event);
}

bool CimdrawSceneInteractionWorkbenchController::dispatchMouseRelease(CimdrawScene* scene,
                                                                 QGraphicsSceneMouseEvent* event) const
{
    CimdrawSceneInteractionController controller;
    return controller.dispatchMouseRelease(scene, currentTool(), event);
}

bool CimdrawSceneInteractionWorkbenchController::dispatchMouseDoubleClick(CimdrawScene* scene,
                                                                     QGraphicsSceneMouseEvent* event) const
{
    CimdrawSceneInteractionController controller;
    return controller.dispatchMouseDoubleClick(scene, currentTool(), event);
}

bool CimdrawSceneInteractionWorkbenchController::handleMenuAction(CimdrawScene* scene, QAction* action) const
{
    CimdrawSceneInteractionController controller;
    return controller.handleMenuAction(scene, action);
}

bool CimdrawSceneInteractionWorkbenchController::dispatchMenuAction(CimdrawScene* scene, QAction* action) const
{
    return handleMenuAction(scene, action);
}

QAction* CimdrawSceneInteractionWorkbenchController::execContextMenu(CimdrawScene* scene,
                                                                QMenu*& menu,
                                                                const QPoint& screenPos) const
{
    CimdrawSceneContextMenuController controller;
    menu = controller.ensureMenu(menu, scene);
    return controller.execMenu(menu, screenPos);
}

QAction* CimdrawSceneInteractionWorkbenchController::dispatchContextMenu(CimdrawScene* scene,
                                                                    QMenu*& menu,
                                                                    const QPoint& screenPos) const
{
    return execContextMenu(scene, menu, screenPos);
}

bool CimdrawSceneInteractionWorkbenchController::handleDragEnter(CimdrawScene* scene,
                                                            const QMimeData* mimeData) const
{
    CimdrawSceneInteractionController controller;
    return controller.handleDragEnter(scene, mimeData);
}

bool CimdrawSceneInteractionWorkbenchController::handleDragMove(const QMimeData* mimeData) const
{
    CimdrawSceneInteractionController controller;
    return controller.handleDragMove(mimeData);
}

bool CimdrawSceneInteractionWorkbenchController::handleDrop(CimdrawScene* scene,
                                                       const QMimeData* mimeData,
                                                       const QPointF& scenePos) const
{
    CimdrawSceneInteractionController controller;
    return controller.handleDrop(scene, mimeData, scenePos);
}

bool CimdrawSceneInteractionWorkbenchController::dispatchDrag(CimdrawScene* scene,
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
