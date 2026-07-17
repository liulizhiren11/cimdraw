#include "CimdrawSceneSelectionManager.h"

#include "Item/CimdrawHandle.h"

bool CimdrawSceneSelectionManager::canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene)
{
    if (!item || !scene)
        return false;
    if (item->scene() != scene)
        return false;
    if (cimdrawIsHandle(item))
        return false;
    return true;
}

QList<QGraphicsItem*> CimdrawSceneSelectionManager::pruned(const QList<QGraphicsItem*>& selection,
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

QGraphicsItem* CimdrawSceneSelectionManager::firstSelection(const QList<QGraphicsItem*>& selection,
                                                       const QGraphicsScene* scene) const
{
    const QList<QGraphicsItem*> kept = pruned(selection, scene);
    return kept.isEmpty() ? nullptr : kept.first();
}

QList<QGraphicsItem*> CimdrawSceneSelectionManager::replaceSelection(const QList<QGraphicsItem*>& items,
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

void CimdrawSceneSelectionManager::clearSelectionState(const QList<QGraphicsItem*>& items,
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

bool CimdrawSceneSelectionManager::addSelection(QList<QGraphicsItem*>& selection,
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

bool CimdrawSceneSelectionManager::removeSelection(QList<QGraphicsItem*>& selection,
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
