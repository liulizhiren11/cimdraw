#include "CimdrawSceneDeferredUpdateController.h"

#include <QMetaObject>
#include <QTimer>

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "item/CimdrawConnectLine.h"

namespace {

bool topologyRebuildNeededForLines(const QList<CimdrawConnectLine*>& lines)
{
    if (lines.isEmpty())
        return true;
    for (CimdrawConnectLine* line : lines)
    {
        if (!line)
            continue;
        if (line->hasTopologyBindings())
        {
            return true;
        }
    }
    return false;
}

} // namespace

void CimdrawSceneDeferredUpdateController::scheduleTopologyRebuild(CimdrawScene* scene)
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

void CimdrawSceneDeferredUpdateController::scheduleConnectorPostprocess(CimdrawScene* scene,
                                                                   const QList<CimdrawConnectLine*>& affectedLines)
{
    if (!scene)
        return;

    for (CimdrawConnectLine* line : affectedLines)
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
        const QList<CimdrawConnectLine*> lines = pendingConnectorLines_.values();
        pendingConnectorLines_.clear();
        if (scene->getPaintState())
            return;
        if (topologyRebuildNeededForLines(lines))
        {
            scene->rebuildTopologyIndex();
            return;
        }

        QList<CimdrawConnectLine*> nudgeAuto;
        nudgeAuto.reserve(lines.size());
        for (CimdrawConnectLine* line : lines)
        {
            if (line && line->pathRoutingMode() == ConnectorPathRoutingMode::Auto
                && cimdrawConnectStrategyUsesOrthogonalEdit(line->getConnectStrategy()))
            {
                nudgeAuto.append(line);
            }
        }
        if (nudgeAuto.size() >= 2)
            scene->applyConnectorNudging(nudgeAuto);
    });
}

void CimdrawSceneDeferredUpdateController::scheduleConnectLinesPathRecompute(CimdrawScene* scene,
                                                                        const QList<CimdrawConnectLine*>& lines)
{
    if (!scene)
        return;

    for (CimdrawConnectLine* line : lines)
    {
        if (line && line->scene() == scene)
            pendingPathRecomputeLines_.insert(line);
    }
    if (pathRecomputePending_ || pendingPathRecomputeLines_.isEmpty())
        return;

    pathRecomputePending_ = true;
    QTimer::singleShot(0, scene, [this, scene]() {
        pathRecomputePending_ = false;
        const QList<CimdrawConnectLine*> batch = pendingPathRecomputeLines_.values();
        pendingPathRecomputeLines_.clear();
        if (batch.isEmpty() || scene->getPaintState())
            return;

        if (CimdrawView* view = scene->getView())
            view->setUpdatesEnabled(false);

        for (CimdrawConnectLine* line : batch)
        {
            if (line)
                line->finalizePathAfterItemDrag();
        }

        if (CimdrawView* view = scene->getView())
            view->setUpdatesEnabled(true);
        scene->update();

        QMetaObject::invokeMethod(scene, [this, scene]() {
            flushDeferredItemPropertyPanel(scene);
        }, Qt::QueuedConnection);
    });
}

void CimdrawSceneDeferredUpdateController::cancelConnectLinePathRecompute(CimdrawConnectLine* line)
{
    if (line)
        pendingPathRecomputeLines_.remove(line);
}

bool CimdrawSceneDeferredUpdateController::isConnectLinePathUpdatePending() const
{
    return pathRecomputePending_ || !pendingPathRecomputeLines_.isEmpty();
}

void CimdrawSceneDeferredUpdateController::requestDeferredItemPropertyPanel()
{
    itemPropertyPanelFlushPending_ = true;
}

void CimdrawSceneDeferredUpdateController::flushDeferredItemPropertyPanel(CimdrawScene* scene)
{
    if (!scene || !itemPropertyPanelFlushPending_)
        return;
    itemPropertyPanelFlushPending_ = false;
    emit scene->itemPropertyChanged();
}
