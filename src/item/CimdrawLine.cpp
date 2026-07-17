#include "Item/CimdrawLine.h"
#include "CimdrawObjectFactory.h"
#include "CimdrawAttributeManager.h"
#include "CimdrawToolboxIconPaint.h"

class CimdrawLinePrivate
{
    Q_DECLARE_PUBLIC(CimdrawLine)
public:
    CimdrawLinePrivate(CimdrawLine* line)
        :q_ptr(line)
    {

    }
    QPolygonF points;
    QPolygonF initialPoints;
    CimdrawLine* q_ptr;
};

CimdrawLine::CimdrawLine(QGraphicsItem* parent)
    :d_ptr(new CimdrawLinePrivate(this))
{
    handleStruct.handles.reserve(LEFT);
    for (auto item : handleStruct.handles)
    {
        delete item;
    }
    handleStruct.handles.clear();
}

CimdrawLine::CimdrawLine(const QRectF& pos,QGraphicsItem* parent)
    :d_ptr(new CimdrawLinePrivate(this))
{
    handleStruct.handles.reserve(LEFT);
    for (auto item : handleStruct.handles)
    {
        delete item;
    }
    handleStruct.handles.clear();
    setUsePen(CimdrawAttributeManager::usePen_);
    setItemPenColor(CimdrawAttributeManager::penColor_);
    setItemPenWidth(CimdrawAttributeManager::penWidth_);
    setItemPenStyle(CimdrawAttributeManager::penStyle_);

    setUseBrush(CimdrawAttributeManager::useBrush_);
    setItemBrushColor(CimdrawAttributeManager::brushColor_);
    setItemBrushStyle(CimdrawAttributeManager::brushStyle_);

    setUseFont(CimdrawAttributeManager::useFont_);
}

QPainterPath CimdrawLine::shape() const
{
    QPainterPath path;
    if (d_ptr->points.size() > 1)
    {
        path.moveTo(d_ptr->points[0]);
        path.lineTo(d_ptr->points[1]);
    }
    QPen pen;
    pen.setColor(itemPenColor);
    pen.setWidth(itemPenWidth);
    pen.setStyle(Qt::PenStyle(itemPenStyle));
    return shapeFromPath(path, pen);
}

QRectF CimdrawLine::boundingRect() const
{
    if (itemPenWidth == 0.0)
    {
        const qreal x1 = d_ptr->points[0].x();
        const qreal x2 = d_ptr->points[1].x();
        const qreal y1 = d_ptr->points[0].y();
        const qreal y2 = d_ptr->points[1].y();
        qreal lx = qMin(x1, x2);
        qreal rx = qMax(x1, x2);
        qreal ty = qMin(y1, y2);
        qreal by = qMax(y1, y2);
        return QRectF(lx, ty, rx - lx, by - ty);
    }
    return shape().controlPointRect();
}

void CimdrawLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(CimdrawLine);
    prepare(painter);
    if (d->points.size() > 1)
    {
        painter->drawLine(d->points.at(0), d->points.at(1));
    }
}

void CimdrawLine::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    cimdrawApplyItemDefaultPenBrush(painter);
    const QPointF a(rect.left() + rect.width() * 0.2, rect.bottom() - rect.height() * 0.25);
    const QPointF b(rect.right() - rect.width() * 0.2, rect.top() + rect.height() * 0.25);
    painter->drawLine(QLineF(a, b));
}

void CimdrawLine::updateCoordinate()
{
    Q_D(CimdrawLine);
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

bool CimdrawLine::saveXml(QDomElement *g)
{
    Q_D(CimdrawLine);
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

bool CimdrawLine::loadXml(QDomElement *g)
{
    Q_D(CimdrawLine);
    if (!CimdrawItem::loadXml(g))
        return false;
    const int n = g->attribute(QStringLiteral("ptnum")).toInt();
    d->points.clear();
    for (int i = 0; i < n; ++i) {
        const qreal x = g->attribute(QStringLiteral("ptx_%1").arg(i)).toDouble();
        const qreal y = g->attribute(QStringLiteral("pty_%1").arg(i)).toDouble();
        d->points.append(QPointF(x, y));
    }
    d->initialPoints = d->points;
    if (d->points.size() >= 2) {
        itemPosition = d->points.boundingRect();
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        updateHandles();
    }
    return true;
}

QString CimdrawLine::className() const
{
    return "CimdrawLine";
}

QString CimdrawLine::shapeName() const
{
    return "line";
}

void CimdrawLine::addPoint(const QPointF& point)
{
    Q_D(CimdrawLine);
    d->points.append(mapFromScene(point));
    int dir = d->points.count();
    CimdrawHandle *shr = new CimdrawHandle(this, dir + LEFT, dir == 1 ? false : true);
    shr->setState(HANDLE_INACTIVE);
    handleStruct.handles.push_back(shr);
}

int CimdrawLine::handleCount() const
{
    return handleStruct.handles.size() + LEFT;
}

void CimdrawLine::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    Q_D(CimdrawLine);
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

void CimdrawLine::control(int dir, const QPointF& delta)
{
    Q_D(CimdrawLine);
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

void CimdrawLine::updateHandles()
{
    Q_D(CimdrawLine);
    for (int i = 0; i < d->points.size(); ++i)
    {
        handleStruct.handles[i]->move(d->points[i].x(), d->points[i].y());
    }
}

QPointF CimdrawLine::opposite(int handle)
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

REGISTER_OBJECT_CREATOR(CimdrawLine, line)
