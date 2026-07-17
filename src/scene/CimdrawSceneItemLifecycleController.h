#ifndef CIMDRAWSCENEITEMLIFECYCLECONTROLLER_H
#define CIMDRAWSCENEITEMLIFECYCLECONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class CimdrawScene;

class CimdrawSceneItemLifecycleController
{
public:
    bool removeSceneItem(CimdrawScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;
    bool insertSelection(CimdrawScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;
    bool deleteSelection(CimdrawScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;

    int addSelections(CimdrawScene* scene,
                      QList<QGraphicsItem*>& selection,
                      const QList<QGraphicsItem*>& items) const;
    int removeSelections(CimdrawScene* scene,
                         QList<QGraphicsItem*>& selection,
                         const QList<QGraphicsItem*>& items) const;
};

#endif
