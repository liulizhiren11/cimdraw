#ifndef CIMDRAWSCENESELECTIONWORKBENCHCONTROLLER_H
#define CIMDRAWSCENESELECTIONWORKBENCHCONTROLLER_H

#include <QGraphicsItem>
#include <QList>

class QGraphicsScene;
class CimdrawScene;

class CimdrawSceneSelectionWorkbenchController
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

    bool removeSceneItem(CimdrawScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    bool insertSelection(CimdrawScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    bool deleteSelection(CimdrawScene* scene, QList<QGraphicsItem*>& selection, QGraphicsItem* item) const;
    int addSelections(CimdrawScene* scene, QList<QGraphicsItem*>& selection, const QList<QGraphicsItem*>& items) const;
    int removeSelections(CimdrawScene* scene, QList<QGraphicsItem*>& selection, const QList<QGraphicsItem*>& items) const;
};

#endif
