#ifndef LZSCENEINTERACTIONCONTROLLER_H
#define LZSCENEINTERACTIONCONTROLLER_H

#include <QPointF>

class QAction;
class QGraphicsSceneMouseEvent;
class QMimeData;
class LzScene;
class LzTool;

class LzSceneInteractionController
{
public:
    bool dispatchMousePress(LzScene* scene, LzTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseMove(LzScene* scene, LzTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseRelease(LzScene* scene, LzTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseDoubleClick(LzScene* scene, LzTool* tool, QGraphicsSceneMouseEvent* event) const;

    bool handleMenuAction(LzScene* scene, QAction* action) const;
    bool handleDragEnter(LzScene* scene, const QMimeData* mimeData) const;
    bool handleDragMove(const QMimeData* mimeData) const;
    bool handleDrop(LzScene* scene, const QMimeData* mimeData, const QPointF& scenePos) const;
    bool acceptsTextDrop(const QMimeData* mimeData) const;
    bool handleTextDrop(LzScene* scene, const QString& text, const QPointF& scenePos) const;
};

#endif
