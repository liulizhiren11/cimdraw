#include "CimdrawEllipse.h"
#include "CimdrawObjectFactory.h"
#include <QPainter>
#include "CimdrawToolboxIconPaint.h"

CimdrawEllipse::CimdrawEllipse(QGraphicsItem* parent)
    :CimdrawItem(parent)
{

}

CimdrawEllipse::CimdrawEllipse(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath CimdrawEllipse::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawEllipse::boundingRect() const
{
    return itemPosition;
}

void CimdrawEllipse::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    prepare(painter);
    painter->drawEllipse(itemPosition); // 使用初始矩形进行绘制
}

void CimdrawEllipse::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 1 || rect.height() < 1)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    painter->drawEllipse(rect);
}

void CimdrawEllipse::updateCoordinate()
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

bool CimdrawEllipse::saveXml(QDomElement *g)
{
    g->setAttribute("shape", CIMDRAW_ELLIPSE);
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawEllipse::loadXml(QDomElement *g)
{
    CimdrawItem::loadXml(g);
    updateCoordinate();
    return true;
}

QString CimdrawEllipse::className() const
{
    return "CimdrawEllipse";
}

QString CimdrawEllipse::shapeName() const
{
    return "ellipse";
}

REGISTER_OBJECT_CREATOR(CimdrawEllipse, ellipse)
