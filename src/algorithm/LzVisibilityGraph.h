#ifndef LZVISIBILITYGRAPH_H
#define LZVISIBILITYGRAPH_H

#include <QPoint>
#include <QPointF>
#include <QList>
#include <QHash>
#include "LzAStar.h"

class QGraphicsItem;

/**
 * @brief 正交可见性图路由（proposal-phase1-algorithms-first 任务 1.1）
 * 节点：起点、终点、障碍膨胀矩形角点；边：水平/垂直且与障碍不相交的线段。
 * 在可见性图上 A* 得到路径点序列，再经与现有一致的后处理输出。
 */
class LzVisibilityGraph
{
public:
    /// 在可见性图上规划正交路径；若不可达返回空列表。astar 用于线段与障碍相交检测（与网格 A* 共用 ObstacleClearanceMargin）
    static QList<QPoint> planPath(
        const QPointF& startScene,
        const QPointF& endScene,
        const QList<QGraphicsItem*>& obstacles,
        const LzAStar* astar,
        int turnPenalty = 5,
        int gridAlignStep = StepSize);
};

#endif // LZVISIBILITYGRAPH_H
