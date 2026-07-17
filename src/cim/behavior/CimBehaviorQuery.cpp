#include "cim/behavior/CimBehaviorQuery.h"

#include "item/TmpBase.h"
#include "wiring/base/CimdrawWiringItemBase.h"

CimBehaviorResult CimBehaviorQuery::resultForItem(QGraphicsItem* item) const
{
    CimBehaviorResult result;
    if (!item)
        return result;

    if (auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(item))
    {
        return wiring->effectiveBehaviorResult();
    }

    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
    {
        result.objectId = shape->cimdrawObjectId();
        result.valid = shape->participatesInTopology();
        result.available = shape->isVisible() && shape->participatesInTopology();
        result.conductivity = shape->participatesInTopology();
        result.terminalConnectivity = shape->participatesInTopology()
            ? CimBehaviorTerminalConnectivity::Connected
            : CimBehaviorTerminalConnectivity::Unknown;
    }

    return result;
}
