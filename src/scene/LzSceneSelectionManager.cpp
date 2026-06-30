#include "LzSceneSelectionManager.h"

#include "Item/LzHandle.h"

bool LzSceneSelectionManager::canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene)
{
    if (!item || !scene)
        return false;
    if (item->scene() != scene)
        return false;
    if (lzIsHandle(item))
        return false;
    return true;
}

QList<QGraphicsItem*> LzSceneSelectionManager::pruned(const QList<QGraphicsItem*>& selection,
                                                      const QGraphicsScene* scene) const
{
    QList<QGraphicsItem*> kept;
    kept.reserve(selection.size());
    for (QGraphicsItem* item : selection)
    {
        if (canTouchSelectionItem(item, scene))
            kept.append(item);
    }
    return kept;
}

QGraphicsItem* LzSceneSelectionManager::firstSelection(const QList<QGraphicsItem*>& selection,
                                                       const QGraphicsScene* scene) const
{
    const QList<QGraphicsItem*> kept = pruned(selection, scene);
    return kept.isEmpty() ? nullptr : kept.first();
}

QList<QGraphicsItem*> LzSceneSelectionManager::replaceSelection(const QList<QGraphicsItem*>& items,
                                                                const QGraphicsScene* scene) const
{
    QList<QGraphicsItem*> kept;
    kept.reserve(items.size());
    for (QGraphicsItem* item : items)
    {
        if (!canTouchSelectionItem(item, scene))
            continue;
        if (!kept.contains(item))
        {
            item->setSelected(true);
            kept.append(item);
        }
    }
    return kept;
}

void LzSceneSelectionManager::clearSelectionState(const QList<QGraphicsItem*>& items,
                                                  const QGraphicsScene* scene) const
{
    for (QGraphicsItem* item : items)
    {
        if (!canTouchSelectionItem(item, scene))
            continue;
        if (item->isSelected())
            item->setSelected(false);
    }
}

bool LzSceneSelectionManager::addSelection(QList<QGraphicsItem*>& selection,
                                           QGraphicsItem* item,
                                           const QGraphicsScene* scene) const
{
    if (!canTouchSelectionItem(item, scene))
        return false;
    if (selection.contains(item))
        return false;

    item->setSelected(true);
    selection.append(item);
    return true;
}

bool LzSceneSelectionManager::removeSelection(QList<QGraphicsItem*>& selection,
                                              QGraphicsItem* item,
                                              const QGraphicsScene* scene) const
{
    if (!selection.contains(item))
        return false;
    selection.removeOne(item);
    if (canTouchSelectionItem(item, scene) && item->isSelected())
        item->setSelected(false);
    return true;
}
