#ifndef CIMDRAWSCENEINTERACTIONWORKBENCHCONTROLLER_H
#define CIMDRAWSCENEINTERACTIONWORKBENCHCONTROLLER_H

#include <QPoint>
#include <QPointF>

class QAction;
class QGraphicsSceneMouseEvent;
class QMenu;
class QMimeData;
class CimdrawScene;
class CimdrawTool;

class CimdrawSceneInteractionWorkbenchController
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

    CimdrawTool* currentTool() const;

    bool dispatchMouse(CimdrawScene* scene, MouseAction action, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseRelease(CimdrawScene* scene, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseDoubleClick(CimdrawScene* scene, QGraphicsSceneMouseEvent* event) const;

    bool dispatchMenuAction(CimdrawScene* scene, QAction* action) const;
    QAction* dispatchContextMenu(CimdrawScene* scene, QMenu*& menu, const QPoint& screenPos) const;
    bool handleMenuAction(CimdrawScene* scene, QAction* action) const;
    QAction* execContextMenu(CimdrawScene* scene, QMenu*& menu, const QPoint& screenPos) const;

    bool dispatchDrag(CimdrawScene* scene,
                      DragAction action,
                      const QMimeData* mimeData,
                      const QPointF& scenePos = QPointF()) const;
    bool handleDragEnter(CimdrawScene* scene, const QMimeData* mimeData) const;
    bool handleDragMove(const QMimeData* mimeData) const;
    bool handleDrop(CimdrawScene* scene, const QMimeData* mimeData, const QPointF& scenePos) const;
};

#endif
