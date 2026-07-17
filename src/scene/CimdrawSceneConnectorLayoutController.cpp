#include "CimdrawSceneConnectorLayoutController.h"

#include <QSet>

#include "Item/CimdrawConnectLine.h"
#include "CimdrawScene.h"
#include "algorithm/CimdrawAStar.h"
#include "algorithm/CimdrawConnectorAlgorithm.h"

namespace {

QList<CimdrawConnectLine*> connectorLinesFromScene(CimdrawScene* scene)
{
    QList<CimdrawConnectLine*> lines;
    if (!scene)
        return lines;

    for (QGraphicsItem* item : scene->items())
    {
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
            lines.append(line);
    }
    return lines;
}

}

QList<CimdrawConnectLine*> CimdrawSceneConnectorLayoutController::expandNudgingScope(
    CimdrawScene* scene,
    const QList<CimdrawConnectLine*>& seedLines) const
{
    if (!scene)
        return {};
    if (seedLines.isEmpty())
        return connectorLinesFromScene(scene);

    QSet<CimdrawConnectLine*> scope;
    QRectF interest;
    bool hasInterest = false;
    for (CimdrawConnectLine* line : seedLines)
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
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
            scope.insert(line);
    }
    return scope.values();
}

bool CimdrawSceneConnectorLayoutController::applyConnectorNudging(
    CimdrawScene* scene,
    const QList<CimdrawConnectLine*>& seedLines) const
{
    QList<CimdrawConnectLine*> lines = expandNudgingScope(scene, seedLines);
    if (lines.size() < 2)
        return false;

    QList<QVector<QPointF>> paths;
    for (CimdrawConnectLine* line : lines)
        paths.append(line->pathInSceneCoords());

    const QList<QVector<QPointF>> nudged =
        CimdrawConnectorAlgorithm::computeNudgingOffsets(paths, 6.0, 10.0);
    if (nudged.size() != lines.size())
        return false;

    bool changed = false;
    CimdrawAStar astar;
    for (int i = 0; i < lines.size(); ++i)
    {
        QList<QPoint> nudgedInt;
        nudgedInt.reserve(nudged[i].size());
        for (const QPointF& point : nudged[i])
            nudgedInt.append(point.toPoint());

        const QRectF queryRect = lines[i]->sceneBoundingRect().adjusted(-40.0, -40.0, 40.0, 40.0);
        const QList<QGraphicsItem*> obstacles =
            CimdrawConnectLine::collectRoutingObstacles(lines[i], scene, queryRect);
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
