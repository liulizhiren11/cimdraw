#include "CimdrawAStar.h"
#include <QVector>
#include <queue>
#include <limits>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QLineF>
#include <QPainterPath>

namespace {

const int kDirectionNone = -1;

struct SearchState
{
    QPoint pt;
    int dirIndex = kDirectionNone;

    bool operator==(const SearchState& other) const
    {
        return pt == other.pt && dirIndex == other.dirIndex;
    }
};

inline uint qHash(const SearchState& state, uint seed = 0)
{
    return ::qHash(state.pt, seed) ^ (::qHash(state.dirIndex, seed) << 1);
}

struct DirectionalNode
{
    SearchState state;
    int g = 0;
    int h = 0;
    int f = 0;
    int straightSteps = 0;

    bool operator>(const DirectionalNode& other) const
    {
        if (f != other.f)
            return f > other.f;
        if (h != other.h)
            return h > other.h;
        return straightSteps < other.straightSteps;
    }
};

static QList<QPoint> reconstructDirectionalPath(const QHash<SearchState, SearchState>& cameFrom,
                                                SearchState current)
{
    QList<QPoint> totalPath;
    totalPath.append(current.pt);
    int safety = 0;
    const int kMaxPathLen = 500000;
    while (cameFrom.contains(current) && ++safety < kMaxPathLen)
    {
        current = cameFrom.value(current);
        totalPath.prepend(current.pt);
    }
    if (safety >= kMaxPathLen)
        return {};
    return totalPath;
}

struct ShortcutResult
{
    bool clear = false;
    bool needsBend = false;
    QPoint bend;
};

} // namespace

CimdrawAStar::CimdrawAStar() = default;

void CimdrawAStar::setGridStep(int stepPixels)
{
    m_gridStep = qBound(MinGridStep, stepPixels, StepSize);
}

void CimdrawAStar::setMaxSearchIterations(int maxIterations)
{
    m_maxSearchIterations = maxIterations;
}

QRectF CimdrawAStar::expandedObstacleSceneRect(const QGraphicsItem* item, int margin)
{
    if (!item)
        return {};
    QRectF base;
    const QPainterPath localShape = item->shape();
    if (!localShape.isEmpty())
        base = item->mapToScene(localShape).boundingRect();
    else
        base = item->sceneBoundingRect();
    if (!base.isValid())
        return {};
    base.adjust(-margin, -margin, margin, margin);
    return base;
}

// 线段与矩形是否相交（含端点在内）
static bool segmentIntersectsRect(const QPointF& a, const QPointF& b, const QRectF& rect)
{
    if (rect.contains(a) && rect.contains(b))
        return true;
    QLineF seg(a, b);
    QLineF top(rect.topLeft(), rect.topRight());
    QLineF right(rect.topRight(), rect.bottomRight());
    QLineF bottom(rect.bottomRight(), rect.bottomLeft());
    QLineF left(rect.bottomLeft(), rect.topLeft());
    QPointF dummy;
    if (seg.intersects(top, &dummy) == QLineF::BoundedIntersection) return true;
    if (seg.intersects(right, &dummy) == QLineF::BoundedIntersection) return true;
    if (seg.intersects(bottom, &dummy) == QLineF::BoundedIntersection) return true;
    if (seg.intersects(left, &dummy) == QLineF::BoundedIntersection) return true;
    return false;
}

// 点到膨胀矩形边界的最小距离（点在矩形外时）；在矩形内返回 0
static qreal minDistanceToExpandedRect(const QPointF& pt, const QRectF& expanded)
{
    if (expanded.contains(pt))
        return 0;
    qreal dx = 0, dy = 0;
    if (pt.x() < expanded.left())
        dx = expanded.left() - pt.x();
    else if (pt.x() > expanded.right())
        dx = pt.x() - expanded.right();
    if (pt.y() < expanded.top())
        dy = expanded.top() - pt.y();
    else if (pt.y() > expanded.bottom())
        dy = pt.y() - expanded.bottom();
    return qSqrt(dx * dx + dy * dy);
}

// 避障权重：距膨胀区边界越近代价越大，贴边、沿边路径不可取，优先走通道中间
static const int kProximityMarginSteps = 5;   // 与 StepSize 无关的固定像素带（5*20=100px）
static const int kProximityCostMax = 20;      // 贴边时最多约 20 格等价代价，强推路径远离边界
static const qreal kProximityMarginPx = qreal(kProximityMarginSteps) * qreal(StepSize);

static int proximityCost(const QPoint& pt, const QList<QGraphicsItem*>& items)
{
    const qreal proximityMargin = kProximityMarginPx;
    qreal minDist = 1e9;
    QPointF pf(pt);
    for (QGraphicsItem* it : items)
    {
        if (!it || !it->isVisible())
            continue;
        const QRectF expanded = CimdrawAStar::expandedObstacleSceneRect(it, ObstacleClearanceMargin);
        if (!expanded.isValid())
            continue;
        if (expanded.contains(pf))
            return 99;
        qreal d = minDistanceToExpandedRect(pf, expanded);
        if (d < minDist)
            minDist = d;
    }
    if (minDist >= proximityMargin || minDist <= 0)
        return 0;
    // 越贴近边界代价越大，上限 kProximityCostMax
    return qMin(kProximityCostMax,
                qMax(1, int(qreal(kProximityCostMax) * (1.0 - minDist / proximityMargin))));
}

// ============= 障碍检测（含安全边距） =============
static ShortcutResult bestOrthogonalShortcut(const QPoint& a, const QPoint& b,
                                             const QList<QGraphicsItem*>& items,
                                             const CimdrawAStar* astar)
{
    ShortcutResult result;
    if (a == b)
    {
        result.clear = true;
        return result;
    }

    if (a.x() == b.x() || a.y() == b.y())
    {
        result.clear = !astar->segmentIntersectsObstacles(a, b, items);
        return result;
    }

    const QPoint candidate1(b.x(), a.y());
    const QPoint candidate2(a.x(), b.y());
    const bool clear1 = !astar->segmentIntersectsObstacles(a, candidate1, items)
        && !astar->segmentIntersectsObstacles(candidate1, b, items);
    const bool clear2 = !astar->segmentIntersectsObstacles(a, candidate2, items)
        && !astar->segmentIntersectsObstacles(candidate2, b, items);

    if (!clear1 && !clear2)
        return result;

    result.clear = true;
    result.needsBend = true;

    if (clear1 && !clear2)
    {
        result.bend = candidate1;
        return result;
    }
    if (clear2 && !clear1)
    {
        result.bend = candidate2;
        return result;
    }

    const int cost1 = proximityCost(candidate1, items);
    const int cost2 = proximityCost(candidate2, items);
    if (cost1 != cost2)
    {
        result.bend = (cost1 < cost2) ? candidate1 : candidate2;
        return result;
    }

    result.bend = (qAbs(b.x() - a.x()) >= qAbs(b.y() - a.y())) ? candidate1 : candidate2;
    return result;
}

bool CimdrawAStar::isBlocked(const QPoint & pt,
    QList<QGraphicsItem*> items)
{
    QPointF pf(pt);
    for (QGraphicsItem* it : items)
    {
        if (!it || !it->isVisible())
            continue;
        const QRectF expanded = CimdrawAStar::expandedObstacleSceneRect(it, ObstacleClearanceMargin);
        if (!expanded.isValid())
            continue;
        if (expanded.contains(pf))
            return true;
    }
    return false;
}

// 若终点被堵住，可选：找最近可走格点；找不到则返回 (0,0)
QPoint CimdrawAStar::nearestWalkable(const QPoint & goalAligned,
    QList<QGraphicsItem*> items,
    int maxRing)
{
    if (!isBlocked(goalAligned, items))
        return goalAligned;

    for (int r = 1; r <= maxRing; ++r)
    {
        for (int dx = -r; dx <= r; ++dx)
        {
            for (int dy = -r; dy <= r; ++dy)
            {
                if (qAbs(dx) != r && qAbs(dy) != r) 
                    continue;
                QPoint cand = goalAligned + QPoint(dx * m_gridStep, dy * m_gridStep);
                if (!isBlocked(cand, items))
                    return cand;                 // 找到最近空格点
            }
        }
    }
    return {};      // 依约：空 QPoint = 没找到
}

// 线段 (a,b) 与障碍相交检测：若线段穿过任一障碍（含安全边距膨胀区）则返回 true；
// goalAllow：仅当线段终点 b 等于 goalAllow 且该点落在此图元内时，才忽略该图元（允许最后一步落到终点）。
// 若写成「只要 goal 落在图元内就整段忽略该图元」，则在检查邻居段 (p,n)（n≠goal）时也会跳过该障碍，路径会垂直穿障。
bool CimdrawAStar::segmentIntersectsObstacle(const QPoint& a, const QPoint& b,
    const QList<QGraphicsItem*>& items,
    const QPoint& goalAllow) const
{
    QPointF af(a), bf(b);

    for (QGraphicsItem* it : items)
    {
        if (!it || !it->isVisible())
            continue;
        if (!goalAllow.isNull() && b == goalAllow
            && it->shape().contains(it->mapFromScene(QPointF(goalAllow))))
            continue;

        QRectF expanded = CimdrawAStar::expandedObstacleSceneRect(it, ObstacleClearanceMargin);
        if (!expanded.isValid())
            continue;
        expanded.adjust(-1, -1, 1, 1);
        if (segmentIntersectsRect(af, bf, expanded))
            return true;
    }
    return false;
}

// 曼哈顿距离启发函数，用于估算当前点到目标点的代价
int CimdrawAStar::heuristic(const QPoint & a, const QPoint & b)
{
    return (qAbs(a.x() - b.x()) + qAbs(a.y() - b.y())) / m_gridStep;
}

// 获取当前点的邻居节点，排除掉障碍点（与 isBlocked 一致使用膨胀矩形，确保路径不穿过图元）
QList<QPoint> CimdrawAStar::getNeighbors(const QPoint& p, const QPoint& goal, QList<QGraphicsItem*> items)
{
    const QList<QPoint> dirs = { {m_gridStep,0}, {-m_gridStep,0}, {0,m_gridStep}, {0,-m_gridStep} };
    QList<QPoint> neighbors;

    for (const QPoint& d : dirs)
    {
        QPoint n = p + d;
        // 与 isBlocked 一致：凡在膨胀区内的点一律视为不可走（除非是终点），避免路径擦边或穿过图元
        if (n != goal && isBlocked(n, items))
            continue;
        // 线段 (p,n) 穿过障碍时也不允许该邻居，避免路径“切角”穿过图元
        if (segmentIntersectsObstacle(p, n, items, goal))
            continue;
        neighbors.append(n);
    }
    return neighbors;
}

// 根据 cameFrom 回溯路径，生成从 start 到 goal 的路径
QList<QPoint> CimdrawAStar::reconstructPath(const QHash<QPoint, QPoint>&cameFrom, QPoint current)
{
    QList<QPoint> totalPath;
    totalPath.append(current); // 从终点开始回溯
    while (cameFrom.contains(current))
    {
        current = cameFrom[current];
        totalPath.prepend(current);// 插入到前面，形成正向路径
    }
    return totalPath;
}

// 将点对齐到网格
QPoint CimdrawAStar::alignToGrid(const QPoint& pt, int step)
{
    return QPoint((pt.x() / step) * step, (pt.y() / step) * step);
}

// A* 算法，带有方向变化惩罚；turnPenalty 与 proximityScale 用于路径优先模式
QList<QPoint> CimdrawAStar::aStarWithDirectionPenalty(const QPoint& start, const QPoint& goal, QList<QGraphicsItem*> items,
                                                 int turnPenalty, qreal proximityScale)
{
    // ① 网格对齐（与 m_gridStep 一致）
    const int step = m_gridStep;
    auto alignToGridLocal = [step](const QPoint& p)
    {
        auto floorDiv = [step](int v)
        {
            return int(std::floor(v / double(step))) * step;
        };
        return QPoint(floorDiv(p.x()), floorDiv(p.y()));
    };
    const QPoint startAligned = alignToGridLocal(start);
    const QPoint goalAligned = alignToGridLocal(goal);

    // ② 若终点在障碍里 => 找最近空格；找不到直接失败
    const bool goalBlocked = isBlocked(goalAligned, items);
    const int walkRing = items.size() > 60 ? 24 : 60;
    const QPoint realGoal = goalBlocked ? nearestWalkable(goalAligned, items, walkRing) : goalAligned;
    if (goalBlocked && realGoal.isNull())
        return {};              // 标记“无效路线”，瞬时返回

    // ③ 关键：把 realGoal 传给 getNeighbors(...)
    //     * 如果真实终点在障碍里，nearestWalkable 已替换成外圈空格点；
    //     * 如果真实终点本来就空，可走到本身。

    QSet<SearchState> closedSet;
    QHash<SearchState, int> gScore;
    QHash<SearchState, SearchState> cameFrom;

    auto cmp = [](const DirectionalNode& a, const DirectionalNode& b) { return a > b; };
    std::priority_queue<DirectionalNode, std::vector<DirectionalNode>, decltype(cmp)> openSet(cmp);
    const SearchState startState{ startAligned, kDirectionNone };
    const int hStart = heuristic(startAligned, realGoal);
    gScore.insert(startState, 0);
    openSet.push({ startState, 0, hStart, hStart, 0 });

    const QPoint kDirections[4] = {
        QPoint(step, 0),
        QPoint(-step, 0),
        QPoint(0, step),
        QPoint(0, -step)
    };

    const int defaultIterLimit =
        qMin(100000, int(200000.0 * double(StepSize) / double(qMax(1, step))));
    const int iterLimit =
        m_maxSearchIterations > 0 ? m_maxSearchIterations : defaultIterLimit;
    int iterations = 0;

    while (!openSet.empty())
    {
        if (++iterations > iterLimit)
            return {};              // 超限直接失败

        const DirectionalNode cur = openSet.top();
        openSet.pop();
        if (cur.g != gScore.value(cur.state, INT_MAX))
            continue;

        if (cur.state.pt == realGoal)
        {
            return reconstructDirectionalPath(cameFrom, cur.state);
        }

        if (closedSet.contains(cur.state))
            continue;
        closedSet.insert(cur.state);

        for (int dirIndex = 0; dirIndex < 4; ++dirIndex)
        {
            const QPoint nbr = cur.state.pt + kDirections[dirIndex];
            if (nbr != realGoal && isBlocked(nbr, items))
                continue;
            if (segmentIntersectsObstacle(cur.state.pt, nbr, items, realGoal))
                continue;

            const SearchState nextState{ nbr, dirIndex };
            if (closedSet.contains(nextState))
                continue;

            const bool sameDir = (cur.state.dirIndex == kDirectionNone || cur.state.dirIndex == dirIndex);
            const int turnCost = sameDir ? 0 : turnPenalty;
            const int proximity = static_cast<int>(proximityScale * proximityCost(nbr, items));
            const int tentativeG = cur.g + 1 + turnCost + proximity;

            if (tentativeG >= gScore.value(nextState, INT_MAX))
                continue;

            cameFrom.insert(nextState, cur.state);
            gScore.insert(nextState, tentativeG);
            const int hNbr = heuristic(nbr, realGoal);
            const int straightSteps = sameDir ? (cur.straightSteps + 1) : 1;
            openSet.push({ nextState, tentativeG, hNbr, tentativeG + hNbr, straightSteps });
        }
    }
    return {};                      // openSet 清空 ⇒ 不可达
}

// 简化路径，只保留关键点（改进版：更智能地移除冗余点）
QList<QPoint> CimdrawAStar::simplifyOrthogonalPath(const QList<QPoint>& path)
{
    if (path.isEmpty() || path.size() < 2)
    {
        return path; // 如果路径为空或只有一个点，直接返回
    }
    
    if (path.size() == 2)
    {
        return path; // 只有两个点，直接返回
    }

    QList<QPoint> simplified;
    simplified.append(path.first()); // 添加起点
    
    // 第一步：移除所有共线的中间点
    for (int i = 1; i < path.size() - 1; ++i)
    {
        QPoint prev = simplified.last();
        QPoint curr = path[i];
        QPoint next = path[i + 1];
        
        // 计算方向向量
        QPoint dir1 = curr - prev;
        QPoint dir2 = next - curr;
        
        // 判断是否共线：如果两个方向向量平行且同向，则共线
        bool isCollinear = false;
        
        // 都是垂直方向
        if (dir1.x() == 0 && dir2.x() == 0 && dir1.y() != 0 && dir2.y() != 0)
        {
            isCollinear = ((dir1.y() > 0) == (dir2.y() > 0));
        }
        // 都是水平方向
        else if (dir1.y() == 0 && dir2.y() == 0 && dir1.x() != 0 && dir2.x() != 0)
        {
            isCollinear = ((dir1.x() > 0) == (dir2.x() > 0));
        }
        
        // 如果不共线，说明是拐点，需要保留
        if (!isCollinear)
        {
            simplified.append(curr);
        }
    }
    
    simplified.append(path.last()); // 添加终点
    
    // 第二步：进一步优化，移除可以跳过的中间点
    if (simplified.size() <= 2)
    {
        return simplified;
    }
    
    QList<QPoint> optimized;
    optimized.append(simplified.first());
    
    for (int i = 1; i < simplified.size() - 1; ++i)
    {
        QPoint prev = optimized.last();
        QPoint curr = simplified[i];
        QPoint next = simplified[i + 1];
        
        // 检查从 prev 到 next 是否可以直接连接（在同一直线上）
        bool canSkip = false;
        if (prev.x() == next.x() || prev.y() == next.y())
        {
            // 在同一直线上，可以跳过中间点
            canSkip = true;
        }
        else
        {
            // 检查是否是冗余的 L 形路径
            QPoint directDir = next - prev;
            QPoint viaDir1 = curr - prev;
            QPoint viaDir2 = next - curr;
            
            // 确保所有方向都是正交的
            bool directOrtho = (directDir.x() == 0 || directDir.y() == 0);
            bool via1Ortho = (viaDir1.x() == 0 || viaDir1.y() == 0);
            bool via2Ortho = (viaDir2.x() == 0 || viaDir2.y() == 0);
            
            if (directOrtho && via1Ortho && via2Ortho)
            {
                // 计算路径长度（曼哈顿距离）
                int directLen = qAbs(directDir.x()) + qAbs(directDir.y());
                int viaLen = qAbs(viaDir1.x()) + qAbs(viaDir1.y()) + qAbs(viaDir2.x()) + qAbs(viaDir2.y());
                
                // 如果路径长度相同，可以跳过中间点
                if (directLen == viaLen)
                {
                    canSkip = true;
                }
            }
        }
        
        if (!canSkip)
        {
            optimized.append(curr);
        }
    }
    
    optimized.append(simplified.last());
    
    // 第三步：再次检查并移除共线的点（确保彻底）
    if (optimized.size() <= 2)
    {
        return optimized;
    }
    
    QList<QPoint> finalResult;
    finalResult.append(optimized.first());
    
    for (int i = 1; i < optimized.size() - 1; ++i)
    {
        QPoint prev = finalResult.last();
        QPoint curr = optimized[i];
        QPoint next = optimized[i + 1];
        
        QPoint dir1 = curr - prev;
        QPoint dir2 = next - curr;
        
        // 再次检查是否共线
        bool isCollinear = false;
        if (dir1.x() == 0 && dir2.x() == 0 && dir1.y() != 0 && dir2.y() != 0)
        {
            isCollinear = ((dir1.y() > 0) == (dir2.y() > 0));
        }
        else if (dir1.y() == 0 && dir2.y() == 0 && dir1.x() != 0 && dir2.x() != 0)
        {
            isCollinear = ((dir1.x() > 0) == (dir2.x() > 0));
        }
        
        if (!isCollinear)
        {
            finalResult.append(curr);
        }
    }
    
    finalResult.append(optimized.last());
    return finalResult;
}

// 优化路径，使其更接近 Visio 的连线效果
QList<QPoint> CimdrawAStar::optimizePathForVisioStyle(const QList<QPoint>& path, const QPoint& start, const QPoint& end,
                                                 const QList<QGraphicsItem*>& obstacleItems)
{
    if (path.size() <= 2)
    {
        return path; // 路径太短，无需优化
    }
    
    QList<QPoint> optimized = path;
    const int step = m_gridStep;
    
    // 第一步：对于 L 形路径（3个点），优化拐点位置使其更自然
    if (optimized.size() == 3)
    {
        QPoint p1 = optimized[0];
        QPoint p2 = optimized[1];
        QPoint p3 = optimized[2];
        
        // 计算两个可能的 L 形路径的拐点
        QPoint candidate1(p1.x(), p3.y()); // 先水平后垂直
        QPoint candidate2(p3.x(), p1.y()); // 先垂直后水平
        
        // 对齐到网格
        candidate1 = QPoint((candidate1.x() / step) * step,
                           (candidate1.y() / step) * step);
        candidate2 = QPoint((candidate2.x() / step) * step,
                           (candidate2.y() / step) * step);
        
        // 计算两个候选路径的总长度
        int len1 = qAbs(candidate1.x() - p1.x()) + qAbs(candidate1.y() - p1.y()) +
                   qAbs(p3.x() - candidate1.x()) + qAbs(p3.y() - candidate1.y());
        int len2 = qAbs(candidate2.x() - p1.x()) + qAbs(candidate2.y() - p1.y()) +
                   qAbs(p3.x() - candidate2.x()) + qAbs(p3.y() - candidate2.y());
        
        // 长度不同选更短的；长度相同时按起点到终点的主导方向：终点偏右则先水平，偏下则先垂直
        if (len1 < len2)
            optimized[1] = candidate1;
        else if (len2 < len1)
            optimized[1] = candidate2;
        else
        {
            int dx = qAbs(p3.x() - p1.x());
            int dy = qAbs(p3.y() - p1.y());
            optimized[1] = (dx >= dy) ? candidate1 : candidate2;
        }
    }
    // 对多拐点路径不做逐点“对称化”位移。
    // 之前这里单独平移当前拐点、却不联动相邻点，后续在严格正交恢复时
    // 会把这些对角段重新展开成一串小台阶，正是拖动后出现楼梯状折线的来源。
    
    // 第三步：移除可能产生的重复点
    QList<QPoint> finalPath;
    finalPath.append(optimized.first());
    for (int i = 1; i < optimized.size(); ++i)
    {
        if (optimized[i] != finalPath.last())
        {
            finalPath.append(optimized[i]);
        }
    }

    if (!obstacleItems.isEmpty() && pathSegmentsIntersectObstacles(finalPath, obstacleItems))
        return path;
    return finalPath;
}

// 确保路径严格正交，并确保起点和终点的连接方向正确
QList<QPoint> CimdrawAStar::ensureOrthogonalPath(const QList<QPoint>& path, const QPoint& originalStart, const QPoint& originalEnd)
{
    if (path.isEmpty())
    {
        // 路径为空时创建简单 L 形：按起点到终点的主导方向决定先水平还是先垂直
        QList<QPoint> simplePath;
        simplePath.append(originalStart);
        int dx = qAbs(originalEnd.x() - originalStart.x());
        int dy = qAbs(originalEnd.y() - originalStart.y());
        if (originalStart != originalEnd)
        {
            if (dx >= dy && originalStart.x() != originalEnd.x())
                simplePath.append(QPoint(originalEnd.x(), originalStart.y()));
            else if (originalStart.y() != originalEnd.y())
                simplePath.append(QPoint(originalStart.x(), originalEnd.y()));
            if (simplePath.last() != originalEnd)
                simplePath.append(originalEnd);
        }
        return simplePath;
    }
    
    if (path.size() < 2)
    {
        QList<QPoint> result;
        result.append(originalStart);
        if (path.first() != originalStart && path.first() != originalEnd)
        {
            result.append(path.first());
        }
        if (result.last() != originalEnd)
        {
            result.append(originalEnd);
        }
        return result;
    }
    
    QList<QPoint> result;
    
    // 第一步：从原始起点开始，确保第一段是水平或垂直的
    result.append(originalStart);
    QPoint pathStart = path.first();
    
    // 如果原始起点和路径起点不同，需要添加正交连接段
    if (originalStart != pathStart)
    {
        QPoint delta = pathStart - originalStart;
        
        // 优先选择水平或垂直方向（选择距离更近的）
        if (qAbs(delta.x()) >= qAbs(delta.y()) && delta.x() != 0)
        {
            // 先水平移动
            QPoint intermediate(pathStart.x(), originalStart.y());
            if (intermediate != originalStart && intermediate != pathStart)
            {
                result.append(intermediate);
            }
        }
        else if (delta.y() != 0)
        {
            // 先垂直移动
            QPoint intermediate(originalStart.x(), pathStart.y());
            if (intermediate != originalStart && intermediate != pathStart)
            {
                result.append(intermediate);
            }
        }
    }
    
    // 第二步：添加路径中间的点，跳过与上一个点共线的点
    QPoint lastPoint = result.last();
    for (int i = 0; i < path.size(); ++i)
    {
        QPoint current = path[i];
        
        // 如果当前点与上一个点相同，跳过
        if (current == lastPoint)
            continue;
        
        // 确保从 lastPoint 到 current 的路径是正交的
        QPoint delta = current - lastPoint;
        
        // 如果 delta 不是正交的，需要添加中间点
        if (delta.x() != 0 && delta.y() != 0)
        {
            // 需要先水平后垂直，或先垂直后水平
            // 优先选择更短的方向
            if (qAbs(delta.x()) >= qAbs(delta.y()))
            {
                // 先水平
                QPoint intermediate(lastPoint.x() + delta.x(), lastPoint.y());
                if (intermediate != lastPoint && intermediate != current)
                {
                    result.append(intermediate);
                    lastPoint = intermediate;
                }
            }
            else
            {
                // 先垂直
                QPoint intermediate(lastPoint.x(), lastPoint.y() + delta.y());
                if (intermediate != lastPoint && intermediate != current)
                {
                    result.append(intermediate);
                    lastPoint = intermediate;
                }
            }
        }
        
        // 添加当前点（如果与上一个点不同）
        if (current != lastPoint)
        {
            result.append(current);
            lastPoint = current;
        }
    }
    
    // 第三步：确保到达原始终点的最后一段是水平或垂直的
    QPoint pathEnd = result.last();
    if (pathEnd != originalEnd)
    {
        QPoint delta = originalEnd - pathEnd;
        
        // 确保最后一段是水平或垂直的
        if (delta.x() != 0 && delta.y() != 0)
        {
            // 需要先水平后垂直，或先垂直后水平
            if (qAbs(delta.x()) >= qAbs(delta.y()))
            {
                // 先水平
                QPoint intermediate(pathEnd.x() + delta.x(), pathEnd.y());
                if (intermediate != pathEnd && intermediate != originalEnd)
                {
                    result.append(intermediate);
                    pathEnd = intermediate;
                }
            }
            else
            {
                // 先垂直
                QPoint intermediate(pathEnd.x(), pathEnd.y() + delta.y());
                if (intermediate != pathEnd && intermediate != originalEnd)
                {
                    result.append(intermediate);
                    pathEnd = intermediate;
                }
            }
        }
        
        // 添加最终终点
        if (originalEnd != pathEnd)
        {
            result.append(originalEnd);
        }
    }
    
    // 第四步：移除重复的连续点和共线的中间点
    QList<QPoint> finalResult;
    if (result.isEmpty())
        return result;
        
    finalResult.append(result.first());
    
    for (int i = 1; i < result.size(); ++i)
    {
        // 跳过重复点
        if (result[i] == finalResult.last())
            continue;
        
        // 如果只有两个点，直接添加
        if (finalResult.size() < 2)
        {
            finalResult.append(result[i]);
            continue;
        }
        
        // 检查是否共线：如果三个点共线，移除中间点
        QPoint p1 = finalResult[finalResult.size() - 2];
        QPoint p2 = finalResult.last();
        QPoint p3 = result[i];
        
        QPoint dir1 = p2 - p1;
        QPoint dir2 = p3 - p2;
        
        // 判断是否共线
        bool isCollinear = false;
        if (dir1.x() == 0 && dir2.x() == 0 && dir1.y() != 0 && dir2.y() != 0)
        {
            // 都是垂直方向，检查方向是否一致
            isCollinear = ((dir1.y() > 0) == (dir2.y() > 0));
        }
        else if (dir1.y() == 0 && dir2.y() == 0 && dir1.x() != 0 && dir2.x() != 0)
        {
            // 都是水平方向，检查方向是否一致
            isCollinear = ((dir1.x() > 0) == (dir2.x() > 0));
        }
        
        if (isCollinear)
        {
            // 移除中间点 p2，用 p3 替换
            finalResult.removeLast();
        }
        
        finalResult.append(p3);
    }
    
    return finalResult;
}

// 进一步简化路径，将U形或Z形路径简化为L形路径
QList<QPoint> CimdrawAStar::simplifyToMinimalPath(const QList<QPoint>& path)
{
    if (path.size() <= 3)
    {
        return path; // 路径已经足够简单
    }
    
    QPoint start = path.first();
    QPoint end = path.last();
    
    // 尝试将路径简化为L形：先水平后垂直，或先垂直后水平
    QList<QPoint> minimalPath;
    minimalPath.append(start);
    
    // 如果起点和终点可以直接用L形连接
    if (start.x() != end.x() && start.y() != end.y())
    {
        // 计算两个可能的L形路径
        QPoint candidate1(end.x(), start.y()); // 先水平后垂直
        QPoint candidate2(start.x(), end.y()); // 先垂直后水平
        
        int len1 = qAbs(candidate1.x() - start.x()) + qAbs(candidate1.y() - start.y()) +
                   qAbs(end.x() - candidate1.x()) + qAbs(end.y() - candidate1.y());
        int len2 = qAbs(candidate2.x() - start.x()) + qAbs(candidate2.y() - start.y()) +
                   qAbs(end.x() - candidate2.x()) + qAbs(end.y() - candidate2.y());
        
        // 长度不同时选更短的；长度相同时按起点到终点的主导方向选：往右推则先水平，往下推则先垂直
        int dx = qAbs(end.x() - start.x());
        int dy = qAbs(end.y() - start.y());
        bool preferHorizontalFirst = (len1 < len2) || (len1 == len2 && dx >= dy);
        minimalPath.append(preferHorizontalFirst ? candidate1 : candidate2);
    }
    
    minimalPath.append(end);
    
    // 如果简化后的路径只有2个点，说明起点和终点在同一直线上
    if (minimalPath.size() == 2)
    {
        return minimalPath;
    }
    
    // 检查简化后的路径是否比原路径更短或相等
    int originalLen = 0;
    for (int i = 1; i < path.size(); ++i)
    {
        originalLen += qAbs(path[i].x() - path[i-1].x()) + qAbs(path[i].y() - path[i-1].y());
    }
    
    int minimalLen = qAbs(minimalPath[1].x() - minimalPath[0].x()) + qAbs(minimalPath[1].y() - minimalPath[0].y()) +
                     qAbs(minimalPath[2].x() - minimalPath[1].x()) + qAbs(minimalPath[2].y() - minimalPath[1].y());
    
    // 如果简化后的路径长度不超过原路径，使用简化路径
    if (minimalLen <= originalLen)
    {
        return minimalPath;
    }
    
    // 否则，尝试简化中间部分
    QList<QPoint> result = path;
    
    // 循环简化，直到无法再简化
    bool changed = true;
    while (changed && result.size() > 3)
    {
        changed = false;
        QList<QPoint> simplified;
        simplified.append(result.first());
        
        for (int i = 1; i < result.size() - 1; ++i)
        {
            QPoint prev = simplified.last();
            QPoint curr = result[i];
            QPoint next = result[i + 1];
            
            // 检查是否可以跳过当前点
            bool canSkip = false;
            
            // 如果 prev 和 next 在同一直线上，可以跳过
            if (prev.x() == next.x() || prev.y() == next.y())
            {
                canSkip = true;
            }
            // 如果 prev->curr->next 可以简化为 prev->next（L形）
            else
            {
                QPoint directDir = next - prev;
                QPoint viaDir1 = curr - prev;
                QPoint viaDir2 = next - curr;
                
                if ((directDir.x() == 0 || directDir.y() == 0) &&
                    (viaDir1.x() == 0 || viaDir1.y() == 0) &&
                    (viaDir2.x() == 0 || viaDir2.y() == 0))
                {
                    int directLen = qAbs(directDir.x()) + qAbs(directDir.y());
                    int viaLen = qAbs(viaDir1.x()) + qAbs(viaDir1.y()) + qAbs(viaDir2.x()) + qAbs(viaDir2.y());
                    
                    if (directLen <= viaLen)
                    {
                        canSkip = true;
                    }
                }
            }
            
            if (!canSkip)
            {
                simplified.append(curr);
            }
            else
            {
                changed = true;
            }
        }
        
        simplified.append(result.last());
        result = simplified;
    }
    
    return result;
}

bool CimdrawAStar::pathSegmentsIntersectObstacles(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const
{
    if (path.size() < 2 || items.isEmpty())
        return false;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        if (segmentIntersectsObstacle(path[i], path[i + 1], items, QPoint()))
            return true;
    }
    return false;
}

bool CimdrawAStar::segmentIntersectsObstacles(const QPoint& a, const QPoint& b, const QList<QGraphicsItem*>& items) const
{
    return segmentIntersectsObstacle(a, b, items, QPoint());
}

// 判断两点能否用正交折线（最多两段）连接且不与障碍相交
static bool orthogonalConnectionClear(const QPoint& a, const QPoint& b,
    const QList<QGraphicsItem*>& items, const CimdrawAStar* astar)
{
    if (a == b)
        return true;
    if (a.x() == b.x() || a.y() == b.y())
        return !astar->segmentIntersectsObstacles(a, b, items);
    QPoint mid1(b.x(), a.y());
    bool clear1 = !astar->segmentIntersectsObstacles(a, mid1, items)
        && !astar->segmentIntersectsObstacles(mid1, b, items);
    if (clear1)
        return true;
    QPoint mid2(a.x(), b.y());
    return !astar->segmentIntersectsObstacles(a, mid2, items)
        && !astar->segmentIntersectsObstacles(mid2, b, items);
}

QList<QPoint> CimdrawAStar::simplifyPathByStringPulling(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const
{
    if (path.size() <= 2)
        return path;

    QList<QPoint> out;
    out.append(path.first());
    int i = 0;
    while (i < path.size() - 1)
    {
        int bestJ = -1;
        ShortcutResult bestShortcut;
        for (int j = path.size() - 1; j > i; --j)
        {
            const ShortcutResult shortcut = bestOrthogonalShortcut(path[i], path[j], items, this);
            if (shortcut.clear)
            {
                bestJ = j;
                bestShortcut = shortcut;
                break;
            }
        }
        if (bestJ >= 0)
        {
            if (bestShortcut.needsBend && bestShortcut.bend != out.last() && bestShortcut.bend != path[bestJ])
                out.append(bestShortcut.bend);
            out.append(path[bestJ]);
            i = bestJ;
        }
        else
        {
            out.append(path[i + 1]);
            ++i;
        }
    }
    return out;
}

QList<QPoint> CimdrawAStar::simplifyPathByDiagonalShortcuts(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const
{
    if (path.size() <= 2)
        return path;

    QList<QPoint> result = path;
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < result.size() - 2; ++i)
        {
            if (!segmentIntersectsObstacles(result[i], result[i + 2], items))
            {
                result.removeAt(i + 1);
                changed = true;
                break;
            }
        }
    }
    return result;
}
