#include "LzSceneDeferredUpdateController.h"

#include <QMetaObject>
#include <QTimer>

#include "LzScene.h"
#include "LzView.h"
#include "item/LzConnectLine.h"

namespace {

bool topologyRebuildNeededForLines(const QList<LzConnectLine*>& lines)
{
    if (lines.isEmpty())
        return true;
    for (LzConnectLine* line : lines)
    {
        if (!line)
            continue;
        if (line->isTopologyRelationLink())
            return true;
        if (!line->topologyEdgeStableId().isEmpty()
            || !line->topologyStartNodeStableId().isEmpty()
            || !line->topologyEndNodeStableId().isEmpty())
        {
            return true;
        }
    }
    return false;
}

} // namespace

void LzSceneDeferredUpdateController::scheduleTopologyRebuild(LzScene* scene)
{
    if (!scene)
        return;
    if (topologyRebuildPending_ || connectorPostprocessPending_)
        return;

    topologyRebuildPending_ = true;
    QTimer::singleShot(0, scene, [this, scene]() {
        topologyRebuildPending_ = false;
        scene->rebuildTopologyIndex();
    });
}

void LzSceneDeferredUpdateController::scheduleConnectorPostprocess(LzScene* scene,
                                                                   const QList<LzConnectLine*>& affectedLines)
{
    if (!scene)
        return;

    for (LzConnectLine* line : affectedLines)
    {
        if (line && line->scene() == scene)
            pendingConnectorLines_.insert(line);
    }
    if (connectorPostprocessPending_)
        return;

    connectorPostprocessPending_ = true;
    QTimer::singleShot(0, scene, [this, scene]() {
        connectorPostprocessPending_ = false;
        topologyRebuildPending_ = false;
        const QList<LzConnectLine*> lines = pendingConnectorLines_.values();
        pendingConnectorLines_.clear();
        if (scene->getPaintState())
            return;
        if (topologyRebuildNeededForLines(lines))
        {
            scene->rebuildTopologyIndex();
            return;
        }

        QList<LzConnectLine*> nudgeAuto;
        nudgeAuto.reserve(lines.size());
        for (LzConnectLine* line : lines)
        {
            if (line && line->pathRoutingMode() == ConnectorPathRoutingMode::Auto
                && lzConnectStrategyUsesOrthogonalEdit(line->getConnectStrategy()))
            {
                nudgeAuto.append(line);
            }
        }
        if (nudgeAuto.size() >= 2)
            scene->applyConnectorNudging(nudgeAuto);
    });
}

void LzSceneDeferredUpdateController::scheduleConnectLinesPathRecompute(LzScene* scene,
                                                                        const QList<LzConnectLine*>& lines)
{
    if (!scene)
        return;

    for (LzConnectLine* line : lines)
    {
        if (line && line->scene() == scene)
            pendingPathRecomputeLines_.insert(line);
    }
    if (pathRecomputePending_ || pendingPathRecomputeLines_.isEmpty())
        return;

    pathRecomputePending_ = true;
    QTimer::singleShot(0, scene, [this, scene]() {
        pathRecomputePending_ = false;
        const QList<LzConnectLine*> batch = pendingPathRecomputeLines_.values();
        pendingPathRecomputeLines_.clear();
        if (batch.isEmpty() || scene->getPaintState())
            return;

        if (LzView* view = scene->getView())
            view->setUpdatesEnabled(false);

        for (LzConnectLine* line : batch)
        {
            if (line)
                line->finalizePathAfterItemDrag();
        }

        if (LzView* view = scene->getView())
            view->setUpdatesEnabled(true);
        scene->update();

        QMetaObject::invokeMethod(scene, [this, scene]() {
            flushDeferredItemPropertyPanel(scene);
        }, Qt::QueuedConnection);
    });
}

void LzSceneDeferredUpdateController::cancelConnectLinePathRecompute(LzConnectLine* line)
{
    if (line)
        pendingPathRecomputeLines_.remove(line);
}

bool LzSceneDeferredUpdateController::isConnectLinePathUpdatePending() const
{
    return pathRecomputePending_ || !pendingPathRecomputeLines_.isEmpty();
}

void LzSceneDeferredUpdateController::requestDeferredItemPropertyPanel()
{
    itemPropertyPanelFlushPending_ = true;
}

void LzSceneDeferredUpdateController::flushDeferredItemPropertyPanel(LzScene* scene)
{
    if (!scene || !itemPropertyPanelFlushPending_)
        return;
    itemPropertyPanelFlushPending_ = false;
    emit scene->itemPropertyChanged();
}
