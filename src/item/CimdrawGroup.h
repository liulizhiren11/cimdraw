#ifndef CIMDRAWGROUP_H
#define CIMDRAWGROUP_H

#include <QObject>
#include <QGraphicsItemGroup>
#include <QVector>
#include "TmpBase.h"
#include "CimdrawConnectConfig.h"

class CimdrawConnectLine;

struct CimdrawConnectLinePathSnapshot
{
    CimdrawConnectLine* line = nullptr;
    QVector<QPointF> scenePoints;
    ConnectorPathRoutingMode routingMode = ConnectorPathRoutingMode::Auto;
};

class CimdrawGroup : public QObject, public TmpBase<QGraphicsItemGroup>
{
    Q_OBJECT
    Q_PROPERTY(bool usePen READ getUsePen WRITE setUsePen)
    Q_PROPERTY(QColor itemPenColor READ getItemPenColor WRITE setItemPenColor)
    Q_PROPERTY(int itemPenWidth READ getItemPenWidth WRITE setItemPenWidth)
    Q_PROPERTY(int itemPenStyle READ getItemPenStyle WRITE setItemPenStyle)

    Q_PROPERTY(bool useBrush READ getUseBrush WRITE setUseBrush)
    Q_PROPERTY(QColor itemBrushColor READ getItemBrushColor WRITE setItemBrushColor)
    Q_PROPERTY(int itemBrushStyle READ getItemBrushStyle WRITE setItemBrushStyle)

    Q_PROPERTY(bool useFont READ getUseFont WRITE setUseFont)
    Q_PROPERTY(QFont itemFont READ getItemFont WRITE setItemFont)

    Q_PROPERTY(QPointF position READ pos WRITE setPos)
    Q_PROPERTY(qreal rotate READ getItemDegree WRITE setItemDegree)
    Q_PROPERTY(QPointF sceneTransformOriginPoint READ getSceneTransformOriginPoint)
public:
    explicit CimdrawGroup(QGraphicsItem* parent = nullptr);
    CimdrawGroup(const QRectF& pos,QGraphicsItem* parent = nullptr);

    static void refreshConnectionsForItemTree(QGraphicsItem* item, bool preservePathShape = false);
    /** 收集与待分组图元相连（或自身为）的连接线路径快照 */
    static void collectConnectLineSnapshots(const QList<QGraphicsItem*>& items,
                                           QVector<CimdrawConnectLinePathSnapshot>& out);
    static void restoreConnectLineSnapshots(const QVector<CimdrawConnectLinePathSnapshot>& snapshots);
    /** 分组/解组时按场景折点原样恢复，不做中间折点保形重排 */
    static void restoreConnectLineSnapshotsExact(const QVector<CimdrawConnectLinePathSnapshot>& snapshots);
    /** 仅收集作为分组直接子项的连接线（不含组外相连线） */
    static void collectConnectLineSnapshotsInGroup(const CimdrawGroup* group,
                                                 QVector<CimdrawConnectLinePathSnapshot>& out);
    /** 恢复图元与端点形状之间的 addConnect / 拓扑绑定（不改变路径折点） */
    static void reattachConnectLinesAmongItems(const QList<QGraphicsItem*>& items);
    /** 仅登记连接关系，不 sync 端点（粘贴后路径已 restore 时使用） */
    static void registerConnectLinesAmongItems(const QList<QGraphicsItem*>& items);
    /** 将两端节点均在选中范围内的连接线一并纳入分组列表 */
    static QList<QGraphicsItem*> expandItemsWithInternalConnectLines(const QList<QGraphicsItem*>& items);

    enum { Type = UserType + 2 };
	int  type() const 
    { 
        return Type; 
    }

    ~CimdrawGroup();

    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void control(int dir, const QPointF & delta);

	void stretch(int handle, double sx, double sy, const QPointF & origin);

    bool saveXml(QDomElement* g) override;

    bool loadXml(QDomElement* g) override;

    void addToGroup(QGraphicsItem* item);

    void removeFromGroup(QGraphicsItem* item);

    void updateCoordinate();
    /** 按子项（含组内连线）外接矩形刷新 itemPosition，移动/改线后保持选择框正确 */
    void syncItemBoundsFromChildren();
    /** 复制分组选择框几何（粘贴/duplicate 时保持与原分组一致） */
    void copyItemFrameFrom(const CimdrawGroup* source);

    QGraphicsItem* duplicate() override;

    void setWidth(qreal width) override;

    void setHeight(qreal height) override;

    void setSize(QSizeF size) override;

    void setPreserveLinePathsDuringGeometryUpdate(bool preserve);
    bool preserveLinePathsDuringGeometryUpdate() const;

    // 分组在当前业务口径中仅作为编辑容器，不作为拓扑参与者。
    bool isTopologyGraphNode() const override { return false; }
    bool participatesInTopology() const override { return false; }
    CimdrawTopologyDomain topologyDomain() const override { return CimdrawTopologyDomain::None; }

    qreal itemLeft() override;

    qreal itemTop() override;

    qreal itemBottom() override;

    qreal itemRight() override;

    qreal centerX() override;

    qreal centerY() override;

protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
    bool preserveLinePathsDuringGeometryUpdate_ = false;
};

#endif
