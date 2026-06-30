#ifndef LZSCENESELECTIONWORKBENCHCONTROLLER_H
#define LZSCENESELECTIONWORKBENCHCONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class QGraphicsScene;
class LzScene;

class LzSceneSelectionWorkbenchController
{
public:
    QList<QGraphicsItem*> pruned(const QList<QGraphicsItem*>& selection, const QGraphicsScene* scene) const;
    QGraphicsItem* firstSelection(QList<QGraphicsItem*>& selection, const QGraphicsScene* scene) const;
    void replaceSelection(QList<QGraphicsItem*>& selection,
                          const QList<QGraphicsItem*>& items,
                          const QGraphicsScene* scene) const;
    void clearSelection(QList<QGraphicsItem*>& selection, const QGraphicsScene* scene) const;
    bool addSelection(QList<QGraphicsItem*>& selection, QGraphicsItem* item, const QGraphicsScene* scene) const;
    bool removeSelection(QList<QGraphicsItem*>& selection, QGraphicsItem* item, const QGraphicsScene* scene) const;

    bool removeSceneItem(LzScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    bool insertSelection(LzScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    bool deleteSelection(LzScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    int addSelections(LzScene* scene, QList<QGraphicsItem*>& selection, const QList<QGraphicsItem*>& items) const;
    int removeSelections(LzScene* scene, QList<QGraphicsItem*>& selection, const QList<QGraphicsItem*>& items) const;
};

#endif
