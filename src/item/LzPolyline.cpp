#include "Item/LzPolyline.h"
#include "LzObjectFactory.h"
#include "LzAttributeManager.h"
#include "LzToolboxIconPaint.h"

class LzPolylinePrivate
{
    Q_DECLARE_PUBLIC(LzPolyline)
public:
    LzPolylinePrivate(LzPolyline* polyline)
        :q_ptr(polyline)
    {

    }
    QPolygonF points;
    QPolygonF initialPoints;
    LzPolyline* q_ptr;
};

LzPolyline::LzPolyline(QGraphicsItem* parent)
    :d_ptr(new LzPolylinePrivate(this))
{

}

LzPolyline::LzPolyline(const QRectF& pos,QGraphicsItem* parent)
    :d_ptr(new LzPolylinePrivate(this))
{

}

QPainterPath LzPolyline::shape() const
{
    QPainterPath path;
    if (!d_ptr->points.isEmpty())
        path.addPolygon(d_ptr->points);
    QPen pen;
    pen.setColor(itemPenColor);
    pen.setWidth(itemPenWidth);
    pen.setStyle(Qt::PenStyle(itemPenStyle));
    return shapeFromPath(path, pen);
}

QRectF LzPolyline::boundingRect() const
{
    return shape().controlPointRect();
}

void LzPolyline::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(LzPolyline);
    prepare(painter);
    painter->drawPolyline(d->points);
}

void LzPolyline::paintToolboxIcon(QPainter* painter, const QRectF& r)
{
    if (!painter || r.width() < 2 || r.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    QPolygonF pts;
    pts << QPointF(r.left() + r.width() * 0.22, r.top() + r.height() * 0.38)
        << QPointF(r.center().x(), r.top() + r.height() * 0.18)
        << QPointF(r.right() - r.width() * 0.22, r.top() + r.height() * 0.52)
        << QPointF(r.center().x(), r.bottom() - r.height() * 0.22);
    painter->drawPolyline(pts);
}

void LzPolyline::updateCoordinate()
{
    Q_D(LzPolyline);
    QPointF pt1, pt2, delta;
    QPolygonF pts = mapToScene(d->points);
    if (!parentItem())
    {
        pt1 = mapToScene(transformOriginPoint());
        pt2 = mapToScene(boundingRect().center());
        delta = pt1 - pt2;

        for (int i = 0; i < pts.count(); ++i)
        {
            pts[i] += delta;
        }

        prepareGeometryChange();
        d->points = mapFromScene(pts);
        itemPosition = d->points.boundingRect();
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();

        setTransform(transform().translate(delta.x(), delta.y()));
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
    }
    d->initialPoints = d->points;
}

bool LzPolyline::saveXml(QDomElement *g)
{
    Q_D(LzPolyline);
    g->setAttribute("shape", LZ_LINE);
    g->setAttribute("ptnum", d->points.count());
    for (int i = 0; i < d->points.size(); ++i)
    {
        QPointF pt = d->points.at(i);
        QString szpt = QString("ptx_%1").arg(i);
        g->setAttribute(szpt, pt.x());
        szpt = QString("pty_%1").arg(i);
        g->setAttribute(szpt, pt.y());
    }
    LzItem::saveXml(g);
    return true;
}

bool LzPolyline::loadXml(QDomElement *g)
{
    if (!LzItem::loadXml(g))
        return false;
    return true;
}

QString LzPolyline::className() const
{
    return "LzPolyline";
}

QString LzPolyline::shapeName() const
{
    return "polyline";
}

void LzPolyline::addPoint(const QPointF& point)
{
    Q_D(LzPolyline);
    d->points.append(mapFromScene(point));
    int dir = d->points.count();
    LzHandle *shr = new LzHandle(this, dir + LEFT, true);
    shr->setState(HANDLE_INACTIVE);
    handleStruct.handles.push_back(shr);
}

void LzPolyline::endPoint(const QPointF & point)
{
    Q_D(LzPolyline);
    int nPoints = d->points.count();
    if (nPoints > 2 && (d->points[nPoints - 1] == d->points[nPoints - 2] ||
        d->points[nPoints - 1].x() - 1 == d->points[nPoints - 2].x() &&
        d->points[nPoints - 1].y() == d->points[nPoints - 2].y()))
    {
        delete handleStruct.handles[LEFT + nPoints - 1];
        d->points.remove(nPoints - 1);
        handleStruct.handles.resize(LEFT + nPoints - 1);
    }
    d->initialPoints = d->points;
}

void LzPolyline::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    Q_D(LzPolyline);
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
    d->points = trans.map(d->initialPoints);
    itemPosition= d->points.boundingRect();
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    updateHandles();
}

void LzPolyline::control(int dir, const QPointF& delta)
{
    Q_D(LzPolyline);
    QPointF pt = mapFromScene(delta);
    if (dir <= LEFT)
    {
        return;
    }
    d->points[dir - LEFT - 1] = pt;
    prepareGeometryChange();
    itemPosition = d->points.boundingRect();
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    d->initialPoints = d->points;
    updateHandles();
}

void LzPolyline::updateHandles()
{
    Q_D(LzPolyline);
    LzItem::updateHandles();
    for (int i = 0; i < d->points.size(); ++i)
    {
        handleStruct.handles[LEFT+i]->move(d->points[i].x(), d->points[i].y());
    }
}

QPointF LzPolyline::opposite(int handle)
{
    QPointF pt;
    switch (handle) {
    case RIGHT:
    case LEFT:
    case TOP:
    case LEFT_TOP:
    case RIGHT_TOP:
        pt = handleStruct.handles[1]->pos();
        break;
    case RIGHT_BOTTOM:
    case LEFT_BOTTOM:
    case BOTTOM:
        pt = handleStruct.handles[0]->pos();
        break;
    }
    return pt;
}

REGISTER_OBJECT_CREATOR(LzPolyline, polyline)
