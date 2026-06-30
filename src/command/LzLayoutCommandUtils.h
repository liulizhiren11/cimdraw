#ifndef LZLAYOUTCOMMANDUTILS_H
#define LZLAYOUTCOMMANDUTILS_H

#include <QGraphicsItem>
#include <QList>
#include <QPointer>
#include <QVector>

#include "Item/LzConnectLine.h"
#include "Item/LzGroup.h"
#include "LzScene.h"

struct LzLayoutConnectLineSnapshot
{
    QPointer<LzConnectLine> line;
    QVector<QPointF> scenePoints;
    ConnectorPathRoutingMode routingMode = ConnectorPathRoutingMode::Auto;
};

inline QVector<LzLayoutConnectLineSnapshot> lzCaptureLayoutConnectLineSnapshots(
    const QList<QGraphicsItem*>& items)
{
    QVector<LzConnectLinePathSnapshot> raw;
    LzGroup::collectConnectLineSnapshots(items, raw);

    QVector<LzLayoutConnectLineSnapshot> snapshots;
    snapshots.reserve(raw.size());
    for (const LzConnectLinePathSnapshot& snap : raw)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        LzLayoutConnectLineSnapshot copy;
        copy.line = snap.line;
        copy.scenePoints = snap.scenePoints;
        copy.routingMode = snap.routingMode;
        snapshots.append(copy);
    }
    return snapshots;
}

inline void lzRestoreLayoutConnectLineSnapshots(const QVector<LzLayoutConnectLineSnapshot>& snapshots)
{
    for (const LzLayoutConnectLineSnapshot& snap : snapshots)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        snap.line->restorePathSnapshot(snap.scenePoints, snap.routingMode);
    }
}

inline void lzBeginLayoutCommand(LzScene* scene)
{
    if (scene)
        scene->setInteractiveTransformActive(true);
}

inline void lzEndLayoutCommand(LzScene* scene)
{
    if (!scene)
        return;
    scene->setInteractiveTransformActive(false);
    scene->requestDeferredItemPropertyPanel();
    scene->flushDeferredItemPropertyPanel();
}

#endif
