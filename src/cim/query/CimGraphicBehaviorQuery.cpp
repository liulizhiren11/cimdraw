#include "cim/query/CimGraphicBehaviorQuery.h"

#include "CimdrawScene.h"
#include "cim/behavior/CimBehaviorQuery.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"

CimBehaviorResult CimGraphicBehaviorQuery::resultForItem(QGraphicsItem* item) const
{
    CimBehaviorQuery query;
    return query.resultForItem(item);
}

CimBehaviorResult CimGraphicBehaviorQuery::resultForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery objectQuery;
    return resultForItem(objectQuery.sceneItemByMrid(mrid, scene, queryState.queryContext));
}

CimBehaviorResult CimGraphicBehaviorQuery::selectedResult(CimdrawScene* scene,
                                                          const CimGraphicQueryState& queryState) const
{
    Q_UNUSED(queryState);
    if (!scene)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
        return {};

    return resultForItem(selections.first());
}

CimBehaviorResult CimGraphicBehaviorQuery::resultForWiringKey(CimdrawScene* scene,
                                                              const QString& wiringDataKey) const
{
    if (!scene || wiringDataKey.trimmed().isEmpty())
        return {};
    return scene->wiringBehaviorResult(wiringDataKey);
}
