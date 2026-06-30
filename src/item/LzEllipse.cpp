#include "LzEllipse.h"
#include "LzObjectFactory.h"
#include <QPainter>
#include "LzToolboxIconPaint.h"

LzEllipse::LzEllipse(QGraphicsItem* parent)
    :LzItem(parent)
{

}

LzEllipse::LzEllipse(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath LzEllipse::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzEllipse::boundingRect() const
{
    return itemPosition;
}

void LzEllipse::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    prepare(painter);
    painter->drawEllipse(itemPosition); // 使用初始矩形进行绘制
}

void LzEllipse::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 1 || rect.height() < 1)
        return;
    lzApplyItemDefaultPenBrush(painter);
    painter->drawEllipse(rect);
}

void LzEllipse::updateCoordinate()
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
    }
    itemInitialRect = itemPosition;
}

bool LzEllipse::saveXml(QDomElement *g)
{
    g->setAttribute("shape", LZ_ELLIPSE);
    LzItem::saveXml(g);
    return true;
}

bool LzEllipse::loadXml(QDomElement *g)
{
    LzItem::loadXml(g);
    updateCoordinate();
    return true;
}

QString LzEllipse::className() const
{
    return "LzEllipse";
}

QString LzEllipse::shapeName() const
{
    return "ellipse";
}

REGISTER_OBJECT_CREATOR(LzEllipse, ellipse)
