#ifndef CIMDRAWSCENEGROUPCONTROLLER_H
#define CIMDRAWSCENEGROUPCONTROLLER_H

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QList>

class CimdrawScene;

class CimdrawSceneGroupController
{
public:
    QGraphicsItemGroup* createGroup(CimdrawScene* scene, const QList<QGraphicsItem*>& items) const;
};

#endif
