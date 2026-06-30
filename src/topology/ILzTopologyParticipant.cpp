#include "ILzTopologyParticipant.h"

#include "item/LzGroup.h"
#include "item/LzItem.h"

ILzTopologyParticipant* lzTopologyParticipantForItem(QGraphicsItem* item)
{
    if (!item)
        return nullptr;
    if (auto* group = dynamic_cast<LzGroup*>(item))
        return group;
    if (auto* lzItem = dynamic_cast<LzItem*>(item))
        return lzItem;
    return nullptr;
}

const ILzTopologyParticipant* lzTopologyParticipantForItem(const QGraphicsItem* item)
{
    return lzTopologyParticipantForItem(const_cast<QGraphicsItem*>(item));
}
