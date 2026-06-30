#ifndef LZSCENEITEMLIFECYCLECONTROLLER_H
#define LZSCENEITEMLIFECYCLECONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class LzScene;

class LzSceneItemLifecycleController
{
public:
    bool removeSceneItem(LzScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;
    bool insertSelection(LzScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;
    bool deleteSelection(LzScene* scene,
                         QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item) const;

    int addSelections(LzScene* scene,
                      QList<QGraphicsItem*>& selection,
                      const QList<QGraphicsItem*>& items) const;
    int removeSelections(LzScene* scene,
                         QList<QGraphicsItem*>& selection,
                         const QList<QGraphicsItem*>& items) const;
};

#endif
