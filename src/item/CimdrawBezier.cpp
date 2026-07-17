#include "Item/CimdrawBezier.h"
#include "CimdrawObjectFactory.h"
#include "CimdrawAttributeManager.h"
#include "CimdrawToolboxIconPaint.h"
#include <QPainterPath>

class CimdrawBezierPrivate
{
    Q_DECLARE_PUBLIC(CimdrawBezier)
public:
    CimdrawBezierPrivate(CimdrawBezier* bezier)
        :q_ptr(bezier)
    {

    }
    QPolygonF points;
    QPolygonF initialPoints;
    CimdrawBezier* q_ptr;
};

CimdrawBezier::CimdrawBezier(QGraphicsItem* parent)
    :d_ptr(new CimdrawBezierPrivate(this))
{

}

CimdrawBezier::CimdrawBezier(const QRectF& pos,QGraphicsItem* parent)
    :d_ptr(new CimdrawBezierPrivate(this))
{

}

QPainterPath CimdrawBezier::shape() const
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

QRectF CimdrawBezier::boundingRect() const
{
    return shape().controlPointRect();
}

void CimdrawBezier::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(CimdrawBezier);
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

void CimdrawBezier::paintToolboxIcon(QPainter* painter, const QRectF& r)
{
    if (!painter || r.width() < 2 || r.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    QPainterPath path;
    const QPointF p0(r.left() + r.width() * 0.12, r.center().y());
    const QPointF c1(r.left() + r.width() * 0.35, r.top() + r.height() * 0.12);
    const QPointF c2(r.right() - r.width() * 0.35, r.bottom() - r.height() * 0.12);
    const QPointF p3(r.right() - r.width() * 0.12, r.center().y());
    path.moveTo(p0);
    path.cubicTo(c1, c2, p3);
    painter->drawPath(path);
}

void CimdrawBezier::updateCoordinate()
{
    Q_D(CimdrawBezier);
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

bool CimdrawBezier::saveXml(QDomElement *g)
{
    Q_D(CimdrawBezier);
    g->setAttribute("shape", CIMDRAW_LINE);
    g->setAttribute("ptnum", d->points.count());
    for (int i = 0; i < d->points.size(); ++i)
    {
        QPointF pt = d->points.at(i);
        QString szpt = QString("ptx_%1").arg(i);
        g->setAttribute(szpt, pt.x());
        szpt = QString("pty_%1").arg(i);
        g->setAttribute(szpt, pt.y());
    }
    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawBezier::loadXml(QDomElement *g)
{
    if (!CimdrawItem::loadXml(g))
        return false;
    return true;
}

QString CimdrawBezier::className() const
{
    return "CimdrawBezier";
}

QString CimdrawBezier::shapeName() const
{
    return "bezier";
}

void CimdrawBezier::addPoint(const QPointF& point)
{
    Q_D(CimdrawBezier);
    d->points.append(mapFromScene(point));
    int dir = d->points.count();
    CimdrawHandle *shr = new CimdrawHandle(this, dir + LEFT, true);
    shr->setState(HANDLE_INACTIVE);
    handleStruct.handles.push_back(shr);
}

void CimdrawBezier::endPoint(const QPointF & point)
{
    Q_D(CimdrawBezier);
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

void CimdrawBezier::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    Q_D(CimdrawBezier);
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

void CimdrawBezier::control(int dir, const QPointF& delta)
{
    Q_D(CimdrawBezier);
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

void CimdrawBezier::updateHandles()
{
    Q_D(CimdrawBezier);
    CimdrawItem::updateHandles();
    for (int i = 0; i < d->points.size(); ++i)
    {
        handleStruct.handles[LEFT+i]->move(d->points[i].x(), d->points[i].y());
    }
}

QPointF CimdrawBezier::opposite(int handle)
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

REGISTER_OBJECT_CREATOR(CimdrawBezier, bezier)
