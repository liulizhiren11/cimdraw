#include "CimdrawConnectorAlgorithm.h"
#include "CimdrawAStar.h"
#include "CimdrawVisibilityGraph.h"
#include <QtMath>
#include <algorithm>
#include <cmath>

CimdrawConnectorAlgorithm::InsertWaypointResult CimdrawConnectorAlgorithm::projectPointOnPolyline(
    const QVector<QPointF>& polyline,
    const QPointF& point)
{
    InsertWaypointResult out;
    if (polyline.size() < 2)
        return out;
    qreal minDist = 1e10;
    for (int i = 0; i < polyline.size() - 1; ++i)
    {
        const QPointF a = polyline[i];
        const QPointF b = polyline[i + 1];
        QPointF ab = b - a;
        qreal len = qSqrt(ab.x() * ab.x() + ab.y() * ab.y());
        if (len < 1e-6)
            continue;
        QPointF ap = point - a;
        qreal t = qBound(qreal(0), (ap.x() * ab.x() + ap.y() * ab.y()) / (len * len), qreal(1));
        QPointF proj = a + t * ab;
        qreal dist = (point - proj).manhattanLength();
        if (dist < minDist)
        {
            minDist = dist;
            out.segmentIndex = i;
            out.projected = proj;
            out.valid = true;
        }
    }
    return out;
}

QPointF CimdrawConnectorAlgorithm::orthogonalCornerPosition(
    const QPointF& prev,
    const QPointF& next,
    const QPointF& cursorPosition)
{
    QPointF pos1(prev.x(), next.y());
    QPointF pos2(next.x(), prev.y());
    qreal d1 = (cursorPosition - pos1).manhattanLength();
    qreal d2 = (cursorPosition - pos2).manhattanLength();
    return (d1 <= d2) ? pos1 : pos2;
}

bool CimdrawConnectorAlgorithm::preferHorizontalFirst(const QPointF& start, const QPointF& end)
{
    qreal dx = qAbs(end.x() - start.x());
    qreal dy = qAbs(end.y() - start.y());
    return dx >= dy;
}

static QList<QGraphicsItem*> filterObstaclesNearSegment(const QList<QGraphicsItem*>& obstacles,
                                                        const QPointF& a, const QPointF& b, qreal padPx)
{
    QRectF box(a, b);
    box = box.normalized().adjusted(-padPx, -padPx, padPx, padPx);
    QList<QGraphicsItem*> out;
    out.reserve(obstacles.size());
    for (QGraphicsItem* it : obstacles)
    {
        if (it && it->isVisible() && it->sceneBoundingRect().intersects(box))
            out.append(it);
    }
    return out;
}

static bool polylineSegmentsClear(const QList<QPoint>& path, CimdrawAStar& astar,
                                  const QList<QGraphicsItem*>& obstacles)
{
    for (int i = 0; i + 1 < path.size(); ++i)
    {
        if (astar.segmentIntersectsObstacles(path[i], path[i + 1], obstacles))
            return false;
    }
    return path.size() >= 2;
}

static int manhattanPathLength(const QList<QPoint>& path)
{
    int len = 0;
    for (int i = 1; i < path.size(); ++i)
        len += qAbs(path[i].x() - path[i - 1].x()) + qAbs(path[i].y() - path[i - 1].y());
    return len;
}

/// draw.io 风格：先试直线/L 形正交路径，不穿障则立即返回，避免无谓 A* 与多轮后处理
static QVector<QPoint> tryFastDrawIoOrthogonalPath(const QPoint& start, const QPoint& end,
                                                 const QPointF& startScene, const QPointF& endScene,
                                                 const QList<QGraphicsItem*>& obstacles, CimdrawAStar& astar)
{
    QList<QList<QPoint>> candidates;
    if (start.x() == end.x() || start.y() == end.y())
    {
        candidates.append({start, end});
    }
    else
    {
        const QPoint hCorner(end.x(), start.y());
        const QPoint vCorner(start.x(), end.y());
        if (CimdrawConnectorAlgorithm::preferHorizontalFirst(startScene, endScene))
        {
            candidates.append({start, hCorner, end});
            candidates.append({start, vCorner, end});
        }
        else
        {
            candidates.append({start, vCorner, end});
            candidates.append({start, hCorner, end});
        }
    }

    QList<QPoint> best;
    int bestLen = INT_MAX;
    for (const QList<QPoint>& cand : candidates)
    {
        if (!polylineSegmentsClear(cand, astar, obstacles))
            continue;
        const int len = manhattanPathLength(cand);
        if (len < bestLen)
        {
            bestLen = len;
            best = cand;
        }
    }
    if (best.isEmpty())
        return {};
    QVector<QPoint> out;
    out.reserve(best.size());
    for (const QPoint& p : best)
        out.append(p);
    return out;
}

/// 不做避障的最简正交路径（一段或两段）。仅作最终兜底。
static QVector<QPoint> simpleOrthogonalPathAllowThrough(const QPoint& startPt, const QPoint& endPt,
    const QPointF& startScene, const QPointF& endScene)
{
    if (startPt == endPt)
        return QVector<QPoint>() << startPt;
    QVector<QPoint> out;
    out.append(startPt);
    if (startPt.x() == endPt.x() || startPt.y() == endPt.y())
    {
        out.append(endPt);
        return out;
    }
    if (CimdrawConnectorAlgorithm::preferHorizontalFirst(startScene, endScene))
        out.append(QPoint(endPt.x(), startPt.y()));
    else
        out.append(QPoint(startPt.x(), endPt.y()));
    out.append(endPt);
    return out;
}

static int countPathObstacleHits(const QList<QPoint>& path, CimdrawAStar& astar,
                                 const QList<QGraphicsItem*>& obstacles)
{
    int hits = 0;
    for (int i = 0; i + 1 < path.size(); ++i)
    {
        if (astar.segmentIntersectsObstacles(path[i], path[i + 1], obstacles))
            ++hits;
    }
    return hits;
}

/// 在 L 形候选与细网格 A* 中选穿障最少的路径，避免直接退回「允许穿障」直线
static QVector<QPoint> simpleOrthogonalPathBestEffort(const QPoint& startPt, const QPoint& endPt,
    const QPointF& startScene, const QPointF& endScene,
    const QList<QGraphicsItem*>& obstacles)
{
    if (startPt == endPt)
        return QVector<QPoint>() << startPt;

    CimdrawAStar astar;
    astar.setGridStep(StepSize);

    QList<QList<QPoint>> candidates;
    if (startPt.x() == endPt.x() || startPt.y() == endPt.y())
        candidates.append({startPt, endPt});
    else
    {
        const QPoint hCorner(endPt.x(), startPt.y());
        const QPoint vCorner(startPt.x(), endPt.y());
        if (CimdrawConnectorAlgorithm::preferHorizontalFirst(startScene, endScene))
        {
            candidates.append({startPt, hCorner, endPt});
            candidates.append({startPt, vCorner, endPt});
        }
        else
        {
            candidates.append({startPt, vCorner, endPt});
            candidates.append({startPt, hCorner, endPt});
        }
    }

    QList<QPoint> best;
    int bestHits = INT_MAX;
    int bestLen = INT_MAX;
    for (const QList<QPoint>& cand : candidates)
    {
        const int hits = countPathObstacleHits(cand, astar, obstacles);
        const int len = manhattanPathLength(cand);
        if (hits < bestHits || (hits == bestHits && len < bestLen))
        {
            bestHits = hits;
            bestLen = len;
            best = cand;
        }
    }
    if (bestHits == 0)
    {
        QVector<QPoint> out;
        out.reserve(best.size());
        for (const QPoint& p : best)
            out.append(p);
        return out;
    }

    CimdrawAStar fineAstar;
    fineAstar.setGridStep(MinGridStep);
    auto floorAlign = [](const QPoint& p, int s) {
        auto f = [s](int v) { return int(std::floor(v / double(s))) * s; };
        return QPoint(f(p.x()), f(p.y()));
    };
    const QList<QPoint> finePath = fineAstar.aStarWithDirectionPenalty(
        floorAlign(startPt, MinGridStep), floorAlign(endPt, MinGridStep), obstacles);
    if (finePath.size() >= 2
        && !fineAstar.pathSegmentsIntersectObstacles(finePath, obstacles))
    {
        QVector<QPoint> out;
        out.reserve(finePath.size());
        for (const QPoint& p : finePath)
            out.append(p);
        return out;
    }

    if (!best.isEmpty())
    {
        QVector<QPoint> out;
        out.reserve(best.size());
        for (const QPoint& p : best)
            out.append(p);
        return out;
    }
    return simpleOrthogonalPathAllowThrough(startPt, endPt, startScene, endScene);
}

// 从 startPt 到 pathStart 选一个不穿障碍的正交拐点；若两种 L 都穿则仍返回其一以保持正交，由路径校验触发回退
static QPoint chooseStartIntermediate(const QPoint& startPt, const QPoint& pathStart,
    const QList<QGraphicsItem*>& obstacles, CimdrawAStar& astar)
{
    QPoint delta = pathStart - startPt;
    if (delta.x() == 0 || delta.y() == 0)
        return pathStart;
    QPoint horFirst(startPt.x() + delta.x(), startPt.y());
    QPoint verFirst(startPt.x(), startPt.y() + delta.y());
    bool horOk = !astar.segmentIntersectsObstacles(startPt, horFirst, obstacles)
        && !astar.segmentIntersectsObstacles(horFirst, pathStart, obstacles);
    bool verOk = !astar.segmentIntersectsObstacles(startPt, verFirst, obstacles)
        && !astar.segmentIntersectsObstacles(verFirst, pathStart, obstacles);
    if (horOk && !verOk) return horFirst;
    if (verOk && !horOk) return verFirst;
    if (horOk && verOk) return (qAbs(delta.x()) >= qAbs(delta.y())) ? horFirst : verFirst;
    int badH = int(astar.segmentIntersectsObstacles(startPt, horFirst, obstacles))
        + int(astar.segmentIntersectsObstacles(horFirst, pathStart, obstacles));
    int badV = int(astar.segmentIntersectsObstacles(startPt, verFirst, obstacles))
        + int(astar.segmentIntersectsObstacles(verFirst, pathStart, obstacles));
    return (badH <= badV) ? horFirst : verFirst;
}

static QPoint chooseEndIntermediate(const QPoint& pathEnd, const QPoint& endPt,
    const QList<QGraphicsItem*>& obstacles, CimdrawAStar& astar)
{
    QPoint delta = endPt - pathEnd;
    if (delta.x() == 0 || delta.y() == 0)
        return pathEnd;
    QPoint horFirst(pathEnd.x() + delta.x(), pathEnd.y());
    QPoint verFirst(pathEnd.x(), pathEnd.y() + delta.y());
    bool horOk = !astar.segmentIntersectsObstacles(pathEnd, horFirst, obstacles)
        && !astar.segmentIntersectsObstacles(horFirst, endPt, obstacles);
    bool verOk = !astar.segmentIntersectsObstacles(pathEnd, verFirst, obstacles)
        && !astar.segmentIntersectsObstacles(verFirst, endPt, obstacles);
    if (horOk && !verOk) return horFirst;
    if (verOk && !horOk) return verFirst;
    if (horOk && verOk) return (qAbs(delta.x()) >= qAbs(delta.y())) ? horFirst : verFirst;
    int badH = int(astar.segmentIntersectsObstacles(pathEnd, horFirst, obstacles))
        + int(astar.segmentIntersectsObstacles(horFirst, endPt, obstacles));
    int badV = int(astar.segmentIntersectsObstacles(pathEnd, verFirst, obstacles))
        + int(astar.segmentIntersectsObstacles(verFirst, endPt, obstacles));
    return (badH <= badV) ? horFirst : verFirst;
}

/// 对角相邻两点间插入 L 形拐点：优先选不穿障的；两种都穿时选「穿障段数」更少的一侧（避免此前双 false 仍强行选边导致横穿障碍）
static QPoint pickOrthogonalMid(const QPoint& prev, const QPoint& curr,
    const QList<QGraphicsItem*>& obstacles, CimdrawAStar& astar)
{
    const int dx = curr.x() - prev.x();
    const int dy = curr.y() - prev.y();
    const QPoint mid1(prev.x() + dx, prev.y());
    const QPoint mid2(prev.x(), prev.y() + dy);
    const bool clear1 = !astar.segmentIntersectsObstacles(prev, mid1, obstacles)
        && !astar.segmentIntersectsObstacles(mid1, curr, obstacles);
    const bool clear2 = !astar.segmentIntersectsObstacles(prev, mid2, obstacles)
        && !astar.segmentIntersectsObstacles(mid2, curr, obstacles);
    if (clear1 && !clear2) return mid1;
    if (clear2 && !clear1) return mid2;
    if (clear1 && clear2) return (qAbs(dx) >= qAbs(dy)) ? mid1 : mid2;
    const int bad1 = int(astar.segmentIntersectsObstacles(prev, mid1, obstacles))
        + int(astar.segmentIntersectsObstacles(mid1, curr, obstacles));
    const int bad2 = int(astar.segmentIntersectsObstacles(prev, mid2, obstacles))
        + int(astar.segmentIntersectsObstacles(mid2, curr, obstacles));
    return (bad1 <= bad2) ? mid1 : mid2;
}

static QList<QPoint> expandDiagonalSegments(const QList<QPoint>& path, const QList<QGraphicsItem*>& obstacles,
                                            CimdrawAStar& astar)
{
    if (path.size() < 2)
        return path;
    QList<QPoint> expanded;
    expanded.append(path.first());
    for (int i = 1; i < path.size(); ++i)
    {
        QPoint prev = expanded.last();
        QPoint curr = path[i];
        const QPoint delta = curr - prev;
        if (delta.x() != 0 && delta.y() != 0)
        {
            const QPoint mid = pickOrthogonalMid(prev, curr, obstacles, astar);
            if (mid != prev && mid != curr)
                expanded.append(mid);
        }
        if (curr != expanded.last())
            expanded.append(curr);
    }
    return expanded;
}

// 合并过近的连续点，减少控制点数量（首尾必保留）
static QList<QPoint> removeNearDuplicatePoints(const QList<QPoint>& path, int minDistPx)
{
    if (path.size() <= 2)
        return path;
    QList<QPoint> out;
    out.append(path.first());
    for (int i = 1; i < path.size() - 1; ++i)
    {
        QPoint prev = out.last();
        QPoint curr = path[i];
        qreal dist = qSqrt(qreal((curr.x() - prev.x()) * (curr.x() - prev.x()) + (curr.y() - prev.y()) * (curr.y() - prev.y())));
        if (dist >= minDistPx)
            out.append(curr);
    }
    out.append(path.last());
    return out;
}

// 修剪首尾：去掉距离 startPt/endPt 过近的路径点，避免 A* 在狭窄处产生缠绕/螺旋
static void trimPathEnd(QList<QPoint>& path, const QPoint& endPt, int marginPx)
{
    if (path.size() < 2)
        return;
    auto dist = [](const QPoint& a, const QPoint& b) {
        return qSqrt(qreal((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y())));
    };
    while (path.size() >= 2 && dist(path.last(), endPt) < marginPx)
        path.removeLast();
}
static void trimPathStart(QList<QPoint>& path, const QPoint& startPt, int marginPx)
{
    if (path.size() < 2)
        return;
    auto dist = [](const QPoint& a, const QPoint& b) {
        return qSqrt(qreal((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y())));
    };
    while (path.size() >= 2 && dist(path.first(), startPt) < marginPx)
        path.removeAt(0);
}

// 收缩首尾：强制首/尾为连接点，去掉与首/尾极近的中间点，避免起点/终点处控制点扎堆
static QList<QPoint> collapseEndpoints(const QList<QPoint>& path, const QPoint& startPt, const QPoint& endPt, qreal epsilonPx)
{
    if (path.size() <= 2)
        return path;
    auto dist = [](const QPoint& a, const QPoint& b) {
        return qSqrt(qreal((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y())));
    };
    int i = 1;
    while (i < path.size() - 1 && dist(path[i], startPt) < epsilonPx)
        ++i;
    int j = path.size() - 1;
    while (j > i && dist(path[j - 1], endPt) < epsilonPx)
        --j;
    QList<QPoint> out;
    out.append(startPt);
    for (int k = i; k < j; ++k)
        out.append(path[k]);
    out.append(endPt);
    return out;
}

// 保证路径严格正交：对每对形成对角线的相邻点插入正交拐点，使每段均为水平或垂直
// 首尾段在两种 L 均安全时优先选用离 startPt/endPt 较远的拐点；否则按避障与 pickOrthogonalMid 一致
static QList<QPoint> ensureStrictOrthogonal(const QList<QPoint>& path, const QPoint& startPt, const QPoint& endPt,
    const QList<QGraphicsItem*>& obstacles, CimdrawAStar& astar)
{
    if (path.size() < 2)
        return path;
    auto dist = [](const QPoint& a, const QPoint& b) {
        return qSqrt(qreal((a.x() - b.x()) * (a.x() - b.x()) + (a.y() - b.y()) * (a.y() - b.y())));
    };
    const qreal minGap = 20.0; // 首尾拐点与连接点的最小距离，避免两点扎堆
    QList<QPoint> out;
    out.append(path.first());
    for (int i = 1; i < path.size(); ++i)
    {
        QPoint prev = out.last();
        QPoint curr = path[i];
        int dx = curr.x() - prev.x();
        int dy = curr.y() - prev.y();
        if (dx != 0 && dy != 0)
        {
            QPoint mid1(prev.x() + dx, prev.y());
            QPoint mid2(prev.x(), prev.y() + dy);
            const bool clear1 = !astar.segmentIntersectsObstacles(prev, mid1, obstacles)
                && !astar.segmentIntersectsObstacles(mid1, curr, obstacles);
            const bool clear2 = !astar.segmentIntersectsObstacles(prev, mid2, obstacles)
                && !astar.segmentIntersectsObstacles(mid2, curr, obstacles);
            QPoint mid;
            if (clear1 && !clear2) mid = mid1;
            else if (clear2 && !clear1) mid = mid2;
            else if (clear1 && clear2)
            {
                const bool isFirstSeg = (prev == path.first());
                const bool isLastSeg = (curr == path.last());
                if (isFirstSeg)
                {
                    const qreal d1 = dist(mid1, startPt);
                    const qreal d2 = dist(mid2, startPt);
                    mid = (d1 >= d2 && d1 >= minGap) ? mid1 : ((d2 >= minGap) ? mid2 : (d1 >= d2 ? mid1 : mid2));
                }
                else if (isLastSeg)
                {
                    const qreal d1 = dist(mid1, endPt);
                    const qreal d2 = dist(mid2, endPt);
                    mid = (d1 >= d2 && d1 >= minGap) ? mid1 : ((d2 >= minGap) ? mid2 : (d1 >= d2 ? mid1 : mid2));
                }
                else
                    mid = (qAbs(dx) >= qAbs(dy)) ? mid1 : mid2;
            }
            else
                mid = pickOrthogonalMid(prev, curr, obstacles, astar);
            if (mid != prev && mid != curr)
                out.append(mid);
        }
        if (curr != out.last())
            out.append(curr);
    }
    return out;
}

static void pathPriorityParams(CimdrawConnectorAlgorithm::PathPriorityMode priority,
                                int* outTurnPenalty, qreal* outProximityScale)
{
    int turnPenalty = 5;
    qreal proximityScale = 1.0;
    switch (priority)
    {
    case CimdrawConnectorAlgorithm::PathShortest:    turnPenalty = 2;  proximityScale = 0.5; break;
    case CimdrawConnectorAlgorithm::PathFewerBends:  turnPenalty = 15; proximityScale = 1.0; break;
    case CimdrawConnectorAlgorithm::PathBalanced:    turnPenalty = 5;  proximityScale = 1.0; break;
    }
    *outTurnPenalty = turnPenalty;
    *outProximityScale = proximityScale;
}

QVector<QPoint> CimdrawConnectorAlgorithm::planOrthogonalPathQuick(
    const QPointF& startScene,
    const QPointF& endScene,
    const QList<QGraphicsItem*>& obstacles)
{
    const QPoint startPt = startScene.toPoint();
    const QPoint endPt = endScene.toPoint();
    const QList<QGraphicsItem*> routeObstacles =
        filterObstaclesNearSegment(obstacles, startScene, endScene, qreal(ObstacleClearanceMargin * 4));

    CimdrawAStar astar;
    astar.setGridStep(StepSize);
    const QVector<QPoint> fastPath =
        tryFastDrawIoOrthogonalPath(startPt, endPt, startScene, endScene, routeObstacles, astar);
    if (!fastPath.isEmpty())
        return fastPath;

    auto floorAlign = [](const QPoint& p, int s) {
        auto f = [s](int v) { return int(std::floor(v / double(s))) * s; };
        return QPoint(f(p.x()), f(p.y()));
    };
    const QPoint alignedStart = floorAlign(startPt, StepSize);
    const QPoint alignedEnd = floorAlign(endPt, StepSize);
    astar.setMaxSearchIterations(6000);
    QList<QPoint> path = astar.aStarWithDirectionPenalty(alignedStart, alignedEnd, routeObstacles, 5, 1.0);
    astar.setMaxSearchIterations(0);
    if (path.isEmpty())
        return simpleOrthogonalPathBestEffort(startPt, endPt, startScene, endScene, routeObstacles);

    QList<QPoint> ortho = astar.simplifyOrthogonalPath(path);
    if (ortho.size() >= 2 && !astar.pathSegmentsIntersectObstacles(ortho, routeObstacles))
    {
        QVector<QPoint> out;
        out.reserve(ortho.size());
        for (const QPoint& p : ortho)
            out.append(p);
        return out;
    }

    QList<QPoint> pulled = astar.simplifyPathByStringPulling(ortho, routeObstacles);
    if (pulled.size() >= 2 && !astar.pathSegmentsIntersectObstacles(pulled, routeObstacles))
    {
        QVector<QPoint> out;
        out.reserve(pulled.size());
        for (const QPoint& p : pulled)
            out.append(p);
        return out;
    }

    return simpleOrthogonalPathBestEffort(startPt, endPt, startScene, endScene, routeObstacles);
}

QVector<QPoint> CimdrawConnectorAlgorithm::planOrthogonalPath(
    const QPointF& startScene,
    const QPointF& endScene,
    const QList<QGraphicsItem*>& obstacles,
    ConnectorRoutingStrategy strategy,
    CimdrawConnectorAlgorithm::PathPriorityMode priority)
{
    // 优先级：1) 严格正交  2) 避障  3) 最短路径
    int turnPenalty;
    qreal proximityScale;
    pathPriorityParams(priority, &turnPenalty, &proximityScale);

    QPoint startPt = startScene.toPoint();
    QPoint endPt = endScene.toPoint();

    const QList<QGraphicsItem*> routeObstacles =
        filterObstaclesNearSegment(obstacles, startScene, endScene, qreal(ObstacleClearanceMargin * 4));

    CimdrawAStar quickAstar;
    quickAstar.setGridStep(StepSize);
    const QVector<QPoint> fastPath =
        tryFastDrawIoOrthogonalPath(startPt, endPt, startScene, endScene, routeObstacles, quickAstar);
    if (!fastPath.isEmpty())
        return fastPath;

    const int kStepSequence[] = { StepSize, 10, MinGridStep };
    const int kStepCount = int(sizeof(kStepSequence) / sizeof(kStepSequence[0]));
    ConnectorRoutingStrategy effStrategy = strategy;
    if (obstacles.size() > 35 || effStrategy != VisibilityGraph)
        effStrategy = GridAStar;
    const int effStepCount = kStepCount;

    for (int stepIdx = 0; stepIdx < effStepCount; ++stepIdx)
    {
        const int gridStep = kStepSequence[stepIdx];
        CimdrawAStar astar;
        astar.setGridStep(gridStep);
        auto floorAlign = [](const QPoint& p, int s) {
            auto f = [s](int v) { return int(std::floor(v / double(s))) * s; };
            return QPoint(f(p.x()), f(p.y()));
        };
        QPoint alignedStart = floorAlign(startPt, gridStep);
        QPoint alignedEnd = floorAlign(endPt, gridStep);

        QList<QPoint> path;
        if (effStrategy == VisibilityGraph && routeObstacles.size() <= 25)
        {
            path = CimdrawVisibilityGraph::planPath(startScene, endScene, routeObstacles, &astar, turnPenalty, gridStep);
            if (path.size() < 2)
                path.clear();
        }
        if (path.isEmpty())
            path = astar.aStarWithDirectionPenalty(alignedStart, alignedEnd, routeObstacles, turnPenalty,
                                                     proximityScale);

        if (path.isEmpty())
        {
            if (stepIdx + 1 >= effStepCount)
                return simpleOrthogonalPathBestEffort(startPt, endPt, startScene, endScene, routeObstacles);
            continue;
        }

    QList<QPoint> orthoPath = expandDiagonalSegments(astar.simplifyOrthogonalPath(path), routeObstacles, astar);
    QList<QPoint> pulled = astar.simplifyPathByStringPulling(orthoPath, routeObstacles);
    if (pulled.size() >= 2 && pulled.size() < orthoPath.size()
        && !astar.pathSegmentsIntersectObstacles(pulled, routeObstacles))
        orthoPath = expandDiagonalSegments(pulled, routeObstacles, astar);
    QList<QPoint> optimizedPath = astar.optimizePathForVisioStyle(orthoPath, alignedStart, alignedEnd, routeObstacles);
    optimizedPath = expandDiagonalSegments(optimizedPath, routeObstacles, astar);
    QList<QPoint> finalPath = optimizedPath;

    // 修剪首尾：去掉距连接点过近的路径点，避免终点/起点附近出现缠绕、螺旋或贴图元
    QList<QPoint> trimmed = optimizedPath;
    trimPathEnd(trimmed, endPt, ObstacleClearanceMargin);
    trimPathStart(trimmed, startPt, ObstacleClearanceMargin);
    if (trimmed.size() >= 2)
        finalPath = trimmed;

    // 保留仅做共线简化的路径，用于最终若仍穿过图元时的回退
    QList<QPoint> orthoOnly = orthoPath;

    if (finalPath.isEmpty())
    {
        finalPath.append(startPt);
        if (startPt.x() != endPt.x() && startPt.y() != endPt.y())
        {
            if (preferHorizontalFirst(startScene, endScene))
                finalPath.append(QPoint(endPt.x(), startPt.y()));
            else
                finalPath.append(QPoint(startPt.x(), endPt.y()));
        }
        finalPath.append(endPt);
    }
    else
    {
        QPoint pathStart = finalPath.first();
        if (startPt != pathStart)
        {
            QPoint delta = pathStart - startPt;
            if (delta.x() == 0 || delta.y() == 0)
                finalPath[0] = startPt;
            else
            {
                QPoint intermediate = chooseStartIntermediate(startPt, pathStart, obstacles, astar);
                bool tooClose = (qAbs(intermediate.x() - startPt.x()) + qAbs(intermediate.y() - startPt.y()) <= 4)
                    || (qAbs(intermediate.x() - pathStart.x()) + qAbs(intermediate.y() - pathStart.y()) <= 4);
                if (!tooClose && intermediate != pathStart && intermediate != startPt)
                {
                    finalPath.prepend(intermediate);
                    finalPath.prepend(startPt);
                }
                else
                    finalPath[0] = startPt;
            }
        }
        else
            finalPath[0] = startPt;
        QPoint pathEnd = finalPath.last();
        if (endPt != pathEnd)
        {
            QPoint delta = endPt - pathEnd;
            if (delta.x() == 0 || delta.y() == 0)
            {
                // 共线时也需保证到 endPt 的线段不穿图元，否则不延伸
                if (!astar.segmentIntersectsObstacles(pathEnd, endPt, obstacles))
                    finalPath[finalPath.size() - 1] = endPt;
            }
            else
            {
                QPoint intermediate = chooseEndIntermediate(pathEnd, endPt, obstacles, astar);
                bool tooClose = (qAbs(intermediate.x() - pathEnd.x()) + qAbs(intermediate.y() - pathEnd.y()) <= 4)
                    || (qAbs(intermediate.x() - endPt.x()) + qAbs(intermediate.y() - endPt.y()) <= 4);
                if (!tooClose && intermediate != pathEnd && intermediate != endPt)
                {
                    finalPath.append(intermediate);
                    // 仅当从 intermediate 到 endPt 的线段不穿任何图元时才延伸到 endPt，避免横穿图元
                    if (!astar.segmentIntersectsObstacles(intermediate, endPt, obstacles))
                        finalPath.append(endPt);
                }
                else if (!astar.segmentIntersectsObstacles(pathEnd, endPt, obstacles))
                    finalPath[finalPath.size() - 1] = endPt;
            }
        }
        else
            finalPath[finalPath.size() - 1] = endPt;
    }

    QList<QPoint> strictOrthoPath;
    if (finalPath.size() >= 2)
    {
        strictOrthoPath.append(finalPath.first());
        for (int i = 1; i < finalPath.size(); ++i)
        {
            QPoint prev = strictOrthoPath.last();
            QPoint curr = finalPath[i];
            QPoint delta = curr - prev;
            if (delta.x() != 0 && delta.y() != 0)
            {
                const QPoint mid = pickOrthogonalMid(prev, curr, obstacles, astar);
                if (mid != prev && mid != curr)
                    strictOrthoPath.append(mid);
            }
            if (curr != strictOrthoPath.last())
                strictOrthoPath.append(curr);
        }
    }
    else
        strictOrthoPath = finalPath;

    int prevSize = strictOrthoPath.size();
    for (int it = 0; it < 5; ++it)
    {
        strictOrthoPath = astar.simplifyOrthogonalPath(strictOrthoPath);
        if (strictOrthoPath.size() == prevSize)
            break;
        prevSize = strictOrthoPath.size();
    }
    QList<QPoint> pathBeforeMinimal = strictOrthoPath;
    strictOrthoPath = astar.simplifyToMinimalPath(strictOrthoPath);
    if (astar.pathSegmentsIntersectObstacles(strictOrthoPath, obstacles))
        strictOrthoPath = pathBeforeMinimal;  // 避障优先：简化后若穿障则回退

    {
        QList<QPoint> pulledStrict = astar.simplifyPathByStringPulling(strictOrthoPath, routeObstacles);
        if (pulledStrict.size() >= 2 && pulledStrict.size() < strictOrthoPath.size()
            && !astar.pathSegmentsIntersectObstacles(pulledStrict, routeObstacles))
            strictOrthoPath = expandDiagonalSegments(pulledStrict, routeObstacles, astar);
    }

    // 最终校验：若仍穿过图元，回退到仅做共线简化的路径（不做 Visio 优化和最小化），保证不穿图元
    if (astar.pathSegmentsIntersectObstacles(strictOrthoPath, obstacles) && !orthoOnly.isEmpty())
    {
        QPoint pathStart = orthoOnly.first();
        QPoint pathEnd = orthoOnly.last();
        QList<QPoint> fallback;
        fallback.append(startPt);
        if (pathStart != startPt)
        {
            QPoint delta = pathStart - startPt;
            if (delta.x() != 0 && delta.y() != 0)
            {
                QPoint intermediate = chooseStartIntermediate(startPt, pathStart, obstacles, astar);
                bool tooClose = (qAbs(intermediate.x() - startPt.x()) + qAbs(intermediate.y() - startPt.y()) <= 4)
                    || (qAbs(intermediate.x() - pathStart.x()) + qAbs(intermediate.y() - pathStart.y()) <= 4);
                if (!tooClose && intermediate != startPt && intermediate != pathStart)
                    fallback.append(intermediate);
            }
            fallback.append(pathStart);
        }
        for (int i = 1; i < orthoOnly.size() - 1; ++i)
            fallback.append(orthoOnly[i]);
        if (pathEnd != endPt)
        {
            fallback.append(pathEnd);
            QPoint delta = endPt - pathEnd;
            if (delta.x() != 0 && delta.y() != 0)
            {
                QPoint intermediate = chooseEndIntermediate(pathEnd, endPt, obstacles, astar);
                bool tooClose = (qAbs(intermediate.x() - pathEnd.x()) + qAbs(intermediate.y() - pathEnd.y()) <= 4)
                    || (qAbs(intermediate.x() - endPt.x()) + qAbs(intermediate.y() - endPt.y()) <= 4);
                if (!tooClose && intermediate != pathEnd && intermediate != endPt)
                    fallback.append(intermediate);
            }
        }
        fallback.append(endPt);
        // 将 fallback 转为严格正交（补对角线中点）
        strictOrthoPath.clear();
        if (fallback.size() >= 2)
        {
            strictOrthoPath.append(fallback.first());
            for (int i = 1; i < fallback.size(); ++i)
            {
                QPoint prev = strictOrthoPath.last();
                QPoint curr = fallback[i];
                QPoint delta = curr - prev;
                if (delta.x() != 0 && delta.y() != 0)
                {
                    const QPoint mid = pickOrthogonalMid(prev, curr, obstacles, astar);
                    if (mid != prev && mid != curr)
                        strictOrthoPath.append(mid);
                }
                strictOrthoPath.append(curr);
            }
        }
        else
            strictOrthoPath = fallback;
        // 若 fallback 仍穿图元则放弃 fallback，保留之前的 pathBeforeMinimal
        if (astar.pathSegmentsIntersectObstacles(strictOrthoPath, obstacles))
            strictOrthoPath = pathBeforeMinimal;
    }

    if (strictOrthoPath.count() < 2)
    {
        // 严格正交：两点时若未共线则补拐点，绝不输出对角线
        QPoint a = startPt;
        QPoint b = endPt;
        if (a.x() == b.x() || a.y() == b.y())
            return QVector<QPoint>() << a << b;
        return simpleOrthogonalPathBestEffort(a, b, QPointF(a), QPointF(b), obstacles);
    }

    // 多次共线简化直到稳定，尽量少控制点
    for (int it = 0; it < 3; ++it)
    {
        QList<QPoint> next = astar.simplifyOrthogonalPath(strictOrthoPath);
        if (next.size() >= strictOrthoPath.size())
            break;
        if (astar.pathSegmentsIntersectObstacles(next, routeObstacles))
            break;
        strictOrthoPath = next;
    }
    // 合并过近的连续点（放宽到 2*gridStep，进一步减少控制点）
    QList<QPoint> merged = removeNearDuplicatePoints(strictOrthoPath, 2 * gridStep);
    if (merged.size() >= 2 && !astar.pathSegmentsIntersectObstacles(merged, obstacles))
        strictOrthoPath = merged;
    // 首尾收缩：强制首/尾为连接点，去掉与首/尾 20px 内的中间点，使首尾各只保留一个控制点
    static const qreal kEndpointsCollapsePx = 20.0;
    QList<QPoint> collapsed = collapseEndpoints(strictOrthoPath, startPt, endPt, kEndpointsCollapsePx);
    if (collapsed.size() >= 2 && !astar.pathSegmentsIntersectObstacles(collapsed, obstacles))
        strictOrthoPath = collapsed;

    // 收缩后首/尾可能与下一段形成对角线，补正交拐点，保证严格正交（首尾优先选离连接点较远的拐点）
    QList<QPoint> ortho = ensureStrictOrthogonal(strictOrthoPath, startPt, endPt, obstacles, astar);
    if (ortho.size() >= 2 && !astar.pathSegmentsIntersectObstacles(ortho, obstacles))
        strictOrthoPath = ortho;

    // 统一简化整条路径：再去掉中途共线点与过近点，使首尾+中途控制点整体最少
    for (int it = 0; it < 3; ++it)
    {
        QList<QPoint> next = astar.simplifyOrthogonalPath(strictOrthoPath);
        if (next.size() >= strictOrthoPath.size())
            break;
        if (astar.pathSegmentsIntersectObstacles(next, routeObstacles))
            break;
        strictOrthoPath = next;
    }
    // 最终再拉线一次，多轮直到稳定，尽量跳过可直达的中间点
    QList<QPoint> finalPulled = astar.simplifyPathByStringPulling(strictOrthoPath, obstacles);
    for (int pass = 0; pass < 2; ++pass)
    {
        QList<QPoint> next = astar.simplifyPathByStringPulling(finalPulled, obstacles);
        if (next.size() >= finalPulled.size())
            break;
        finalPulled = next;
    }
    if (finalPulled.size() < strictOrthoPath.size() && finalPulled.size() >= 2)
    {
        strictOrthoPath = finalPulled;
        QList<QPoint> expanded;
        expanded.append(strictOrthoPath.first());
        for (int i = 1; i < strictOrthoPath.size(); ++i)
        {
            QPoint prev = expanded.last();
            QPoint curr = strictOrthoPath[i];
            QPoint delta = curr - prev;
            if (delta.x() != 0 && delta.y() != 0)
            {
                const QPoint mid = pickOrthogonalMid(prev, curr, obstacles, astar);
                if (mid != prev && mid != curr)
                    expanded.append(mid);
            }
            if (curr != expanded.last())
                expanded.append(curr);
        }
        strictOrthoPath = expanded;
    }
    QList<QPoint> finalMerged = removeNearDuplicatePoints(strictOrthoPath, 2 * gridStep);
    if (finalMerged.size() >= 2 && !astar.pathSegmentsIntersectObstacles(finalMerged, obstacles))
        strictOrthoPath = finalMerged;
    // 再尝试 U/Z 简化为 L，进一步减少中途拐点（仅在仍不穿障时采用，避障优先于最短）
    QList<QPoint> minimal = astar.simplifyToMinimalPath(strictOrthoPath);
    if (minimal.size() < strictOrthoPath.size() && minimal.size() >= 2
        && !astar.pathSegmentsIntersectObstacles(minimal, obstacles))
        strictOrthoPath = minimal;

    // 严格正交保证：任意相邻两点若非水平/垂直则插入拐点，确保输出仅含水平与垂直线段（优先级最高）
    QList<QPoint> orthoOut;
    orthoOut.append(strictOrthoPath.first());
    for (int i = 1; i < strictOrthoPath.size(); ++i)
    {
        QPoint prev = orthoOut.last();
        QPoint curr = strictOrthoPath[i];
        QPoint delta = curr - prev;
        if (delta.x() != 0 && delta.y() != 0)
        {
            const QPoint mid = pickOrthogonalMid(prev, curr, obstacles, astar);
            if (mid != prev && mid != curr)
                orthoOut.append(mid);
        }
        if (curr != orthoOut.last())
            orthoOut.append(curr);
    }
    strictOrthoPath = orthoOut;

    QVector<QPoint> result;
    for (const QPoint& p : strictOrthoPath)
        result.append(p);
    if (!astar.pathSegmentsIntersectObstacles(strictOrthoPath, obstacles))
        return result;
    break;
    }

    return simpleOrthogonalPathBestEffort(startPt, endPt, startScene, endScene, routeObstacles);
}

// ---------- Nudging（任务 1.2）：通道检测与偏移 ----------
namespace {
struct Segment {
    int pathIndex = 0;
    int pointIndex = 0;   // 段起点在路径中的下标
    bool horizontal = true;
    qreal fixedCoord = 0; // 水平段 = y，垂直段 = x
    qreal varMin = 0, varMax = 0;
    qreal center() const { return (varMin + varMax) / 2; }
};
static const qreal kEpsilon = 1e-6;
inline bool overlap(qreal a1, qreal a2, qreal b1, qreal b2, qreal minLen)
{
    qreal lo = qMax(a1, b1);
    qreal hi = qMin(a2, b2);
    return hi - lo >= minLen - kEpsilon;
}
}

QList<QVector<QPointF>> CimdrawConnectorAlgorithm::computeNudgingOffsets(const QList<QVector<QPointF>>& paths,
                                                                   qreal spacing,
                                                                   qreal minOverlap)
{
    QList<QVector<QPointF>> out;
    for (const QVector<QPointF>& p : paths)
        out.append(p);
    if (paths.isEmpty())
        return out;

    QList<Segment> hSegs, vSegs;
    for (int pi = 0; pi < paths.size(); ++pi)
    {
        const QVector<QPointF>& path = paths[pi];
        for (int i = 0; i < path.size() - 1; ++i)
        {
            const QPointF& a = path[i];
            const QPointF& b = path[i + 1];
            if (qAbs(a.y() - b.y()) < kEpsilon)
            {
                Segment s;
                s.pathIndex = pi;
                s.pointIndex = i;
                s.horizontal = true;
                s.fixedCoord = (a.y() + b.y()) / 2;
                s.varMin = qMin(a.x(), b.x());
                s.varMax = qMax(a.x(), b.x());
                hSegs.append(s);
            }
            else if (qAbs(a.x() - b.x()) < kEpsilon)
            {
                Segment s;
                s.pathIndex = pi;
                s.pointIndex = i;
                s.horizontal = false;
                s.fixedCoord = (a.x() + b.x()) / 2;
                s.varMin = qMin(a.y(), b.y());
                s.varMax = qMax(a.y(), b.y());
                vSegs.append(s);
            }
        }
    }

    QVector<QVector<QPointF>> deltas(paths.size());
    for (int i = 0; i < paths.size(); ++i)
        deltas[i].resize(paths[i].size(), QPointF(0, 0));

    auto processChannel = [&out, &deltas, spacing](QList<Segment>& segs, bool isHorizontal)
    {
        if (segs.size() < 2)
            return;
        std::sort(segs.begin(), segs.end(), [](const Segment& a, const Segment& b) {
            return a.center() < b.center();
        });
        int n = segs.size();
        for (int i = 0; i < n; ++i)
        {
            qreal offset = (i - (n - 1) / 2.0) * spacing;
            const Segment& s = segs[i];
            if (isHorizontal)
            {
                deltas[s.pathIndex][s.pointIndex] += QPointF(0, offset);
                deltas[s.pathIndex][s.pointIndex + 1] += QPointF(0, offset);
            }
            else
            {
                deltas[s.pathIndex][s.pointIndex] += QPointF(offset, 0);
                deltas[s.pathIndex][s.pointIndex + 1] += QPointF(offset, 0);
            }
        }
    };

    auto runOverlapGroups = [&](QList<Segment>& segs, bool isHorizontal)
    {
        if (segs.size() < 2)
            return;
        QVector<int> parent(segs.size());
        for (int i = 0; i < segs.size(); ++i)
            parent[i] = i;
        auto find = [&parent](int i)
        {
            while (parent[i] != i)
                i = parent[i] = parent[parent[i]];
            return i;
        };
        for (int i = 0; i < segs.size(); ++i)
            for (int j = i + 1; j < segs.size(); ++j)
                if (overlap(segs[i].varMin, segs[i].varMax, segs[j].varMin, segs[j].varMax, minOverlap))
                {
                    int pi = find(i), pj = find(j);
                    if (pi != pj)
                        parent[pi] = pj;
                }
        QHash<int, QList<Segment>> groups;
        for (int i = 0; i < segs.size(); ++i)
            groups[find(i)].append(segs[i]);
        for (auto it = groups.begin(); it != groups.end(); ++it)
            if (it.value().size() >= 2)
                processChannel(it.value(), isHorizontal);
    };

    QHash<qreal, QList<Segment>> hChannels;
    for (const Segment& s : hSegs)
    {
        qreal key = qRound(s.fixedCoord / spacing) * spacing;
        hChannels[key].append(s);
    }
    for (auto it = hChannels.begin(); it != hChannels.end(); ++it)
        runOverlapGroups(it.value(), true);

    QHash<qreal, QList<Segment>> vChannels;
    for (const Segment& s : vSegs)
    {
        qreal key = qRound(s.fixedCoord / spacing) * spacing;
        vChannels[key].append(s);
    }
    for (auto it = vChannels.begin(); it != vChannels.end(); ++it)
        runOverlapGroups(it.value(), false);

    for (int pi = 0; pi < paths.size(); ++pi)
    {
        for (int j = 0; j < out[pi].size(); ++j)
            out[pi][j] += deltas[pi][j];
    }
    return out;
}
