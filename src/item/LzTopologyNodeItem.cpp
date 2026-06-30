#include "LzTopologyNodeItem.h"
#include "LzObjectFactory.h"
#include "LzConnectPoint.h"
#include <QPainter>
#include <QPainterPath>

LzTopologyNodeItem::LzTopologyNodeItem(QGraphicsItem* parent)
    : LzItem(parent)
{
}

LzTopologyNodeItem::LzTopologyNodeItem(const QRectF& pos, QGraphicsItem* parent)
    : LzItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0, 0, itemWidth, itemHeight);
    itemInitialRect = itemPosition;
    pointStruct.count = 4;
    pointStruct.points.reserve(LEFT_DIRECTION + 1);
    const QPointF offsets[4] = {
        QPointF(0.0, -0.5),
        QPointF(0.5, 0.0),
        QPointF(0.0, 0.5),
        QPointF(-0.5, 0.0),
    };

    for (int i = TOP_DIRECTION; i <= LEFT_DIRECTION; ++i)
    {
        LzConnectPoint* point = new LzConnectPoint(this);
        point->setDir(i);
        point->setDirection(static_cast<CONNECT_DIRECTION>(i));
        point->bindToItem(this, offsets[i]);
        pointStruct.points.push_back(point);
    }
    updateCoordinate();
    updatePoints();
}

QPainterPath LzTopologyNodeItem::shape() const
{
    QPainterPath path;
    path.addRoundedRect(itemPosition, 6.0, 6.0);
    return path;
}

QRectF LzTopologyNodeItem::boundingRect() const
{
    return itemPosition;
}

void LzTopologyNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    QPen pen = painter->pen();
    pen.setColor(QColor(0, 100, 120));
    pen.setWidth(2);
    painter->setPen(pen);
    if (!getUseBrush())
        painter->setBrush(QColor(220, 248, 252, 200));
    painter->drawRoundedRect(itemPosition, 6.0, 6.0);
}

void LzTopologyNodeItem::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    QPen pen(QColor(0, 100, 120), 2);
    painter->setPen(pen);
    painter->setBrush(QColor(220, 248, 252, 200));
    painter->drawRoundedRect(rect, 6.0, 6.0);
}

void LzTopologyNodeItem::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(itemPosition.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
        updatePoints();
    }
    itemInitialRect = itemPosition;
}

bool LzTopologyNodeItem::saveXml(QDomElement* g)
{
    g->setAttribute(QStringLiteral("shape"), LZ_TOPOLOGY_NODE);
    LzItem::saveXml(g);
    return true;
}

bool LzTopologyNodeItem::loadXml(QDomElement* g)
{
    if (!LzItem::loadXml(g))
        return false;
    if (pointStruct.points.isEmpty())
    {
        itemPosition = QRectF(0, 0, itemWidth, itemHeight);
        itemInitialRect = itemPosition;
        pointStruct.count = 4;
        pointStruct.points.reserve(LEFT_DIRECTION + 1);
        const QPointF offsets[4] = {
            QPointF(0.0, -0.5),
            QPointF(0.5, 0.0),
            QPointF(0.0, 0.5),
            QPointF(-0.5, 0.0),
        };
        for (int i = TOP_DIRECTION; i <= LEFT_DIRECTION; ++i)
        {
            LzConnectPoint* point = new LzConnectPoint(this);
            point->setDir(i);
            point->setDirection(static_cast<CONNECT_DIRECTION>(i));
            point->bindToItem(this, offsets[i]);
            pointStruct.points.push_back(point);
        }
    }
    updateCoordinate();
    return true;
}

void LzTopologyNodeItem::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    QTransform trans;
    switch (handle)
    {
    case RIGHT:
    case LEFT:
        sy = 1;
        break;
    case TOP:
    case BOTTOM:
        sx = 1;
        break;
    default:
        break;
    }

    trans.translate(origin.x(), origin.y());
    trans.scale(sx, sy);
    trans.translate(-origin.x(), -origin.y());

    prepareGeometryChange();
    itemPosition = trans.mapRect(itemInitialRect);
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    updateHandles();
    updatePoints();
}

QGraphicsItem* LzTopologyNodeItem::duplicate()
{
    LzTopologyNodeItem* item = new LzTopologyNodeItem(rect());
    item->itemWidth = getWidth();
    item->itemHeight = getHeight();
    item->setPos(pos().x(), pos().y());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(0);
    item->updateCoordinate();
    return item;
}

QString LzTopologyNodeItem::className() const
{
    return QStringLiteral("LzTopologyNodeItem");
}

QString LzTopologyNodeItem::shapeName() const
{
    return QStringLiteral("topologynode");
}

bool LzTopologyNodeItem::isTopologyGraphNode() const
{
    return true;
}

bool LzTopologyNodeItem::participatesInTopology() const
{
    return true;
}

LzTopologyDomain LzTopologyNodeItem::topologyDomain() const
{
    return LzTopologyDomain::None;
}

void LzTopologyNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

REGISTER_OBJECT_CREATOR(LzTopologyNodeItem, topologynode)
