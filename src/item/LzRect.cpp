#include "LzRect.h"
#include "LzObjectFactory.h"
#include <QPainter>
#include "LzToolboxIconPaint.h"

LzRect::LzRect(QGraphicsItem* parent)
    :LzItem(parent)
{

}

LzRect::LzRect(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    pointStruct.count = 4;
    pointStruct.points.reserve(LEFT_DIRECTION);
    QPointF offsets[4] =
    {
        QPointF(0.0, -0.5), 
        QPointF(0.5, 0.0), 
        QPointF(0.0, 0.5),
        QPointF(-0.5, 0.0) 
    };

    for (int i = TOP_DIRECTION; i <= LEFT_DIRECTION; ++i)
    {
        LzConnectPoint* point = new LzConnectPoint(this); // 注意放在 parent 图元上而不是 this 上
        point->setDir(i);
        point->setDirection(static_cast<CONNECT_DIRECTION>(i)); // 如果有两个方法都设置方向，可以合并

        point->bindToItem(this, offsets[i]); // 设置相对于本体的位置

        pointStruct.points.push_back(point);
    }
    updateCoordinate();
    updatePoints();
}

QPainterPath LzRect::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzRect::boundingRect() const
{
    return itemPosition;
}

void LzRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    prepare(painter);
    painter->drawRect(itemPosition.toRect()); // 使用初始矩形进行绘制
}

void LzRect::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 1 || rect.height() < 1)
        return;
    lzApplyItemDefaultPenBrush(painter);
    painter->drawRect(rect.toRect());
}

void LzRect::updateCoordinate()
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

bool LzRect::saveXml(QDomElement *g)
{
    g->setAttribute("shape", LZ_RECT);
    LzItem::saveXml(g);
    return true;
}

bool LzRect::loadXml(QDomElement *g)
{
    LzItem::loadXml(g);
    updateCoordinate();
    return true;
}

void LzRect::stretch(int handle, double sx, double sy, const QPointF & origin)
{
    QTransform trans ;
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

    trans.translate(origin.x(),origin.y());
    trans.scale(sx,sy);
    trans.translate(-origin.x(),-origin.y());

    prepareGeometryChange();
    itemPosition = trans.mapRect(itemInitialRect);
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    updateHandles();
    updatePoints();
}

QGraphicsItem* LzRect::duplicate()
{
    LzRect* item = new LzRect(rect());
    item->itemWidth = getWidth();
    item->itemHeight = getHeight();
    item->setPos(pos().x(),pos().y());
    item->setTransform(transform());
    item->setTransformOriginPoint(transformOriginPoint());
    item->setRotation(rotation());
    item->setScale(scale());
    item->setZValue(0);
    item->updateCoordinate();
    return item;
}

QString LzRect::className() const
{
    return "LzRect";
}

QString LzRect::shapeName() const
{
    return "rect";
}

void LzRect::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

REGISTER_OBJECT_CREATOR(LzRect, rect)
