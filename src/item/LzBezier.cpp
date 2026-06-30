#include "Item/LzBezier.h"
#include "LzObjectFactory.h"
#include "LzAttributeManager.h"
#include "LzToolboxIconPaint.h"
#include <QPainterPath>

class LzBezierPrivate
{
    Q_DECLARE_PUBLIC(LzBezier)
public:
    LzBezierPrivate(LzBezier* bezier)
        :q_ptr(bezier)
    {

    }
    QPolygonF points;
    QPolygonF initialPoints;
    LzBezier* q_ptr;
};

LzBezier::LzBezier(QGraphicsItem* parent)
    :d_ptr(new LzBezierPrivate(this))
{

}

LzBezier::LzBezier(const QRectF& pos,QGraphicsItem* parent)
    :d_ptr(new LzBezierPrivate(this))
{

}

QPainterPath LzBezier::shape() const
{
    QPainterPath path;
    path.moveTo(d_ptr->points.at(0));
    int i=1;

    while (i + 2 < d_ptr->points.size())
    {
        path.cubicTo(d_ptr->points.at(i), d_ptr->points.at(i+1), d_ptr->points.at(i+2));
        i += 3;
    }
    while (i < d_ptr->points.size())
    {
        path.lineTo(d_ptr->points.at(i));
        ++i;
    }
    QPen pen;
    pen.setColor(itemPenColor);
    pen.setWidth(itemPenWidth);
    pen.setStyle(Qt::PenStyle(itemPenStyle));
    return shapeFromPath(path, pen);
}

QRectF LzBezier::boundingRect() const
{
    return shape().controlPointRect();
}

void LzBezier::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(LzBezier);
    prepare(painter);
    QPainterPath path;
    path.moveTo(d->points.at(0));
    int i=1;
    while (i + 2 < d->points.size())
    {
        path.cubicTo(d->points.at(i), d->points.at(i+1), d->points.at(i+2));
        i += 3;
    }
    while (i < d->points.size())
    {
        path.lineTo(d->points.at(i));
        ++i;
    }
    painter->drawPath(path);
}

void LzBezier::paintToolboxIcon(QPainter* painter, const QRectF& r)
{
    if (!painter || r.width() < 2 || r.height() < 2)
        return;
    lzApplyItemDefaultPenBrush(painter);
    QPainterPath path;
    const QPointF p0(r.left() + r.width() * 0.12, r.center().y());
    const QPointF c1(r.left() + r.width() * 0.35, r.top() + r.height() * 0.12);
    const QPointF c2(r.right() - r.width() * 0.35, r.bottom() - r.height() * 0.12);
    const QPointF p3(r.right() - r.width() * 0.12, r.center().y());
    path.moveTo(p0);
    path.cubicTo(c1, c2, p3);
    painter->drawPath(path);
}

void LzBezier::updateCoordinate()
{
    Q_D(LzBezier);
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

bool LzBezier::saveXml(QDomElement *g)
{
    Q_D(LzBezier);
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

bool LzBezier::loadXml(QDomElement *g)
{
    if (!LzItem::loadXml(g))
        return false;
    return true;
}

QString LzBezier::className() const
{
    return "LzBezier";
}

QString LzBezier::shapeName() const
{
    return "bezier";
}

void LzBezier::addPoint(const QPointF& point)
{
    Q_D(LzBezier);
    d->points.append(mapFromScene(point));
    int dir = d->points.count();
    LzHandle *shr = new LzHandle(this, dir + LEFT, true);
    shr->setState(HANDLE_INACTIVE);
    handleStruct.handles.push_back(shr);
}

void LzBezier::endPoint(const QPointF & point)
{
    Q_D(LzBezier);
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

void LzBezier::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    Q_D(LzBezier);
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

void LzBezier::control(int dir, const QPointF& delta)
{
    Q_D(LzBezier);
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

void LzBezier::updateHandles()
{
    Q_D(LzBezier);
    LzItem::updateHandles();
    for (int i = 0; i < d->points.size(); ++i)
    {
        handleStruct.handles[LEFT+i]->move(d->points[i].x(), d->points[i].y());
    }
}

QPointF LzBezier::opposite(int handle)
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

REGISTER_OBJECT_CREATOR(LzBezier, bezier)
