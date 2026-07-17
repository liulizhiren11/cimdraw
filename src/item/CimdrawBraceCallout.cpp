#include "Item/CimdrawBraceCallout.h"

#include <QPainter>
#include <QPainterPathStroker>

#include "CimdrawObjectFactory.h"
#include "CimdrawToolboxIconPaint.h"

namespace {

QPainterPath cimdrawBraceCalloutPathForRect(const QRectF& rect)
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

CimdrawBraceCallout::CimdrawBraceCallout(QGraphicsItem* parent)
    : CimdrawItem(parent)
{
    setUseFont(false);
}

CimdrawBraceCallout::CimdrawBraceCallout(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawItem(parent)
{
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0, 0, itemWidth, itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath CimdrawBraceCallout::symbolPath() const
{
    return cimdrawBraceCalloutPathForRect(itemPosition);
}

QPainterPath CimdrawBraceCallout::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(qMax<qreal>(1.0, itemPenWidth));
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    return stroker.createStroke(symbolPath());
}

QRectF CimdrawBraceCallout::boundingRect() const
{
    return shape().controlPointRect();
}

void CimdrawBraceCallout::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    painter->drawPath(symbolPath());
}

void CimdrawBraceCallout::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    painter->drawPath(cimdrawBraceCalloutPathForRect(rect));
}

void CimdrawBraceCallout::updateCoordinate()
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

void CimdrawBraceCallout::stretch(int handle, double sx, double sy, const QPointF& origin)
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

QGraphicsItem* CimdrawBraceCallout::duplicate()
{
    CimdrawBraceCallout* item = new CimdrawBraceCallout(rect());
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

bool CimdrawBraceCallout::saveXml(QDomElement* g)
{
    g->setAttribute("shape", CIMDRAW_BRACE_CALLOUT);
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawBraceCallout::loadXml(QDomElement* g)
{
    if (!CimdrawItem::loadXml(g))
        return false;
    updateCoordinate();
    return true;
}

QString CimdrawBraceCallout::className()
{
    return "CimdrawBraceCallout";
}

QString CimdrawBraceCallout::shapeName() const
{
    return "bracecallout";
}

REGISTER_OBJECT_CREATOR(CimdrawBraceCallout, bracecallout)
