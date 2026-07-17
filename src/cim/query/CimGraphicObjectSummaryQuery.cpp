#include "cim/query/CimGraphicObjectSummaryQuery.h"

#include <QObject>

#include "CimdrawScene.h"
#include "Item/TmpBase.h"
#include "cim/behavior/CimBehaviorQuery.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/query/CimGraphicRenderStateQuery.h"
#include "cim/ui/CimSceneBuilder.h"
#include "wiring/base/CimdrawWiringItemBase.h"

namespace {

QGraphicsItem* findSceneItemByGraphicItemId(CimdrawScene* scene, const QString& graphicItemId)
{
    if (!scene || graphicItemId.isEmpty())
        return nullptr;

    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items)
    {
        if (item && item->data(CimSceneBuilder::CimGraphicItemIdDataKey).toString() == graphicItemId)
            return item;
    }
    return nullptr;
}

QString cimClassNameForGraphicClassName(const QString& graphicClassName)
{
    if (graphicClassName == QStringLiteral("CimdrawPowerCircuitBreakerItem"))
        return QStringLiteral("Breaker");
    if (graphicClassName == QStringLiteral("CimdrawPowerDisconnectorItem"))
        return QStringLiteral("Disconnector");
    if (graphicClassName == QStringLiteral("CimdrawPowerBusbarSectionItem"))
        return QStringLiteral("BusbarSection");
    if (graphicClassName == QStringLiteral("CimdrawPowerTransformer2wItem")
        || graphicClassName == QStringLiteral("CimdrawPowerTransformer3wItem")
        || graphicClassName == QStringLiteral("CimdrawPowerAutoXfmrItem")
        || graphicClassName == QStringLiteral("CimdrawPowerEarthingXfmrItem")
        || graphicClassName == QStringLiteral("CimdrawPowerStationXfmrItem"))
        return QStringLiteral("PowerTransformer");
    if (graphicClassName == QStringLiteral("CimdrawPowerLoadItem"))
        return QStringLiteral("EnergyConsumer");
    if (graphicClassName == QStringLiteral("CimdrawPowerGeneratorItem"))
        return QStringLiteral("SynchronousMachine");
    if (graphicClassName == QStringLiteral("CimdrawPowerPvInfeedItem"))
        return QStringLiteral("SolarGeneratingUnit");
    if (graphicClassName == QStringLiteral("CimdrawPowerWindInfeedItem"))
        return QStringLiteral("WindGeneratingUnit");
    if (graphicClassName == QStringLiteral("CimdrawPowerEssItem"))
        return QStringLiteral("BatteryUnit");
    if (graphicClassName == QStringLiteral("CimdrawPowerCableItem"))
        return QStringLiteral("ACLineSegment");
    if (graphicClassName == QStringLiteral("CimdrawPowerLoadBreakItem"))
        return QStringLiteral("LoadBreakSwitch");
    if (graphicClassName == QStringLiteral("CimdrawPowerEarthSwitchItem"))
        return QStringLiteral("GroundDisconnector");
    if (graphicClassName == QStringLiteral("CimdrawPowerCapacitorItem"))
        return QStringLiteral("LinearShuntCompensator");
    if (graphicClassName == QStringLiteral("CimdrawPowerReactorItem")
        || graphicClassName == QStringLiteral("CimdrawPowerSplitReactorItem")
        || graphicClassName == QStringLiteral("CimdrawPowerSvgCompItem")
        || graphicClassName == QStringLiteral("CimdrawPowerArcCoilItem"))
        return QStringLiteral("ShuntCompensator");
    if (graphicClassName == QStringLiteral("CimdrawPowerMotorItem"))
        return QStringLiteral("AsynchronousMachine");
    if (graphicClassName == QStringLiteral("CimdrawPowerGridItem"))
        return QStringLiteral("ExternalNetworkInjection");
    if (graphicClassName == QStringLiteral("CimdrawPowerArresterItem"))
        return QStringLiteral("SurgeArrester");
    if (graphicClassName == QStringLiteral("CimdrawPowerGroundItem"))
        return QStringLiteral("Ground");
    if (graphicClassName == QStringLiteral("CimdrawPowerCurrentTransformerItem"))
        return QStringLiteral("CurrentTransformer");
    if (graphicClassName == QStringLiteral("CimdrawPowerVoltageTransformerItem"))
        return QStringLiteral("VoltageTransformer");
    if (graphicClassName == QStringLiteral("CimdrawPowerFuseItem"))
        return QStringLiteral("Fuse");
    if (graphicClassName == QStringLiteral("CimdrawPowerJunctionItem"))
        return QStringLiteral("Junction");
    return graphicClassName;
}

} // namespace

QGraphicsItem* CimGraphicObjectSummaryQuery::sceneItemByMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryContext& queryContext) const
{
    const QHash<QString, QGraphicsItem*> emptyShapeByMrid;
    return sceneItemByMrid(mrid,
                           scene,
                           queryContext.shapeByMrid ? *queryContext.shapeByMrid : emptyShapeByMrid,
                           queryContext.idIndex);
}

QGraphicsItem* CimGraphicObjectSummaryQuery::sceneItemByMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const QHash<QString, QGraphicsItem*>& shapeByMrid,
    const CimIdIndex* idIndex) const
{
    if (mrid.trimmed().isEmpty() || !scene)
        return nullptr;

    QGraphicsItem* item = nullptr;
    if (idIndex)
        item = findSceneItemByGraphicItemId(scene, idIndex->graphicItemIdForMrid(mrid));
    if (!item)
        item = shapeByMrid.value(mrid, nullptr);
    if (!item || item->scene() != scene)
        return nullptr;
    return item;
}

QString CimGraphicObjectSummaryQuery::mridForSceneItem(QGraphicsItem* item,
                                                       const CimGraphicQueryContext& queryContext) const
{
    return mridForSceneItem(item, queryContext.idIndex);
}

QString CimGraphicObjectSummaryQuery::mridForSceneItem(QGraphicsItem* item,
                                                       const CimIdIndex* idIndex) const
{
    if (!item)
        return {};

    QString mrid;
    if (idIndex)
    {
        const QString graphicItemId =
            item->data(CimSceneBuilder::CimGraphicItemIdDataKey).toString();
        mrid = idIndex->mridForGraphicItemId(graphicItemId);
    }
    if (mrid.isEmpty())
        mrid = item->data(CimSceneBuilder::CimMridDataKey).toString();
    return mrid;
}

CimGraphicObjectSummary CimGraphicObjectSummaryQuery::objectSummaryForSceneItem(
    QGraphicsItem* item,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummary summary;
    if (!item)
        return summary;

    summary.mrid = mridForSceneItem(item, queryState.queryContext);
    if (summary.mrid.isEmpty())
        summary.mrid = item->data(CimSceneBuilder::CimMridDataKey).toString();

    if (!summary.mrid.trimmed().isEmpty())
    {
        summary.visualized = true;
        summary.mappable = true;
        summary.visualState = CimGraphicObjectVisualState::Visualized;
    }

    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
    {
        summary.objectId = shape->cimdrawObjectId();
        summary.graphicClassName = shape->className();
        summary.className = cimClassNameForGraphicClassName(summary.graphicClassName);
        summary.displayName = shape->displayName().trimmed();
        if (summary.displayName.isEmpty() || summary.displayName == QStringLiteral("TmpBase"))
            summary.displayName = summary.className.trimmed();
        if (summary.displayName.isEmpty())
            summary.displayName = QObject::tr("未命名图元");
        summary.participatesInTopology = shape->participatesInTopology();
        summary.isTopologyGraphNode = shape->isTopologyGraphNode();
        summary.topologyDomain = shape->topologyDomain();

        if (auto* wiring = dynamic_cast<CimdrawWiringItemBase*>(shape))
        {
            summary.powerTopologyRole = wiring->powerTopologyRole();
            summary.supportsDirectedFlow = wiring->supportsFlowDirectionAction();
            CimGraphicRenderStateQuery renderStateQuery;
            summary.renderState = renderStateQuery.resultForItem(shape);
        }
    }

    CimBehaviorQuery behaviorQuery;
    summary.behaviorResult = behaviorQuery.resultForItem(item);
    return summary;
}

CimGraphicObjectSummary CimGraphicObjectSummaryQuery::objectSummaryForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummary summary =
        objectSummaryForSceneItem(sceneItemByMrid(mrid, scene, queryState.queryContext), queryState);
    if (summary.mrid.isEmpty())
        summary.mrid = mrid;
    return summary;
}

CimGraphicObjectSummary CimGraphicObjectSummaryQuery::selectedObjectSummary(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    if (!scene)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
        return {};

    return objectSummaryForSceneItem(selections.first(), queryState);
}
