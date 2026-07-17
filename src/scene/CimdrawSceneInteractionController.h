#ifndef CIMDRAWSCENEINTERACTIONCONTROLLER_H
#define CIMDRAWSCENEINTERACTIONCONTROLLER_H

#include <QPointF>

class QAction;
class QGraphicsSceneMouseEvent;
class QMimeData;
class CimdrawScene;
class CimdrawTool;

class CimdrawSceneInteractionController
{
public:
    bool dispatchMousePress(CimdrawScene* scene, CimdrawTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseMove(CimdrawScene* scene, CimdrawTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseRelease(CimdrawScene* scene, CimdrawTool* tool, QGraphicsSceneMouseEvent* event) const;
    bool dispatchMouseDoubleClick(CimdrawScene* scene, CimdrawTool* tool, QGraphicsSceneMouseEvent* event) const;

    bool handleMenuAction(CimdrawScene* scene, QAction* action) const;
    bool handleDragEnter(CimdrawScene* scene, const QMimeData* mimeData) const;
    bool handleDragMove(const QMimeData* mimeData) const;
    bool handleDrop(CimdrawScene* scene, const QMimeData* mimeData, const QPointF& scenePos) const;
    bool acceptsTextDrop(const QMimeData* mimeData) const;
    bool handleTextDrop(CimdrawScene* scene, const QString& text, const QPointF& scenePos) const;
};

#endif
