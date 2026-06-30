#ifndef LZSCENEINTERACTIONWORKBENCHCONTROLLER_H
#define LZSCENEINTERACTIONWORKBENCHCONTROLLER_H

#include <QPoint>
#include <QPointF>

class QAction;
class QGraphicsSceneMouseEvent;
class QMenu;
class QMimeData;
class LzScene;
class LzTool;

class LzSceneInteractionWorkbenchController
{
public:
    enum class MouseAction
    {
        Press,
        Move,
        Release,
        DoubleClick
    };

    enum class DragAction
    {
        Enter,
        Move,
        Drop
    };

    LzTool* currentTool() const;

    bool dispatchMouse(LzScene* scene, MouseAction action, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMousePress(LzScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseDoubleClick(LzScene* scene, QGraphicsSceneMouseEvent* event) const;

    bool dispatchMenuAction(LzScene* scene, QAction* action) const;
    QAction* dispatchContextMenu(LzScene* scene, QMenu*& menu, const QPoint& screenPos) const;
    bool handleMenuAction(LzScene* scene, QAction* action) const;
    QAction* execContextMenu(LzScene* scene, QMenu*& menu, const QPoint& screenPos) const;

    bool dispatchDrag(LzScene* scene,
                      DragAction action,
                      const QMimeData* mimeData,
                      const QPointF& scenePos = QPointF()) const;
    bool handleDragEnter(LzScene* scene, const QMimeData* mimeData) const;
    bool handleDragMove(const QMimeData* mimeData) const;
    bool handleDrop(LzScene* scene, const QMimeData* mimeData, const QPointF& scenePos) const;
};

#endif
