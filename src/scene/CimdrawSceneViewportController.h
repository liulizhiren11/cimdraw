#ifndef CIMDRAWSCENEVIEWPORTCONTROLLER_H
#define CIMDRAWSCENEVIEWPORTCONTROLLER_H

#include <QRectF>

class QGraphicsSceneMouseEvent;
class CimdrawScene;

class CimdrawSceneViewportController
{
public:
    QRectF contentsRect(CimdrawScene* scene) const;
    bool dispatchCompatMouseEvent(CimdrawScene* scene, QGraphicsSceneMouseEvent* event) const;
};

#endif
