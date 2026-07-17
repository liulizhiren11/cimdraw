#include "cim/query/CimGraphicRenderStateQuery.h"

#include "CimdrawScene.h"
#include "Item/TmpBase.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"
#include "wiring/base/CimdrawWiringItemBase.h"

CimGraphicRenderStateSource CimGraphicRenderStateQuery::sourceForItem(QGraphicsItem* item) const
{
    if (const auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(qgraphicsitem_cast<TmpShape*>(item)))
        return wiring->graphicRenderStateSource();
    return {};
}

CimGraphicRenderStateSource CimGraphicRenderStateQuery::sourceForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery objectQuery;
    return sourceForItem(objectQuery.sceneItemByMrid(mrid, scene, queryState.queryContext));
}

CimGraphicRenderStateSource CimGraphicRenderStateQuery::selectedSource(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    Q_UNUSED(queryState);
    if (!scene)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
        return {};

    return sourceForItem(selections.first());
}

CimGraphicRenderState CimGraphicRenderStateQuery::resultForSource(
    const CimGraphicRenderStateSource& source,
    bool alarmPulse) const
{
    return cimGraphicRenderStateFromSource(source, alarmPulse);
}

CimGraphicRenderState CimGraphicRenderStateQuery::resultForItem(QGraphicsItem* item) const
{
    const auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(qgraphicsitem_cast<TmpShape*>(item));
    if (!wiring)
        return {};

    const CimGraphicRenderStateSource source = sourceForItem(item);
    if (source.valid)
        return resultForSource(source, wiring->effectiveAlarmPulse());
    return {};
}

CimGraphicRenderState CimGraphicRenderStateQuery::resultForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    const CimGraphicRenderStateSource source = sourceForMrid(mrid, scene, queryState);
    if (!source.valid)
        return {};

    CimGraphicObjectSummaryQuery objectQuery;
    const auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(
        qgraphicsitem_cast<TmpShape*>(objectQuery.sceneItemByMrid(mrid, scene, queryState.queryContext)));
    return resultForSource(source, wiring ? wiring->effectiveAlarmPulse() : false);
}

CimGraphicRenderState CimGraphicRenderStateQuery::selectedResult(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    const CimGraphicRenderStateSource source = selectedSource(scene, queryState);
    if (!source.valid)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    const auto* wiring = selections.count() == 1
        ? dynamic_cast<CimdrawWiringItemBase*>(qgraphicsitem_cast<TmpShape*>(selections.first()))
        : nullptr;
    return resultForSource(source, wiring ? wiring->effectiveAlarmPulse() : false);
}
