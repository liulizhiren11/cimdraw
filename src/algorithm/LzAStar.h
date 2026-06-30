#ifndef LZASTAR_H
#define LZASTAR_H

#include <QPoint>
#include <QVector>
#include <QSet>
#include <QHash>
#include <QRect>
#include <QGraphicsItem>
#include <QPainterPath>

// 步进大小（网格粒度，越大路径点越少，但窄通道需至少约 2*ObstacleClearanceMargin 宽）
constexpr int StepSize = 20;
/// 最细网格步进（像素），用于窄通道局部加密
constexpr int MinGridStep = 5;
// 障碍物安全边距：路径与障碍至少保持该像素距离，贴边路径视为不可取，需明显留空
constexpr int ObstacleClearanceMargin = 60;

// A* 用节点结构体
struct Node {
    QPoint pt;         // 当前点坐标
    QPoint dir;        // 当前方向
    int g;             // 起点到当前点的代价
    int h;             // 到目标的启发估计（用于 f 相等时优先扩展更接近目标的节点）
    int f;             // 总估价 f = g + h

    // 优先队列：f 小优先；f 相同时 h 小优先（更接近目标的先扩展，利于缩短路径）
    bool operator>(const Node& other) const {
        return f > other.f || (f == other.f && h > other.h);
    }
};

class LzAStar
{
public:
    LzAStar();

    /// 网格 A* 步进（像素），默认 StepSize；窄通道可设为 10、5 等更细粒度（不小于 MinGridStep）
    void setGridStep(int stepPixels);
    int gridStep() const { return m_gridStep; }

    /** 限制 A* 扩展次数；<=0 表示使用默认上限（约 10 万）。交互路径建议 4000～8000 */
    void setMaxSearchIterations(int maxIterations);
    int maxSearchIterations() const { return m_maxSearchIterations; }

    /// turnPenalty: 拐弯惩罚（默认 5）；proximityScale: 贴近障碍代价缩放（默认 1.0）。用于路径优先模式（最短/少拐弯/平衡）
    QList<QPoint> aStarWithDirectionPenalty(const QPoint& start, const QPoint& goal, QList<QGraphicsItem*> items,
                                            int turnPenalty = 5, qreal proximityScale = 1.0);

    QList<QPoint> simplifyOrthogonalPath(const QList<QPoint>& path);
    
    // 优化路径，使其更接近 Visio 的连线效果；若 obstacleItems 非空且优化后穿障则退回原 path
    QList<QPoint> optimizePathForVisioStyle(const QList<QPoint>& path, const QPoint& start, const QPoint& end,
                                            const QList<QGraphicsItem*>& obstacleItems = {});
    
    // 确保路径严格正交，并确保起点和终点的连接方向正确
    QList<QPoint> ensureOrthogonalPath(const QList<QPoint>& path, const QPoint& originalStart, const QPoint& originalEnd);
    
    // 进一步简化路径，将U形或Z形路径简化为L形路径
    QList<QPoint> simplifyToMinimalPath(const QList<QPoint>& path);

    /// 路径中是否存在任意线段与障碍相交（用于简化后校验，避免简化成穿图元的直线/L形）
    bool pathSegmentsIntersectObstacles(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const;

    /// 单段线段是否与障碍相交（供外部选不穿图元的首尾拐点）
    bool segmentIntersectsObstacles(const QPoint& a, const QPoint& b, const QList<QGraphicsItem*>& items) const;

    /// 图元在场景中的避障矩形（优先 shape，再 sceneBoundingRect），并膨胀 margin
    static QRectF expandedObstacleSceneRect(const QGraphicsItem* item,
                                          int margin = ObstacleClearanceMargin);

    /// 正交路径拉线简化：在保证不穿障前提下，贪心跳过可直达的中间点，减少路径点与路径长度
    QList<QPoint> simplifyPathByStringPulling(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const;

    /// 对角捷径：若相邻两段可被一条直线（含对角线）替代且不穿障，则删中间点，缩短路径并减少点数
    QList<QPoint> simplifyPathByDiagonalShortcuts(const QList<QPoint>& path, const QList<QGraphicsItem*>& items) const;

private:
    bool isBlocked(const QPoint& pt, QList<QGraphicsItem*> items);
    /// 线段 (a,b) 是否与任一障碍相交（允许端点落在含 goalAllow 的障碍上）
    bool segmentIntersectsObstacle(const QPoint& a, const QPoint& b,
                                   const QList<QGraphicsItem*>& items,
                                   const QPoint& goalAllow = QPoint()) const;
    QPoint nearestWalkable(const QPoint& goalAligned, QList<QGraphicsItem*> items, int maxRing = 50);
    int heuristic(const QPoint& a, const QPoint& b);
    QList<QPoint> getNeighbors(const QPoint& p, const QPoint& goal, QList<QGraphicsItem*> items);
    QList<QPoint> reconstructPath(const QHash<QPoint, QPoint>& cameFrom, QPoint current);
    QPoint alignToGrid(const QPoint& pt, int step);

    int m_gridStep = StepSize;
    int m_maxSearchIterations = 0;
};

#endif // LZASTAR_H