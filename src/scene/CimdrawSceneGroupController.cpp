#include "CimdrawSceneGroupController.h"

#include "Item/CimdrawGroup.h"
#include "CimdrawScene.h"

QGraphicsItemGroup* CimdrawSceneGroupController::createGroup(CimdrawScene* scene,
                                                        const QList<QGraphicsItem*>& items) const
{
    if (!scene || items.isEmpty())
        return nullptr;

    QList<QGraphicsItem*> ancestors;
    int n = 0;
    if (!items.isEmpty())
    {
        QGraphicsItem* parent = items.at(n++);
        while ((parent = parent->parentItem()))
            ancestors.append(parent);
    }

    QGraphicsItem* commonAncestor = nullptr;
    if (!ancestors.isEmpty())
    {
        while (n < items.size())
        {
            int commonIndex = -1;
            QGraphicsItem* parent = items.at(n++);
            do
            {
                const int index = ancestors.indexOf(parent, qMax(0, commonIndex));
                if (index != -1)
                {
                    commonIndex = index;
                    break;
                }
            } while ((parent = parent->parentItem()));

            if (commonIndex == -1)
            {
                commonAncestor = nullptr;
                break;
            }
            commonAncestor = ancestors.at(commonIndex);
        }
    }

    auto* group = new CimdrawGroup(commonAncestor);
    group->setPreserveLinePathsDuringGeometryUpdate(true);

    QVector<CimdrawConnectLinePathSnapshot> lineSnapshots;
    CimdrawGroup::collectConnectLineSnapshots(items, lineSnapshots);

    for (QGraphicsItem* item : items)
    {
        if (!item)
            continue;
        item->setSelected(false);
        auto* parentGroup = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
        if (!parentGroup)
            group->addToGroup(item);
    }

    group->syncItemBoundsFromChildren();
    group->updateCoordinate();
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    CimdrawGroup::reattachConnectLinesAmongItems(items);
    CimdrawGroup::refreshConnectionsForItemTree(group, true);
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    group->setPreserveLinePathsDuringGeometryUpdate(false);
    return group;
}
