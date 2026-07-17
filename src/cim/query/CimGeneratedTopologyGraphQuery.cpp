#include "cim/query/CimGeneratedTopologyGraphQuery.h"

#include <QObject>

#include "CimdrawScene.h"
#include "Item/TmpBase.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"
#include "cim/query/CimGraphicSummaryText.h"
#include "item/CimdrawConnectLine.h"
#include "scene/CimdrawSceneTopologyStateController.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"
#include "topology/PowerTopologyProjection.h"
#include "topology/TopologyBindingUtils.h"

namespace {

const PowerTopologyDeviceRecord* powerDeviceRecord(const PowerTopologyAnalysisSnapshot& snapshot,
                                                   const QString& deviceId)
{
    const int index = snapshot.deviceIndex.value(deviceId, -1);
    if (index < 0 || index >= snapshot.devices.size())
        return nullptr;
    return &snapshot.devices.at(index);
}

bool buildRelationWorkbenchContext(CimdrawScene* scene,
                                   CimdrawSceneTopologyWorkbenchController* workbenchController,
                                   CimdrawSceneTopologyStateController* stateController)
{
    if (!scene || !workbenchController || !stateController)
        return false;
    workbenchController->rebuildTopologyIndex(scene, *stateController);
    return true;
}

const CimGeneratedTopologySourceSummary* generatedSourceSummaryByNodeId(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    auto it = sourceSummariesByNodeId.constFind(nodeId);
    if (it == sourceSummariesByNodeId.constEnd())
        return nullptr;
    return &it.value();
}

bool generatedEndpointSupportsDirectedFlow(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    const CimGeneratedTopologySourceSummary* summary =
        generatedSourceSummaryByNodeId(sourceSummariesByNodeId, nodeId);
    return summary && summary->supportsDirectedFlow;
}

int generatedEndpointFlowSign(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    const CimGeneratedTopologySourceSummary* summary =
        generatedSourceSummaryByNodeId(sourceSummariesByNodeId, nodeId);
    return summary ? summary->flowSign : 1;
}

bool generatedEndpointFlowsForward(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    return generatedEndpointFlowSign(sourceSummariesByNodeId, nodeId) >= 0;
}

CimdrawConnectLine::ArrowHeadStyle generatedArrowForStartEndpointFlow(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    return generatedEndpointFlowsForward(sourceSummariesByNodeId, nodeId)
        ? CimdrawConnectLine::ArrowEnd
        : CimdrawConnectLine::ArrowStart;
}

CimdrawConnectLine::ArrowHeadStyle generatedArrowForEndEndpointFlow(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    return generatedEndpointFlowsForward(sourceSummariesByNodeId, nodeId)
        ? CimdrawConnectLine::ArrowStart
        : CimdrawConnectLine::ArrowEnd;
}

bool generatedStartEndpointRequestsReverse(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    return generatedEndpointSupportsDirectedFlow(sourceSummariesByNodeId, nodeId)
        && !generatedEndpointFlowsForward(sourceSummariesByNodeId, nodeId);
}

bool generatedEndEndpointRequestsReverse(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    return generatedEndpointSupportsDirectedFlow(sourceSummariesByNodeId, nodeId)
        && generatedEndpointFlowsForward(sourceSummariesByNodeId, nodeId);
}

PowerTopologyRole generatedEndpointPowerRole(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const QString& nodeId)
{
    const CimGeneratedTopologySourceSummary* summary =
        generatedSourceSummaryByNodeId(sourceSummariesByNodeId, nodeId);
    return summary ? summary->powerTopologyRole : PowerTopologyRole::GenericEquipment;
}

CimdrawConnectLine::ArrowHeadStyle inferGeneratedTopologyArrow(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const TopologyEdgeMeta& edgeMeta,
    const CimdrawConnectLine* sourceLine)
{
    if (sourceLine)
    {
        const CimdrawConnectLine::ArrowHeadStyle sourceArrow = sourceLine->arrowHead();
        if (sourceArrow != CimdrawConnectLine::ArrowNone)
            return sourceArrow;
    }

    if (generatedEndpointSupportsDirectedFlow(sourceSummariesByNodeId, edgeMeta.startNodeStableId))
        return generatedArrowForStartEndpointFlow(sourceSummariesByNodeId, edgeMeta.startNodeStableId);
    if (generatedEndpointSupportsDirectedFlow(sourceSummariesByNodeId, edgeMeta.endNodeStableId))
        return generatedArrowForEndEndpointFlow(sourceSummariesByNodeId, edgeMeta.endNodeStableId);

    return CimdrawConnectLine::ArrowEnd;
}

bool shouldReverseGeneratedTopologyDirection(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId,
    const TopologyEdgeMeta& edgeMeta)
{
    const PowerTopologyRole startRole =
        generatedEndpointPowerRole(sourceSummariesByNodeId, edgeMeta.startNodeStableId);
    const PowerTopologyRole endRole =
        generatedEndpointPowerRole(sourceSummariesByNodeId, edgeMeta.endNodeStableId);

    if (startRole != PowerTopologyRole::Busbar && endRole == PowerTopologyRole::Busbar)
        return true;
    if (startRole == PowerTopologyRole::Busbar && endRole != PowerTopologyRole::Busbar)
        return false;

    if (generatedStartEndpointRequestsReverse(sourceSummariesByNodeId, edgeMeta.startNodeStableId))
        return true;
    if (generatedEndEndpointRequestsReverse(sourceSummariesByNodeId, edgeMeta.endNodeStableId))
        return true;

    return false;
}

CimdrawConnectLine::ArrowHeadStyle mirroredGeneratedTopologyArrow(
    CimdrawConnectLine::ArrowHeadStyle arrowStyle)
{
    switch (arrowStyle)
    {
    case CimdrawConnectLine::ArrowEnd:
        return CimdrawConnectLine::ArrowStart;
    case CimdrawConnectLine::ArrowStart:
        return CimdrawConnectLine::ArrowEnd;
    case CimdrawConnectLine::ArrowBoth:
        return CimdrawConnectLine::ArrowBoth;
    case CimdrawConnectLine::ArrowNone:
    default:
        return CimdrawConnectLine::ArrowNone;
    }
}

} // namespace

CimGeneratedTopologySourceSummary
CimGeneratedTopologyGraphQuery::generatedTopologySourceSummaryForSceneItem(
    QGraphicsItem* item,
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot) const
{
    CimGeneratedTopologySourceSummary summary;
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return summary;

    CimGraphicObjectSummaryQuery objectQuery;
    const CimGraphicObjectSummary objectSummary = objectQuery.objectSummaryForSceneItem(item);
    summary.nodeId = cimdrawTopologyBindingIdForShape(shape);
    summary.displayName = objectSummary.displayName;
    summary.topologyDomain = objectSummary.topologyDomain;
    summary.participatesInTopology = objectSummary.participatesInTopology;
    summary.isTopologyGraphNode = objectSummary.isTopologyGraphNode;
    summary.powerTopologyRole = objectSummary.powerTopologyRole;
    summary.supportsDirectedFlow = objectSummary.supportsDirectedFlow;
    summary.flowSign = cimGraphicObjectSummaryFlowSign(objectSummary);

    if (runtimeSnapshot && !summary.nodeId.isEmpty())
    {
        if (const PowerTopologyDeviceRecord* record = powerDeviceRecord(*runtimeSnapshot, summary.nodeId))
        {
            if (!record->displayName.trimmed().isEmpty())
                summary.displayName = record->displayName;
            summary.powerTopologyRole = record->role;
            summary.supportsDirectedFlow = record->supportsDirectedFlow;
            summary.flowSign = record->flowSign;
        }
    }

    summary.displayLabel = summary.displayName.isEmpty()
        ? QObject::tr("未命名图元")
        : summary.displayName;
    return summary;
}

QVector<QPair<const QGraphicsItem*, CimGeneratedTopologySourceSummary>>
CimGeneratedTopologyGraphQuery::generatedTopologySourceSummaries(
    CimdrawScene* scene,
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot,
    bool ensureNodeIds) const
{
    QVector<QPair<const QGraphicsItem*, CimGeneratedTopologySourceSummary>> summaries;
    if (!scene)
        return summaries;

    const QList<QGraphicsItem*> allItems = scene->items();
    summaries.reserve(allItems.size());
    for (QGraphicsItem* item : allItems)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || shape->parentItem())
            continue;

        CimGeneratedTopologySourceSummary summary =
            generatedTopologySourceSummaryForSceneItem(shape, runtimeSnapshot);
        if (ensureNodeIds && summary.nodeId.isEmpty())
            summary.nodeId = cimdrawEnsureTopologyBindingIdForShape(shape);
        if (!summary.isValid() || summary.nodeId.isEmpty())
            continue;

        summaries.push_back(qMakePair(static_cast<const QGraphicsItem*>(shape), summary));
    }

    return summaries;
}

CimGeneratedTopologyGraphSummary CimGeneratedTopologyGraphQuery::generatedTopologyGraphSummary(
    CimdrawScene* scene,
    CimdrawTopologyDomain domainFilter) const
{
    CimGeneratedTopologyGraphSummary summary;
    if (!scene)
        return summary;

    const PowerTopologyAnalysisSnapshot runtimeSnapshot =
        PowerTopologyProjection::fromRuntimeScene(scene).snapshot();
    const auto sourceSummaries = generatedTopologySourceSummaries(scene, &runtimeSnapshot, true);

    QHash<QString, CimGeneratedTopologySourceSummary> sourceSummariesByNodeId;
    summary.nodeSummaries.reserve(sourceSummaries.size());
    for (const auto& sourceEntry : sourceSummaries)
    {
        const CimGeneratedTopologySourceSummary& sourceSummary = sourceEntry.second;
        if (!sourceSummary.isValid())
            continue;
        if (domainFilter != CimdrawTopologyDomain::None
            && sourceSummary.topologyDomain != domainFilter)
        {
            continue;
        }

        summary.nodeSummaries.push_back(sourceSummary);
        sourceSummariesByNodeId.insert(sourceSummary.nodeId, sourceSummary);
    }

    summary.relationSummaries = generatedTopologyRelationSummaries(scene, sourceSummariesByNodeId);
    return summary;
}

QVector<CimGeneratedTopologyRelationSummary>
CimGeneratedTopologyGraphQuery::generatedTopologyRelationSummaries(
    CimdrawScene* scene,
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId) const
{
    QVector<CimGeneratedTopologyRelationSummary> summaries;
    if (!scene || sourceSummariesByNodeId.isEmpty())
        return summaries;

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return summaries;

    QHash<QString, const CimdrawConnectLine*> sourceLinesByEdgeId;
    const QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : allItems)
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (!line)
            continue;

        const TopologyEdgeMeta edgeInfo =
            workbenchController.resolveRelationEdgeMeta(scene,
                                                        stateController,
                                                        line->resolvedRelationEdgeLookupHint());
        if (!edgeInfo.edgeStableId.isEmpty())
            sourceLinesByEdgeId.insert(edgeInfo.edgeStableId, line);
    }

    const QVector<TopologyEdgeMeta> relationEdges =
        workbenchController.relationEdgeMetas(scene, stateController);
    summaries.reserve(relationEdges.size());
    for (const TopologyEdgeMeta& edgeMeta : relationEdges)
    {
        if (edgeMeta.edgeStableId.isEmpty())
            continue;
        if (!sourceSummariesByNodeId.contains(edgeMeta.startNodeStableId)
            || !sourceSummariesByNodeId.contains(edgeMeta.endNodeStableId))
        {
            continue;
        }

        CimGeneratedTopologyRelationSummary summary;
        summary.edgeStableId = edgeMeta.edgeStableId;
        summary.startNodeId = edgeMeta.startNodeStableId;
        summary.endNodeId = edgeMeta.endNodeStableId;
        summary.startPortKey = edgeMeta.startPortKey;
        summary.endPortKey = edgeMeta.endPortKey;
        summary.relationType = edgeMeta.relationType;
        summary.relationLabel = cimTopologyRelationTypeLabel(edgeMeta.relationType);
        summary.arrowHeadStyle = static_cast<int>(inferGeneratedTopologyArrow(
            sourceSummariesByNodeId, edgeMeta, sourceLinesByEdgeId.value(edgeMeta.edgeStableId, nullptr)));

        if (shouldReverseGeneratedTopologyDirection(sourceSummariesByNodeId, edgeMeta))
        {
            qSwap(summary.startNodeId, summary.endNodeId);
            qSwap(summary.startPortKey, summary.endPortKey);
            summary.arrowHeadStyle = static_cast<int>(mirroredGeneratedTopologyArrow(
                static_cast<CimdrawConnectLine::ArrowHeadStyle>(summary.arrowHeadStyle)));
        }

        if (summary.isValid())
            summaries.push_back(summary);
    }

    return summaries;
}
