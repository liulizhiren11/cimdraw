#include "LzArc.h"
#include "LzObjectFactory.h"
#include <QPainter>
#include "LzToolboxIconPaint.h"

class LzArcPrivate
{
    Q_DECLARE_PUBLIC(LzArc)
public:
    LzArcPrivate(LzArc* arc)
        :q_ptr(arc)
    {

    }
    //起始角
    double startAngle;
    //偏移
    double sweepAngle;
    //
    bool sweepFlag;
    LzArc* q_ptr;
};

LzArc::LzArc(QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzArcPrivate(this))
{
    Q_D(LzArc);
    setUseFont(false);
    d->startAngle = 0.0;
    d->sweepAngle = 270.0;
    d->sweepFlag = 0;
}

LzArc::LzArc(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzArcPrivate(this))
{
    Q_D(LzArc);
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

QPainterPath LzArc::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzArc::boundingRect() const
{
    return itemPosition;
}

void LzArc::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const LzArc);
    prepare(painter);
    painter->drawArc(itemPosition, d_ptr->startAngle*16,d_ptr->sweepAngle*16); // 使用初始矩形进行绘制
}

void LzArc::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    /** 与 LzArc 构造默认 startAngle/sweepAngle 一致 */
    constexpr double kStart = 0.0;
    constexpr double kSweep = 270.0;
    painter->drawArc(rect, int(kStart * 16), int(kSweep * 16));
}

void LzArc::updateCoordinate()
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

bool LzArc::saveXml(QDomElement *g)
{
    Q_D(const LzArc);
    g->setAttribute("shape", LZ_ARC);
    g->setAttribute("startAngle", d->startAngle);
    g->setAttribute("sweepAngle", d->sweepAngle);
    g->setAttribute("sweepFlag", d->sweepFlag);
    LzItem::saveXml(g);
    return true;
}

bool LzArc::loadXml(QDomElement *g)
{
    Q_D(LzArc);
    LzItem::loadXml(g);
    setStartAngle(g->attribute("startAngle").toDouble());
    setSweepAngle(g->attribute("sweepAngle").toDouble());
    setSweepFlag(g->attribute("sweepFlag").toDouble());
    updateCoordinate();
    return true;
}

void LzArc::stretch(int handle, double sx, double sy, const QPointF & origin)
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

QGraphicsItem* LzArc::duplicate()
{
    LzArc* item = new LzArc(rect());
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

QString LzArc::className() const
{
    return "LzArc";
}

QString LzArc::shapeName() const
{
    return "arc";
}

const double LzArc::getStartAngle()
{
    Q_D(const LzArc);
    return d->startAngle;
}

void LzArc::setStartAngle(const double& data)
{
    Q_D(LzArc);
    d->startAngle = data;
    update();
}

const double LzArc::getSweepAngle()
{
    Q_D(const LzArc);
    return d->sweepAngle;
}

void LzArc::setSweepAngle(const double& data)
{
    Q_D(LzArc);
    d->sweepAngle = data;
    update();
}

const bool LzArc::getSweepFlag()
{
    Q_D(const LzArc);
    return d->sweepFlag;
}

void LzArc::setSweepFlag(bool data)
{
    Q_D(LzArc);
    d->sweepFlag = data;
    update();
}

REGISTER_OBJECT_CREATOR(LzArc, arc)
