#include "Item/LzBraceCallout.h"

#include <QPainter>
#include <QPainterPathStroker>

#include "LzObjectFactory.h"
#include "LzToolboxIconPaint.h"

namespace {

QPainterPath lzBraceCalloutPathForRect(const QRectF& rect)
{
    QPainterPath path;
    if (rect.width() <= 0.0 || rect.height() <= 0.0)
        return path;

    const qreal w = rect.width();
    const qreal h = rect.height();
    const QRectF arcRect(rect.left() + w * 0.10, rect.top() + h * 0.08,
                         w * 0.72, h * 0.84);
    path.arcMoveTo(arcRect, 0.0);
    path.arcTo(arcRect, 0.0, 270.0);
    return path;
}

}

LzBraceCallout::LzBraceCallout(QGraphicsItem* parent)
    : LzItem(parent)
{
    setUseFont(false);
}

LzBraceCallout::LzBraceCallout(const QRectF& pos, QGraphicsItem* parent)
    : LzItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0, 0, itemWidth, itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath LzBraceCallout::symbolPath() const
{
    return lzBraceCalloutPathForRect(itemPosition);
}

QPainterPath LzBraceCallout::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(qMax<qreal>(1.0, itemPenWidth));
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    return stroker.createStroke(symbolPath());
}

QRectF LzBraceCallout::boundingRect() const
{
    return shape().controlPointRect();
}

void LzBraceCallout::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    painter->drawPath(symbolPath());
}

void LzBraceCallout::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    painter->drawPath(lzBraceCalloutPathForRect(rect));
}

void LzBraceCallout::updateCoordinate()
{
    const QPointF pt1 = mapToScene(transformOriginPoint());
    const QPointF pt2 = mapToScene(itemPosition.center());
    const QPointF delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2.0, -itemHeight / 2.0, itemWidth, itemHeight);
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

void LzBraceCallout::stretch(int handle, double sx, double sy, const QPointF& origin)
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

QGraphicsItem* LzBraceCallout::duplicate()
{
    LzBraceCallout* item = new LzBraceCallout(rect());
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

bool LzBraceCallout::saveXml(QDomElement* g)
{
    g->setAttribute("shape", LZ_BRACE_CALLOUT);
    LzItem::saveXml(g);
    return true;
}

bool LzBraceCallout::loadXml(QDomElement* g)
{
    if (!LzItem::loadXml(g))
        return false;
    updateCoordinate();
    return true;
}

QString LzBraceCallout::className()
{
    return "LzBraceCallout";
}

QString LzBraceCallout::shapeName() const
{
    return "bracecallout";
}

REGISTER_OBJECT_CREATOR(LzBraceCallout, bracecallout)
