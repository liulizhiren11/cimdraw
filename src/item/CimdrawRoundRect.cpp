#include "CimdrawRoundRect.h"
#include "CimdrawObjectFactory.h"
#include <QPainter>
#include "CimdrawToolboxIconPaint.h"

class CimdrawRoundRectPrivate
{
    Q_DECLARE_PUBLIC(CimdrawRoundRect)
public:
    CimdrawRoundRectPrivate(CimdrawRoundRect* roundrect)
        :q_ptr(roundrect)
    {

    }

    QPointF roundness;
    CimdrawRoundRect* q_ptr;
};

CimdrawRoundRect::CimdrawRoundRect(QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawRoundRectPrivate(this))
{
    Q_D(CimdrawRoundRect);
    setUseFont(false);
    d->roundness.setX(24);
    d->roundness.setY(24);
}

CimdrawRoundRect::CimdrawRoundRect(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawRoundRectPrivate(this))
{
    Q_D(CimdrawRoundRect);
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    d->roundness.setX(24);
    d->roundness.setY(24);
    updateCoordinate();
}

QPainterPath CimdrawRoundRect::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawRoundRect::boundingRect() const
{
    return itemPosition;
}

void CimdrawRoundRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const CimdrawRoundRect);
    prepare(painter);
    
    // Qt 6 中 drawRoundRect 使用 QRectF 和 qreal 参数
    painter->drawRoundedRect(itemPosition, 
                           d->roundness.x(), 
                           d->roundness.y(), 
                           Qt::AbsoluteSize);
}

void CimdrawRoundRect::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    /** 与 CimdrawRoundRect(const QRectF&) 默认 roundness 一致 */
    constexpr qreal kRx = 24;
    constexpr qreal kRy = 24;
    painter->drawRoundedRect(rect, kRx, kRy, Qt::AbsoluteSize);
}

void CimdrawRoundRect::updateCoordinate()
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

bool CimdrawRoundRect::saveXml(QDomElement *g)
{
    Q_D(const CimdrawRoundRect);
    g->setAttribute("shape", CIMDRAW_ROUNDRECT);
    g->setAttribute("rx", d->roundness.x());
    g->setAttribute("ry", d->roundness.y());
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawRoundRect::loadXml(QDomElement *g)
{
    Q_D(CimdrawRoundRect);
    CimdrawItem::loadXml(g);
    updateCoordinate();
    return true;
}

void CimdrawRoundRect::setRx(const double& data)
{
    Q_D(CimdrawRoundRect);
    if(data<0 || data > itemWidth/2)
    {
        return ;
    }
    d->roundness.setX(data);
}

const double CimdrawRoundRect::getRx()
{
    Q_D(const CimdrawRoundRect);
    return d->roundness.x();
}

void CimdrawRoundRect::setRy(const double& data)
{
    Q_D(CimdrawRoundRect);
    if(data<0 || data > itemHeight/2)
    {
        return ;
    }
    d->roundness.setY(data);
}

const double CimdrawRoundRect::getRy()
{
    Q_D(const CimdrawRoundRect);
    return d->roundness.y();
}

QString CimdrawRoundRect::className() const
{
    return "CimdrawRoundRect";
}

QString CimdrawRoundRect::shapeName() const
{
    return "roundrect";
}

REGISTER_OBJECT_CREATOR(CimdrawRoundRect, roundrect)
