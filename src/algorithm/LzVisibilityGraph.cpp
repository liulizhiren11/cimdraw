#include "LzVisibilityGraph.h"
#include "LzAStar.h"
#include <QSet>
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace {

inline QPoint alignToStep(int x, int y, int step)
{
    auto floorDiv = [step](int v) {
        return static_cast<int>(std::floor(v / static_cast<double>(step))) * step;
    };
    return QPoint(floorDiv(x), floorDiv(y));
}

// 关键点集合：起点、终点、每个障碍膨胀矩形的四角，对齐到 gridAlignStep 后去重
QVector<QPoint> collectKeyPoints(const QPointF& startScene, const QPointF& endScene,
                                 const QList<QGraphicsItem*>& obstacles, int gridAlignStep)
{
    QSet<QPoint> seen;
    QVector<QPoint> out;
    auto add = [&seen, &out](const QPoint& p) {
        if (seen.contains(p))
            return;
        seen.insert(p);
        out.append(p);
    };

    add(alignToStep(static_cast<int>(startScene.x()), static_cast<int>(startScene.y()), gridAlignStep));
    add(alignToStep(static_cast<int>(endScene.x()), static_cast<int>(endScene.y()), gridAlignStep));

    for (QGraphicsItem* it : obstacles)
    {
        if (!it || !it->isVisible())
            continue;
        const QRectF r = LzAStar::expandedObstacleSceneRect(it, ObstacleClearanceMargin);
        if (!r.isValid())
            continue;
        add(alignToStep(static_cast<int>(r.left()),  static_cast<int>(r.top()),    gridAlignStep));
        add(alignToStep(static_cast<int>(r.right()),  static_cast<int>(r.top()),   gridAlignStep));
        add(alignToStep(static_cast<int>(r.right()), static_cast<int>(r.bottom()), gridAlignStep));
        add(alignToStep(static_cast<int>(r.left()),  static_cast<int>(r.bottom()), gridAlignStep));
    }
    return out;
}

// 邻接表：节点 -> [(邻居, 边权), ...]
using AdjList = QHash<QPoint, QList<QPair<QPoint, int>>>;

void addEdge(AdjList& adj, const QPoint& a, const QPoint& b, int weight)
{
    if (a == b || weight <= 0)
        return;
    adj[a].append(qMakePair(b, weight));
}

void buildVisibleEdges(const QVector<QPoint>& points, const QList<QGraphicsItem*>& obstacles,
                       const LzAStar* astar, AdjList& adj)
{
    for (const QPoint& p : points)
    {
        for (const QPoint& q : points)
        {
            if (p == q)
                continue;
            if (p.y() == q.y())
            {
                int w = qAbs(p.x() - q.x());
                if (w <= 0)
                    continue;
                if (!astar->segmentIntersectsObstacles(p, q, obstacles))
                    addEdge(adj, p, q, w);
            }
            else if (p.x() == q.x())
            {
                int w = qAbs(p.y() - q.y());
                if (w <= 0)
                    continue;
                if (!astar->segmentIntersectsObstacles(p, q, obstacles))
                    addEdge(adj, p, q, w);
            }
        }
    }
}

// 曼哈顿距离
int manhattan(const QPoint& a, const QPoint& b)
{
    return qAbs(a.x() - b.x()) + qAbs(a.y() - b.y());
}

// 可见性图上 A*：代价 = 边权 + 拐弯惩罚；启发式 = 曼哈顿距离。状态 = (pt, prevDir)
// 注意：必须用「f 最小」优先弹出；此前对 QList 排序后 takeFirst 会弹出最差节点，导致开放集爆炸、界面卡死。
QList<QPoint> aStarOnVisibilityGraph(const QPoint& start, const QPoint& goal,
                                     const AdjList& adj, int turnPenalty)
{
    struct OpenNode {
        int f = 0;
        int h = 0;
        int g = 0;
        QPoint pt;
        QPoint prevDir;
        bool operator>(const OpenNode& o) const
        {
            return f > o.f || (f == o.f && h > o.h);
        }
    };

    QHash<QPair<QPoint, QPoint>, int> bestG;
    QHash<QPair<QPoint, QPoint>, QPair<QPoint, QPoint>> cameFrom;
    QSet<QPair<QPoint, QPoint>> closed;

    std::priority_queue<OpenNode, std::vector<OpenNode>, std::greater<OpenNode>> open;

    const QPair<QPoint, QPoint> startKey(start, QPoint(0, 0));
    bestG.insert(startKey, 0);
    const int h0 = manhattan(start, goal);
    open.push({ h0, h0, 0, start, QPoint(0, 0) });

    const int kIterLimit = 25000;
    int iterations = 0;

    while (!open.empty())
    {
        if (++iterations > kIterLimit)
            return QList<QPoint>();

        OpenNode cur = open.top();
        open.pop();

        const QPair<QPoint, QPoint> curKey(cur.pt, cur.prevDir);
        if (cur.g != bestG.value(curKey, std::numeric_limits<int>::max()))
            continue;
        if (closed.contains(curKey))
            continue;
        closed.insert(curKey);

        if (cur.pt == goal)
        {
            QList<QPoint> path;
            path.append(goal);
            QPair<QPoint, QPoint> key(goal, cur.prevDir);
            int safety = 0;
            while (key.first != start && ++safety < 10000)
            {
                if (!cameFrom.contains(key))
                    return QList<QPoint>();
                QPair<QPoint, QPoint> prev = cameFrom.value(key);
                path.prepend(prev.first);
                key = prev;
            }
            if (key.first != start)
                return QList<QPoint>();
            return path;
        }

        auto it = adj.constFind(cur.pt);
        if (it == adj.constEnd())
            continue;
        for (const QPair<QPoint, int>& e : it.value())
        {
            QPoint nbr = e.first;
            int edgeCost = e.second;
            QPoint dir(nbr.x() - cur.pt.x(), nbr.y() - cur.pt.y());
            int turn = (cur.prevDir.x() != 0 || cur.prevDir.y() != 0) &&
                       (dir.x() != cur.prevDir.x() || dir.y() != cur.prevDir.y())
                       ? turnPenalty : 0;
            int tentativeG = cur.g + edgeCost + turn;
            QPair<QPoint, QPoint> nbrKey(nbr, dir);
            if (tentativeG >= bestG.value(nbrKey, std::numeric_limits<int>::max()))
                continue;
            bestG.insert(nbrKey, tentativeG);
            cameFrom.insert(nbrKey, qMakePair(cur.pt, cur.prevDir));
            const int hn = manhattan(nbr, goal);
            open.push({ tentativeG + hn, hn, tentativeG, nbr, dir });
        }
    }
    return QList<QPoint>();
}

} // namespace

QList<QPoint> LzVisibilityGraph::planPath(
    const QPointF& startScene,
    const QPointF& endScene,
    const QList<QGraphicsItem*>& obstacles,
    const LzAStar* astar,
    int turnPenalty,
    int gridAlignStep)
{
    if (!astar)
        return QList<QPoint>();

    // 障碍多时 O(n²) 建边 + 每边多次 segment 检测，易长时间卡 UI；直接让上层走网格 A*
    if (obstacles.size() > 80)
        return QList<QPoint>();

    QVector<QPoint> points = collectKeyPoints(startScene, endScene, obstacles, gridAlignStep);
    // 关键点过多时 O(n²) 建边 + A* 过重，直接放弃可见性图由上层回退网格 A*
    if (points.size() > 150)
        return QList<QPoint>();
    QPoint start = alignToStep(static_cast<int>(startScene.x()), static_cast<int>(startScene.y()), gridAlignStep);
    QPoint goal  = alignToStep(static_cast<int>(endScene.x()), static_cast<int>(endScene.y()), gridAlignStep);

    if (!points.contains(start))
        points.append(start);
    if (!points.contains(goal))
        points.append(goal);

    AdjList adj;
    buildVisibleEdges(points, obstacles, astar, adj);
    return aStarOnVisibilityGraph(start, goal, adj, turnPenalty);
}
