#include "CimdrawArc.h"
#include "CimdrawObjectFactory.h"
#include <QPainter>
#include "CimdrawToolboxIconPaint.h"

class CimdrawArcPrivate
{
    Q_DECLARE_PUBLIC(CimdrawArc)
public:
    CimdrawArcPrivate(CimdrawArc* arc)
        :q_ptr(arc)
    {

    }
    //起始角
    double startAngle;
    //偏移
    double sweepAngle;
    //
    bool sweepFlag;
    CimdrawArc* q_ptr;
};

CimdrawArc::CimdrawArc(QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawArcPrivate(this))
{
    Q_D(CimdrawArc);
    setUseFont(false);
    d->startAngle = 0.0;
    d->sweepAngle = 270.0;
    d->sweepFlag = 0;
}

CimdrawArc::CimdrawArc(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawArcPrivate(this))
{
    Q_D(CimdrawArc);
    setUseFont(false);
    d->startAngle = 0.0;
    d->sweepAngle = 270.0;
    d->sweepFlag = 0;
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
    updatePoints();
}

QPainterPath CimdrawArc::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawArc::boundingRect() const
{
    return itemPosition;
}

void CimdrawArc::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const CimdrawArc);
    prepare(painter);
    painter->drawArc(itemPosition, d_ptr->startAngle*16,d_ptr->sweepAngle*16); // 使用初始矩形进行绘制
}

void CimdrawArc::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    /** 与 CimdrawArc 构造默认 startAngle/sweepAngle 一致 */
    constexpr double kStart = 0.0;
    constexpr double kSweep = 270.0;
    painter->drawArc(rect, int(kStart * 16), int(kSweep * 16));
}

void CimdrawArc::updateCoordinate()
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

bool CimdrawArc::saveXml(QDomElement *g)
{
    Q_D(const CimdrawArc);
    g->setAttribute("shape", CIMDRAW_ARC);
    g->setAttribute("startAngle", d->startAngle);
    g->setAttribute("sweepAngle", d->sweepAngle);
    g->setAttribute("sweepFlag", d->sweepFlag);
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawArc::loadXml(QDomElement *g)
{
    Q_D(CimdrawArc);
    CimdrawItem::loadXml(g);
    setStartAngle(g->attribute("startAngle").toDouble());
    setSweepAngle(g->attribute("sweepAngle").toDouble());
    setSweepFlag(g->attribute("sweepFlag").toDouble());
    updateCoordinate();
    return true;
}

void CimdrawArc::stretch(int handle, double sx, double sy, const QPointF & origin)
{
    QTransform trans  ;
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

QGraphicsItem* CimdrawArc::duplicate()
{
    CimdrawArc* item = new CimdrawArc(rect());
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

QString CimdrawArc::className() const
{
    return "CimdrawArc";
}

QString CimdrawArc::shapeName() const
{
    return "arc";
}

const double CimdrawArc::getStartAngle()
{
    Q_D(const CimdrawArc);
    return d->startAngle;
}

void CimdrawArc::setStartAngle(const double& data)
{
    Q_D(CimdrawArc);
    d->startAngle = data;
    update();
}

const double CimdrawArc::getSweepAngle()
{
    Q_D(const CimdrawArc);
    return d->sweepAngle;
}

void CimdrawArc::setSweepAngle(const double& data)
{
    Q_D(CimdrawArc);
    d->sweepAngle = data;
    update();
}

const bool CimdrawArc::getSweepFlag()
{
    Q_D(const CimdrawArc);
    return d->sweepFlag;
}

void CimdrawArc::setSweepFlag(bool data)
{
    Q_D(CimdrawArc);
    d->sweepFlag = data;
    update();
}

REGISTER_OBJECT_CREATOR(CimdrawArc, arc)
