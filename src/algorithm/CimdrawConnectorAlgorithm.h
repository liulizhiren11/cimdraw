#ifndef CIMDRAWCONNECTORALGORITHM_H
#define CIMDRAWCONNECTORALGORITHM_H

#include <QPoint>
#include <QPointF>
#include <QList>
#include <QVector>

class QGraphicsItem;

/**
 * @brief draw.io 风格连接线调整算法
 * 提供：路径点插入投影、正交拐点约束、正交路径规划等，与 CimdrawAStar 配合使用
 */
class CimdrawConnectorAlgorithm
{
public:
    /// 路由策略：网格 A*（默认）或正交可见性图；可见性图无解时自动回退到网格 A*
    enum ConnectorRoutingStrategy { GridAStar, VisibilityGraph };

    /// 路径优先模式（proposal-phase1 任务 1.5）：最短 / 少拐弯 / 平衡
    enum PathPriorityMode { PathShortest, PathFewerBends, PathBalanced };

    CimdrawConnectorAlgorithm() = default;

    // ============= 路径点插入（添加路径点） =============
    /// 在折线线段上投影点，得到插入位置（segmentIndex 为“插入到该段之后”的段下标，projected 为投影点）
    struct InsertWaypointResult
    {
        int segmentIndex = -1;   ///< 插入到该段之后，即新点索引 = segmentIndex + 1
        QPointF projected;      ///< 投影点（与 polyline 同坐标系）
        bool valid = false;
    };
    static InsertWaypointResult projectPointOnPolyline(
        const QVector<QPointF>& polyline,
        const QPointF& point);

    // ============= 正交拐点约束（拖拽中间路径点） =============
    /// 正交时拐点只能落在 (prev.x(), next.y()) 或 (next.x(), prev.y())，按与 cursor 距离或主导方向选一
    static QPointF orthogonalCornerPosition(
        const QPointF& prev,
        const QPointF& next,
        const QPointF& cursorPosition);

    // ============= 正交路径规划（清除路径点并重新规划 / 端点拖拽） =============
    /// 规划起点到终点的正交路径，避开 obstacles，返回路径点（含起点终点）
    /// strategy：GridAStar（默认）或 VisibilityGraph；VisibilityGraph 无解时自动回退网格 A*
    /// priority：PathShortest / PathFewerBends / PathBalanced（默认）
    static QVector<QPoint> planOrthogonalPath(
        const QPointF& startScene,
        const QPointF& endScene,
        const QList<QGraphicsItem*>& obstacles,
        ConnectorRoutingStrategy strategy = GridAStar,
        PathPriorityMode priority = PathBalanced);

    /// 拖动结束等交互后的轻量规划：单次 A* + 简化，不做多轮后处理
    static QVector<QPoint> planOrthogonalPathQuick(
        const QPointF& startScene,
        const QPointF& endScene,
        const QList<QGraphicsItem*>& obstacles);

    // ============= L 形选择（按主导方向） =============
    /// 两种 L 形拐点：先水平后垂直 = (end.x(), start.y())，先垂直后水平 = (start.x(), end.y())
    /// 返回 true 表示选先水平后垂直，false 表示先垂直后水平；长度相同时按 dx>=dy 选
    static bool preferHorizontalFirst(const QPointF& start, const QPointF& end);

    // ============= 连接线共享段 Nudging（proposal-phase1 任务 1.2） =============
    /// 对多条路径中共享同一水平/垂直通道的线段分配偏移并应用，返回 nudging 后的路径列表（与输入一一对应）
    static QList<QVector<QPointF>> computeNudgingOffsets(const QList<QVector<QPointF>>& paths,
                                                        qreal spacing = 6.0,
                                                        qreal minOverlap = 10.0);
};

#endif // CIMDRAWCONNECTORALGORITHM_H
