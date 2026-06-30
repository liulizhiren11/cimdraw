#ifndef LZCONNECTLINE_H
#define LZCONNECTLINE_H

#include <QObject>
#include <QGraphicsItem>
#include <QScopedPointer>
#include <QVector>
#include <QLineF>
#include "LzItem.h"
#include "LzConnectConfig.h"
#include "algorithm/LzConnectorAlgorithm.h"
#include "topology/TopologyTypes.h"

class QPainterPath;
class QPainter;
class LzConnectLinePrivate;
class LzConnectPoint;
class QGraphicsSceneContextMenuEvent;
class LzScene;

class LzConnectLine : public LzItem
{
    Q_OBJECT
public:
    /// draw.io 风格拐角样式：直角 / 圆角 / 弧线（仅影响绘制，路径几何仍为正交）
    enum CornerStyle { Sharp, Rounded, Curved };

    /// 箭头：无 / 终点 / 起点 / 双向
    enum ArrowHeadStyle { ArrowNone = 0, ArrowEnd = 1, ArrowStart = 2, ArrowBoth = 3 };

    enum { Type = UserType + 4 };  // 必须与 LzItem::Type(UserType+3) 区分，否则矩形等会被误当作连接线导致崩溃

    Q_PROPERTY(int cornerStyle READ cornerStyleProperty WRITE setCornerStyleProperty)
    Q_PROPERTY(int arrowHeadStyle READ arrowHeadStyle WRITE setArrowHeadStyle)
    Q_PROPERTY(bool lineJumpEnabled READ lineJumpEnabled WRITE setLineJumpEnabled)
    Q_PROPERTY(int connectStrategy READ connectStrategyProperty WRITE setConnectStrategyProperty)

    int type() const override { return Type; }

    explicit LzConnectLine(QGraphicsItem* parent = nullptr);

    LzConnectLine(const QRectF& pos,QGraphicsItem* parent = nullptr);

    ~LzConnectLine() override;

    /** 工具箱图标：与 paint 使用的正交路径 + Sharp 拐角逻辑一致（无箭头） */
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);

    /**
     * @brief    获取形状
     * @return   形状
     * @date     2025-05-01
    */
    QPainterPath shape() const;

    /**
     * @brief    获取边界矩形
     * @return   边界矩形
     * @date     2025-05-01
    */
    QRectF boundingRect() const override;

    /**
     * @brief    绘制连线
     * @param    painter 画笔
     * @param    option 选项
     * @param    widget 窗口
     * @date     2025-05-01
    */  
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    /**
     * @brief    更新坐标
     * @date     2025-05-01
    */
    void updateCoordinate() override;

    /**
     * @brief    保存连线
     * @param    g 连线
     * @date     2025-05-01
    */
    bool saveXml(QDomElement* g) override;

    /**
     * @brief    加载连线
     * @param    g 连线
     * @date     2025-05-01
    */
    bool loadXml(QDomElement* g) override;

    /**
     * @brief    获取类名
     * @return   类名
     * @date     2025-05-01
    */
    QString className() const;

    /**
     * @brief    获取连线名称
     * @return   连线名称
     * @date     2025-05-01
    */
    QString shapeName() const;

    /** 复制几何与样式；端点图元指针置空，保留拓扑端点稳定 ID 字符串供剪贴板粘贴重绑 */
    QGraphicsItem* duplicate() override;

    /**
     * @brief    添加点
     * @param    point 点
     * @date     2025-05-01
    */
    void addPoint(const QPointF& point);

    /**
     * @brief    获取控制点数量
     * @return   控制点数量
     * @date     2025-05-01
    */
    int handleCount() const;

    /**
     * @brief    拉伸连线
     * @param    handle 控制点
     * @param    sx 缩放比例
     * @param    sy 缩放比例
     * @param    point 点
     * @date     2025-05-01
    */
    void stretch(int handle, double sx, double sy, const QPointF &) override;

    /**
     * @brief    控制连线
     * @param    dir 方向
     * @param    delta 移动的距离
     * @date     2025-05-01
    */
    void control(int dir, const QPointF& delta) override;

    /**
     * @brief    仅更新端点并设为简单 L 形路径（拖动时轻量预览，避免每帧跑 A*）
     * @param    dir 控制点索引，delta 场景坐标下的新位置
     */
    void setEndpointPreview(int dir, const QPointF& scenePos);

    /** 从起点端口拖向未连接终点时的轻量正交预览（不跑 A*，不受 Ctrl 约束） */
    void setWireDragPreviewEnd(const QPointF& endScene);

    /** 端点拖拽时按给定基准路径计算新轨迹，避免拖拽过程中链式合并后继续偏移 */
    void applyEndpointDragAtScenePos(int controlIndex, const QPointF& scenePos,
                                     const QVector<QPointF>& baseScenePath = {});

    /**
     * @brief    更新控制点
     * @date     2025-05-01
    */
    void updateHandles();

    /**
     * @brief    设置开始图元
     * @param    item 开始图元
     * @date     2025-05-01
    */
    void setStartItem(QGraphicsItem* item);

    /**
     * @brief    获取开始图元
     * @return   开始图元
     * @date     2025-05-01
    */
    QGraphicsItem* getStartItem();
    QGraphicsItem* getStartItem() const;

    /**
     * @brief    设置结束图元
     * @param    item 结束图元
     * @date     2025-05-01
    */
    void setEndItem(QGraphicsItem* item);

    /**
     * @brief    获取结束图元
     * @return   结束点
     * @date     2025-05-01
    */  
    QGraphicsItem* getEndItem();
    QGraphicsItem* getEndItem() const;

    /**
     * @brief    更新连线位置
     * @param    item 连线
     * @param    delta 移动的距离
     * @date     2025-05-01
    */
    void updatePosition(QGraphicsItem* item, const QPointF& delta);

    void setStartConnectPort(LzConnectPoint* port);
    void setEndConnectPort(LzConnectPoint* port);
    LzConnectPoint* startConnectPort() const;
    LzConnectPoint* endConnectPort() const;
    /** 父图元几何变化（缩放/端口重绑）后，按绑定端口重算路径 */
    void refreshEndpointsFromAttachedItems();
    /** 撤销/还原/粘贴恢复后，重新挂回起止图元的 connected list。 */
    void reattachToEndpointShapes(bool refreshPath = true);
    /** 仅登记 shape↔line 双向连接与拓扑 ID，不改动路径折点 */
    void registerEndpointAttachments();
    /** 仅同步起止点到绑定端口，不跑 A* */
    void syncEndpointPositionsFromAttachedItems();
    /** 拖动图元结束后：仅同步端口并保留/更新 L 形预览路径，不跑 A* */
    void finalizePathAfterItemDrag();
    /** Manual：仅同步首尾端口，保留中间用户折点 */
    void syncEndpointsPreservingUserPath();
    /** Manual：父级整体平移时按端点位移整体平移路径，避免重新拼接导致斜段 */
    void translateManualPathWithAttachedItems();
    /** 组合/解组或父级变换后：按场景坐标恢复路径，避免触发全量重算 */
    void preservePathAfterHierarchyChange();
    /** 用分组前快照恢复路径：仅更新首尾到端口，中间折点保持场景坐标不变 */
    void restorePathSnapshot(const QVector<QPointF>& sceneSnapshot,
                             ConnectorPathRoutingMode routingMode);
    /** 按场景折点原样写入路径，不吸附端口、不 sanitize（粘贴/撤销用） */
    void applyScenePathExact(const QVector<QPointF>& scenePoints,
                             ConnectorPathRoutingMode routingMode);
    /** 仅将连在母线端的首/尾折点贴到已绑端口，中间折点不变 */
    void snapAttachedBusbarEndpointsToPorts();
    void cancelDeferredPathRecompute();

    ConnectorPathRoutingMode pathRoutingMode() const;
    void setPathRoutingMode(ConnectorPathRoutingMode mode);
    /** 拖折点/拖线段/插点后进入 Manual；首次从 Auto 切到 Manual 时可选择清理继承的自动路由毛刺 */
    void markPathAsUserEdited(bool normalizeInheritedPath = true);

    /**
     * @brief    移动连线
     * @param    delta 移动的距离
     * @date     2025-05-01
    */
    void move(const QPointF& delta) final;

    /**
     * @brief    设置连线策略
     * @param    stratgey 连线策略
     * @date     2025-05-01
    */
    void setConnectStrategy(CONNECTION_STRATEGY stratgey);

    /**
     * @brief    连线策略
     * @return   连线策略
     * @date     2025-05-01
    */
    CONNECTION_STRATEGY getConnectStrategy() const;
    int connectStrategyProperty() const;
    void setConnectStrategyProperty(int strategy);

    /**
     * @brief    重写itemChange
     * @param    change 变化
     * @param    value 值
     * @date     2025-05-01
    */
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

    /**
     * @brief    重写：用与路径点的距离判定命中，避免 ItemIgnoresTransformations 导致 handle 点不中
     * @param    point 场景坐标
     */
    int collidesWithHandle(const QPointF& point) const override;

    /**
     * @brief    规划正交路径
     * @param    startPoint 起始点
     * @param    endPoint 结束点
     * @return   路径点
     * @date     2025-05-01
    */
    QVector<QPoint> planOrthogonalPath(const QPointF& startPoint,
                                       const QPointF& endPoint);

    /// 与 planOrthogonalPath 相同的障碍收集规则（叶子图元，不含 LzGroup/连接点），供 Nudging 等复用
    static QList<QGraphicsItem*> collectRoutingObstacles(const LzConnectLine* line, LzScene* scene,
                                                         const QRectF& queryRect = QRectF());

    /**
     * @brief    根据当前端点重新计算完整正交路径（用于拖动结束后延迟执行，避免拖动时每帧跑 A*）
     */
    void recomputePathFromCurrentEndpoints(bool schedulePostprocess = true);

    /** 绑定终点后是否会与已有连线形成设备级连通环路 */
    bool wouldCompleteConnectivityCycle(QGraphicsItem* endItem) const;

    /** 绑定终点端口并按严格正交重算路径（完成拉线时调用） */
    void attachEndAndRecompute(QGraphicsItem* endItem, LzConnectPoint* endPort);

    /** 仅绑定起点，终点落在场景坐标（draw.io 悬空边）；路径进入 Manual */
    void finalizeWireWithFloatingEnd(const QPointF& endScene);

    /**
     * @brief    用给定的场景坐标路径点替换当前路径（用于 Nudging 等后处理，不重规划）
     * @param    scenePoints 路径点序列（场景坐标），点数需与当前路径一致
     */
    void setPathFromScenePoints(const QVector<QPointF>& scenePoints);

    /// 当前路径点序列（场景坐标），供 Nudging 等使用
    QVector<QPointF> pathInSceneCoords() const;
    /// 编辑入口使用的场景路径：Auto 返回一次性简化后的继承路径，Manual 保持原样
    QVector<QPointF> editablePathInSceneCoords() const;

    /**
     * @brief    draw.io 风格：在场景坐标处插入路径点（如双击线段添加拐点）
     * @param    scenePos 场景坐标
     * @return   成功时返回新插入点的索引（0-based），失败返回 -1
     */
    /** 命中折线段下标（场景坐标），未命中返回 -1 */
    int segmentIndexAtScenePos(const QPointF& scenePos, qreal tolerancePx = 12.0) const;
    int segmentIndexAtScenePosForPath(const QVector<QPointF>& scenePath,
                                      const QPointF& scenePos,
                                      qreal tolerancePx = 12.0) const;

    /**
     * draw.io 风格：沿法向平移指定折线段（基于 baseScenePath + 累计 sceneOffset）
     * @param baseScenePath 按下时的路径快照
     * @param segmentIndex 段下标 i（点 i 与 i+1 之间）
     * @param sceneOffset 自按下点起的场景位移
     */
    void applySegmentDragFromBase(const QVector<QPointF>& baseScenePath,
                                  int segmentIndex,
                                  const QPointF& sceneOffset);

    int insertWaypointAtScenePos(const QPointF& scenePos);

    /**
     * @brief    draw.io 风格：删除指定索引的路径点（起点/终点不可删）
     * @param    pointIndex 路径点索引，0 为起点，count-1 为终点
     * @return   是否成功删除
     */
    bool removeWaypointAt(int pointIndex);

    /**
     * @brief    draw.io 风格：清除所有中间路径点并按当前端点重新规划正交路径
     */
    void clearWaypointsAndReplan();

    /**
     * @brief    draw.io 风格：反转连线方向（交换起点/终点后重新规划）
     */
    void reverseDirection();

    /**
     * @brief    显示连接线右键菜单（供 LzHandle 等转发调用，避免访问 protected）
     */
    void showContextMenu(QGraphicsSceneContextMenuEvent* event);

    CornerStyle cornerStyle() const;
    void setCornerStyle(CornerStyle style);
    int cornerStyleProperty() const;
    void setCornerStyleProperty(int style);

    ArrowHeadStyle arrowHead() const;
    void setArrowHead(ArrowHeadStyle style);
    int arrowHeadStyle() const;
    void setArrowHeadStyle(int style);

    bool lineJumpEnabled() const;
    void setLineJumpEnabled(bool enabled);

    /// 路由策略：网格 A* 或可见性图（proposal-phase1 任务 1.1）
    LzConnectorAlgorithm::ConnectorRoutingStrategy routingStrategy() const;
    void setRoutingStrategy(LzConnectorAlgorithm::ConnectorRoutingStrategy strategy);

    /// 路径优先模式：最短 / 少拐弯 / 平衡（proposal-phase1 任务 1.5）
    LzConnectorAlgorithm::PathPriorityMode pathPriorityMode() const;
    void setPathPriorityMode(LzConnectorAlgorithm::PathPriorityMode mode);

    /// 拓扑：边稳定 ID 与端点节点 ID（与 TmpBase::topologyNodeStableId 对齐）
    QString topologyEdgeStableId() const;
    void setTopologyEdgeStableId(const QString& id);
    QString ensureTopologyEdgeStableId();
    TopologyRelationType topologyRelationType() const;
    void setTopologyRelationType(TopologyRelationType t);
    QString topologyStartNodeStableId() const;
    QString topologyEndNodeStableId() const;
    QString topologyStartPortKey() const;
    QString topologyEndPortKey() const;
    void setTopologyStartPortKey(const QString& key);
    void setTopologyEndPortKey(const QString& key);
    void syncTopologyEndpointsFromItems();

    /** 两端均为拓扑节点图元时的语义连线（与几何连线工具共用 LzConnectLine） */
    bool isTopologyRelationLink() const;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
    void applyScenePathFast(const QVector<QPointF>& scenePts);

    Q_DECLARE_PRIVATE(LzConnectLine);
    QScopedPointer<LzConnectLinePrivate> d_ptr;
};

#endif
