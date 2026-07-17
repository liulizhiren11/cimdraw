#include "CimdrawSceneTopologyStateController.h"

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "item/CimdrawConnectLine.h"
#include "topology/PowerTopologyProjection.h"
#include "topology/TopologyGraphIndex.h"

namespace {

bool relationEdgeMetaMatchesLookupHint(const TopologyEdgeMeta& meta,
                                       const TopologyEdgeLookupHint& lookupHint)
{
    if (!lookupHint.edgeStableId.isEmpty() && meta.edgeStableId != lookupHint.edgeStableId)
        return false;
    if (!lookupHint.startNodeStableId.isEmpty() && meta.startNodeStableId != lookupHint.startNodeStableId)
        return false;
    if (!lookupHint.endNodeStableId.isEmpty() && meta.endNodeStableId != lookupHint.endNodeStableId)
        return false;
    if (!lookupHint.startPortKey.isEmpty() && meta.startPortKey != lookupHint.startPortKey)
        return false;
    if (!lookupHint.endPortKey.isEmpty() && meta.endPortKey != lookupHint.endPortKey)
        return false;
    if (lookupHint.relationTypeSpecified && meta.relationType != lookupHint.relationType)
        return false;
    return true;
}

} // namespace

CimdrawSceneTopologyStateController::CimdrawSceneTopologyStateController() = default;
CimdrawSceneTopologyStateController::~CimdrawSceneTopologyStateController() = default;
CimdrawSceneTopologyStateController::CimdrawSceneTopologyStateController(CimdrawSceneTopologyStateController&&) noexcept = default;
CimdrawSceneTopologyStateController&
CimdrawSceneTopologyStateController::operator=(CimdrawSceneTopologyStateController&&) noexcept = default;

void CimdrawSceneTopologyStateController::invalidateSnapshots()
{
    powerTopologySnapshotDirty_ = true;
    powerTopologySnapshot_.reset();
}

void CimdrawSceneTopologyStateController::rebuildTopologyIndex(CimdrawScene* scene)
{
    if (!scene)
        return;

    invalidateSnapshots();
    for (QGraphicsItem* item : scene->items())
    {
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
            line->setZValue(CimdrawConnectLineLayerZ);
    }
    if (!topologyIndex_)
        topologyIndex_ = std::make_unique<TopologyGraphIndex>();
    topologyIndex_->rebuildFromScene(scene);
}

const TopologyGraphIndex* CimdrawSceneTopologyStateController::topologyIndex() const
{
    return topologyIndex_.get();
}

TopologyGraphIndex* CimdrawSceneTopologyStateController::topologyIndex()
{
    return topologyIndex_.get();
}

TopologyNodeRelationInfo CimdrawSceneTopologyStateController::relationNodeInfo(CimdrawScene* scene,
                                                                               const QString& nodeStableId)
{
    TopologyNodeRelationInfo info;
    info.nodeStableId = nodeStableId;
    if (!scene || nodeStableId.trimmed().isEmpty())
        return info;

    rebuildTopologyIndex(scene);
    const TopologyGraphIndex* index = topologyIndex();
    if (!index)
        return info;

    info.incidentEdgeIds = index->edgeIdsIncidentToNode(nodeStableId);
    info.adjacentNodeIds = index->adjacentNodeStableIds(nodeStableId);
    return info;
}

TopologyEdgeMeta CimdrawSceneTopologyStateController::relationEdgeMeta(CimdrawScene* scene,
                                                                       const QString& edgeStableId)
{
    TopologyEdgeMeta meta;
    if (!scene || edgeStableId.trimmed().isEmpty())
        return meta;

    rebuildTopologyIndex(scene);
    const TopologyGraphIndex* index = topologyIndex();
    if (!index)
        return meta;

    index->edgeMeta(edgeStableId, &meta);
    return meta;
}

TopologyEdgeMeta CimdrawSceneTopologyStateController::resolveRelationEdgeMeta(
    CimdrawScene* scene,
    const TopologyEdgeLookupHint& lookupHint)
{
    if (!scene || !lookupHint.hasLookupFields())
        return {};

    rebuildTopologyIndex(scene);
    const TopologyGraphIndex* index = topologyIndex();
    if (!index)
        return {};

    if (!lookupHint.edgeStableId.isEmpty())
    {
        TopologyEdgeMeta meta;
        index->edgeMeta(lookupHint.edgeStableId, &meta);
        if (!meta.edgeStableId.isEmpty() && relationEdgeMetaMatchesLookupHint(meta, lookupHint))
            return meta;
    }

    const QVector<TopologyEdgeMeta> edges = index->edgeMetas();
    for (const TopologyEdgeMeta& meta : edges)
    {
        if (relationEdgeMetaMatchesLookupHint(meta, lookupHint))
            return meta;
    }
    return {};
}

QVector<TopologyEdgeMeta> CimdrawSceneTopologyStateController::relationEdgeMetas(CimdrawScene* scene)
{
    if (!scene)
        return {};

    rebuildTopologyIndex(scene);
    const TopologyGraphIndex* index = topologyIndex();
    if (!index)
        return {};

    return index->edgeMetas();
}

void CimdrawSceneTopologyStateController::highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightNode(nodeStableId, on, c, QColor());
}

void CimdrawSceneTopologyStateController::highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightEdges(QStringList{edgeStableId}, on, c);
}

void CimdrawSceneTopologyStateController::highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightEdges(edgeStableIds, on, c);
}

void CimdrawSceneTopologyStateController::clearTopologyHighlights()
{
    if (topologyIndex_)
        topologyIndex_->clearAllHighlights();
}

PowerTopologyAnalysisSnapshot CimdrawSceneTopologyStateController::buildPowerTopologySnapshot(CimdrawScene* scene)
{
    if (!scene)
        return {};

    if (powerTopologySnapshotDirty_ || !powerTopologySnapshot_)
    {
        powerTopologySnapshot_ = std::make_unique<PowerTopologyAnalysisSnapshot>(
            PowerTopologyProjection::fromRuntimeScene(scene).snapshot());
        powerTopologySnapshotDirty_ = false;
    }
    return *powerTopologySnapshot_;
}

PowerTopologyDocumentExport CimdrawSceneTopologyStateController::buildPowerTopologyDocumentExport(CimdrawScene* scene)
{
    PowerTopologyDocumentExport document;
    if (!scene)
        return document;

    const PowerTopologyAnalysisSnapshot snapshot = buildPowerTopologySnapshot(scene);
    document.devices = snapshot.devices;
    document.conductors = snapshot.conductors;
    document.nodes = snapshot.nodes;

    if (CimdrawView* view = scene->getView())
    {
        const QPoint viewportCenter(view->viewport()->width() / 2, view->viewport()->height() / 2);
        document.viewCenter = view->mapToScene(viewportCenter);
        document.viewScale = view->transform().m11();
        if (qFuzzyIsNull(document.viewScale))
            document.viewScale = 1.0;
    }
    else
    {
        document.viewCenter = scene->getContentsRect().center();
        document.viewScale = 1.0;
    }

    return document;
}
