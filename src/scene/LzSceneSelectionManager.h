#ifndef LZSCENESELECTIONMANAGER_H
#define LZSCENESELECTIONMANAGER_H

#include <QGraphicsItem>
#include <QList>

class QGraphicsScene;

class LzSceneSelectionManager
{
public:
    static bool canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene);

    QList<QGraphicsItem*> pruned(const QList<QGraphicsItem*>& selection,
                                 const QGraphicsScene* scene) const;
    QGraphicsItem* firstSelection(const QList<QGraphicsItem*>& selection,
                                  const QGraphicsScene* scene) const;
    QList<QGraphicsItem*> replaceSelection(const QList<QGraphicsItem*>& items,
                                           const QGraphicsScene* scene) const;

    void clearSelectionState(const QList<QGraphicsItem*>& items, const QGraphicsScene* scene) const;
    bool addSelection(QList<QGraphicsItem*>& selection,
                      QGraphicsItem* item,
                      const QGraphicsScene* scene) const;
    bool removeSelection(QList<QGraphicsItem*>& selection,
                         QGraphicsItem* item,
                         const QGraphicsScene* scene) const;
};

#endif
