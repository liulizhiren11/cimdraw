#include "LzSceneConnectorLayoutController.h"

#include <QSet>

#include "Item/LzConnectLine.h"
#include "LzScene.h"
#include "algorithm/LzAStar.h"
#include "algorithm/LzConnectorAlgorithm.h"

namespace {

QList<LzConnectLine*> connectorLinesFromScene(LzScene* scene)
{
    QList<LzConnectLine*> lines;
    if (!scene)
        return lines;

    for (QGraphicsItem* item : scene->items())
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            lines.append(line);
    }
    return lines;
}

}

QList<LzConnectLine*> LzSceneConnectorLayoutController::expandNudgingScope(
    LzScene* scene,
    const QList<LzConnectLine*>& seedLines) const
{
    if (!scene)
        return {};
    if (seedLines.isEmpty())
        return connectorLinesFromScene(scene);

    QSet<LzConnectLine*> scope;
    QRectF interest;
    bool hasInterest = false;
    for (LzConnectLine* line : seedLines)
    {
        if (!line || line->scene() != scene)
            continue;
        scope.insert(line);
        const QRectF bounds = line->sceneBoundingRect().adjusted(-24.0, -24.0, 24.0, 24.0);
        interest = hasInterest ? interest.united(bounds) : bounds;
        hasInterest = true;
    }
    if (!hasInterest)
        return connectorLinesFromScene(scene);

    for (QGraphicsItem* item : scene->items(interest, Qt::IntersectsItemBoundingRect))
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            scope.insert(line);
    }
    return scope.values();
}

bool LzSceneConnectorLayoutController::applyConnectorNudging(
    LzScene* scene,
    const QList<LzConnectLine*>& seedLines) const
{
    QList<LzConnectLine*> lines = expandNudgingScope(scene, seedLines);
    if (lines.size() < 2)
        return false;

    QList<QVector<QPointF>> paths;
    for (LzConnectLine* line : lines)
        paths.append(line->pathInSceneCoords());

    const QList<QVector<QPointF>> nudged =
        LzConnectorAlgorithm::computeNudgingOffsets(paths, 6.0, 10.0);
    if (nudged.size() != lines.size())
        return false;

    bool changed = false;
    LzAStar astar;
    for (int i = 0; i < lines.size(); ++i)
    {
        QList<QPoint> nudgedInt;
        nudgedInt.reserve(nudged[i].size());
        for (const QPointF& point : nudged[i])
            nudgedInt.append(point.toPoint());

        const QRectF queryRect = lines[i]->sceneBoundingRect().adjusted(-40.0, -40.0, 40.0, 40.0);
        const QList<QGraphicsItem*> obstacles =
            LzConnectLine::collectRoutingObstacles(lines[i], scene, queryRect);
        if (astar.pathSegmentsIntersectObstacles(nudgedInt, obstacles))
            continue;

        if (nudged[i] != paths[i])
        {
            lines[i]->setPathFromScenePoints(nudged[i]);
            changed = true;
        }
    }
    return changed;
}
