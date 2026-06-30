#ifndef LZSCENEVIEWPORTCONTROLLER_H
#define LZSCENEVIEWPORTCONTROLLER_H

#include <QRectF>

class QGraphicsSceneMouseEvent;
class LzScene;

class LzSceneViewportController
{
public:
    QRectF contentsRect(LzScene* scene) const;
    bool dispatchCompatMouseEvent(LzScene* scene, QGraphicsSceneMouseEvent* event) const;
};

#endif
