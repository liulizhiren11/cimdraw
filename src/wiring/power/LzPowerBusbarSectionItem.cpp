#include "LzPowerBusbarSectionItem.h"
#include "Item/LzConnectPoint.h"
#include "Item/LzConnectLine.h"
#include "Item/LzObjectFactory.h"
#include "Item/TmpBase.h"
#include "wiring/base/LzWiringDrawPalette.h"

#include <QDateTime>
#include <QPainterPath>
#include <QtMath>

namespace {

constexpr qreal kBusbarPickExpandY = 28.0;
constexpr qreal kReuseAlongBarScenePx = 28.0;
constexpr qreal kBusbarSelectionExpandY = 10.0;
constexpr qreal kBusbarMinWidth = 24.0;

} // namespace

void LzPowerBusbarSectionItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(LzWiringDrawPalette::busbarFill(runState, alarmPulse));
    const qreal m = qMax(1.0, rect.height() * 0.12);
    painter->drawRoundedRect(rect.adjusted(1, m, -1, -m), 2.0, 2.0);
}

LzPowerBusbarSectionItem::LzPowerBusbarSectionItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::BusbarSection, parent)
{
}

LzPowerBusbarSectionItem::LzPowerBusbarSectionItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::BusbarSection, pos, parent)
{
}

QString LzPowerBusbarSectionItem::className()
{
    return QStringLiteral("LzPowerBusbarSectionItem");
}

QString LzPowerBusbarSectionItem::shapeName() const
{
    return QStringLiteral("busbarsection");
}

LzDrawTypeId LzPowerBusbarSectionItem::drawTypeForXml() const
{
    return LZ_SLD_BUSBAR_SECTION;
}

LzWiringItemBase* LzPowerBusbarSectionItem::cloneForDuplicate() const
{
    return new LzPowerBusbarSectionItem(rect());
}

void LzPowerBusbarSectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

void LzPowerBusbarSectionItem::copyAlongEdgeConnectPointsFrom(const LzPowerBusbarSectionItem* source)
{
    if (!source || source == this)
        return;

    for (LzConnectPoint* p : pointStruct.points)
        delete p;
    pointStruct.points.clear();
    pointStruct.count = 0;

    for (LzConnectPoint* sp : source->pointStruct.points)
    {
        if (!sp)
            continue;
        auto* p = new LzConnectPoint(this);
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

QGraphicsItem* LzPowerBusbarSectionItem::duplicate()
{
    auto* dup = qgraphicsitem_cast<LzPowerBusbarSectionItem*>(LzWiringItemBase::duplicate());
    if (!dup)
        return nullptr;
    copyAlongEdgeConnectPointsFrom(this);
    return dup;
}

QRectF LzPowerBusbarSectionItem::busbarBodyRect() const
{
    const QRectF r = itemPosition;
    const qreal m = qMax(1.0, r.height() * 0.12);
    return r.adjusted(1, m, -1, -m);
}

bool LzPowerBusbarSectionItem::hitBusbarPickRegion(const QPointF& scenePos) const
{
    const QPointF local = mapFromScene(scenePos);
    return busbarBodyRect().adjusted(0, -kBusbarPickExpandY, 0, kBusbarPickExpandY).contains(local);
}

bool LzPowerBusbarSectionItem::snapSceneToEdgePort(const QPointF& scenePos, qreal maxDist,
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

LzConnectPoint* LzPowerBusbarSectionItem::ensureConnectPointNearScene(const QPointF& scenePos,
    qreal maxSnapDist)
{
    QPointF anchor;
    if (!snapSceneToEdgePort(scenePos, maxSnapDist, anchor))
        return nullptr;
    return ensureConnectPointAtScene(anchor);
}

bool LzPowerBusbarSectionItem::projectSceneToConnectAnchor(const QPointF& scenePos, QPointF& norm,
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

LzConnectPoint* LzPowerBusbarSectionItem::ensureConnectPointAtScene(const QPointF& scenePos)
{
    QPointF norm;
    CONNECT_DIRECTION dir = NONE_DIRECTION;
    if (!projectSceneToConnectAnchor(scenePos, norm, dir))
        return nullptr;

    const QPointF anchorScene = mapToScene(
        itemPosition.center() + QPointF(norm.x() * itemWidth, norm.y() * itemHeight));

    for (LzConnectPoint* p : pointStruct.points)
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

    auto* point = new LzConnectPoint(this);
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

bool LzPowerBusbarSectionItem::isConnectPointUsed(const LzConnectPoint* port) const
{
    if (!port)
        return false;
    for (QGraphicsItem* item : connectStruct.connects)
    {
        const auto* line = qgraphicsitem_cast<const LzConnectLine*>(item);
        if (!line)
            continue;
        if (line->startConnectPort() == port || line->endConnectPort() == port)
            return true;
    }
    return false;
}

LzConnectPoint* LzPowerBusbarSectionItem::findConnectPortNearScene(const QPointF& scenePos,
                                                            qreal maxDistPx) const
{
    LzConnectPoint* best = nullptr;
    qreal bestDist = maxDistPx;
    for (LzConnectPoint* p : pointStruct.points)
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

void LzPowerBusbarSectionItem::removeConnectPointIfUnused(LzConnectPoint* port)
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

void lzReattachBusbarPortsForConnectLine(LzConnectLine* line, bool preservePathShape)
{
    if (!line)
        return;

    const QVector<QPointF> path = line->pathInSceneCoords();
    if (path.isEmpty())
        return;

    const auto reattachEnd = [line, &path](bool isStart) {
        QGraphicsItem* const item = isStart ? line->getStartItem() : line->getEndItem();
        auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(item);
        if (!bus)
            return;

        const QPointF scenePt = isStart ? path.first() : path.last();
        LzConnectPoint* port = bus->ensureConnectPointAtScene(scenePt);
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

QPainterPath LzPowerBusbarSectionItem::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition.adjusted(0, -kBusbarSelectionExpandY, 0, kBusbarSelectionExpandY));
    return path;
}

void LzPowerBusbarSectionItem::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    LzWiringItemBase::stretch(handle, sx, sy, origin);
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

void LzPowerBusbarSectionItem::rebindAlongEdgeConnectPorts()
{
    const QRectF body = busbarBodyRect();
    const QPointF c = itemPosition.center();
    const qreal w = qMax(1.0, itemWidth);
    const qreal h = qMax(1.0, itemHeight);

    for (LzConnectPoint* p : pointStruct.points)
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

REGISTER_OBJECT_CREATOR(LzPowerBusbarSectionItem, busbarsection)
