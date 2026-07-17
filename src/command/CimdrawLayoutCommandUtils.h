#ifndef CIMDRAWLAYOUTCOMMANDUTILS_H
#define CIMDRAWLAYOUTCOMMANDUTILS_H

#include <QGraphicsItem>
#include <QList>
#include <QPointer>
#include <QVector>

#include "Item/CimdrawConnectLine.h"
#include "Item/CimdrawGroup.h"
#include "CimdrawScene.h"

struct CimdrawLayoutConnectLineSnapshot
{
    QPointer<CimdrawConnectLine> line;
    QVector<QPointF> scenePoints;
    ConnectorPathRoutingMode routingMode = ConnectorPathRoutingMode::Auto;
};

inline QVector<CimdrawLayoutConnectLineSnapshot> cimdrawCaptureLayoutConnectLineSnapshots(
    const QList<QGraphicsItem*>& items)
{
    QVector<CimdrawConnectLinePathSnapshot> raw;
    CimdrawGroup::collectConnectLineSnapshots(items, raw);

    QVector<CimdrawLayoutConnectLineSnapshot> snapshots;
    snapshots.reserve(raw.size());
    for (const CimdrawConnectLinePathSnapshot& snap : raw)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        CimdrawLayoutConnectLineSnapshot copy;
        copy.line = snap.line;
        copy.scenePoints = snap.scenePoints;
        copy.routingMode = snap.routingMode;
        snapshots.append(copy);
    }
    return snapshots;
}

inline void cimdrawRestoreLayoutConnectLineSnapshots(const QVector<CimdrawLayoutConnectLineSnapshot>& snapshots)
{
    for (const CimdrawLayoutConnectLineSnapshot& snap : snapshots)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        snap.line->restorePathSnapshot(snap.scenePoints, snap.routingMode);
    }
}

inline void cimdrawBeginLayoutCommand(CimdrawScene* scene)
{
    if (scene)
        scene->setInteractiveTransformActive(true);
}

inline void cimdrawEndLayoutCommand(CimdrawScene* scene)
{
    if (!scene)
        return;
    scene->setInteractiveTransformActive(false);
    scene->requestDeferredItemPropertyPanel();
    scene->flushDeferredItemPropertyPanel();
}

#endif
