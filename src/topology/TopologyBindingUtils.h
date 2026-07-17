#ifndef TOPOLOGYBINDINGUTILS_H
#define TOPOLOGYBINDINGUTILS_H

#include <QString>
#include <QGraphicsItem>

#include "item/CimdrawGroup.h"
#include "item/CimdrawItem.h"
#include "item/TmpBase.h"

inline QString cimdrawTopologyBindingIdForItem(QGraphicsItem* item)
{
    if (!item)
        return {};
    if (auto* li = dynamic_cast<CimdrawItem*>(item))
        return li->topologyNodeStableId();
    if (auto* grp = dynamic_cast<CimdrawGroup*>(item))
        return grp->topologyNodeStableId();
    return {};
}

inline QString cimdrawTopologyBindingIdForShape(TmpShape* shape)
{
    return shape ? shape->topologyNodeStableId() : QString();
}

inline QString cimdrawEnsureTopologyBindingIdForShape(TmpShape* shape)
{
    return shape ? shape->ensureTopologyNodeStableId() : QString();
}

#endif // TOPOLOGYBINDINGUTILS_H
