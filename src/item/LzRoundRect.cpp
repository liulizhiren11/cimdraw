#include "LzRoundRect.h"
#include "LzObjectFactory.h"
#include <QPainter>
#include "LzToolboxIconPaint.h"

class LzRoundRectPrivate
{
    Q_DECLARE_PUBLIC(LzRoundRect)
public:
    LzRoundRectPrivate(LzRoundRect* roundrect)
        :q_ptr(roundrect)
    {

    }

    QPointF roundness;
    LzRoundRect* q_ptr;
};

LzRoundRect::LzRoundRect(QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzRoundRectPrivate(this))
{
    Q_D(LzRoundRect);
    setUseFont(false);
    d->roundness.setX(24);
    d->roundness.setY(24);
}

LzRoundRect::LzRoundRect(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzRoundRectPrivate(this))
{
    Q_D(LzRoundRect);
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    d->roundness.setX(24);
    d->roundness.setY(24);
    updateCoordinate();
}

QPainterPath LzRoundRect::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzRoundRect::boundingRect() const
{
    return itemPosition;
}

void LzRoundRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const LzRoundRect);
    prepare(painter);
    
    // Qt 6 中 drawRoundRect 使用 QRectF 和 qreal 参数
    painter->drawRoundedRect(itemPosition, 
                           d->roundness.x(), 
                           d->roundness.y(), 
                           Qt::AbsoluteSize);
}

void LzRoundRect::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    /** 与 LzRoundRect(const QRectF&) 默认 roundness 一致 */
    constexpr qreal kRx = 24;
    constexpr qreal kRy = 24;
    painter->drawRoundedRect(rect, kRx, kRy, Qt::AbsoluteSize);
}

void LzRoundRect::updateCoordinate()
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

bool LzRoundRect::saveXml(QDomElement *g)
{
    Q_D(const LzRoundRect);
    g->setAttribute("shape", LZ_ROUNDRECT);
    g->setAttribute("rx", d->roundness.x());
    g->setAttribute("ry", d->roundness.y());
    LzItem::saveXml(g);
    return true;
}

bool LzRoundRect::loadXml(QDomElement *g)
{
    Q_D(LzRoundRect);
    LzItem::loadXml(g);
    updateCoordinate();
    return true;
}

void LzRoundRect::setRx(const double& data)
{
    Q_D(LzRoundRect);
    if(data<0 || data > itemWidth/2)
    {
        return ;
    }
    d->roundness.setX(data);
}

const double LzRoundRect::getRx()
{
    Q_D(const LzRoundRect);
    return d->roundness.x();
}

void LzRoundRect::setRy(const double& data)
{
    Q_D(LzRoundRect);
    if(data<0 || data > itemHeight/2)
    {
        return ;
    }
    d->roundness.setY(data);
}

const double LzRoundRect::getRy()
{
    Q_D(const LzRoundRect);
    return d->roundness.y();
}

QString LzRoundRect::className() const
{
    return "LzRoundRect";
}

QString LzRoundRect::shapeName() const
{
    return "roundrect";
}

REGISTER_OBJECT_CREATOR(LzRoundRect, roundrect)
