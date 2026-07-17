#include "cim/query/CimTopologySummaryQuery.h"

#include "CimdrawScene.h"
#include "Item/TmpBase.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"
#include "cim/query/CimGraphicSummaryText.h"
#include "item/CimdrawConnectLine.h"
#include "scene/CimdrawSceneTopologyStateController.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"
#include "topology/TopologyBindingUtils.h"

namespace {

bool buildRelationWorkbenchContext(CimdrawScene* scene,
                                   CimdrawSceneTopologyWorkbenchController* workbenchController,
                                   CimdrawSceneTopologyStateController* stateController)
{
    if (!scene || !workbenchController || !stateController)
        return false;
    workbenchController->rebuildTopologyIndex(scene, *stateController);
    return true;
}

void appendUnique(QStringList* values, const QString& value)
{
    if (!values || value.isEmpty() || values->contains(value))
        return;
    values->push_back(value);
}

} // namespace

CimRelationEdgeSummary CimTopologySummaryQuery::relationEdgeSummaryForSceneItem(QGraphicsItem* item) const
{
    CimRelationEdgeSummary summary;
    auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
    auto* scene = item ? qobject_cast<CimdrawScene*>(item->scene()) : nullptr;
    if (!line || !scene)
        return summary;

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return summary;

    summary.edgeMeta =
        workbenchController.resolveRelationEdgeMeta(scene, stateController, line->resolvedRelationEdgeLookupHint());
    summary.relationLink = line->isTopologyRelationLink();
    return summary;
}

CimRelationEdgeSummary CimTopologySummaryQuery::selectedRelationEdgeSummary(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
        return {};

    return relationEdgeSummaryForSceneItem(selections.first());
}

CimTopologyObjectSummary CimTopologySummaryQuery::topologyObjectSummaryForSceneItem(QGraphicsItem* item) const
{
    CimTopologyObjectSummary summary;
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    auto* scene = item ? qobject_cast<CimdrawScene*>(item->scene()) : nullptr;
    if (!shape || !scene)
        return summary;

    summary.nodeId = cimdrawTopologyBindingIdForShape(shape);
    if (summary.nodeId.isEmpty())
        return summary;

    if (shape->isTopologyGraphNode())
        summary.relationInfo = scene->relationNodeInfoValue(summary.nodeId);

    if (shape->topologyDomain() == CimdrawTopologyDomain::PowerSystem)
    {
        summary.deviceNodeIds = scene->powerDeviceNodeIds(summary.nodeId);
        summary.conductorIds = scene->powerDeviceConductorIds(summary.nodeId);
        summary.connectedDevices = scene->connectedPowerDevices(summary.nodeId);
        summary.reachableDevices = scene->reachablePowerDevices(summary.nodeId);
    }

    return summary;
}

CimTopologyObjectSummary CimTopologySummaryQuery::topologyObjectSummaryForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery objectQuery;
    return topologyObjectSummaryForSceneItem(
        objectQuery.sceneItemByMrid(mrid, scene, queryState.queryContext));
}

CimTopologyObjectSummary CimTopologySummaryQuery::selectedTopologyObjectSummary(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    Q_UNUSED(queryState);
    if (!scene)
        return {};

    const QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.count() != 1)
        return {};

    return topologyObjectSummaryForSceneItem(selections.first());
}

CimTopologyHighlightSummary
CimTopologySummaryQuery::topologyHighlightSummaryForSceneItem(QGraphicsItem* item) const
{
    CimTopologyHighlightSummary summary;

    if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
    {
        const CimRelationEdgeSummary relationSummary = relationEdgeSummaryForSceneItem(line);
        summary.canHighlight = relationSummary.isValid();
        if (!summary.canHighlight)
            return summary;

        appendUnique(&summary.edgeIds, relationSummary.edgeMeta.edgeStableId);
        appendUnique(&summary.nodeIds, relationSummary.edgeMeta.startNodeStableId);
        appendUnique(&summary.nodeIds, relationSummary.edgeMeta.endNodeStableId);
        return summary;
    }

    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return summary;

    CimGraphicObjectSummaryQuery objectQuery;
    const CimGraphicObjectSummary objectSummary = objectQuery.objectSummaryForSceneItem(shape);
    summary.canHighlight = objectSummary.participatesInTopology || objectSummary.isTopologyGraphNode;
    if (!summary.canHighlight)
        return summary;

    const CimTopologyObjectSummary topologySummary = topologyObjectSummaryForSceneItem(shape);
    if (!topologySummary.isValid())
        return summary;

    appendUnique(&summary.nodeIds, topologySummary.nodeId);
    if (objectSummary.isTopologyGraphNode)
    {
        for (const QString& edgeId : topologySummary.relationInfo.incidentEdgeIds)
            appendUnique(&summary.edgeIds, edgeId);
        for (const QString& nodeId : topologySummary.relationInfo.adjacentNodeIds)
            appendUnique(&summary.nodeIds, nodeId);
    }

    if (objectSummary.topologyDomain == CimdrawTopologyDomain::PowerSystem)
    {
        for (const QString& edgeId : topologySummary.conductorIds)
            appendUnique(&summary.edgeIds, edgeId);
        for (const QString& nodeId : topologySummary.connectedDevices)
            appendUnique(&summary.nodeIds, nodeId);
    }

    return summary;
}

QStringList CimTopologySummaryQuery::topologySummaryLinesForSceneItem(QGraphicsItem* item) const
{
    if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        return cimRelationEdgeSummaryLines(relationEdgeSummaryForSceneItem(line));

    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return {};

    CimGraphicObjectSummaryQuery objectQuery;
    return cimTopologyObjectSummaryLines(objectQuery.objectSummaryForSceneItem(shape),
                                         topologyObjectSummaryForSceneItem(shape));
}
