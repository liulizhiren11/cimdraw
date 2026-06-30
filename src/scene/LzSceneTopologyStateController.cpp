#include "LzSceneTopologyStateController.h"

#include "LzScene.h"
#include "LzView.h"
#include "item/LzConnectLine.h"
#include "topology/PowerTopologyBuilder.h"
#include "topology/TopologyGraphIndex.h"

LzSceneTopologyStateController::LzSceneTopologyStateController() = default;
LzSceneTopologyStateController::~LzSceneTopologyStateController() = default;
LzSceneTopologyStateController::LzSceneTopologyStateController(LzSceneTopologyStateController&&) noexcept = default;
LzSceneTopologyStateController&
LzSceneTopologyStateController::operator=(LzSceneTopologyStateController&&) noexcept = default;

void LzSceneTopologyStateController::invalidateSnapshots()
{
    powerTopologySnapshotDirty_ = true;
    powerTopologySnapshot_.reset();
}

void LzSceneTopologyStateController::rebuildTopologyIndex(LzScene* scene)
{
    if (!scene)
        return;

    invalidateSnapshots();
    for (QGraphicsItem* item : scene->items())
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            line->setZValue(LzConnectLineLayerZ);
    }
    if (!topologyIndex_)
        topologyIndex_ = std::make_unique<TopologyGraphIndex>();
    topologyIndex_->rebuildFromScene(scene);
}

const TopologyGraphIndex* LzSceneTopologyStateController::topologyIndex() const
{
    return topologyIndex_.get();
}

TopologyGraphIndex* LzSceneTopologyStateController::topologyIndex()
{
    return topologyIndex_.get();
}

void LzSceneTopologyStateController::highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightNode(nodeStableId, on, c, QColor());
}

void LzSceneTopologyStateController::highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightEdges(QStringList{edgeStableId}, on, c);
}

void LzSceneTopologyStateController::highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c)
{
    if (topologyIndex_)
        topologyIndex_->highlightEdges(edgeStableIds, on, c);
}

void LzSceneTopologyStateController::clearTopologyHighlights()
{
    if (topologyIndex_)
        topologyIndex_->clearAllHighlights();
}

PowerTopologyAnalysisSnapshot LzSceneTopologyStateController::buildPowerTopologySnapshot(LzScene* scene)
{
    if (!scene)
        return {};

    if (powerTopologySnapshotDirty_ || !powerTopologySnapshot_)
    {
        powerTopologySnapshot_ = std::make_unique<PowerTopologyAnalysisSnapshot>(
            PowerTopologyBuilder::build(scene));
        powerTopologySnapshotDirty_ = false;
    }
    return *powerTopologySnapshot_;
}

PowerTopologyDocumentExport LzSceneTopologyStateController::buildPowerTopologyDocumentExport(LzScene* scene)
{
    PowerTopologyDocumentExport document;
    if (!scene)
        return document;

    const PowerTopologyAnalysisSnapshot snapshot = buildPowerTopologySnapshot(scene);
    document.devices = snapshot.devices;
    document.conductors = snapshot.conductors;
    document.nodes = snapshot.nodes;

    if (LzView* view = scene->getView())
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
