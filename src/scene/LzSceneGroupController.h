#ifndef LZSCENEGROUPCONTROLLER_H
#define LZSCENEGROUPCONTROLLER_H

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QList>

class LzScene;

class LzSceneGroupController
{
public:
    QGraphicsItemGroup* createGroup(LzScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
