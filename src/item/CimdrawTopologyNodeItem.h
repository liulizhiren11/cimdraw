#ifndef CIMDRAWTOPOLOGYNODEITEM_H
#define CIMDRAWTOPOLOGYNODEITEM_H

#include "CimdrawItem.h"

class QGraphicsSceneMouseEvent;
class QPainter;
class QPainterPath;

/**
 * @brief 拓扑节点图元：带四向连接点，用于与 CimdrawConnectLine 配置节点间拓扑关系
 */
class CimdrawTopologyNodeItem : public CimdrawItem
{
    Q_OBJECT
public:
    explicit CimdrawTopologyNodeItem(QGraphicsItem* parent = nullptr);
    CimdrawTopologyNodeItem(const QRectF& pos, QGraphicsItem* parent = nullptr);

    QPainterPath shape() const;
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    /** 工具箱图标：与 paint 中圆角矩形描边/填充一致 */
    static void paintToolboxIcon(QPainter* painter, const QRectF& rect);
    void updateCoordinate();
    bool saveXml(QDomElement* g);
    bool loadXml(QDomElement* g);
    void stretch(int handle, double sx, double sy, const QPointF& origin);
    QGraphicsItem* duplicate();
    QString className() const;
    QString shapeName() const;
    bool isTopologyGraphNode() const override;
    bool participatesInTopology() const override;
    CimdrawTopologyDomain topologyDomain() const override;

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
};

#endif
