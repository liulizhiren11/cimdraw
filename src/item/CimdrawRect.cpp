#include "CimdrawRect.h"
#include "CimdrawObjectFactory.h"
#include <QPainter>
#include "CimdrawToolboxIconPaint.h"

CimdrawRect::CimdrawRect(QGraphicsItem* parent)
    :CimdrawItem(parent)
{

}

CimdrawRect::CimdrawRect(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent)
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
        CimdrawConnectPoint* point = new CimdrawConnectPoint(this); // 注意放在 parent 图元上而不是 this 上
        point->setDir(i);
        point->setDirection(static_cast<CONNECT_DIRECTION>(i)); // 如果有两个方法都设置方向，可以合并

        point->bindToItem(this, offsets[i]); // 设置相对于本体的位置

        pointStruct.points.push_back(point);
    }
    updateCoordinate();
    updatePoints();
}

QPainterPath CimdrawRect::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawRect::boundingRect() const
{
    return itemPosition;
}

void CimdrawRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    prepare(painter);
    painter->drawRect(itemPosition.toRect()); // 使用初始矩形进行绘制
}

void CimdrawRect::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 1 || rect.height() < 1)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    painter->drawRect(rect.toRect());
}

void CimdrawRect::updateCoordinate()
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

bool CimdrawRect::saveXml(QDomElement *g)
{
    g->setAttribute("shape", CIMDRAW_RECT);
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawRect::loadXml(QDomElement *g)
{
    CimdrawItem::loadXml(g);
    updateCoordinate();
    return true;
}

void CimdrawRect::stretch(int handle, double sx, double sy, const QPointF & origin)
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

QGraphicsItem* CimdrawRect::duplicate()
{
    CimdrawRect* item = new CimdrawRect(rect());
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

QString CimdrawRect::className() const
{
    return "CimdrawRect";
}

QString CimdrawRect::shapeName() const
{
    return "rect";
}

void CimdrawRect::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

REGISTER_OBJECT_CREATOR(CimdrawRect, rect)
