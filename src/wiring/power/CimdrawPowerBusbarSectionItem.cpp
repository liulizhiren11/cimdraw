#include "CimdrawPowerBusbarSectionItem.h"
#include "Item/CimdrawConnectPoint.h"
#include "Item/CimdrawConnectLine.h"
#include "Item/CimdrawObjectFactory.h"
#include "Item/TmpBase.h"
#include "wiring/base/CimdrawWiringDrawPalette.h"

#include <QDateTime>
#include <QPainterPath>
#include <QtMath>

namespace {

constexpr qreal kBusbarPickExpandY = 28.0;
constexpr qreal kReuseAlongBarScenePx = 28.0;
constexpr qreal kBusbarSelectionExpandY = 10.0;
constexpr qreal kBusbarMinWidth = 24.0;

} // namespace

void CimdrawPowerBusbarSectionItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(CimdrawWiringDrawPalette::busbarFill(runState, alarmPulse));
    const qreal m = qMax(1.0, rect.height() * 0.12);
    painter->drawRoundedRect(rect.adjusted(1, m, -1, -m), 2.0, 2.0);
}

CimdrawPowerBusbarSectionItem::CimdrawPowerBusbarSectionItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::BusbarSection, parent)
{
}

CimdrawPowerBusbarSectionItem::CimdrawPowerBusbarSectionItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::BusbarSection, pos, parent)
{
}

QString CimdrawPowerBusbarSectionItem::className()
{
    return QStringLiteral("CimdrawPowerBusbarSectionItem");
}

QString CimdrawPowerBusbarSectionItem::shapeName() const
{
    return QStringLiteral("busbarsection");
}

CimdrawDrawTypeId CimdrawPowerBusbarSectionItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_BUSBAR_SECTION;
}

CimdrawWiringItemBase* CimdrawPowerBusbarSectionItem::cloneForDuplicate() const
{
    return new CimdrawPowerBusbarSectionItem(rect());
}

void CimdrawPowerBusbarSectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

void CimdrawPowerBusbarSectionItem::copyAlongEdgeConnectPointsFrom(const CimdrawPowerBusbarSectionItem* source)
{
    if (!source || source == this)
        return;

    for (CimdrawConnectPoint* p : pointStruct.points)
        delete p;
    pointStruct.points.clear();
    pointStruct.count = 0;

    for (CimdrawConnectPoint* sp : source->pointStruct.points)
    {
        if (!sp)
            continue;
        auto* p = new CimdrawConnectPoint(this);
        p->setDir(static_cast<int>(pointStruct.points.size()));
        p->setDirection(sp->getDirection());
        p->setConnectionLeadOut(sp->connectionLeadOut());
        p->bindToItem(this, sp->getRelativePos());
        pointStruct.points.push_back(p);
    }
    pointStruct.count = static_cast<ConnectCount>(pointStruct.points.size());
    rebindAlongEdgeConnectPorts();
    updatePoints();
}

QGraphicsItem* CimdrawPowerBusbarSectionItem::duplicate()
{
    auto* dup = qgraphicsitem_cast<CimdrawPowerBusbarSectionItem*>(CimdrawWiringItemBase::duplicate());
    if (!dup)
        return nullptr;
    copyAlongEdgeConnectPointsFrom(this);
    return dup;
}

QRectF CimdrawPowerBusbarSectionItem::busbarBodyRect() const
{
    const QRectF r = itemPosition;
    const qreal m = qMax(1.0, r.height() * 0.12);
    return r.adjusted(1, m, -1, -m);
}

bool CimdrawPowerBusbarSectionItem::hitBusbarPickRegion(const QPointF& scenePos) const
{
    const QPointF local = mapFromScene(scenePos);
    return busbarBodyRect().adjusted(0, -kBusbarPickExpandY, 0, kBusbarPickExpandY).contains(local);
}

bool CimdrawPowerBusbarSectionItem::snapSceneToEdgePort(const QPointF& scenePos, qreal maxDist,
    QPointF& anchorScene) const
{
    if (maxDist <= 0.0)
        return false;

    const QPointF local = mapFromScene(scenePos);
    const QRectF body = busbarBodyRect();
    const qreal x = qBound(body.left(), local.x(), body.right());
    const QPointF topLocal(x, body.top());
    const QPointF bottomLocal(x, body.bottom());
    const qreal dTop = QLineF(local, topLocal).length();
    const qreal dBottom = QLineF(local, bottomLocal).length();
    const qreal dEdge = qMin(dTop, dBottom);

    qreal dHoriz = 0.0;
    if (local.x() < body.left())
        dHoriz = body.left() - local.x();
    else if (local.x() > body.right())
        dHoriz = local.x() - body.right();

    if (dEdge > maxDist && dHoriz > maxDist)
        return false;

    anchorScene = mapToScene(dTop <= dBottom ? topLocal : bottomLocal);
    return true;
}

CimdrawConnectPoint* CimdrawPowerBusbarSectionItem::ensureConnectPointNearScene(const QPointF& scenePos,
    qreal maxSnapDist)
{
    QPointF anchor;
    if (!snapSceneToEdgePort(scenePos, maxSnapDist, anchor))
        return nullptr;
    return ensureConnectPointAtScene(anchor);
}

bool CimdrawPowerBusbarSectionItem::projectSceneToConnectAnchor(const QPointF& scenePos, QPointF& norm,
    CONNECT_DIRECTION& dir) const
{
    if (!hitBusbarPickRegion(scenePos))
        return false;

    const QPointF local = mapFromScene(scenePos);
    const QRectF body = busbarBodyRect();
    const qreal x = qBound(body.left(), local.x(), body.right());
    const bool topSide = local.y() < body.center().y();
    const qreal y = topSide ? body.top() : body.bottom();
    dir = topSide ? TOP_DIRECTION : BOTTOM_DIRECTION;

    const QPointF c = itemPosition.center();
    const qreal w = qMax(1.0, itemWidth);
    const qreal h = qMax(1.0, itemHeight);
    norm = QPointF((x - c.x()) / w, (y - c.y()) / h);
    return true;
}

CimdrawConnectPoint* CimdrawPowerBusbarSectionItem::ensureConnectPointAtScene(const QPointF& scenePos)
{
    QPointF norm;
    CONNECT_DIRECTION dir = NONE_DIRECTION;
    if (!projectSceneToConnectAnchor(scenePos, norm, dir))
        return nullptr;

    const QPointF anchorScene = mapToScene(
        itemPosition.center() + QPointF(norm.x() * itemWidth, norm.y() * itemHeight));

    for (CimdrawConnectPoint* p : pointStruct.points)
    {
        if (!p || p->getDirection() != dir)
            continue;
        const QPointF c = p->connectionCenterInScene();
        if (qAbs(c.x() - anchorScene.x()) <= kReuseAlongBarScenePx
            && qAbs(c.y() - anchorScene.y()) <= kBusbarPickExpandY)
        {
            return p;
        }
    }

    auto* point = new CimdrawConnectPoint(this);
    point->setDir(static_cast<int>(pointStruct.points.size()));
    point->setDirection(dir);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(this, norm);
    pointStruct.points.push_back(point);
    pointStruct.count = static_cast<ConnectCount>(pointStruct.points.size());
    updatePoints();
    point->update();
    return point;
}

bool CimdrawPowerBusbarSectionItem::isConnectPointUsed(const CimdrawConnectPoint* port) const
{
    if (!port)
        return false;
    for (QGraphicsItem* item : connectStruct.connects)
    {
        const auto* line = qgraphicsitem_cast<const CimdrawConnectLine*>(item);
        if (!line)
            continue;
        if (line->startConnectPort() == port || line->endConnectPort() == port)
            return true;
    }
    return false;
}

CimdrawConnectPoint* CimdrawPowerBusbarSectionItem::findConnectPortNearScene(const QPointF& scenePos,
                                                            qreal maxDistPx) const
{
    CimdrawConnectPoint* best = nullptr;
    qreal bestDist = maxDistPx;
    for (CimdrawConnectPoint* p : pointStruct.points)
    {
        if (!p)
            continue;
        const qreal d = QLineF(p->connectionCenterInScene(), scenePos).length();
        if (d < bestDist)
        {
            bestDist = d;
            best = p;
        }
    }
    return best;
}

void CimdrawPowerBusbarSectionItem::removeConnectPointIfUnused(CimdrawConnectPoint* port)
{
    if (!port || isConnectPointUsed(port))
        return;
    if (!pointStruct.points.removeOne(port))
        return;
    pointStruct.count = static_cast<ConnectCount>(pointStruct.points.size());
    delete port;
    updatePoints();
    update();
}

void cimdrawReattachBusbarPortsForConnectLine(CimdrawConnectLine* line, bool preservePathShape)
{
    if (!line)
        return;

    const QVector<QPointF> path = line->pathInSceneCoords();
    if (path.isEmpty())
        return;

    const auto reattachEnd = [line, &path](bool isStart) {
        QGraphicsItem* const item = isStart ? line->getStartItem() : line->getEndItem();
        auto* bus = dynamic_cast<CimdrawPowerBusbarSectionItem*>(item);
        if (!bus)
            return;

        const QPointF scenePt = isStart ? path.first() : path.last();
        CimdrawConnectPoint* port = bus->ensureConnectPointAtScene(scenePt);
        if (!port)
            return;
        if (isStart)
            line->setStartConnectPort(port);
        else
            line->setEndConnectPort(port);
        bus->addConnect(line);
    };

    reattachEnd(true);
    reattachEnd(false);
    if (preservePathShape)
        line->syncEndpointPositionsFromAttachedItems();
    else
        line->refreshEndpointsFromAttachedItems();
}

QPainterPath CimdrawPowerBusbarSectionItem::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition.adjusted(0, -kBusbarSelectionExpandY, 0, kBusbarSelectionExpandY));
    return path;
}

void CimdrawPowerBusbarSectionItem::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    CimdrawWiringItemBase::stretch(handle, sx, sy, origin);
    rebindAlongEdgeConnectPorts();
    updatePoints();
    refreshConnectedLines();
    if (itemWidth < kBusbarMinWidth)
    {
        const qreal cx = itemPosition.center().x();
        const qreal cy = itemPosition.center().y();
        const qreal half = kBusbarMinWidth * 0.5;
        itemPosition = QRectF(cx - half, itemPosition.top(), kBusbarMinWidth, itemPosition.height());
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        itemInitialRect = itemPosition;
        updateHandles();
        rebindAlongEdgeConnectPorts();
        update();
    }
}

void CimdrawPowerBusbarSectionItem::rebindAlongEdgeConnectPorts()
{
    const QRectF body = busbarBodyRect();
    const QPointF c = itemPosition.center();
    const qreal w = qMax(1.0, itemWidth);
    const qreal h = qMax(1.0, itemHeight);

    for (CimdrawConnectPoint* p : pointStruct.points)
    {
        if (!p)
            continue;
        QPointF norm = p->getRelativePos();
        qreal x = c.x() + norm.x() * w;
        CONNECT_DIRECTION dir = p->getDirection();
        if (dir == LEFT_DIRECTION)
        {
            x = body.left();
            dir = TOP_DIRECTION;
        }
        else if (dir == RIGHT_DIRECTION)
        {
            x = body.right();
            dir = TOP_DIRECTION;
        }
        x = qBound(body.left(), x, body.right());
        const qreal y = (dir == TOP_DIRECTION) ? body.top() : body.bottom();
        norm = QPointF((x - c.x()) / w, (y - c.y()) / h);
        p->setDirection(dir);
        p->setConnectionLeadOut(0.0);
        p->bindToItem(this, norm);
    }
}

REGISTER_OBJECT_CREATOR(CimdrawPowerBusbarSectionItem, busbarsection)
