#include "ICimdrawTopologyParticipant.h"

#include "item/CimdrawGroup.h"
#include "item/CimdrawItem.h"

ICimdrawTopologyParticipant* cimdrawTopologyParticipantForItem(QGraphicsItem* item)
{
    if (!item)
        return nullptr;
    if (auto* group = dynamic_cast<CimdrawGroup*>(item))
        return group;
    if (auto* cimdrawItem = dynamic_cast<CimdrawItem*>(item))
        return cimdrawItem;
    return nullptr;
}

const ICimdrawTopologyParticipant* cimdrawTopologyParticipantForItem(const QGraphicsItem* item)
{
    return cimdrawTopologyParticipantForItem(const_cast<QGraphicsItem*>(item));
}
