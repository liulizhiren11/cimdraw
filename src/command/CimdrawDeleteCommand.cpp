#include "CimdrawDeleteCommand.h"
#include "CimdrawScene.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawConnectPoint.h"
#include "item/CimdrawGroup.h"
#include "item/CimdrawItem.h"
#include "item/TmpBase.h"

#include <QHash>
#include <QSet>

namespace {

QString stableNodeIdForDeleteRestore(QGraphicsItem* item)
{
    if (!item)
        return {};
    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        return shape->ensureCimdrawObjectId();
    return {};
}

int connectPointOrdinalForDeleteRestore(QGraphicsItem* item, CimdrawConnectPoint* port)
{
    if (!item || !port)
        return -1;
    int ordinal = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* candidate = qgraphicsitem_cast<CimdrawConnectPoint*>(child))
        {
            if (candidate == port)
                return ordinal;
            ++ordinal;
        }
    }
    return -1;
}

CimdrawConnectPoint* connectPointByOrdinalForDeleteRestore(QGraphicsItem* item, int ordinal)
{
    if (!item || ordinal < 0)
        return nullptr;
    int current = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* port = qgraphicsitem_cast<CimdrawConnectPoint*>(child))
        {
            if (current == ordinal)
                return port;
            ++current;
        }
    }
    return nullptr;
}

struct DeletedLineEndpointSnapshot
{
    QString startNodeId;
    QString endNodeId;
    int startPortOrdinal = -1;
    int endPortOrdinal = -1;
};

DeletedLineEndpointSnapshot captureLineSnapshotForDeleteRestore(CimdrawConnectLine* line)
{
    DeletedLineEndpointSnapshot snapshot;
    if (!line)
        return snapshot;
    snapshot.startNodeId = stableNodeIdForDeleteRestore(line->getStartItem());
    snapshot.endNodeId = stableNodeIdForDeleteRestore(line->getEndItem());
    snapshot.startPortOrdinal =
        connectPointOrdinalForDeleteRestore(line->getStartItem(), line->startConnectPort());
    snapshot.endPortOrdinal =
        connectPointOrdinalForDeleteRestore(line->getEndItem(), line->endConnectPort());
    return snapshot;
}

}

class CimdrawDeleteCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawDeleteCommand)
public:
    CimdrawDeleteCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawDeleteCommand* command)
        :q_ptr(command),scene(scene)
    {
        if(scene)
        {
            this->items = items;
            QSet<CimdrawConnectLine*> seenLines;
            for (QGraphicsItem* item : std::as_const(this->items))
            {
                auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
                if (line && !seenLines.contains(line))
                {
                    seenLines.insert(line);
                    deletedLineSnapshots.insert(line, captureLineSnapshotForDeleteRestore(line));
                }

                auto* shape = qgraphicsitem_cast<TmpShape*>(item);
                if (!shape)
                    continue;
                const QVector<QGraphicsItem*> connected = shape->connectedItems();
                for (QGraphicsItem* connectedItem : connected)
                {
                    auto* connectedLine = qgraphicsitem_cast<CimdrawConnectLine*>(connectedItem);
                    if (!connectedLine || seenLines.contains(connectedLine))
                        continue;
                    seenLines.insert(connectedLine);
                    deletedLineSnapshots.insert(connectedLine,
                                                captureLineSnapshotForDeleteRestore(connectedLine));
                }
            }
        }
    }
    QList<QGraphicsItem*> items;
    CimdrawScene* scene;
    CimdrawDeleteCommand* q_ptr;
    QHash<CimdrawConnectLine*, DeletedLineEndpointSnapshot> deletedLineSnapshots;
};

CimdrawDeleteCommand::CimdrawDeleteCommand(QList<QGraphicsItem*> items, CimdrawScene* scene,QUndoCommand* parent)
    :d_ptr(new CimdrawDeleteCommandPrivate(items, scene, this))
{

}

void CimdrawDeleteCommand::undo()
{
    QHash<QString, QGraphicsItem*> stableIdToItem;
    for(auto item:d_ptr->items)
    {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                d_ptr->scene->insertSelection(shape);
                const QString stableId = stableNodeIdForDeleteRestore(shape);
                if (!stableId.isEmpty())
                    stableIdToItem.insert(stableId, shape);
            }
        }
    }

    for (auto it = d_ptr->deletedLineSnapshots.constBegin(); it != d_ptr->deletedLineSnapshots.constEnd(); ++it)
    {
        CimdrawConnectLine* line = it.key();
        if (!line || line->scene() != d_ptr->scene)
            continue;
        const DeletedLineEndpointSnapshot& snapshot = it.value();
        if (!line->getStartItem() && !snapshot.startNodeId.isEmpty())
        {
            if (QGraphicsItem* startItem = stableIdToItem.value(snapshot.startNodeId, nullptr))
            {
                line->setStartItem(startItem);
                line->setStartConnectPort(
                    connectPointByOrdinalForDeleteRestore(startItem, snapshot.startPortOrdinal));
            }
        }
        if (!line->getEndItem() && !snapshot.endNodeId.isEmpty())
        {
            if (QGraphicsItem* endItem = stableIdToItem.value(snapshot.endNodeId, nullptr))
            {
                line->setEndItem(endItem);
                line->setEndConnectPort(
                    connectPointByOrdinalForDeleteRestore(endItem, snapshot.endPortOrdinal));
            }
        }
    }
}

void CimdrawDeleteCommand::redo()
{
    for(auto item:d_ptr->items)
    {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                d_ptr->scene->deleteSelection(shape);
            }
        }
    }
}
