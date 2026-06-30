#include "LzConnectLine.h"
#include "LzObjectFactory.h"
#include "LzConnectConfig.h"
#include "LzConnectPoint.h"
#include "TmpBase.h"
#include "LzGroup.h"
#include "LzScene.h"
#include "LzView.h"
#include "LzPowerBusbarSectionItem.h"
#include "wiring/base/LzWiringItemBase.h"
#include "command/LzEditConnectLinePathCommand.h"
#include "algorithm/LzConnectorAlgorithm.h"
#include "algorithm/LzAStar.h"
#include "algorithm/LzSnowflakeId.h"
#include "topology/ILzTopologyParticipant.h"
#include "topology/TopologyTypes.h"
#include <QDomElement>
#include <QMetaObject>
#include <QTimer>
#include <QSet>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <optional>
#include <variant>
#include <array>
#include <algorithm>
#include <iterator>
#include <vector>
#include <queue>

namespace {

TopologyRelationType topologyRelationFromInt(int v)
{
    if (v < 0 || v > static_cast<int>(TopologyRelationType::Reference))
        return TopologyRelationType::Physical;
    return static_cast<TopologyRelationType>(v);
}

QString stableTopologyNodeIdForItem(QGraphicsItem* item)
{
    if (!item)
        return {};
    if (auto* li = dynamic_cast<LzItem*>(item))
        return li->topologyNodeStableId();
    if (auto* grp = dynamic_cast<LzGroup*>(item))
        return grp->topologyNodeStableId();
    return {};
}

int connectPointOrdinalOnItem(QGraphicsItem* item, LzConnectPoint* port)
{
    if (!item || !port)
        return -1;
    int ordinal = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* candidate = qgraphicsitem_cast<LzConnectPoint*>(child))
        {
            if (candidate == port)
                return ordinal;
            ++ordinal;
        }
    }
    return -1;
}

bool isTopologyGraphNodeItem(QGraphicsItem* item)
{
    if (!item)
        return false;
    if (const auto* participant = lzTopologyParticipantForItem(item))
        return participant->participatesInTopology();
    if (auto* s = qgraphicsitem_cast<TmpShape*>(item))
        return s->isTopologyGraphNode();
    return false;
}

bool shouldMoveConnectLineAsSelectedBlock(const LzConnectLine* line)
{
    if (!line || !line->isSelected())
        return false;

    const QGraphicsItem* startItem = line->getStartItem();
    const QGraphicsItem* endItem = line->getEndItem();
    if (startItem && !startItem->isSelected())
        return false;
    if (endItem && !endItem->isSelected())
        return false;
    return startItem || endItem;
}

LzConnectPoint* nearestConnectPointOnItem(QGraphicsItem* item, const QPointF& scenePos)
{
    if (!item)
        return nullptr;
    LzConnectPoint* best = nullptr;
    qreal bestDist = std::numeric_limits<qreal>::max();
    for (QGraphicsItem* child : item->childItems())
    {
        auto* port = qgraphicsitem_cast<LzConnectPoint*>(child);
        if (!port)
            continue;
        const qreal dist = QLineF(port->connectionCenterInScene(), scenePos).length();
        if (!best || dist < bestDist)
        {
            best = port;
            bestDist = dist;
        }
    }
    return best;
}

/** 沿已完成的连线在设备间 BFS，用于判断新连线是否会封闭环路 */
static bool connectivityPathExists(TmpShape* from, TmpShape* to, const LzConnectLine* excludeLine)
{
    if (!from || !to)
        return false;
    if (from == to)
        return true;

    QSet<TmpShape*> visited;
    QList<TmpShape*> queue;
    queue.append(from);
    visited.insert(from);

    while (!queue.isEmpty())
    {
        TmpShape* cur = queue.takeFirst();
        for (QGraphicsItem* conn : cur->connectedItems())
        {
            auto* line = qgraphicsitem_cast<LzConnectLine*>(conn);
            if (!line || line == excludeLine)
                continue;
            TmpShape* neighbor = nullptr;
            if (line->getStartItem() == cur)
                neighbor = qgraphicsitem_cast<TmpShape*>(line->getEndItem());
            else if (line->getEndItem() == cur)
                neighbor = qgraphicsitem_cast<TmpShape*>(line->getStartItem());
            if (!neighbor)
                continue;
            if (neighbor == to)
                return true;
            if (!visited.contains(neighbor))
            {
                visited.insert(neighbor);
                queue.append(neighbor);
            }
        }
    }
    return false;
}

void detachLineFromEndpointShape(QGraphicsItem* item, LzConnectLine* line)
{
    if (!item || !line)
        return;
    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        shape->removeConnect(line);
}

void attachLineToEndpointShape(QGraphicsItem* item, LzConnectLine* line)
{
    if (!item || !line)
        return;
    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        shape->addConnect(line);
}

} // namespace

static void appendOrthogonalBridge(QVector<QPointF>& out, const QPointF& from, const QPointF& to,
                                   bool preferVerticalFirst, qreal eps,
                                   const QList<QGraphicsItem*>& obstacles,
                                   bool allowQuickAStar = true);
static void enforceStrictOrthogonalScenePath(QVector<QPointF>& pts, qreal eps);
static void collapseCollinearScenePoints(QVector<QPointF>& pts, qreal eps);

// 根据拐角样式由折线点构建 QPainterPath（仅用于绘制与 shape，不改变路径几何）
static QPainterPath buildPathFromPoints(const QPolygonF& points, LzConnectLine::CornerStyle style)
{
    QPainterPath path;
    if (points.size() < 2)
        return path;
    if (style == LzConnectLine::Sharp)
    {
        path.moveTo(points.first());
        for (int i = 1; i < points.size(); ++i)
            path.lineTo(points.at(i));
        return path;
    }
    const qreal radius = 8.0;
    path.moveTo(points[0]);
    for (int i = 1; i < points.size(); ++i)
    {
        if (i == points.size() - 1)
        {
            path.lineTo(points[i]);
            break;
        }
        QPointF curr = points[i];
        QPointF d1 = curr - points[i - 1];
        QPointF d2 = points[i + 1] - curr;
        qreal len1 = qSqrt(d1.x() * d1.x() + d1.y() * d1.y());
        qreal len2 = qSqrt(d2.x() * d2.x() + d2.y() * d2.y());
        if (len1 < 1e-6 || len2 < 1e-6)
        {
            path.lineTo(curr);
            continue;
        }
        d1 /= len1;
        d2 /= len2;
        qreal t = qMin(radius, len1 * 0.5);
        t = qMin(t, len2 * 0.5);
        if (t < 1e-6)
        {
            path.lineTo(curr);
            continue;
        }
        QPointF pt1 = curr - d1 * t;
        QPointF pt2 = curr + d2 * t;
        path.lineTo(pt1);
        // Rounded / Curved 均用二次贝塞尔；Rounded 控制点略内收使弯更小
        if (style == LzConnectLine::Rounded)
        {
            QPointF mid = (pt1 + pt2) * 0.5;
            path.quadTo(mid, pt2);
        }
        else
            path.quadTo(curr, pt2);
    }
    return path;
}

namespace {

constexpr qreal kLineJumpGapHalfPx = 6.0;

static bool segmentIntersectionParam(const QPointF& p, const QPointF& r, const QPointF& q, const QPointF& s,
                                     qreal& tOut, qreal& uOut)
{
    const QPointF d1 = r - p;
    const QPointF d2 = s - q;
    const qreal cross = d1.x() * d2.y() - d1.y() * d2.x();
    if (qAbs(cross) < 1e-9)
        return false;
    const QPointF dp = q - p;
    const qreal t = (dp.x() * d2.y() - dp.y() * d2.x()) / cross;
    const qreal u = (dp.x() * d1.y() - dp.y() * d1.x()) / cross;
    if (t < -1e-6 || t > 1.0 + 1e-6 || u < -1e-6 || u > 1.0 + 1e-6)
        return false;
    tOut = t;
    uOut = u;
    return true;
}

static QVector<QPointF> scenePolylineForLine(const LzConnectLine* line)
{
    if (!line)
        return {};
    return line->pathInSceneCoords();
}

static void collectJumpTsOnSegment(const QPointF& a, const QPointF& b,
                                   const QList<QVector<QPointF>>& otherPolylines,
                                   QVector<qreal>& jumpT)
{
    const qreal segLen = QLineF(a, b).length();
    if (segLen < 1e-6)
        return;
    for (const QVector<QPointF>& other : otherPolylines)
    {
        if (other.size() < 2)
            continue;
        for (int j = 0; j < other.size() - 1; ++j)
        {
            qreal t = 0.0;
            qreal u = 0.0;
            if (!segmentIntersectionParam(a, b, other[j], other[j + 1], t, u))
                continue;
            t = qBound(0.0, t, 1.0);
            jumpT.append(t);
        }
    }
    std::sort(jumpT.begin(), jumpT.end());
}

static void drawSegmentWithJumps(QPainter* painter, const QPointF& a, const QPointF& b,
                                 const QVector<qreal>& jumpT)
{
    const qreal segLen = QLineF(a, b).length();
    if (segLen < 1e-6)
        return;
    const qreal gapT = qMin(0.45, kLineJumpGapHalfPx / segLen);
    QVector<QPair<qreal, qreal>> spans;
    spans.append({0.0, 1.0});
    for (qreal t : jumpT)
    {
        const qreal lo = qMax(0.0, t - gapT);
        const qreal hi = qMin(1.0, t + gapT);
        QVector<QPair<qreal, qreal>> next;
        for (const auto& span : spans)
        {
            if (hi <= span.first || lo >= span.second)
            {
                next.append(span);
                continue;
            }
            if (span.first < lo)
                next.append({span.first, lo});
            if (hi < span.second)
                next.append({hi, span.second});
        }
        spans = next;
        if (spans.isEmpty())
            break;
    }
    for (const auto& span : spans)
    {
        if (span.second - span.first < 1e-6)
            continue;
        const QPointF p0 = a + (b - a) * span.first;
        const QPointF p1 = a + (b - a) * span.second;
        painter->drawLine(p0, p1);
    }
}

static void drawPolylineWithJumps(QPainter* painter, const QPolygonF& scenePts,
                                  const QList<QVector<QPointF>>& otherPolylines)
{
    if (scenePts.size() < 2)
        return;
    for (int i = 0; i < scenePts.size() - 1; ++i)
    {
        const QPointF a = scenePts[i];
        const QPointF b = scenePts[i + 1];
        QVector<qreal> jumpT;
        collectJumpTsOnSegment(a, b, otherPolylines, jumpT);
        drawSegmentWithJumps(painter, a, b, jumpT);
    }
}

static void drawArrowHead(QPainter* painter, const QPointF& tip, const QPointF& from,
                          qreal size)
{
    QPointF dir = tip - from;
    const qreal len = QLineF(QPointF(), dir).length();
    if (len < 1e-6)
        return;
    dir /= len;
    const QPointF ortho(-dir.y(), dir.x());
    const QPointF back = tip - dir * size;
    QPolygonF head;
    head << tip << (back + ortho * size * 0.45) << (back - ortho * size * 0.45);
    painter->save();
    painter->setBrush(painter->pen().color());
    painter->drawPolygon(head);
    painter->restore();
}

static void drawArrowHeads(QPainter* painter, const QPolygonF& pts,
                           LzConnectLine::ArrowHeadStyle arrow, qreal penWidth)
{
    if (pts.size() < 2 || arrow == LzConnectLine::ArrowNone)
        return;
    const qreal size = qMax(8.0, penWidth * 3.0);
    if (arrow == LzConnectLine::ArrowEnd || arrow == LzConnectLine::ArrowBoth)
        drawArrowHead(painter, pts.last(), pts[pts.size() - 2], size);
    if (arrow == LzConnectLine::ArrowStart || arrow == LzConnectLine::ArrowBoth)
        drawArrowHead(painter, pts.first(), pts[1], size);
}

} // namespace

void LzConnectLine::paintToolboxIcon(QPainter* painter, const QRectF& rect)
{
    if (!painter || rect.width() < 2 || rect.height() < 2)
        return;
    constexpr qreal kMargin = 4;
    const QRectF rr = rect.adjusted(kMargin, kMargin, -kMargin, -kMargin);
    if (rr.width() < 1 || rr.height() < 1)
        return;
    QPolygonF pts;
    pts << QPointF(rr.left(), rr.center().y()) << QPointF(rr.center().x(), rr.center().y())
        << QPointF(rr.center().x(), rr.bottom()) << QPointF(rr.right(), rr.bottom());
    const QPainterPath path = buildPathFromPoints(pts, LzConnectLine::CornerStyle::Sharp);
    const QColor stroke = LzAttributeManager::usePen_ ? LzAttributeManager::penColor_ : QColor(0, 0, 0);
    const int pw = qMax(1, LzAttributeManager::penWidth_);
    QPen pen(stroke, pw, static_cast<Qt::PenStyle>(LzAttributeManager::penStyle_), Qt::FlatCap, Qt::MiterJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    if (!path.isEmpty())
        painter->drawPath(path);
    else
        painter->drawPolyline(pts);
}

// 从图元上取离参考点最近的连接点中心（场景坐标）；超出吸附半径则保持参考点
static QPointF connectionCenterForItem(QGraphicsItem* item, const QPointF& referenceScene)
{
    if (!item)
        return referenceScene;
    QPointF best = referenceScene;
    qreal bestDist = LzConnectPointSnapRadiusPx;
    for (QGraphicsItem* child : item->childItems())
    {
        if (LzConnectPoint* cp = qgraphicsitem_cast<LzConnectPoint*>(child))
        {
            const QPointF center = cp->connectionCenterInScene();
            const qreal d = QLineF(center, referenceScene).length();
            if (d <= bestDist)
            {
                bestDist = d;
                best = center;
            }
        }
    }
    return best;
}

static bool connectPortStillOnItem(LzConnectPoint* port, QGraphicsItem* item)
{
    return port && item && port->parentItem() == item && port->scene();
}

static LzConnectPoint* resolveConnectPortForSave(QGraphicsItem* item, LzConnectPoint* currentPort,
                                                 const QPointF& scenePos)
{
    if (connectPortStillOnItem(currentPort, item))
        return currentPort;
    if (!item)
        return nullptr;
    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(item))
    {
        if (LzConnectPoint* port = bus->ensureConnectPointAtScene(scenePos))
            return port;
    }
    if (LzConnectPoint* port = nearestConnectPointOnItem(item, scenePos))
        return port;
    if (auto* wiring = dynamic_cast<LzWiringItemBase*>(item))
        return wiring->ensureConnectPointAtScene(scenePos);
    return nullptr;
}

static QPointF endpointSceneForItem(QGraphicsItem* item, LzConnectPoint* pinnedPort,
                                    const QPointF& referenceScene)
{
    if (connectPortStillOnItem(pinnedPort, item))
        return pinnedPort->connectionCenterInScene();
    return connectionCenterForItem(item, referenceScene);
}

static bool itemHasEffectiveRotation(QGraphicsItem* item)
{
    if (!item)
        return false;
    return !qFuzzyIsNull(std::fmod(std::abs(item->rotation()), 360.0));
}

static QPointF baseUnitForDirection(CONNECT_DIRECTION direction)
{
    switch (direction)
    {
    case TOP_DIRECTION:
        return QPointF(0.0, -1.0);
    case RIGHT_DIRECTION:
        return QPointF(1.0, 0.0);
    case BOTTOM_DIRECTION:
        return QPointF(0.0, 1.0);
    case LEFT_DIRECTION:
        return QPointF(-1.0, 0.0);
    default:
        return {};
    }
}

static QPointF normalizedSceneUnit(const QPointF& vector)
{
    const qreal length = QLineF(QPointF(), vector).length();
    if (length < 1e-6)
        return {};
    return QPointF(vector.x() / length, vector.y() / length);
}

static QPointF outwardUnitForPort(LzConnectPoint* port)
{
    if (!port)
        return {};

    const QPointF declaredUnit = baseUnitForDirection(port->getDirection());
    QGraphicsItem* parent = port->parentItem();
    if (parent && itemHasEffectiveRotation(parent) && !declaredUnit.isNull())
    {
        const QPointF localAnchor = port->connectionCenterLocal();
        const QPointF sceneAnchor = port->mapToScene(localAnchor);
        const QPointF sceneNeighbor = port->mapToScene(localAnchor + declaredUnit);
        const QPointF rotatedUnit = normalizedSceneUnit(sceneNeighbor - sceneAnchor);
        if (!rotatedUnit.isNull())
            return rotatedUnit;
    }

    if (!declaredUnit.isNull())
        return declaredUnit;

    if (!parent)
        return {};
    const QPointF anchor = port->connectionCenterInScene();
    const QPointF center = parent->mapToScene(parent->boundingRect().center());
    QPointF d = anchor - center;
    if (QLineF(QPointF(), d).length() < 1.0)
        return QPointF(1.0, 0.0);
    if (qAbs(d.x()) >= qAbs(d.y()))
        return QPointF(d.x() >= 0.0 ? 1.0 : -1.0, 0.0);
    return QPointF(0.0, d.y() >= 0.0 ? 1.0 : -1.0);
}

/** 侧向端口先沿法向离开图元，再转向目标，避免 L 形第一段横穿本体 */
static bool preferVerticalFirstForRoute(const QPointF& from, const QPointF& to, LzConnectPoint* startPort)
{
    constexpr qreal kEps = 1.0;
    if (startPort)
    {
        const QPointF unit = outwardUnitForPort(startPort);
        if (qAbs(unit.x()) >= qAbs(unit.y()))
        {
            if (unit.x() < 0.0)
            {
                if (to.x() > from.x() + kEps)
                    return true;
                if (to.x() < from.x() - kEps)
                    return false;
            }
            else
            {
                if (to.x() < from.x() - kEps)
                    return true;
                if (to.x() > from.x() + kEps)
                    return false;
            }
        }
        else
        {
            if (unit.y() < 0.0)
            {
                if (to.y() > from.y() + kEps)
                    return false;
                if (to.y() < from.y() - kEps)
                    return true;
            }
            else
            {
                if (to.y() < from.y() - kEps)
                    return false;
                if (to.y() > from.y() + kEps)
                    return true;
            }
        }
    }
    return qAbs(to.x() - from.x()) < qAbs(to.y() - from.y());
}

static qreal rayExitDistanceFromRect(const QRectF& rect, const QPointF& anchor, const QPointF& unit)
{
    constexpr qreal kEps = 1e-6;
    qreal distance = 0.0;

    if (unit.x() > kEps)
        distance = qMax(distance, (rect.right() - anchor.x()) / unit.x());
    else if (unit.x() < -kEps)
        distance = qMax(distance, (rect.left() - anchor.x()) / unit.x());

    if (unit.y() > kEps)
        distance = qMax(distance, (rect.bottom() - anchor.y()) / unit.y());
    else if (unit.y() < -kEps)
        distance = qMax(distance, (rect.top() - anchor.y()) / unit.y());

    return qMax(0.0, distance);
}

/** 沿端口法向走出图元外接矩形 + 与 A* 障碍膨胀一致的安全边距，避免路径穿过符号本体 */
static qreal leadOutDistanceForPort(LzConnectPoint* port, const QPointF& anchorScene)
{
    constexpr qreal kMinLead = LzWireRoutingLeadOutPx;
    QGraphicsItem* parent = port ? port->parentItem() : nullptr;
    if (!parent)
        return kMinLead;

    const QRectF expanded = LzAStar::expandedObstacleSceneRect(parent, ObstacleClearanceMargin);
    const QPointF unit = outwardUnitForPort(port);
    if (unit.isNull())
        return kMinLead;

    const qreal margin = 16.0;
    return qMax(kMinLead, rayExitDistanceFromRect(expanded, anchorScene, unit) + margin);
}

static QPointF routingSceneForPort(LzConnectPoint* port, const QPointF& anchorScene)
{
    const QPointF unit = outwardUnitForPort(port);
    if (qAbs(unit.x()) < 0.5 && qAbs(unit.y()) < 0.5)
        return anchorScene;
    return anchorScene + unit * leadOutDistanceForPort(port, anchorScene);
}

static QPointF visualEndpointSceneForPort(LzConnectPoint* port, const QPointF& anchorScene)
{
    Q_UNUSED(port);
    return anchorScene;
}

static void appendPortExitFromAnchor(QVector<QPointF>& out, const QPointF& anchor,
                                     LzConnectPoint* port, qreal eps)
{
    const QPointF visualAnchor = visualEndpointSceneForPort(port, anchor);
    const QPointF exitPt = routingSceneForPort(port, anchor);
    if (QLineF(visualAnchor, exitPt).length() <= eps)
    {
        if (out.isEmpty() || QLineF(out.last(), visualAnchor).length() > eps)
            out.append(visualAnchor);
        return;
    }
    if (out.isEmpty() || QLineF(out.last(), visualAnchor).length() > eps)
        out.append(visualAnchor);
    if (QLineF(out.last(), exitPt).length() > eps)
        out.append(exitPt);
}

static void appendPortApproachToAnchor(QVector<QPointF>& out, const QPointF& routePt,
                                       const QPointF& anchor, LzConnectPoint* port, qreal eps,
                                       const QList<QGraphicsItem*>& obstacles,
                                       bool allowQuickAStar = true)
{
    const QPointF visualAnchor = visualEndpointSceneForPort(port, anchor);
    if (QLineF(routePt, visualAnchor).length() <= eps)
        return;
    bool preferVerticalFirst = qAbs(routePt.x() - visualAnchor.x()) < qAbs(routePt.y() - visualAnchor.y());
    if (port)
    {
        switch (port->getDirection())
        {
        case TOP_DIRECTION:
        case BOTTOM_DIRECTION:
            preferVerticalFirst = true;
            break;
        case LEFT_DIRECTION:
        case RIGHT_DIRECTION:
            preferVerticalFirst = false;
            break;
        default:
            break;
        }
    }
    const QPointF routeTarget = routingSceneForPort(port, anchor);
    if (QLineF(routePt, routeTarget).length() > eps)
        appendOrthogonalBridge(out, routePt, routeTarget, preferVerticalFirst, eps, obstacles,
                               allowQuickAStar);

    if (out.isEmpty() || QLineF(out.last(), routeTarget).length() > eps)
        out.append(routeTarget);
    if (QLineF(out.last(), visualAnchor).length() > eps)
        out.append(visualAnchor);
}

static int countSegmentObstacleHits(const QPointF& a, const QPointF& b, const QPointF& c,
                                    const QList<QGraphicsItem*>& obstacles, LzAStar& astar)
{
    int hits = 0;
    if (astar.segmentIntersectsObstacles(a.toPoint(), b.toPoint(), obstacles))
        ++hits;
    if (astar.segmentIntersectsObstacles(b.toPoint(), c.toPoint(), obstacles))
        ++hits;
    return hits;
}

static QList<QPoint> toPointList(const QVector<QPoint>& pts)
{
    QList<QPoint> out;
    out.reserve(pts.size());
    for (const QPoint& p : pts)
        out.append(p);
    return out;
}

static QList<QGraphicsItem*> obstaclesExceptItems(const QList<QGraphicsItem*>& all,
                                                  QGraphicsItem* a, QGraphicsItem* b)
{
    QList<QGraphicsItem*> out;
    out.reserve(all.size());
    for (QGraphicsItem* it : all)
    {
        if (!it || it == a || it == b)
            continue;
        out.append(it);
    }
    return out;
}

/** 主路由区障碍：已绑定明确端口时，起止图元也参与避障，避免为到达背面端口而横穿图元本体。 */
static QList<QGraphicsItem*> coreObstaclesForLine(const QList<QGraphicsItem*>& all,
                                                  QGraphicsItem* startItem, LzConnectPoint* startPort,
                                                  QGraphicsItem* endItem, LzConnectPoint* endPort)
{
    if (startItem && startItem == endItem)
        return all;

    const bool keepStartItem = connectPortStillOnItem(startPort, startItem);
    const bool keepEndItem = connectPortStillOnItem(endPort, endItem);
    QList<QGraphicsItem*> out;
    out.reserve(all.size());
    for (QGraphicsItem* it : all)
    {
        if (!it)
            continue;
        if (it == startItem && !keepStartItem)
            continue;
        if (it == endItem && !keepEndItem)
            continue;
        out.append(it);
    }
    return out;
}

static QList<QGraphicsItem*> bridgeObstaclesForLine(const QList<QGraphicsItem*>& all,
                                                    QGraphicsItem* startItem, QGraphicsItem* endItem)
{
    return obstaclesExceptItems(all, startItem, endItem);
}

static bool scenePathIntersectsObstacles(const QVector<QPointF>& pts,
                                         const QList<QGraphicsItem*>& obstacles)
{
    if (pts.size() < 2 || obstacles.isEmpty())
        return false;
    LzAStar astar;
    for (int i = 0; i + 1 < pts.size(); ++i)
    {
        if (astar.segmentIntersectsObstacles(pts[i].toPoint(), pts[i + 1].toPoint(), obstacles))
            return true;
    }
    return false;
}

/** 校验路径是否穿过「非端点」图元；起止段允许进入绑定图元，中间段不允许穿任何符号 */
static const QList<QGraphicsItem*>& liteRoutingObstacles()
{
    static const QList<QGraphicsItem*> none;
    return none;
}

static bool scenePathCrossesForeignObstacles(const QVector<QPointF>& pts,
                                            const QList<QGraphicsItem*>& allObstacles,
                                            QGraphicsItem* startItem, QGraphicsItem* endItem)
{
    if (pts.size() < 2 || allObstacles.isEmpty())
        return false;
    LzAStar astar;
    for (int i = 0; i + 1 < pts.size(); ++i)
    {
        QList<QGraphicsItem*> segObs = allObstacles;
        if (i == 0 && startItem)
            segObs = obstaclesExceptItems(allObstacles, startItem, nullptr);
        else if (i + 1 == pts.size() - 1 && endItem)
            segObs = obstaclesExceptItems(allObstacles, nullptr, endItem);
        if (astar.segmentIntersectsObstacles(pts[i].toPoint(), pts[i + 1].toPoint(), segObs))
            return true;
    }
    return false;
}

static void appendScenePointsSkippingFirst(QVector<QPointF>& out, const QVector<QPointF>& pts, qreal eps)
{
    for (int i = 1; i < pts.size(); ++i)
    {
        if (out.isEmpty() || QLineF(out.last(), pts[i]).length() > eps)
            out.append(pts[i]);
    }
}

static bool segmentClearsObstacles(const QPointF& a, const QPointF& b,
                                 const QList<QGraphicsItem*>& obstacles, LzAStar& astar)
{
    return obstacles.isEmpty()
        || !astar.segmentIntersectsObstacles(a.toPoint(), b.toPoint(), obstacles);
}

/** 无 A*：沿合并障碍外接矩形顶/底/左/右绕行 */
static bool appendBBoxOrthogonalDetour(QVector<QPointF>& out, const QPointF& from, const QPointF& to,
                                       const QList<QGraphicsItem*>& obstacles, qreal eps)
{
    if (obstacles.isEmpty())
        return false;

    LzAStar astar;
    if (segmentClearsObstacles(from, to, obstacles, astar))
        return false;

    QRectF block;
    const QPoint a = from.toPoint();
    const QPoint b = to.toPoint();
    for (QGraphicsItem* it : obstacles)
    {
        if (!it || !it->isVisible())
            continue;
        if (!astar.segmentIntersectsObstacles(a, b, {it}))
            continue;
        const QRectF r = LzAStar::expandedObstacleSceneRect(it, ObstacleClearanceMargin);
        if (!r.isValid())
            continue;
        block = block.isValid() ? block.united(r) : r;
    }
    if (!block.isValid())
        return false;
    block.adjust(-8.0, -8.0, 8.0, 8.0);

    const qreal top = block.top();
    const qreal bot = block.bottom();
    const qreal left = block.left();
    const qreal right = block.right();

    auto buildPath = [&](const QVector<QPointF>& mids) -> QVector<QPointF> {
        QVector<QPointF> path;
        path.append(from);
        for (const QPointF& mid : mids)
        {
            if (QLineF(path.last(), mid).length() <= eps)
                continue;
            path.append(mid);
        }
        if (QLineF(path.last(), to).length() > eps)
            path.append(to);
        if (path.size() < 2)
            return {};
        for (int i = 0; i + 1 < path.size(); ++i)
        {
            if (!segmentClearsObstacles(path[i], path[i + 1], obstacles, astar))
                return {};
        }
        QVector<QPointF> tail;
        tail.reserve(path.size() - 1);
        for (int i = 1; i < path.size(); ++i)
            tail.append(path[i]);
        return tail;
    };

    const QVector<QPointF> topRoute = {QPointF(from.x(), top), QPointF(to.x(), top)};
    const QVector<QPointF> botRoute = {QPointF(from.x(), bot), QPointF(to.x(), bot)};
    const QVector<QPointF> leftRoute = {QPointF(left, from.y()), QPointF(left, to.y())};
    const QVector<QPointF> rightRoute = {QPointF(right, from.y()), QPointF(right, to.y())};

    struct RankedPath
    {
        QVector<QPointF> mids;
        qreal len = 0.0;
    };
    QVector<RankedPath> ranked;
    auto rank = [&](const QVector<QPointF>& mids) {
        QVector<QPointF> path;
        path.append(from);
        for (const QPointF& mid : mids)
            path.append(mid);
        path.append(to);
        qreal len = 0.0;
        for (int i = 0; i + 1 < path.size(); ++i)
            len += QLineF(path[i], path[i + 1]).length();
        ranked.append({mids, len});
    };
    rank(topRoute);
    rank(botRoute);
    rank(leftRoute);
    rank(rightRoute);

    std::sort(ranked.begin(), ranked.end(),
              [](const RankedPath& x, const RankedPath& y) { return x.len < y.len; });
    for (const RankedPath& candidate : ranked)
    {
        const QVector<QPointF> tail = buildPath(candidate.mids);
        if (tail.isEmpty())
            continue;
        for (const QPointF& p : tail)
        {
            if (out.isEmpty() || QLineF(out.last(), p).length() > eps)
                out.append(p);
        }
        return true;
    }
    return false;
}

/** 正交桥接：优先 L 形；穿障时矩形绕行或（可选）Quick A* */
static void appendOrthogonalBridge(QVector<QPointF>& out, const QPointF& from, const QPointF& to,
                                   bool preferVerticalFirst, qreal eps,
                                   const QList<QGraphicsItem*>& obstacles,
                                   bool allowQuickAStar)
{
    if (QLineF(from, to).length() <= eps)
    {
        if (out.isEmpty() || QLineF(out.last(), to).length() > eps)
            out.append(to);
        return;
    }
    if (qAbs(from.x() - to.x()) <= eps || qAbs(from.y() - to.y()) <= eps)
    {
        LzAStar astar;
        if (!obstacles.isEmpty()
            && astar.segmentIntersectsObstacles(from.toPoint(), to.toPoint(), obstacles))
        {
            if (!allowQuickAStar
                && appendBBoxOrthogonalDetour(out, from, to, obstacles, eps))
                return;
            if (allowQuickAStar)
            {
                const QVector<QPoint> detour =
                    LzConnectorAlgorithm::planOrthogonalPathQuick(from, to, obstacles);
                const QList<QPoint> detourList = toPointList(detour);
                if (detour.size() >= 2
                    && !astar.pathSegmentsIntersectObstacles(detourList, obstacles))
                {
                    QVector<QPointF> sceneDetour;
                    sceneDetour.reserve(detour.size());
                    for (const QPoint& p : detour)
                        sceneDetour.append(p);
                    appendScenePointsSkippingFirst(out, sceneDetour, eps);
                    return;
                }
            }
            else if (appendBBoxOrthogonalDetour(out, from, to, obstacles, eps))
                return;
        }
        if (out.isEmpty() || QLineF(out.last(), to).length() > eps)
            out.append(to);
        return;
    }

    const QPointF cornerA = preferVerticalFirst ? QPointF(to.x(), from.y()) : QPointF(from.x(), to.y());
    const QPointF cornerB = preferVerticalFirst ? QPointF(from.x(), to.y()) : QPointF(to.x(), from.y());

    LzAStar astar;
    astar.setGridStep(StepSize);
    const int badA = obstacles.isEmpty() ? 0 : countSegmentObstacleHits(from, cornerA, to, obstacles, astar);
    const int badB = obstacles.isEmpty() ? 0 : countSegmentObstacleHits(from, cornerB, to, obstacles, astar);

    if (badA == 0 || badB == 0)
    {
        const QPointF corner = (badA == 0) ? cornerA : cornerB;
        if (out.isEmpty() || QLineF(out.last(), corner).length() > eps)
            out.append(corner);
        if (QLineF(out.last(), to).length() > eps)
            out.append(to);
        return;
    }

    if (!allowQuickAStar && appendBBoxOrthogonalDetour(out, from, to, obstacles, eps))
        return;

    if (allowQuickAStar)
    {
        const QVector<QPoint> detour =
            LzConnectorAlgorithm::planOrthogonalPathQuick(from, to, obstacles);
        const QList<QPoint> detourList = toPointList(detour);
        if (detour.size() >= 2
            && !astar.pathSegmentsIntersectObstacles(detourList, obstacles))
        {
            QVector<QPointF> sceneDetour;
            sceneDetour.reserve(detour.size());
            for (const QPoint& p : detour)
                sceneDetour.append(p);
            appendScenePointsSkippingFirst(out, sceneDetour, eps);
            return;
        }
    }
    else if (appendBBoxOrthogonalDetour(out, from, to, obstacles, eps))
        return;

    const QPointF corner = (badB < badA) ? cornerB : cornerA;
    if (out.isEmpty() || QLineF(out.last(), corner).length() > eps)
        out.append(corner);
    if (QLineF(out.last(), to).length() > eps)
        out.append(to);
}

static void repairScenePathAgainstObstacles(QVector<QPointF>& pts, qreal eps,
                                          const QList<QGraphicsItem*>& obstacles)
{
    if (pts.size() < 2 || obstacles.isEmpty())
        return;

    LzAStar astar;
    astar.setGridStep(StepSize);
    const int maxRepairPasses = qMax(64, pts.size() * 8);
    int repairPasses = 0;
    for (int i = 0; i + 1 < pts.size();)
    {
        if (++repairPasses > maxRepairPasses)
            break;
        const QPointF a = pts[i];
        const QPointF b = pts[i + 1];
        if (!astar.segmentIntersectsObstacles(a.toPoint(), b.toPoint(), obstacles))
        {
            ++i;
            continue;
        }

        const QVector<QPoint> detour = LzConnectorAlgorithm::planOrthogonalPathQuick(a, b, obstacles);
        const QList<QPoint> detourList = toPointList(detour);
        if (detour.size() < 2
            || astar.pathSegmentsIntersectObstacles(detourList, obstacles))
        {
            ++i;
            continue;
        }

        pts.removeAt(i + 1);
        for (int j = 1; j < detour.size(); ++j)
            pts.insert(i + j, detour[j]);
        i += detour.size() - 1;
    }

    collapseCollinearScenePoints(pts, eps);
    enforceStrictOrthogonalScenePath(pts, eps);
}

// 起点：从 route 点接到 A* 首段；保证每段水平/垂直，并衔接到 p1
static void appendStartPortChain(QVector<QPointF>& out, const QPointF& routePt, const QPointF& p1,
                                 bool firstSegVertical, qreal eps,
                                 const QList<QGraphicsItem*>& obstacles)
{
    if (out.isEmpty() || QLineF(out.last(), routePt).length() > eps)
        out.append(routePt);
    appendOrthogonalBridge(out, routePt, p1, firstSegVertical, eps, obstacles, true);
}

/** 合并共线点后，对仍存在的斜段插入直角拐点（draw.io 严格正交） */
static void enforceStrictOrthogonalScenePath(QVector<QPointF>& pts, qreal eps = 0.5)
{
    if (pts.size() < 2)
        return;
    QVector<QPointF> out;
    out.append(pts.first());
    for (int i = 1; i < pts.size(); ++i)
    {
        QPointF prev = out.last();
        QPointF curr = pts[i];
        if (qAbs(prev.x() - curr.x()) > eps && qAbs(prev.y() - curr.y()) > eps)
        {
            QPointF corner(curr.x(), prev.y());
            if (out.size() >= 2)
            {
                const QPointF pp = out[out.size() - 2];
                if (qAbs(pp.x() - prev.x()) <= eps)
                    corner = QPointF(prev.x(), curr.y());
                else if (qAbs(pp.y() - prev.y()) <= eps)
                    corner = QPointF(curr.x(), prev.y());
            }
            if (QLineF(out.last(), corner).length() > eps)
                out.append(corner);
        }
        if (QLineF(out.last(), curr).length() > eps)
            out.append(curr);
    }
    pts = out;
}

static void collapseCollinearScenePoints(QVector<QPointF>& pts, qreal eps = 0.5)
{
    if (pts.size() < 3)
        return;
    QVector<QPointF> out;
    out.append(pts.first());
    for (int i = 1; i < pts.size() - 1; ++i)
    {
        const QPointF a = out.last();
        const QPointF b = pts[i];
        const QPointF c = pts[i + 1];
        const bool collinearH = qAbs(a.y() - b.y()) <= eps && qAbs(b.y() - c.y()) <= eps;
        const bool collinearV = qAbs(a.x() - b.x()) <= eps && qAbs(b.x() - c.x()) <= eps;

        bool removable = false;
        if (collinearH)
        {
            const qreal dx1 = b.x() - a.x();
            const qreal dx2 = c.x() - b.x();
            removable = dx1 * dx2 >= -eps;
        }
        else if (collinearV)
        {
            const qreal dy1 = b.y() - a.y();
            const qreal dy2 = c.y() - b.y();
            removable = dy1 * dy2 >= -eps;
        }

        if (!removable)
            out.append(b);
    }
    out.append(pts.last());
    pts = out;
}

/** 消除近水平/近垂直斜段，保证相邻点仅水平或垂直相连（draw.io 严格正交） */
static void sanitizeOrthogonalScenePath(QVector<QPointF>& pts, qreal eps = 0.5)
{
    if (pts.size() < 2)
        return;

    enforceStrictOrthogonalScenePath(pts, eps);

    for (int i = 1; i < pts.size(); ++i)
    {
        QPointF& prev = pts[i - 1];
        QPointF& curr = pts[i];
        const qreal dx = curr.x() - prev.x();
        const qreal dy = curr.y() - prev.y();
        if (qAbs(dx) <= eps)
        {
            curr.setX(prev.x());
            continue;
        }
        if (qAbs(dy) <= eps)
        {
            curr.setY(prev.y());
            continue;
        }
        const QPointF corner = (qAbs(dx) >= qAbs(dy)) ? QPointF(curr.x(), prev.y())
                                                       : QPointF(prev.x(), curr.y());
        pts.insert(i, corner);
        ++i;
    }

    collapseCollinearScenePoints(pts, eps);
    enforceStrictOrthogonalScenePath(pts, eps);
}

static QVector<QPointF> simplifiedOrthogonalScenePath(const QVector<QPointF>& scenePts, qreal eps = 0.5)
{
    QVector<QPointF> simplified = scenePts;
    sanitizeOrthogonalScenePath(simplified, eps);
    collapseCollinearScenePoints(simplified, eps);
    return simplified;
}

enum class PortLeadEndpoint
{
    Start,
    End,
};

static bool pathPreservesPortLead(const QVector<QPointF>& scenePts,
                                  LzConnectPoint* port,
                                  PortLeadEndpoint endpoint,
                                  qreal eps = 0.5)
{
    if (!port || scenePts.size() < 2)
        return true;

    const QPointF unit = outwardUnitForPort(port);
    if (unit.isNull())
        return true;

    const QPointF anchor = endpoint == PortLeadEndpoint::Start ? scenePts.first() : scenePts.last();
    const qreal minLead = qMax<qreal>(leadOutDistanceForPort(port, anchor) - eps, eps);
    qreal keptLead = 0.0;

    if (endpoint == PortLeadEndpoint::Start)
    {
        for (int idx = 1; idx < scenePts.size(); ++idx)
        {
            const QPointF prev = scenePts[idx - 1];
            const QPointF curr = scenePts[idx];
            const QPointF delta = curr - prev;
            bool alongLead = false;
            if (qAbs(unit.x()) > 0.5 && qAbs(delta.y()) <= eps)
                alongLead = delta.x() * unit.x() >= -eps;
            else if (qAbs(unit.y()) > 0.5 && qAbs(delta.x()) <= eps)
                alongLead = delta.y() * unit.y() >= -eps;

            if (!alongLead)
                break;

            keptLead += QLineF(prev, curr).length();
            if (keptLead >= minLead)
                return true;
        }
        return false;
    }

    for (int idx = scenePts.size() - 2; idx >= 0; --idx)
    {
        const QPointF curr = scenePts[idx];
        const QPointF next = scenePts[idx + 1];
        const QPointF delta = next - curr;
        bool alongLead = false;
        if (qAbs(unit.x()) > 0.5 && qAbs(delta.y()) <= eps)
            alongLead = delta.x() * unit.x() <= eps;
        else if (qAbs(unit.y()) > 0.5 && qAbs(delta.x()) <= eps)
            alongLead = delta.y() * unit.y() <= eps;

        if (!alongLead)
            break;

        keptLead += QLineF(curr, next).length();
        if (keptLead >= minLead)
            return true;
    }
    return false;
}

static bool pathPreservesEndpointPortLeads(const QVector<QPointF>& scenePts,
                                           const LzConnectLine* line,
                                           qreal eps = 0.5)
{
    if (!line)
        return true;

    return pathPreservesPortLead(scenePts, line->startConnectPort(), PortLeadEndpoint::Start, eps)
        && pathPreservesPortLead(scenePts, line->endConnectPort(), PortLeadEndpoint::End, eps);
}

static QVector<QPointF> mergeOrthogonalStairSteps(const QVector<QPointF>& scenePts, qreal eps = 0.5)
{
    QVector<QPointF> simplified = simplifiedOrthogonalScenePath(scenePts, eps);
    if (simplified.size() < 4)
        return simplified;

    QVector<QPointF> merged;
    merged.reserve(simplified.size());
    merged.append(simplified.first());

    int i = 1;
    while (i < simplified.size() - 1)
    {
        const QPointF anchor = merged.last();
        const QPointF next = simplified[i];
        const bool firstHorizontal = qAbs(anchor.y() - next.y()) <= eps;
        const bool firstVertical = qAbs(anchor.x() - next.x()) <= eps;
        if (!firstHorizontal && !firstVertical)
        {
            merged.append(next);
            ++i;
            continue;
        }

        int last = i;
        qreal spanMin = firstHorizontal ? qMin(anchor.x(), next.x()) : qMin(anchor.y(), next.y());
        qreal spanMax = firstHorizontal ? qMax(anchor.x(), next.x()) : qMax(anchor.y(), next.y());
        while (last + 2 < simplified.size())
        {
            const QPointF mid = simplified[last + 1];
            const QPointF tail = simplified[last + 2];
            const bool secondVertical = qAbs(next.x() - mid.x()) <= eps;
            const bool secondHorizontal = qAbs(next.y() - mid.y()) <= eps;
            const bool thirdHorizontal = qAbs(mid.y() - tail.y()) <= eps;
            const bool thirdVertical = qAbs(mid.x() - tail.x()) <= eps;

            if (firstHorizontal)
            {
                if (!secondVertical || !thirdHorizontal)
                    break;
                if (qAbs(anchor.y() - tail.y()) > eps)
                    break;
                const qreal tailMin = qMin(mid.x(), tail.x());
                const qreal tailMax = qMax(mid.x(), tail.x());
                if (tailMin > spanMax + eps || tailMax < spanMin - eps)
                    break;
                spanMin = qMin(spanMin, tailMin);
                spanMax = qMax(spanMax, tailMax);
            }
            else
            {
                if (!secondHorizontal || !thirdVertical)
                    break;
                if (qAbs(anchor.x() - tail.x()) > eps)
                    break;
                const qreal tailMin = qMin(mid.y(), tail.y());
                const qreal tailMax = qMax(mid.y(), tail.y());
                if (tailMin > spanMax + eps || tailMax < spanMin - eps)
                    break;
                spanMin = qMin(spanMin, tailMin);
                spanMax = qMax(spanMax, tailMax);
            }

            last += 2;
        }

        if (last > i)
        {
            const QPointF far = simplified[last + 1];
            const QPointF mergedPoint = firstHorizontal ? QPointF(far.x(), anchor.y())
                                                        : QPointF(anchor.x(), far.y());
            if (QLineF(merged.last(), mergedPoint).length() > eps)
                merged.append(mergedPoint);
            i = last + 1;
            continue;
        }

        merged.append(next);
        ++i;
    }

    if (QLineF(merged.last(), simplified.last()).length() > eps)
        merged.append(simplified.last());
    sanitizeOrthogonalScenePath(merged, eps);
    return merged;
}

static bool isHorizontalSceneSegment(const QPointF& a, const QPointF& b, qreal eps = 0.5)
{
    return qAbs(a.y() - b.y()) <= eps && qAbs(a.x() - b.x()) > eps;
}

static bool isVerticalSceneSegment(const QPointF& a, const QPointF& b, qreal eps = 0.5)
{
    return qAbs(a.x() - b.x()) <= eps && qAbs(a.y() - b.y()) > eps;
}

static int orthogonalDeltaSign(qreal delta, qreal eps = 0.5)
{
    if (delta > eps)
        return 1;
    if (delta < -eps)
        return -1;
    return 0;
}

static bool buildCompressedMonotonicStairRun(const QVector<QPointF>& pts,
                                             int startIndex,
                                             int endIndex,
                                             const QList<QGraphicsItem*>& obstacles,
                                             qreal eps,
                                             QVector<QPointF>& replacement)
{
    replacement.clear();
    if (startIndex < 0 || endIndex >= pts.size() || endIndex - startIndex < 3)
        return false;

    const QPointF start = pts[startIndex];
    const QPointF end = pts[endIndex];
    if (qAbs(start.x() - end.x()) <= eps || qAbs(start.y() - end.y()) <= eps)
        return false;

    const bool firstHorizontal = isHorizontalSceneSegment(pts[startIndex], pts[startIndex + 1], eps);
    const bool firstVertical = isVerticalSceneSegment(pts[startIndex], pts[startIndex + 1], eps);
    if (!firstHorizontal && !firstVertical)
        return false;

    bool previousHorizontal = firstHorizontal;
    bool seenHorizontal = false;
    bool seenVertical = false;
    int xSign = 0;
    int ySign = 0;
    for (int i = startIndex + 1; i <= endIndex; ++i)
    {
        const QPointF& a = pts[i - 1];
        const QPointF& b = pts[i];
        const bool horizontal = isHorizontalSceneSegment(a, b, eps);
        const bool vertical = isVerticalSceneSegment(a, b, eps);
        if (!horizontal && !vertical)
            return false;
        if (i > startIndex + 1 && horizontal == previousHorizontal)
            return false;
        previousHorizontal = horizontal;

        if (horizontal)
        {
            seenHorizontal = true;
            const int sign = orthogonalDeltaSign(b.x() - a.x(), eps);
            if (sign == 0 || (xSign != 0 && sign != xSign))
                return false;
            xSign = sign;
        }
        else
        {
            seenVertical = true;
            const int sign = orthogonalDeltaSign(b.y() - a.y(), eps);
            if (sign == 0 || (ySign != 0 && sign != ySign))
                return false;
            ySign = sign;
        }
    }

    if (!seenHorizontal || !seenVertical)
        return false;

    auto canUseCorner = [&](const QPointF& corner) {
        if (QLineF(start, corner).length() <= eps || QLineF(corner, end).length() <= eps)
            return false;
        LzAStar astar;
        return !astar.segmentIntersectsObstacles(start.toPoint(), corner.toPoint(), obstacles)
            && !astar.segmentIntersectsObstacles(corner.toPoint(), end.toPoint(), obstacles);
    };

    const QPointF preferredCorner = firstHorizontal ? QPointF(end.x(), start.y())
                                                    : QPointF(start.x(), end.y());
    const QPointF alternateCorner = firstHorizontal ? QPointF(start.x(), end.y())
                                                    : QPointF(end.x(), start.y());
    const QPointF chosenCorner = canUseCorner(preferredCorner) ? preferredCorner
                               : canUseCorner(alternateCorner) ? alternateCorner
                                                               : QPointF();
    if (chosenCorner.isNull() && !canUseCorner(chosenCorner))
        return false;

    replacement = {start, chosenCorner, end};
    sanitizeOrthogonalScenePath(replacement, eps);
    return replacement.size() <= 3;
}

static QVector<QPointF> compressMonotonicOrthogonalStairs(const QVector<QPointF>& scenePts,
                                                          const QList<QGraphicsItem*>& obstacles,
                                                          qreal eps = 0.5)
{
    QVector<QPointF> simplified = simplifiedOrthogonalScenePath(scenePts, eps);
    if (simplified.size() < 4)
        return simplified;

    QVector<QPointF> out;
    out.reserve(simplified.size());
    out.append(simplified.first());

    int startIndex = 0;
    while (startIndex < simplified.size() - 1)
    {
        int bestEnd = -1;
        QVector<QPointF> bestReplacement;
        for (int endIndex = startIndex + 3; endIndex < simplified.size(); ++endIndex)
        {
            QVector<QPointF> replacement;
            if (buildCompressedMonotonicStairRun(simplified, startIndex, endIndex,
                                                 obstacles, eps, replacement))
            {
                bestEnd = endIndex;
                bestReplacement = replacement;
            }
        }

        if (bestEnd > startIndex && bestReplacement.size() >= 2)
        {
            for (int i = 1; i < bestReplacement.size(); ++i)
            {
                if (QLineF(out.last(), bestReplacement[i]).length() > eps)
                    out.append(bestReplacement[i]);
            }
            startIndex = bestEnd;
            continue;
        }

        if (QLineF(out.last(), simplified[startIndex + 1]).length() > eps)
            out.append(simplified[startIndex + 1]);
        ++startIndex;
    }

    sanitizeOrthogonalScenePath(out, eps);
    return out;
}

static bool isMonotonicOrthogonalStairPath(const QVector<QPointF>& scenePts, qreal eps = 0.5)
{
    const QVector<QPointF> simplified = simplifiedOrthogonalScenePath(scenePts, eps);
    if (simplified.size() < 4)
        return false;

    bool previousHorizontal = false;
    bool previousValid = false;
    int xSign = 0;
    int ySign = 0;
    bool seenHorizontal = false;
    bool seenVertical = false;
    for (int i = 1; i < simplified.size(); ++i)
    {
        const QPointF a = simplified[i - 1];
        const QPointF b = simplified[i];
        const bool horizontal = isHorizontalSceneSegment(a, b, eps);
        const bool vertical = isVerticalSceneSegment(a, b, eps);
        if (!horizontal && !vertical)
            return false;
        if (previousValid && horizontal == previousHorizontal)
            return false;
        previousHorizontal = horizontal;
        previousValid = true;

        if (horizontal)
        {
            seenHorizontal = true;
            const int sign = orthogonalDeltaSign(b.x() - a.x(), eps);
            if (sign == 0 || (xSign != 0 && sign != xSign))
                return false;
            xSign = sign;
        }
        else
        {
            seenVertical = true;
            const int sign = orthogonalDeltaSign(b.y() - a.y(), eps);
            if (sign == 0 || (ySign != 0 && sign != ySign))
                return false;
            ySign = sign;
        }
    }
    return seenHorizontal && seenVertical;
}

static QVector<QPointF> rebuildManualMiddleViaObstacleBridge(const QVector<QPointF>& middlePts,
                                                             const QList<QGraphicsItem*>& obstacles,
                                                             qreal eps = 0.5)
{
    if (middlePts.size() < 4 || !isMonotonicOrthogonalStairPath(middlePts, eps))
        return {};

    QVector<QPointF> rebuilt;
    rebuilt.reserve(8);
    rebuilt.append(middlePts.first());
    const QPointF from = middlePts.first();
    const QPointF to = middlePts.last();
    const bool preferVerticalFirst = qAbs(to.x() - from.x()) < qAbs(to.y() - from.y());
    appendOrthogonalBridge(rebuilt, from, to, preferVerticalFirst, eps, obstacles, false);
    sanitizeOrthogonalScenePath(rebuilt, eps);
    if (rebuilt.size() >= 2 && rebuilt.size() < middlePts.size())
        return rebuilt;
    return {};
}

static QVector<QPointF> compressLongestMonotonicStairRunViaObstacleBridge(const QVector<QPointF>& scenePts,
                                                                          const QList<QGraphicsItem*>& obstacles,
                                                                          qreal eps = 0.5)
{
    QVector<QPointF> simplified = simplifiedOrthogonalScenePath(scenePts, eps);
    if (simplified.size() < 4)
        return simplified;

    int bestStart = -1;
    int bestEnd = -1;
    int bestCount = 0;
    for (int start = 0; start < simplified.size() - 3; ++start)
    {
        bool prevHorizontal = false;
        bool prevValid = false;
        bool seenHorizontal = false;
        bool seenVertical = false;
        int xSign = 0;
        int ySign = 0;
        for (int end = start + 1; end < simplified.size(); ++end)
        {
            const QPointF a = simplified[end - 1];
            const QPointF b = simplified[end];
            const bool horizontal = isHorizontalSceneSegment(a, b, eps);
            const bool vertical = isVerticalSceneSegment(a, b, eps);
            if (!horizontal && !vertical)
                break;
            if (prevValid && horizontal == prevHorizontal)
                break;
            prevHorizontal = horizontal;
            prevValid = true;

            if (horizontal)
            {
                seenHorizontal = true;
                const int sign = orthogonalDeltaSign(b.x() - a.x(), eps);
                if (sign == 0 || (xSign != 0 && sign != xSign))
                    break;
                xSign = sign;
            }
            else
            {
                seenVertical = true;
                const int sign = orthogonalDeltaSign(b.y() - a.y(), eps);
                if (sign == 0 || (ySign != 0 && sign != ySign))
                    break;
                ySign = sign;
            }

            const int pointCount = end - start + 1;
            if (seenHorizontal && seenVertical && pointCount >= 4 && pointCount > bestCount)
            {
                bestStart = start;
                bestEnd = end;
                bestCount = pointCount;
            }
        }
    }

    if (bestStart < 0 || bestEnd <= bestStart)
        return simplified;

    QVector<QPointF> run;
    run.reserve(bestEnd - bestStart + 1);
    for (int i = bestStart; i <= bestEnd; ++i)
        run.append(simplified[i]);

    const QVector<QPointF> rebuiltRun = rebuildManualMiddleViaObstacleBridge(run, obstacles, eps);
    if (rebuiltRun.isEmpty() || rebuiltRun.size() >= run.size())
        return simplified;

    QVector<QPointF> out;
    out.reserve(simplified.size() - run.size() + rebuiltRun.size());
    for (int i = 0; i < bestStart; ++i)
        out.append(simplified[i]);
    for (int i = 0; i < rebuiltRun.size(); ++i)
    {
        if (out.isEmpty() || QLineF(out.last(), rebuiltRun[i]).length() > eps)
            out.append(rebuiltRun[i]);
    }
    for (int i = bestEnd + 1; i < simplified.size(); ++i)
    {
        if (out.isEmpty() || QLineF(out.last(), simplified[i]).length() > eps)
            out.append(simplified[i]);
    }
    sanitizeOrthogonalScenePath(out, eps);
    return out;
}

static QVector<QPointF> simplifiedOrthogonalScenePathForLine(const QVector<QPointF>& scenePts,
                                                             const LzConnectLine* line,
                                                             qreal eps = 0.5)
{
    QVector<QPointF> merged = mergeOrthogonalStairSteps(scenePts, eps);
    QVector<QPointF> leadSafeBase = merged;
    if (!pathPreservesEndpointPortLeads(leadSafeBase, line, eps))
        leadSafeBase = simplifiedOrthogonalScenePath(scenePts, eps);

    if (!line || !line->scene())
    {
        QVector<QPointF> compressed = compressMonotonicOrthogonalStairs(merged, {}, eps);
        if (!pathPreservesEndpointPortLeads(compressed, line, eps))
            return leadSafeBase;
        return compressed;
    }

    const QPointF start = merged.first();
    const QPointF end = merged.last();
    const qreal pad = qMax<qreal>(qreal(ObstacleClearanceMargin * 6), qreal(StepSize * 4));
    QRectF queryRect(start, end);
    queryRect = queryRect.normalized().adjusted(-pad, -pad, pad, pad);
    if (QGraphicsItem* startItem = line->getStartItem())
    {
        const QRectF br = LzAStar::expandedObstacleSceneRect(startItem, 0);
        if (br.isValid())
            queryRect = queryRect.united(br);
    }
    if (QGraphicsItem* endItem = line->getEndItem())
    {
        const QRectF br = LzAStar::expandedObstacleSceneRect(endItem, 0);
        if (br.isValid())
            queryRect = queryRect.united(br);
    }

    LzScene* scene_ = dynamic_cast<LzScene*>(line->scene());
    const QList<QGraphicsItem*> routingObstacles =
        scene_ ? LzConnectLine::collectRoutingObstacles(line, scene_, queryRect) : QList<QGraphicsItem*>();
    QVector<QPointF> compressed = compressMonotonicOrthogonalStairs(merged, routingObstacles, eps);
    if (scenePathCrossesForeignObstacles(compressed, routingObstacles,
                                         line->getStartItem(), line->getEndItem()))
    {
        return simplifiedOrthogonalScenePath(scenePts, eps);
    }
    if (!pathPreservesEndpointPortLeads(compressed, line, eps))
        return leadSafeBase;
    return compressed;
}

static QVector<QPointF> normalizedInheritedAutoScenePath(const QVector<QPointF>& scenePts,
                                                         const LzConnectLine* line,
                                                         qreal eps = 0.5)
{
    const QVector<QPointF> normalized = simplifiedOrthogonalScenePathForLine(scenePts, line, eps);
    if (normalized.size() < scenePts.size())
        return normalized;

    const QVector<QPointF> fallback = simplifiedOrthogonalScenePathForLine(scenePts, nullptr, eps);
    if (fallback.size() < normalized.size())
        return fallback;
    return normalized;
}

static bool shouldPersistAsManualPath(const QVector<QPointF>& scenePts,
                                      ConnectorPathRoutingMode currentMode,
                                      const LzConnectLine* line,
                                      qreal eps = 0.5)
{
    if (currentMode == ConnectorPathRoutingMode::Manual)
        return true;
    if (scenePts.size() <= 2)
        return false;

    const QVector<QPointF> normalized = normalizedInheritedAutoScenePath(scenePts, line, eps);
    if (normalized.size() != scenePts.size())
        return true;

    for (int i = 0; i < scenePts.size(); ++i)
    {
        if (QLineF(scenePts.at(i), normalized.at(i)).length() > eps)
            return true;
    }
    return false;
}

static QVector<QPointF> preserveOrthogonalMiddleWithEndpoints(const QVector<QPointF>& originalScenePts,
                                                              const QPointF& startScene,
                                                              LzConnectPoint* startPort,
                                                              const QPointF& endScene,
                                                              LzConnectPoint* endPort,
                                                              const LzConnectLine* line = nullptr,
                                                              qreal eps = 0.5)
{
    QVector<QPointF> simplified = originalScenePts;
    if (simplified.size() < 2)
        return {};

    if (simplified.size() == 2)
    {
        QVector<QPointF> simple = {startScene, endScene};
        sanitizeOrthogonalScenePath(simple, eps);
        return simple;
    }

    auto trimStartLeadIndex = [&](const QVector<QPointF>& pts) -> int {
        if (!startPort || pts.size() < 3)
            return 1;
        const QPointF unit = outwardUnitForPort(startPort);
        const qreal minLead = qMax<qreal>(leadOutDistanceForPort(startPort, startScene) - eps, eps);
        qreal keptLead = 0.0;
        int idx = 1;
        while (idx < pts.size() - 1)
        {
            const QPointF prev = pts[idx - 1];
            const QPointF curr = pts[idx];
            const QPointF delta = curr - prev;
            bool alongLead = false;
            if (qAbs(unit.x()) > 0.5 && qAbs(delta.y()) <= eps)
                alongLead = delta.x() * unit.x() >= -eps;
            else if (qAbs(unit.y()) > 0.5 && qAbs(delta.x()) <= eps)
                alongLead = delta.y() * unit.y() >= -eps;
            if (!alongLead)
                break;
            keptLead += QLineF(prev, curr).length();
            if (keptLead >= minLead)
                break;
            ++idx;
        }
        return qMin(idx, pts.size() - 2);
    };

    auto trimEndLeadIndex = [&](const QVector<QPointF>& pts) -> int {
        if (!endPort || pts.size() < 3)
            return pts.size() - 2;
        const QPointF unit = outwardUnitForPort(endPort);
        const qreal minLead = qMax<qreal>(leadOutDistanceForPort(endPort, endScene) - eps, eps);
        qreal keptLead = 0.0;
        int idx = pts.size() - 2;
        while (idx > 0)
        {
            const QPointF curr = pts[idx];
            const QPointF next = pts[idx + 1];
            const QPointF delta = next - curr;
            bool alongLead = false;
            if (qAbs(unit.x()) > 0.5 && qAbs(delta.y()) <= eps)
                alongLead = delta.x() * unit.x() <= eps;
            else if (qAbs(unit.y()) > 0.5 && qAbs(delta.x()) <= eps)
                alongLead = delta.y() * unit.y() <= eps;
            if (!alongLead)
                break;
            keptLead += QLineF(curr, next).length();
            if (keptLead >= minLead)
                break;
            --idx;
        }
        return qMax(idx, 1);
    };

    const int coreStart = trimStartLeadIndex(simplified);
    const int coreEnd = trimEndLeadIndex(simplified);
    if (coreStart > coreEnd)
    {
        sanitizeOrthogonalScenePath(simplified, eps);
        if (pathPreservesPortLead(simplified, startPort, PortLeadEndpoint::Start, eps)
            && pathPreservesPortLead(simplified, endPort, PortLeadEndpoint::End, eps))
        {
            return simplified;
        }

        QVector<QPointF> simple = {startScene, endScene};
        sanitizeOrthogonalScenePath(simple, eps);
        return simple;
    }

    QVector<QPointF> middlePts;
    middlePts.reserve(coreEnd - coreStart + 1);
    for (int i = coreStart; i <= coreEnd; ++i)
        middlePts.append(simplified[i]);

    if (line && middlePts.size() >= 4 && line->scene())
    {
        const QPointF middleStart = middlePts.first();
        const QPointF middleEnd = middlePts.last();
        const qreal pad = qMax<qreal>(qreal(ObstacleClearanceMargin * 6), qreal(StepSize * 4));
        QRectF queryRect(middleStart, middleEnd);
        queryRect = queryRect.normalized().adjusted(-pad, -pad, pad, pad);
        if (QGraphicsItem* startItem = line->getStartItem())
        {
            const QRectF br = LzAStar::expandedObstacleSceneRect(startItem, 0);
            if (br.isValid())
                queryRect = queryRect.united(br);
        }
        if (QGraphicsItem* endItem = line->getEndItem())
        {
            const QRectF br = LzAStar::expandedObstacleSceneRect(endItem, 0);
            if (br.isValid())
                queryRect = queryRect.united(br);
        }

        if (LzScene* scene_ = dynamic_cast<LzScene*>(line->scene()))
        {
            const QList<QGraphicsItem*> routingObstacles =
                LzConnectLine::collectRoutingObstacles(line, scene_, queryRect);
            const QList<QGraphicsItem*> coreObstacles =
                coreObstaclesForLine(routingObstacles, line->getStartItem(), startPort,
                                     line->getEndItem(), endPort);
            const QVector<QPointF> rebuiltStairCore =
                compressLongestMonotonicStairRunViaObstacleBridge(middlePts, coreObstacles, eps);
            if (rebuiltStairCore.size() >= 2 && rebuiltStairCore.size() < middlePts.size())
                middlePts = rebuiltStairCore;
            const QVector<QPointF> rebuiltMiddle =
                rebuildManualMiddleViaObstacleBridge(middlePts, coreObstacles, eps);
            if (!rebuiltMiddle.isEmpty())
                middlePts = rebuiltMiddle;
            const QVector<QPointF> compressedMiddle =
                compressMonotonicOrthogonalStairs(middlePts, coreObstacles, eps);
            if (compressedMiddle.size() >= 2 && compressedMiddle.size() < middlePts.size())
                middlePts = compressedMiddle;
        }
    }

    QVector<QPointF> out;
    out.reserve(middlePts.size() + 4);
    appendPortExitFromAnchor(out, startScene, startPort, eps);
    const QPointF firstMiddle = middlePts.first();
    const QPointF startRoute = routingSceneForPort(startPort, startScene);
    const bool startPrefer = preferVerticalFirstForRoute(startRoute, firstMiddle, startPort);
    appendStartPortChain(out, startRoute, firstMiddle, startPrefer, eps, liteRoutingObstacles());
    if (QLineF(out.last(), firstMiddle).length() > eps)
        out.append(firstMiddle);

    for (int i = 1; i + 1 < middlePts.size(); ++i)
    {
        const QPointF middle = middlePts[i];
        if (QLineF(out.last(), middle).length() > eps)
            out.append(middle);
    }

    const QPointF lastMiddle = middlePts.last();
    if (QLineF(out.last(), lastMiddle).length() > eps)
        out.append(lastMiddle);
    appendPortApproachToAnchor(out, out.last(), endScene, endPort, eps, liteRoutingObstacles(), true);

    sanitizeOrthogonalScenePath(out, eps);
    if (!line)
        return out;

    const QVector<QPointF> compressed = simplifiedOrthogonalScenePathForLine(out, line, eps);
    if (compressed.size() >= 2 && compressed.size() <= out.size())
        return compressed;
    return out;
}

static QVector<QPointF> previewOrthogonalEndpointDragPath(const QVector<QPointF>& originalScenePts,
                                                          bool isStart,
                                                          const QPointF& draggedScenePos,
                                                          qreal eps = 0.5)
{
    QVector<QPointF> simplified = mergeOrthogonalStairSteps(originalScenePts, eps);
    if (simplified.size() < 2)
        return {};

    if (simplified.size() == 2)
    {
        QVector<QPointF> simple = simplified;
        if (isStart)
            simple.first() = draggedScenePos;
        else
            simple.last() = draggedScenePos;
        sanitizeOrthogonalScenePath(simple, eps);
        return simple;
    }

    QVector<QPointF> out = simplified;
    if (isStart)
    {
        out[0] = draggedScenePos;
        out[1] = LzConnectorAlgorithm::orthogonalCornerPosition(draggedScenePos, simplified[2],
                                                                simplified[1]);
    }
    else
    {
        const int n = out.size();
        out[n - 1] = draggedScenePos;
        out[n - 2] = LzConnectorAlgorithm::orthogonalCornerPosition(simplified[n - 3],
                                                                    draggedScenePos,
                                                                    simplified[n - 2]);
    }
    sanitizeOrthogonalScenePath(out, eps);
    return out;
}

static QList<QGraphicsItem*> filterObstaclesNearSceneSegment(const QList<QGraphicsItem*>& obstacles,
                                                             const QPointF& a, const QPointF& b,
                                                             qreal padPx);

/** 交互收尾：端口链 + L 形 + 矩形绕障（不跑 Quick A*，避免卡死） */
static QVector<QPointF> buildLiteOrthogonalScenePath(const QPointF& anchorS, const QPointF& anchorE,
                                                     LzConnectPoint* startPort, LzConnectPoint* endPort,
                                                     qreal eps, LzConnectLine* line, bool useObstacles,
                                                     bool allowQuickAStar)
{
    const QPointF startRoute = routingSceneForPort(startPort, anchorS);
    const QPointF endRoute = routingSceneForPort(endPort, anchorE);

    QList<QGraphicsItem*> coreObstacles;
    QList<QGraphicsItem*> bridgeObstacles;
    if (useObstacles && line && line->scene())
    {
        const qreal routeQueryPad = qMax<qreal>(qreal(ObstacleClearanceMargin * 6), qreal(StepSize * 4));
        QRectF routeQueryRect(startRoute, endRoute);
        routeQueryRect = routeQueryRect.normalized().adjusted(-routeQueryPad, -routeQueryPad,
                                                              routeQueryPad, routeQueryPad);
        if (QGraphicsItem* startItem = line->getStartItem())
        {
            const QRectF br = LzAStar::expandedObstacleSceneRect(startItem, 0);
            if (br.isValid())
                routeQueryRect = routeQueryRect.united(br);
        }
        if (QGraphicsItem* endItem = line->getEndItem())
        {
            const QRectF br = LzAStar::expandedObstacleSceneRect(endItem, 0);
            if (br.isValid())
                routeQueryRect = routeQueryRect.united(br);
        }
        if (LzScene* scene_ = dynamic_cast<LzScene*>(line->scene()))
        {
            QList<QGraphicsItem*> routingObstacles =
                LzConnectLine::collectRoutingObstacles(line, scene_, routeQueryRect);
            routingObstacles =
                filterObstaclesNearSceneSegment(routingObstacles, startRoute, endRoute, routeQueryPad);
            coreObstacles =
                coreObstaclesForLine(routingObstacles, line->getStartItem(), line->startConnectPort(),
                                     line->getEndItem(), line->endConnectPort());
            bridgeObstacles =
                bridgeObstaclesForLine(routingObstacles, line->getStartItem(), line->getEndItem());
        }
    }
    else
    {
        coreObstacles = liteRoutingObstacles();
        bridgeObstacles = liteRoutingObstacles();
    }

    QVector<QPointF> scenePts;
    appendPortExitFromAnchor(scenePts, anchorS, startPort, eps);
    const bool preferVerticalFirst = preferVerticalFirstForRoute(startRoute, endRoute, startPort);
    appendOrthogonalBridge(scenePts, startRoute, endRoute, preferVerticalFirst, eps, coreObstacles,
                           allowQuickAStar);
    appendPortApproachToAnchor(scenePts,
                               scenePts.isEmpty() ? endRoute : scenePts.last(),
                               anchorE,
                               endPort,
                               eps,
                               bridgeObstacles,
                               allowQuickAStar);
    sanitizeOrthogonalScenePath(scenePts, eps);
    const QPointF visualEnd = visualEndpointSceneForPort(endPort, anchorE);
    if (!scenePts.isEmpty() && QLineF(scenePts.last(), visualEnd).length() > eps)
        scenePts.append(visualEnd);
    return scenePts;
}

static QList<QGraphicsItem*> filterObstaclesNearSceneSegment(const QList<QGraphicsItem*>& obstacles,
                                                             const QPointF& a, const QPointF& b,
                                                             qreal padPx)
{
    QRectF box(a, b);
    box = box.normalized().adjusted(-padPx, -padPx, padPx, padPx);
    QList<QGraphicsItem*> out;
    out.reserve(obstacles.size());
    for (QGraphicsItem* it : obstacles)
    {
        if (!it || !it->isVisible())
            continue;
        const QRectF br = LzAStar::expandedObstacleSceneRect(it, 0);
        if (br.isValid() && br.intersects(box))
            out.append(it);
    }
    return out;
}

// 终点：折线尾点 prev 接到 route 点；避免在终点图元中心高度横穿
static void appendEndPortChain(QVector<QPointF>& out, const QPointF& prev, const QPointF& routePt,
                               bool lastAStarSegVertical, qreal eps,
                               const QList<QGraphicsItem*>& obstacles)
{
    bool preferVerticalFirst = lastAStarSegVertical;
    if (!obstacles.isEmpty())
    {
        LzAStar astar;
        const QPointF horizMid(routePt.x(), prev.y());
        const QPointF vertMid(prev.x(), routePt.y());
        const bool horizCrosses = astar.segmentIntersectsObstacles(prev.toPoint(), horizMid.toPoint(), obstacles)
            || astar.segmentIntersectsObstacles(horizMid.toPoint(), routePt.toPoint(), obstacles);
        const bool vertCrosses = astar.segmentIntersectsObstacles(prev.toPoint(), vertMid.toPoint(), obstacles)
            || astar.segmentIntersectsObstacles(vertMid.toPoint(), routePt.toPoint(), obstacles);
        // preferVerticalFirst=false → 先竖后横；(routePt.x,prev.y) 为水平穿障段，应选先竖后横
        if (horizCrosses && !vertCrosses)
            preferVerticalFirst = false;
        else if (vertCrosses && !horizCrosses)
            preferVerticalFirst = true;
        else if (horizCrosses && vertCrosses)
            preferVerticalFirst = qAbs(prev.x() - routePt.x()) >= qAbs(prev.y() - routePt.y());
    }
    appendOrthogonalBridge(out, prev, routePt, preferVerticalFirst, eps, obstacles, true);
}

class LzConnectLinePrivate
{
    Q_DECLARE_PUBLIC(LzConnectLine)
public:
    LzConnectLinePrivate(LzConnectLine* parent)
        :q_ptr(parent)
    {

    }
    QPolygonF points;
    QPolygonF initialPoints;
    QGraphicsItem* startItem = nullptr;
    QGraphicsItem* endItem = nullptr;
    QMetaObject::Connection startItemDestroyedConnection;
    QMetaObject::Connection endItemDestroyedConnection;
    LzConnectPoint* startConnectPort = nullptr;
    LzConnectPoint* endConnectPort = nullptr;
    
    CONNECTION_STRATEGY linkMode = lzDefaultConnectLineStrategy();
    CONNECT_DIRECTION direction;//方向,是否实时更新
    LzConnectLine* q_ptr;

    /** 拖动连接图元时延迟执行完整 A*，避免每帧跑路径规划导致卡顿 */
    QTimer* deferredPathTimer = nullptr;
    bool recomputingPath = false;
    QVector<QPointF> lastRecomputedScenePath;
    QVector<QPointF> interactiveBaseScenePath;

    /** draw.io 风格拐角样式（仅渲染） */
    LzConnectLine::CornerStyle cornerStyle = LzConnectLine::Sharp;
    LzConnectLine::ArrowHeadStyle arrowHead = LzConnectLine::ArrowNone;
    bool lineJumpEnabled = false;

    /** 路由策略：网格 A* 或可见性图（默认 GridAStar） */
    LzConnectorAlgorithm::ConnectorRoutingStrategy routingStrategy = LzConnectorAlgorithm::GridAStar;

    /** 路径优先模式：最短 / 少拐弯 / 平衡（默认） */
    LzConnectorAlgorithm::PathPriorityMode pathPriorityMode = LzConnectorAlgorithm::PathBalanced;

    QString edgeStableId;
    TopologyRelationType relationType = TopologyRelationType::Physical;
    QString startNodeStableId;
    QString endNodeStableId;
    QString startPortKey;
    QString endPortKey;
    bool topologyRelationLink = false;

    ConnectorPathRoutingMode pathRoutingMode = ConnectorPathRoutingMode::Auto;
};


LzConnectLine::LzConnectLine(QGraphicsItem* parent)
    :d_ptr(new LzConnectLinePrivate(this))
{
    setZValue(LzConnectLineLayerZ);
    setUseBrush(false);
    handleStruct.handles.reserve(LEFT);
    for (auto item : handleStruct.handles)
    {
        delete item;
    }
    handleStruct.handles.clear();
}

LzConnectLine::LzConnectLine(const QRectF& pos,QGraphicsItem* parent)
    :d_ptr(new LzConnectLinePrivate(this))
{
    setZValue(LzConnectLineLayerZ);
    setUseBrush(false);
    handleStruct.handles.reserve(LEFT);
    for (auto item : handleStruct.handles)
    {
        delete item;
    }
    handleStruct.handles.clear();
    setUsePen(LzAttributeManager::usePen_);
    setItemPenColor(LzAttributeManager::penColor_);
    setItemPenWidth(LzAttributeManager::penWidth_);
    setItemPenStyle(LzAttributeManager::penStyle_);

    setUseFont(LzAttributeManager::useFont_);
}

LzConnectLine::~LzConnectLine()
{
    Q_D(LzConnectLine);
    if (d->deferredPathTimer)
        d->deferredPathTimer->stop();
    QObject::disconnect(d->startItemDestroyedConnection);
    QObject::disconnect(d->endItemDestroyedConnection);
    detachLineFromEndpointShape(d->startItem, this);
    detachLineFromEndpointShape(d->endItem, this);
}

QPainterPath LzConnectLine::shape() const
{
    QPainterPath path = buildPathFromPoints(d_ptr->points, d_ptr->cornerStyle);
    if (path.isEmpty() && d_ptr->points.size() >= 2)
    {
        path.moveTo(d_ptr->points.first());
        for (int i = 1; i < d_ptr->points.size(); ++i)
            path.lineTo(d_ptr->points.at(i));
    }
    QPen pen;
    pen.setColor(itemPenColor);
    pen.setWidth(itemPenWidth);
    pen.setStyle(Qt::PenStyle(itemPenStyle));
    // draw.io 风格：扩大可点击区域，便于选中连接线（至少 12 像素宽）
    const qreal minHitWidth = 16.0;
    if (itemPenWidth < minHitWidth)
    {
        QPen widePen(pen);
        widePen.setWidthF(minHitWidth);
        return shapeFromPath(path, widePen);
    }
    return shapeFromPath(path, pen);
}

QRectF LzConnectLine::boundingRect() const
{
    return shape().controlPointRect();
}

void LzConnectLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_D(LzConnectLine);
    prepare(painter);
    if (d->points.size() < 2)
        return;

    const QPolygonF scenePts = pathInSceneCoords();

    if (d->lineJumpEnabled && scene())
    {
        QList<QVector<QPointF>> others;
        for (QGraphicsItem* item : scene()->items())
        {
            if (!item || item == this || !item->isVisible())
                continue;
            if (item->type() != LzConnectLine::Type)
                continue;
            const auto* other = static_cast<const LzConnectLine*>(item);
            others.append(scenePolylineForLine(other));
        }
        painter->setBrush(Qt::NoBrush);
        drawPolylineWithJumps(painter, scenePts, others);
    }
    else
    {
        painter->setBrush(Qt::NoBrush);
        QPainterPath path = buildPathFromPoints(d->points, d->cornerStyle);
        if (!path.isEmpty())
            painter->drawPath(path);
        else
            painter->drawPolyline(d->points);
    }

    painter->setBrush(Qt::NoBrush);
    drawArrowHeads(painter, scenePts, d->arrowHead, itemPenWidth);
}

void LzConnectLine::updateCoordinate()
{
    Q_D(LzConnectLine);
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

bool LzConnectLine::saveXml(QDomElement* g)
{
    if (!g)
        return false;
    Q_D(LzConnectLine);
    QVector<QPointF> scenePts = pathInSceneCoords();
    const ConnectorPathRoutingMode savedRoutingMode =
        shouldPersistAsManualPath(scenePts, d->pathRoutingMode, this)
            ? ConnectorPathRoutingMode::Manual
            : ConnectorPathRoutingMode::Auto;
    if (scenePts.size() >= 2)
    {
        if (LzConnectPoint* port = resolveConnectPortForSave(d->startItem, d->startConnectPort, scenePts.first()))
        {
            d->startConnectPort = port;
            scenePts[0] = port->connectionCenterInScene();
        }
        if (LzConnectPoint* port = resolveConnectPortForSave(d->endItem, d->endConnectPort, scenePts.last()))
        {
            d->endConnectPort = port;
            scenePts[scenePts.size() - 1] = port->connectionCenterInScene();
        }
    }
    syncTopologyEndpointsFromItems();
    ensureTopologyEdgeStableId();
    if (!LzItem::saveXml(g))
        return false;
    QDomElement top = g->ownerDocument().createElement(QStringLiteral("topology"));
    top.setAttribute(QStringLiteral("edgeStableId"), d->edgeStableId);
    top.setAttribute(QStringLiteral("relation"), QString::number(static_cast<int>(d->relationType)));
    top.setAttribute(QStringLiteral("startNode"), d->startNodeStableId);
    top.setAttribute(QStringLiteral("endNode"), d->endNodeStableId);
    top.setAttribute(QStringLiteral("startPort"), d->startPortKey);
    top.setAttribute(QStringLiteral("endPort"), d->endPortKey);
    top.setAttribute(QStringLiteral("pathRoutingMode"),
                     QString::number(static_cast<int>(savedRoutingMode)));
    top.setAttribute(QStringLiteral("cornerStyle"), QString::number(static_cast<int>(d->cornerStyle)));
    top.setAttribute(QStringLiteral("routingStrategy"), QString::number(static_cast<int>(d->routingStrategy)));
    top.setAttribute(QStringLiteral("pathPriority"), QString::number(static_cast<int>(d->pathPriorityMode)));
    top.setAttribute(QStringLiteral("arrowHead"), QString::number(static_cast<int>(d->arrowHead)));
    top.setAttribute(QStringLiteral("lineJump"), d->lineJumpEnabled ? QStringLiteral("1") : QStringLiteral("0"));
    top.setAttribute(QStringLiteral("linkMode"), QString::number(static_cast<int>(d->linkMode)));
    top.setAttribute(QStringLiteral("ptnum"), scenePts.size());
    for (int i = 0; i < scenePts.size(); ++i)
    {
        top.setAttribute(QStringLiteral("ptx_%1").arg(i), scenePts.at(i).x());
        top.setAttribute(QStringLiteral("pty_%1").arg(i), scenePts.at(i).y());
    }
    g->appendChild(top);
    return true;
}

bool LzConnectLine::loadXml(QDomElement* g)
{
    if (!LzItem::loadXml(g))
        return false;
    Q_D(LzConnectLine);
    const QDomElement top = g->firstChildElement(QStringLiteral("topology"));
    if (!top.isNull()) {
        d->edgeStableId = top.attribute(QStringLiteral("edgeStableId"));
        d->relationType = topologyRelationFromInt(top.attribute(QStringLiteral("relation")).toInt());
        d->startNodeStableId = top.attribute(QStringLiteral("startNode"));
        d->endNodeStableId = top.attribute(QStringLiteral("endNode"));
        d->startPortKey = top.attribute(QStringLiteral("startPort"));
        d->endPortKey = top.attribute(QStringLiteral("endPort"));
        d->pathRoutingMode = top.attribute(QStringLiteral("pathRoutingMode")).toInt() == 1
            ? ConnectorPathRoutingMode::Manual
            : ConnectorPathRoutingMode::Auto;
        if (top.hasAttribute(QStringLiteral("cornerStyle")))
            d->cornerStyle = static_cast<CornerStyle>(
                qBound(0, top.attribute(QStringLiteral("cornerStyle")).toInt(), static_cast<int>(Curved)));
        if (top.hasAttribute(QStringLiteral("routingStrategy")))
            d->routingStrategy = static_cast<LzConnectorAlgorithm::ConnectorRoutingStrategy>(
                qBound(0, top.attribute(QStringLiteral("routingStrategy")).toInt(), 1));
        if (top.hasAttribute(QStringLiteral("pathPriority")))
            d->pathPriorityMode = static_cast<LzConnectorAlgorithm::PathPriorityMode>(
                qBound(0, top.attribute(QStringLiteral("pathPriority")).toInt(), 2));
        if (top.hasAttribute(QStringLiteral("arrowHead")))
            d->arrowHead = static_cast<ArrowHeadStyle>(
                qBound(0, top.attribute(QStringLiteral("arrowHead")).toInt(), static_cast<int>(ArrowBoth)));
        if (top.hasAttribute(QStringLiteral("lineJump")))
            d->lineJumpEnabled = top.attribute(QStringLiteral("lineJump")) == QLatin1String("1");
        if (top.hasAttribute(QStringLiteral("linkMode")))
            d->linkMode = static_cast<CONNECTION_STRATEGY>(
                qBound(0, top.attribute(QStringLiteral("linkMode")).toInt(),
                       static_cast<int>(OBSTACLE_AVOIDANCE_CONNECTION)));

        const int n = top.attribute(QStringLiteral("ptnum")).toInt();
        if (n >= 2)
        {
            QVector<QPointF> scenePts;
            scenePts.reserve(n);
            for (int i = 0; i < n; ++i)
            {
                const qreal x = top.attribute(QStringLiteral("ptx_%1").arg(i)).toDouble();
                const qreal y = top.attribute(QStringLiteral("pty_%1").arg(i)).toDouble();
                scenePts.append(QPointF(x, y));
            }
            applyScenePathExact(scenePts, d->pathRoutingMode);
        }
    }

    if (d->points.size() < 2)
    {
        const QVector<QPointF> fallbackScenePts = {
            QPointF(0.0, 0.0),
            QPointF(qMax<qreal>(itemWidth, 1.0), qMax<qreal>(itemHeight, 1.0))
        };
        applyScenePathExact(fallbackScenePts, d->pathRoutingMode);
    }
    return true;
}

QString LzConnectLine::className() const
{
    return "LzConnectLine";
}

QString LzConnectLine::shapeName() const
{
    return "connectline";
}

void LzConnectLine::updateHandles()
{
    Q_D(LzConnectLine);
    const bool showHandles = isSelected() && !parentItem();
    const HANDLE_STATE handleState = showHandles ? HANDLE_ACTIVE : HANDLE_OFF;
    for (int i = 0; i < d->points.size(); ++i)
    {
        handleStruct.handles[i]->move(d->points[i].x(), d->points[i].y());
        handleStruct.handles[i]->setState(handleState);
    }
}

void LzConnectLine::setStartConnectPort(LzConnectPoint* port)
{
    d_ptr->startConnectPort = port;
    const int ordinal = connectPointOrdinalOnItem(d_ptr->startItem, port);
    d_ptr->startPortKey = ordinal >= 0 ? QString::number(ordinal) : QString();
}

void LzConnectLine::setEndConnectPort(LzConnectPoint* port)
{
    d_ptr->endConnectPort = port;
    const int ordinal = connectPointOrdinalOnItem(d_ptr->endItem, port);
    d_ptr->endPortKey = ordinal >= 0 ? QString::number(ordinal) : QString();
}

LzConnectPoint* LzConnectLine::startConnectPort() const
{
    return d_ptr->startConnectPort;
}

LzConnectPoint* LzConnectLine::endConnectPort() const
{
    return d_ptr->endConnectPort;
}

void LzConnectLine::syncEndpointPositionsFromAttachedItems()
{
    Q_D(LzConnectLine);
    if (d->points.isEmpty())
        return;

    QPointF startScene = mapToScene(d->points.first());
    QPointF endScene = mapToScene(d->points.last());
    if (d->startItem)
        startScene = endpointSceneForItem(d->startItem, d->startConnectPort, startScene);
    if (d->endItem)
        endScene = endpointSceneForItem(d->endItem, d->endConnectPort, endScene);

    setPos(0, 0);
    d->points[0] = mapFromScene(startScene);
    d->points[d->points.size() - 1] = mapFromScene(endScene);
}

void LzConnectLine::applyScenePathExact(const QVector<QPointF>& scenePoints,
                                        ConnectorPathRoutingMode routingMode)
{
    Q_D(LzConnectLine);
    cancelDeferredPathRecompute();
    if (scenePoints.size() < 2)
        return;
    setPathRoutingMode(routingMode);
    applyScenePathFast(scenePoints);
    d->initialPoints = d->points;
    d->lastRecomputedScenePath = scenePoints;
    updateHandles();
}

void LzConnectLine::snapAttachedBusbarEndpointsToPorts()
{
    Q_D(LzConnectLine);
    if (d->points.size() < 2)
        return;

    QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() < 2)
        return;

    bool changed = false;
    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(d->startItem))
    {
        LzConnectPoint* port = d->startConnectPort;
        if (!port)
            port = bus->findConnectPortNearScene(scenePts.first());
        if (port)
        {
            if (!d->startConnectPort)
                setStartConnectPort(port);
            const QPointF anchor = port->connectionCenterInScene();
            if (QLineF(scenePts.first(), anchor).length() > 0.5)
            {
                scenePts[0] = anchor;
                changed = true;
            }
            bus->addConnect(this);
        }
    }
    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(d->endItem))
    {
        LzConnectPoint* port = d->endConnectPort;
        if (!port)
            port = bus->findConnectPortNearScene(scenePts.last());
        if (port)
        {
            if (!d->endConnectPort)
                setEndConnectPort(port);
            const QPointF anchor = port->connectionCenterInScene();
            if (QLineF(scenePts.last(), anchor).length() > 0.5)
            {
                scenePts[scenePts.size() - 1] = anchor;
                changed = true;
            }
            bus->addConnect(this);
        }
    }

    if (changed)
        applyScenePathExact(scenePts, d->pathRoutingMode);
}

void LzConnectLine::restorePathSnapshot(const QVector<QPointF>& sceneSnapshot,
                                        ConnectorPathRoutingMode routingMode)
{
    Q_D(LzConnectLine);
    cancelDeferredPathRecompute();
    if (sceneSnapshot.size() < 2)
        return;

    QVector<QPointF> pts = sceneSnapshot;
    const QPointF startAnchor = d->startItem
        ? endpointSceneForItem(d->startItem, d->startConnectPort, pts.first())
        : pts.first();
    const QPointF endAnchor = d->endItem
        ? endpointSceneForItem(d->endItem, d->endConnectPort, pts.last())
        : pts.last();

    if (pts.size() > 2)
        pts = preserveOrthogonalMiddleWithEndpoints(
            pts, startAnchor, d->startConnectPort, endAnchor, d->endConnectPort, this, 0.5);
    else
        pts = {startAnchor, endAnchor};

    sanitizeOrthogonalScenePath(pts, 0.5);

    setPathRoutingMode(routingMode);
    applyScenePathFast(pts);
    if (pts.size() >= 2)
    {
        if (!connectPortStillOnItem(d->startConnectPort, d->startItem))
            d->startConnectPort = resolveConnectPortForSave(d->startItem, d->startConnectPort, pts.first());
        if (!connectPortStillOnItem(d->endConnectPort, d->endItem))
            d->endConnectPort = resolveConnectPortForSave(d->endItem, d->endConnectPort, pts.last());
    }
    syncTopologyEndpointsFromItems();
    d->initialPoints = d->points;
    d->lastRecomputedScenePath = pts;
    d->interactiveBaseScenePath = pts;
    updateHandles();
}

void LzConnectLine::preservePathAfterHierarchyChange()
{
    Q_D(LzConnectLine);
    // Hierarchy changes should preserve the path users currently see on canvas.
    // Cached recompute snapshots may lag behind a just-edited manual path.
    QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() < 2)
        scenePts = d->lastRecomputedScenePath;
    restorePathSnapshot(scenePts, d->pathRoutingMode);
}

void LzConnectLine::refreshEndpointsFromAttachedItems()
{
    if (d_ptr->points.isEmpty())
        return;
    cancelDeferredPathRecompute();
    finalizePathAfterItemDrag();
}

void LzConnectLine::registerEndpointAttachments()
{
    Q_D(LzConnectLine);
    const QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() >= 2)
    {
        if (!connectPortStillOnItem(d->startConnectPort, d->startItem))
            d->startConnectPort = resolveConnectPortForSave(d->startItem, d->startConnectPort, scenePts.first());
        if (!connectPortStillOnItem(d->endConnectPort, d->endItem))
            d->endConnectPort = resolveConnectPortForSave(d->endItem, d->endConnectPort, scenePts.last());
    }
    attachLineToEndpointShape(d->startItem, this);
    attachLineToEndpointShape(d->endItem, this);
    syncTopologyEndpointsFromItems();
}

void LzConnectLine::reattachToEndpointShapes(bool refreshPath)
{
    Q_D(LzConnectLine);
    const QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() >= 2)
    {
        if (!connectPortStillOnItem(d->startConnectPort, d->startItem))
            d->startConnectPort = resolveConnectPortForSave(d->startItem, d->startConnectPort, scenePts.first());
        if (!connectPortStillOnItem(d->endConnectPort, d->endItem))
            d->endConnectPort = resolveConnectPortForSave(d->endItem, d->endConnectPort, scenePts.last());
    }
    attachLineToEndpointShape(d->startItem, this);
    attachLineToEndpointShape(d->endItem, this);

    if (refreshPath)
        refreshEndpointsFromAttachedItems();
    else
        syncEndpointPositionsFromAttachedItems();

    syncTopologyEndpointsFromItems();
}

ConnectorPathRoutingMode LzConnectLine::pathRoutingMode() const
{
    return d_ptr->pathRoutingMode;
}

void LzConnectLine::setPathRoutingMode(ConnectorPathRoutingMode mode)
{
    d_ptr->pathRoutingMode = mode;
}

void LzConnectLine::markPathAsUserEdited(bool normalizeInheritedPath)
{
    Q_D(LzConnectLine);
    if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
        return;

    d->pathRoutingMode = ConnectorPathRoutingMode::Manual;
    if (!normalizeInheritedPath)
        return;

    QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() < 3)
        return;

    const QVector<QPointF> normalized = normalizedInheritedAutoScenePath(scenePts, this, 0.5);
    if (normalized.size() < 2 || normalized.size() == scenePts.size())
        return;

    applyScenePathFast(normalized);
    d->lastRecomputedScenePath = normalized;
    d->initialPoints = d->points;
    updateHandles();
}

void LzConnectLine::syncEndpointsPreservingUserPath()
{
    Q_D(LzConnectLine);
    if (d->points.size() < 2)
        return;

    const QVector<QPointF> originalScenePts = pathInSceneCoords();

    syncEndpointPositionsFromAttachedItems();

    QVector<QPointF> scenePts;
    const QPointF startScene = mapToScene(d->points.first());
    const QPointF endScene = mapToScene(d->points.last());
    if (originalScenePts.size() > 2)
        scenePts = preserveOrthogonalMiddleWithEndpoints(
            originalScenePts, startScene, d->startConnectPort, endScene, d->endConnectPort, this, 0.5);
    else
        scenePts = {startScene, endScene};
    sanitizeOrthogonalScenePath(scenePts, 0.5);
    applyScenePathFast(scenePts);
    d->lastRecomputedScenePath = scenePts;
    d->initialPoints = d->points;
    updateHandles();
}

void LzConnectLine::translateManualPathWithAttachedItems()
{
    Q_D(LzConnectLine);
    if (d->points.size() < 2)
        return;

    QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() < 2)
        return;

    const QPointF oldStart = scenePts.first();
    const QPointF oldEnd = scenePts.last();
    QPointF newStart = oldStart;
    QPointF newEnd = oldEnd;
    if (d->startItem)
        newStart = endpointSceneForItem(d->startItem, d->startConnectPort, oldStart);
    if (d->endItem)
        newEnd = endpointSceneForItem(d->endItem, d->endConnectPort, oldEnd);

    QPointF delta;
    if (d->startItem && d->endItem
        && QLineF(newStart - oldStart, newEnd - oldEnd).length() <= 0.5)
    {
        delta = newStart - oldStart;
    }
    else if (d->startItem)
    {
        delta = newStart - oldStart;
    }
    else if (d->endItem)
    {
        delta = newEnd - oldEnd;
    }
    else
    {
        return;
    }

    if (QLineF(QPointF(), delta).length() <= 0.5)
        return;

    for (QPointF& pt : scenePts)
        pt += delta;

    if (!scenePts.isEmpty())
        scenePts[0] = newStart;
    if (scenePts.size() >= 2)
        scenePts[scenePts.size() - 1] = newEnd;

    sanitizeOrthogonalScenePath(scenePts, 0.5);
    applyScenePathFast(scenePts);
    d->lastRecomputedScenePath = scenePts;
    d->initialPoints = d->points;
    updateHandles();
}

void LzConnectLine::finalizePathAfterItemDrag()
{
    Q_D(LzConnectLine);
    d->interactiveBaseScenePath.clear();
    if (shouldMoveConnectLineAsSelectedBlock(this))
    {
        restorePathSnapshot(pathInSceneCoords(), d->pathRoutingMode);
        return;
    }
    if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
    {
        syncEndpointsPreservingUserPath();
        return;
    }

    recomputePathFromCurrentEndpoints(false);
}

void LzConnectLine::cancelDeferredPathRecompute()
{
    Q_D(LzConnectLine);
    if (d->deferredPathTimer)
        d->deferredPathTimer->stop();
    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
        sc->cancelConnectLinePathRecompute(this);
}

void LzConnectLine::setStartItem(QGraphicsItem* item)
{
    if (d_ptr->startItem == item)
        return;
    QObject::disconnect(d_ptr->startItemDestroyedConnection);
    detachLineFromEndpointShape(d_ptr->startItem, this);
    d_ptr->startItem = item;
    attachLineToEndpointShape(d_ptr->startItem, this);
    if (QObject* endpointObject = dynamic_cast<QObject*>(item))
    {
        d_ptr->startItemDestroyedConnection = QObject::connect(endpointObject, &QObject::destroyed, this,
            [this](QObject*) {
                d_ptr->startItem = nullptr;
                d_ptr->startConnectPort = nullptr;
                syncTopologyEndpointsFromItems();
                if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
                    sc->scheduleTopologyRebuild();
            });
    }
    else
    {
        d_ptr->startItemDestroyedConnection = QMetaObject::Connection();
    }
    if (!item || !connectPortStillOnItem(d_ptr->startConnectPort, item))
        d_ptr->startConnectPort = nullptr;
    syncTopologyEndpointsFromItems();
    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

QGraphicsItem* LzConnectLine::getStartItem()
{
    return d_ptr->startItem;
}

QGraphicsItem* LzConnectLine::getStartItem() const
{
    return d_ptr->startItem;
}

void LzConnectLine::setEndItem(QGraphicsItem* item)
{
    if (d_ptr->endItem == item)
        return;
    QObject::disconnect(d_ptr->endItemDestroyedConnection);
    detachLineFromEndpointShape(d_ptr->endItem, this);
    d_ptr->endItem = item;
    attachLineToEndpointShape(d_ptr->endItem, this);
    if (QObject* endpointObject = dynamic_cast<QObject*>(item))
    {
        d_ptr->endItemDestroyedConnection = QObject::connect(endpointObject, &QObject::destroyed, this,
            [this](QObject*) {
                d_ptr->endItem = nullptr;
                d_ptr->endConnectPort = nullptr;
                syncTopologyEndpointsFromItems();
                if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
                    sc->scheduleTopologyRebuild();
            });
    }
    else
    {
        d_ptr->endItemDestroyedConnection = QMetaObject::Connection();
    }
    if (!item || !connectPortStillOnItem(d_ptr->endConnectPort, item))
        d_ptr->endConnectPort = nullptr;
    syncTopologyEndpointsFromItems();
    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

QGraphicsItem* LzConnectLine::getEndItem()
{
    return d_ptr->endItem;
}

QGraphicsItem* LzConnectLine::getEndItem() const
{
    return d_ptr->endItem;
}

void LzConnectLine::updatePosition(QGraphicsItem* item, const QPointF& delta)
{
    Q_D(LzConnectLine);
    if (item != d_ptr->startItem && item != d_ptr->endItem)
        return;
    if (shouldMoveConnectLineAsSelectedBlock(this))
        return;

    QPointF startScene = mapToScene(d->points.first());
    QPointF endScene = mapToScene(d->points.last());
    if (item == d_ptr->startItem)
    {
        startScene += delta;
        startScene = endpointSceneForItem(d_ptr->startItem, d_ptr->startConnectPort, startScene);
    }
    else
    {
        endScene += delta;
        endScene = endpointSceneForItem(d_ptr->endItem, d_ptr->endConnectPort, endScene);
    }

    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
    {
        if (sc->isInteractiveTransformActive())
        {
            QVector<QPointF> previewScenePts;
            if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
            {
                if (d->interactiveBaseScenePath.size() < 2)
                    d->interactiveBaseScenePath = pathInSceneCoords();
                previewScenePts =
                    preserveOrthogonalMiddleWithEndpoints(d->interactiveBaseScenePath,
                                                          startScene,
                                                          d->startConnectPort,
                                                          endScene,
                                                          d->endConnectPort,
                                                          this,
                                                          0.5);
            }
            else
            {
                const QPointF anchorS = endpointSceneForItem(d_ptr->startItem, d_ptr->startConnectPort, startScene);
                const QPointF anchorE = endpointSceneForItem(d_ptr->endItem, d_ptr->endConnectPort, endScene);
                previewScenePts = simplifiedOrthogonalScenePathForLine(
                    buildLiteOrthogonalScenePath(
                        anchorS, anchorE, d_ptr->startConnectPort, d_ptr->endConnectPort, 0.5, this, true, false),
                    this,
                    0.5);
            }
            if (previewScenePts.size() >= 2)
            {
                applyScenePathFast(previewScenePts);
                d->lastRecomputedScenePath = previewScenePts;
                d->interactiveBaseScenePath = previewScenePts;
            }
            return;
        }
    }
    d->interactiveBaseScenePath.clear();

    if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
    {
        Q_UNUSED(delta);
        syncEndpointsPreservingUserPath();
        return;
    }

    const QPointF anchorS = endpointSceneForItem(d_ptr->startItem, d_ptr->startConnectPort, startScene);
    const QPointF anchorE = endpointSceneForItem(d_ptr->endItem, d_ptr->endConnectPort, endScene);
    static const qreal kSnapEps = 1.0;
    const QVector<QPointF> scenePts = simplifiedOrthogonalScenePathForLine(
        buildLiteOrthogonalScenePath(
            anchorS, anchorE, d_ptr->startConnectPort, d_ptr->endConnectPort, kSnapEps, this, true, false),
        this,
        kSnapEps);
    applyScenePathFast(scenePts);
}

void LzConnectLine::recomputePathFromCurrentEndpoints(bool schedulePostprocess)
{
    Q_D(LzConnectLine);
    if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
        return;
    if (d->points.size() < 2 || d->recomputingPath)
        return;
    d->recomputingPath = true;
    struct RecomputeGuard {
        bool& flag;
        ~RecomputeGuard() { flag = false; }
    } guard{d->recomputingPath};

    QPointF startScene = mapToScene(d->points.first());
    QPointF endScene = mapToScene(d->points.last());
    if (d->startItem)
        startScene = endpointSceneForItem(d->startItem, d->startConnectPort, startScene);
    if (d->endItem)
        endScene = endpointSceneForItem(d->endItem, d->endConnectPort, endScene);
    const QPointF anchorS = endpointSceneForItem(d->startItem, d->startConnectPort, startScene);
    const QPointF anchorE = endpointSceneForItem(d->endItem, d->endConnectPort, endScene);

    static const qreal kSnapEps = 1.0;
    if (d->linkMode == CONNECTION_STRATEGY::DIRECT_CONNECTION)
    {
        const QPointF visualEnd = visualEndpointSceneForPort(d->endConnectPort, anchorE);
        QVector<QPointF> scenePts = {anchorS, visualEnd};
        sanitizeOrthogonalScenePath(scenePts, kSnapEps);
        applyScenePathFast(scenePts);
        d->lastRecomputedScenePath = scenePts;
        if (schedulePostprocess)
        {
            syncTopologyEndpointsFromItems();
            if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
                sc->scheduleConnectorPostprocess({this});
        }
        return;
    }

    const QPointF startRoute = routingSceneForPort(d->startConnectPort, anchorS);
    const QPointF endRoute = routingSceneForPort(d->endConnectPort, anchorE);
    const bool fastAfterDrag = !schedulePostprocess;

    if (fastAfterDrag && !lzConnectStrategyPrefersFullRecompute(d->linkMode))
    {
        const QVector<QPointF> scenePts = simplifiedOrthogonalScenePathForLine(
            buildLiteOrthogonalScenePath(
                anchorS, anchorE, d->startConnectPort, d->endConnectPort, kSnapEps, this, true, false),
            this,
            kSnapEps);
        applyScenePathFast(scenePts);
        d->lastRecomputedScenePath = scenePts;
        return;
    }

    LzScene* scene_ = dynamic_cast<LzScene*>(scene());
    const qreal routeQueryPad = qMax<qreal>(qreal(ObstacleClearanceMargin * 6), qreal(StepSize * 4));
    QRectF routeQueryRect(startRoute, endRoute);
    routeQueryRect = routeQueryRect.normalized().adjusted(-routeQueryPad, -routeQueryPad,
                                                          routeQueryPad, routeQueryPad);
    if (d->startItem)
    {
        const QRectF br = LzAStar::expandedObstacleSceneRect(d->startItem, 0);
        if (br.isValid())
            routeQueryRect = routeQueryRect.united(br);
    }
    if (d->endItem)
    {
        const QRectF br = LzAStar::expandedObstacleSceneRect(d->endItem, 0);
        if (br.isValid())
            routeQueryRect = routeQueryRect.united(br);
    }
    const QList<QGraphicsItem*> routingObstacles =
        scene_ ? collectRoutingObstacles(this, scene_, routeQueryRect) : QList<QGraphicsItem*>();
    const QList<QGraphicsItem*> coreObstacles =
        coreObstaclesForLine(routingObstacles, d->startItem, d->startConnectPort,
                             d->endItem, d->endConnectPort);
    const QList<QGraphicsItem*> bridgeObstacles =
        bridgeObstaclesForLine(routingObstacles, d->startItem, d->endItem);

    QVector<QPoint> corePath = LzConnectorAlgorithm::planOrthogonalPath(startRoute, endRoute, coreObstacles,
                                                                        d_ptr->routingStrategy, d_ptr->pathPriorityMode);
    if (corePath.isEmpty())
        corePath = LzConnectorAlgorithm::planOrthogonalPathQuick(startRoute, endRoute, coreObstacles);
    if (corePath.isEmpty())
    {
        const bool preferVerticalFirst =
            qAbs(endRoute.x() - startRoute.x()) < qAbs(endRoute.y() - startRoute.y());
        const QPointF corner = preferVerticalFirst
            ? QPointF(startRoute.x(), endRoute.y())
            : QPointF(endRoute.x(), startRoute.y());
        corePath = {startRoute.toPoint(), corner.toPoint(), endRoute.toPoint()};
    }

    QVector<QPointF> scenePts;
    appendPortExitFromAnchor(scenePts, anchorS, d->startConnectPort, kSnapEps);
    for (int i = 0; i < corePath.size(); ++i)
    {
        const QPointF p(corePath[i]);
        if (!scenePts.isEmpty() && QLineF(scenePts.last(), p).length() < kSnapEps)
            continue;
        scenePts.append(p);
    }
    appendPortApproachToAnchor(scenePts,
                               scenePts.isEmpty() ? endRoute : scenePts.last(),
                               anchorE,
                               d->endConnectPort,
                               kSnapEps,
                               bridgeObstacles);

    scenePts = preserveOrthogonalMiddleWithEndpoints(scenePts,
                                                     anchorS,
                                                     d->startConnectPort,
                                                     anchorE,
                                                     d->endConnectPort,
                                                     this,
                                                     kSnapEps);

    if (scenePathCrossesForeignObstacles(scenePts, routingObstacles, d->startItem, d->endItem))
    {
        corePath = LzConnectorAlgorithm::planOrthogonalPathQuick(startRoute, endRoute, coreObstacles);
        if (!corePath.isEmpty())
        {
            scenePts.clear();
            appendPortExitFromAnchor(scenePts, anchorS, d->startConnectPort, kSnapEps);
            for (int i = 0; i < corePath.size(); ++i)
            {
                const QPointF p(corePath[i]);
                if (!scenePts.isEmpty() && QLineF(scenePts.last(), p).length() < kSnapEps)
                    continue;
                scenePts.append(p);
            }
            appendPortApproachToAnchor(scenePts,
                                       scenePts.isEmpty() ? endRoute : scenePts.last(),
                                       anchorE,
                                       d->endConnectPort,
                                       kSnapEps,
                                       bridgeObstacles);
            scenePts = preserveOrthogonalMiddleWithEndpoints(scenePts,
                                                             anchorS,
                                                             d->startConnectPort,
                                                             anchorE,
                                                             d->endConnectPort,
                                                             this,
                                                             kSnapEps);
        }
    }

    {
        repairScenePathAgainstObstacles(scenePts, kSnapEps, routingObstacles);

        LzAStar astar;
        bool stillCrosses = false;
        for (int i = 0; i + 1 < scenePts.size(); ++i)
        {
            QList<QGraphicsItem*> segObs = routingObstacles;
            if (i == 0)
                segObs = obstaclesExceptItems(routingObstacles, d->startItem, nullptr);
            else if (i + 1 == scenePts.size() - 1)
                segObs = obstaclesExceptItems(routingObstacles, nullptr, d->endItem);
            if (astar.segmentIntersectsObstacles(scenePts[i].toPoint(), scenePts[i + 1].toPoint(), segObs))
            {
                stillCrosses = true;
                break;
            }
        }
        if (stillCrosses)
        {
            corePath = LzConnectorAlgorithm::planOrthogonalPathQuick(startRoute, endRoute, coreObstacles);
            if (corePath.size() >= 2)
            {
                scenePts.clear();
                appendPortExitFromAnchor(scenePts, anchorS, d->startConnectPort, kSnapEps);
                for (int i = 0; i < corePath.size(); ++i)
                {
                    const QPointF p(corePath[i]);
                    if (!scenePts.isEmpty() && QLineF(scenePts.last(), p).length() < kSnapEps)
                        continue;
                    scenePts.append(p);
                }
                appendPortApproachToAnchor(scenePts,
                                           scenePts.isEmpty() ? endRoute : scenePts.last(),
                                           anchorE,
                                           d->endConnectPort,
                                           kSnapEps,
                                           bridgeObstacles);
                scenePts = preserveOrthogonalMiddleWithEndpoints(scenePts,
                                                                 anchorS,
                                                                 d->startConnectPort,
                                                                 anchorE,
                                                                 d->endConnectPort,
                                                                 this,
                                                                 kSnapEps);
                repairScenePathAgainstObstacles(scenePts, kSnapEps, routingObstacles);
            }
        }
    }

    scenePts = simplifiedOrthogonalScenePathForLine(scenePts, this, kSnapEps);

    if (d->lastRecomputedScenePath.size() == scenePts.size())
    {
        bool same = true;
        for (int i = 0; i < scenePts.size(); ++i)
        {
            if (QLineF(d->lastRecomputedScenePath[i], scenePts[i]).length() > kSnapEps)
            {
                same = false;
                break;
            }
        }
        if (same)
            return;
    }
    d->lastRecomputedScenePath = scenePts;

    setPos(0, 0);
    d->points.clear();
    for (const QPointF& pt : scenePts)
        d->points.append(mapFromScene(pt));

    while (handleStruct.handles.size() > scenePts.size())
        delete handleStruct.handles.takeLast();
    for (int i = handleStruct.handles.size(); i < scenePts.size(); ++i)
    {
        LzHandle* handle = new LzHandle(this, handleStruct.handles.size() + LEFT + 1, true);
        handle->setState(HANDLE_INACTIVE);
        handleStruct.handles.append(handle);
    }
    updateHandles();
    d->initialPoints = d->points;
    prepareGeometryChange();
    QRectF bounds = d->points.boundingRect();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    update();
    if (schedulePostprocess)
    {
        syncTopologyEndpointsFromItems();
        if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
            sc->scheduleConnectorPostprocess({this});
    }
}

void LzConnectLine::applyScenePathFast(const QVector<QPointF>& scenePts)
{
    Q_D(LzConnectLine);
    if (scenePts.size() < 2)
        return;

    setPos(0, 0);
    d->points.clear();
    for (const QPointF& pt : scenePts)
        d->points.append(mapFromScene(pt));
    d->lastRecomputedScenePath = scenePts;

    while (handleStruct.handles.size() > scenePts.size())
        delete handleStruct.handles.takeLast();
    while (handleStruct.handles.size() < scenePts.size())
    {
        LzHandle* h = new LzHandle(this, handleStruct.handles.size() + LEFT + 1, true);
        h->setState(HANDLE_INACTIVE);
        handleStruct.handles.append(h);
    }
    d->initialPoints = d->points;
    prepareGeometryChange();
    itemPosition = d->points.boundingRect();
    itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
    updateHandles();
    update();
}

bool LzConnectLine::wouldCompleteConnectivityCycle(QGraphicsItem* endItem) const
{
    Q_D(const LzConnectLine);
    TmpShape* const start = qgraphicsitem_cast<TmpShape*>(d->startItem);
    TmpShape* const end = qgraphicsitem_cast<TmpShape*>(endItem);
    if (!start || !end || start == end)
        return false;
    return connectivityPathExists(end, start, this);
}

void LzConnectLine::attachEndAndRecompute(QGraphicsItem* endItem, LzConnectPoint* endPort)
{
    if (!endItem || !endPort)
        return;
    cancelDeferredPathRecompute();
    setPathRoutingMode(ConnectorPathRoutingMode::Auto);
    setEndItem(endItem);
    setEndConnectPort(endPort);

    Q_D(LzConnectLine);
    if (d->points.size() < 2)
    {
        const QPointF endScene =
            endpointSceneForItem(endItem, endPort, endPort->connectionCenterInScene());
        QPointF startScene = d->points.isEmpty() ? endScene : mapToScene(d->points.first());
        if (d->startItem)
            startScene = endpointSceneForItem(d->startItem, d->startConnectPort, startScene);
        applyScenePathFast({startScene, endScene});
    }
    else
    {
        QVector<QPointF> scenePts = pathInSceneCoords();
        if (scenePts.size() >= 2)
        {
            scenePts.last() = endpointSceneForItem(endItem, endPort, scenePts.last());
            scenePts = simplifiedOrthogonalScenePathForLine(scenePts, this, 0.5);
            applyScenePathFast(scenePts);
            d->lastRecomputedScenePath = scenePts;
        }
    }
    syncTopologyEndpointsFromItems();
    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
        sc->scheduleTopologyRebuild();
}

void LzConnectLine::finalizeWireWithFloatingEnd(const QPointF& endScene)
{
    Q_D(LzConnectLine);
    cancelDeferredPathRecompute();
    setEndItem(nullptr);
    setEndConnectPort(nullptr);
    setPathRoutingMode(ConnectorPathRoutingMode::Auto);

    QPointF anchorS = d->points.isEmpty() ? endScene : mapToScene(d->points.first());
    if (d->startItem)
        anchorS = endpointSceneForItem(d->startItem, d->startConnectPort, anchorS);

    QVector<QPointF> scenePts;
    if (d->points.size() >= 2)
    {
        scenePts = pathInSceneCoords();
        scenePts.last() = endScene;
    }
    else
    {
        static const qreal kEps = 0.5;
        scenePts = buildLiteOrthogonalScenePath(
            anchorS, endScene, d->startConnectPort, nullptr, kEps, this, true, false);
    }

    static const qreal kSnapEps = 0.5;
    scenePts = simplifiedOrthogonalScenePathForLine(scenePts, this, kSnapEps);
    applyScenePathFast(scenePts);
    d->lastRecomputedScenePath = scenePts;
    syncTopologyEndpointsFromItems();
    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
        sc->scheduleConnectorPostprocess({this});
}

void LzConnectLine::setPathFromScenePoints(const QVector<QPointF>& scenePoints)
{
    Q_D(LzConnectLine);
    if (scenePoints.size() < 2)
        return;
    QVector<QPointF> orthoPts = scenePoints;
    if (d->pathRoutingMode == ConnectorPathRoutingMode::Auto)
        orthoPts = simplifiedOrthogonalScenePathForLine(orthoPts, this, 0.5);
    else
        sanitizeOrthogonalScenePath(orthoPts, 0.5);
    d->points.clear();
    for (auto handle : handleStruct.handles)
        delete handle;
    handleStruct.handles.clear();
    setPos(0, 0);
    for (int i = 0; i < orthoPts.size(); ++i)
    {
        d->points.append(mapFromScene(orthoPts[i]));
        LzHandle* handle = new LzHandle(this, handleStruct.handles.size() + LEFT + 1, true);
        handle->setState(HANDLE_INACTIVE);
        handleStruct.handles.append(handle);
    }
    d->initialPoints = d->points;
    d->lastRecomputedScenePath = orthoPts;
    prepareGeometryChange();
    QRectF bounds = d->points.boundingRect();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    updateHandles();
    update();
}

QVector<QPointF> LzConnectLine::pathInSceneCoords() const
{
    Q_D(const LzConnectLine);
    QVector<QPointF> out;
    for (const QPointF& pt : d->points)
        out.append(mapToScene(pt));
    return out;
}

QVector<QPointF> LzConnectLine::editablePathInSceneCoords() const
{
    Q_D(const LzConnectLine);
    const QVector<QPointF> scenePts = pathInSceneCoords();
    if (scenePts.size() < 3 || d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
        return scenePts;
    return normalizedInheritedAutoScenePath(scenePts, this, 0.5);
}

int LzConnectLine::segmentIndexAtScenePos(const QPointF& scenePos, qreal tolerancePx) const
{
    return segmentIndexAtScenePosForPath(pathInSceneCoords(), scenePos, tolerancePx);
}

int LzConnectLine::segmentIndexAtScenePosForPath(const QVector<QPointF>& scenePath,
                                                 const QPointF& scenePos,
                                                 qreal tolerancePx) const
{
    if (scenePath.size() < 2)
        return -1;
    const auto result = LzConnectorAlgorithm::projectPointOnPolyline(scenePath, scenePos);
    if (!result.valid || result.segmentIndex < 0)
        return -1;
    if (QLineF(result.projected, scenePos).length() > tolerancePx)
        return -1;
    return result.segmentIndex;
}

void LzConnectLine::applySegmentDragFromBase(const QVector<QPointF>& baseScenePath,
                                             int segmentIndex,
                                             const QPointF& sceneOffset)
{
    if (baseScenePath.size() < 2 || segmentIndex < 0
        || segmentIndex >= baseScenePath.size() - 1)
    {
        return;
    }

    QVector<QPointF> pts = baseScenePath;
    const QPointF a = pts[segmentIndex];
    const QPointF b = pts[segmentIndex + 1];
    static const qreal kEps = 0.5;
    const bool horizontal = qAbs(a.y() - b.y()) <= qAbs(a.x() - b.x()) + kEps;
    const bool startPinned = (segmentIndex == 0)
        && connectPortStillOnItem(startConnectPort(), getStartItem());
    const bool endPinned = (segmentIndex == pts.size() - 2)
        && connectPortStillOnItem(endConnectPort(), getEndItem());

    if (startPinned && pts.size() >= 2)
    {
        QVector<QPointF> adjusted;
        adjusted.reserve(pts.size() + 1);
        adjusted.append(pts[0]);

        QPointF moved = pts[1];
        QPointF bridge = pts[0];
        if (horizontal)
        {
            const qreal dy = sceneOffset.y();
            moved.setY(moved.y() + dy);
            bridge.setY(moved.y());
        }
        else
        {
            const qreal dx = sceneOffset.x();
            moved.setX(moved.x() + dx);
            bridge.setX(moved.x());
        }

        if (QLineF(adjusted.last(), bridge).length() > kEps)
            adjusted.append(bridge);
        adjusted.append(moved);
        for (int i = 2; i < pts.size(); ++i)
            adjusted.append(pts[i]);

        setPathFromScenePoints(adjusted);
        markPathAsUserEdited(false);
        return;
    }

    if (endPinned && pts.size() >= 2)
    {
        QVector<QPointF> adjusted;
        adjusted.reserve(pts.size() + 1);
        for (int i = 0; i < pts.size() - 2; ++i)
            adjusted.append(pts[i]);

        QPointF moved = pts[pts.size() - 2];
        QPointF bridge = pts.last();
        if (horizontal)
        {
            const qreal dy = sceneOffset.y();
            moved.setY(moved.y() + dy);
            bridge.setY(moved.y());
        }
        else
        {
            const qreal dx = sceneOffset.x();
            moved.setX(moved.x() + dx);
            bridge.setX(moved.x());
        }

        adjusted.append(moved);
        if (QLineF(moved, bridge).length() > kEps)
            adjusted.append(bridge);
        adjusted.append(pts.last());

        setPathFromScenePoints(adjusted);
        markPathAsUserEdited(false);
        return;
    }

    if (horizontal)
    {
        const qreal dy = sceneOffset.y();
        pts[segmentIndex].setY(pts[segmentIndex].y() + dy);
        pts[segmentIndex + 1].setY(pts[segmentIndex + 1].y() + dy);
    }
    else
    {
        const qreal dx = sceneOffset.x();
        pts[segmentIndex].setX(pts[segmentIndex].x() + dx);
        pts[segmentIndex + 1].setX(pts[segmentIndex + 1].x() + dx);
    }

    setPathFromScenePoints(pts);
    markPathAsUserEdited(false);
}

int LzConnectLine::insertWaypointAtScenePos(const QPointF& scenePos)
{
    if (type() != LzConnectLine::Type)
        return -1;
    Q_D(LzConnectLine);
    if (!d)
        return -1;
    if (d->points.size() < 2)
        return -1;
    QPointF localPos = mapFromScene(scenePos);
    auto result = LzConnectorAlgorithm::projectPointOnPolyline(d->points, localPos);
    if (!result.valid)
        return -1;
    int insertIndex = result.segmentIndex + 1;
    d->points.insert(insertIndex, result.projected);
    LzHandle* handle = new LzHandle(this, handleStruct.handles.size() + LEFT + 1, true);
    handle->setState(isSelected() ? HANDLE_ACTIVE : HANDLE_INACTIVE);
    handleStruct.handles.insert(insertIndex, handle);
    for (int i = insertIndex; i < handleStruct.handles.size(); ++i)
        handleStruct.handles[i]->setDir(LEFT + i + 1);
    d->initialPoints = d->points;
    prepareGeometryChange();
    QRectF bounds = d->points.boundingRect();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    updateHandles();
    update();
    markPathAsUserEdited(false);
    return insertIndex;
}

bool LzConnectLine::removeWaypointAt(int pointIndex)
{
    Q_D(LzConnectLine);
    if (d->points.size() <= 2 || pointIndex <= 0 || pointIndex >= d->points.size() - 1)
        return false;
    d->points.removeAt(pointIndex);
    if (pointIndex < handleStruct.handles.size())
    {
        delete handleStruct.handles[pointIndex];
        handleStruct.handles.removeAt(pointIndex);
    }
    for (int i = pointIndex; i < handleStruct.handles.size(); ++i)
        handleStruct.handles[i]->setDir(LEFT + i + 1);
    d->initialPoints = d->points;
    prepareGeometryChange();
    QRectF bounds = d->points.boundingRect();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    updateHandles();
    update();
    markPathAsUserEdited(false);
    return true;
}

void LzConnectLine::clearWaypointsAndReplan()
{
    setPathRoutingMode(ConnectorPathRoutingMode::Auto);
    recomputePathFromCurrentEndpoints();
}

void LzConnectLine::reverseDirection()
{
    Q_D(LzConnectLine);
    if (d->points.size() < 2)
        return;
    qSwap(d->startItem, d->endItem);
    QPolygonF rev;
    for (int i = d->points.size() - 1; i >= 0; --i)
        rev.append(d->points[i]);
    d->points = rev;
    d->initialPoints = d->points;
    updateHandles();
    recomputePathFromCurrentEndpoints();
}

LzConnectLine::CornerStyle LzConnectLine::cornerStyle() const
{
    return d_ptr->cornerStyle;
}

void LzConnectLine::setCornerStyle(CornerStyle style)
{
    if (d_ptr->cornerStyle == style)
        return;
    d_ptr->cornerStyle = style;
    update();
}

int LzConnectLine::cornerStyleProperty() const
{
    return static_cast<int>(cornerStyle());
}

void LzConnectLine::setCornerStyleProperty(int style)
{
    setCornerStyle(static_cast<CornerStyle>(qBound(0, style, static_cast<int>(Curved))));
}

LzConnectLine::ArrowHeadStyle LzConnectLine::arrowHead() const
{
    return d_ptr->arrowHead;
}

void LzConnectLine::setArrowHead(ArrowHeadStyle style)
{
    if (d_ptr->arrowHead == style)
        return;
    d_ptr->arrowHead = style;
    update();
}

int LzConnectLine::arrowHeadStyle() const
{
    return static_cast<int>(arrowHead());
}

void LzConnectLine::setArrowHeadStyle(int style)
{
    setArrowHead(static_cast<ArrowHeadStyle>(qBound(0, style, static_cast<int>(ArrowBoth))));
}

bool LzConnectLine::lineJumpEnabled() const
{
    return d_ptr->lineJumpEnabled;
}

void LzConnectLine::setLineJumpEnabled(bool enabled)
{
    if (d_ptr->lineJumpEnabled == enabled)
        return;
    d_ptr->lineJumpEnabled = enabled;
    update();
}

LzConnectorAlgorithm::ConnectorRoutingStrategy LzConnectLine::routingStrategy() const
{
    return d_ptr->routingStrategy;
}

void LzConnectLine::setRoutingStrategy(LzConnectorAlgorithm::ConnectorRoutingStrategy strategy)
{
    if (d_ptr->routingStrategy == strategy)
        return;
    d_ptr->routingStrategy = strategy;
    recomputePathFromCurrentEndpoints();
}

LzConnectorAlgorithm::PathPriorityMode LzConnectLine::pathPriorityMode() const
{
    return d_ptr->pathPriorityMode;
}

void LzConnectLine::setPathPriorityMode(LzConnectorAlgorithm::PathPriorityMode mode)
{
    if (d_ptr->pathPriorityMode == mode)
        return;
    d_ptr->pathPriorityMode = mode;
    recomputePathFromCurrentEndpoints();
}


namespace {

void lzPushConnectLinePathChange(LzConnectLine* line,
                                 const LzEditConnectLinePathCommand::State& beforeState,
                                 const QString& actionText)
{
    if (!line)
        return;
    LzScene* sc = dynamic_cast<LzScene*>(line->scene());
    LzView* view = sc ? sc->getView() : nullptr;
    if (!view)
        return;
    LzEditConnectLinePathCommand::pushIfChanged(
        view,
        line,
        beforeState,
        LzEditConnectLinePathCommand::captureState(line),
        actionText);
}

} // namespace

void LzConnectLine::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    showContextMenu(event);
}

void LzConnectLine::showContextMenu(QGraphicsSceneContextMenuEvent* event)
{
    Q_D(LzConnectLine);
    event->accept();
    QPointF scenePos = event->scenePos();
    int handleDir = collidesWithHandle(scenePos);
    int waypointIndex = (handleDir > LEFT) ? (handleDir - LEFT - 1) : -1;
    bool onWaypoint = (waypointIndex >= 0 && waypointIndex < d->points.size());
    bool canRemoveWaypoint = onWaypoint && waypointIndex > 0 && waypointIndex < d->points.size() - 1;
    bool onLine = shape().contains(mapFromScene(scenePos)) || (waypointIndex >= 0);
    if (!onLine && !onWaypoint)
        return;
    QMenu menu;
    QAction* reverseAct = menu.addAction(tr("反转方向"));
    QMenu* linkSub = menu.addMenu(tr("连线方式"));
    QAction* brokenLinkAct = linkSub->addAction(tr("正交折线"));
    QAction* automaticLinkAct = linkSub->addAction(tr("自动路由"));
    QAction* obstacleLinkAct = linkSub->addAction(tr("强化避障"));
    QAction* directLinkAct = linkSub->addAction(tr("直线(兼容)"));
    brokenLinkAct->setCheckable(true);
    automaticLinkAct->setCheckable(true);
    obstacleLinkAct->setCheckable(true);
    directLinkAct->setCheckable(true);
    switch (d->linkMode)
    {
    case BROKEN_CONNECTION:
    case ADSORPTION_CONNECTION:
        brokenLinkAct->setChecked(true);
        break;
    case AUTOMATIC_CONNECTION:
        automaticLinkAct->setChecked(true);
        break;
    case OBSTACLE_AVOIDANCE_CONNECTION:
        obstacleLinkAct->setChecked(true);
        break;
    case DIRECT_CONNECTION:
        directLinkAct->setChecked(true);
        break;
    default:
        brokenLinkAct->setChecked(true);
        break;
    }
    QMenu* cornerSub = menu.addMenu(tr("拐角样式"));
    QAction* sharpAct = cornerSub->addAction(tr("直角"));
    QAction* roundedAct = cornerSub->addAction(tr("圆角"));
    QAction* curvedAct = cornerSub->addAction(tr("弧线"));
    sharpAct->setCheckable(true);
    roundedAct->setCheckable(true);
    curvedAct->setCheckable(true);
    switch (d->cornerStyle)
    {
    case Sharp:  sharpAct->setChecked(true); break;
    case Rounded: roundedAct->setChecked(true); break;
    case Curved: curvedAct->setChecked(true); break;
    }
    QMenu* routingSub = menu.addMenu(tr("路由策略"));
    QAction* gridAct = routingSub->addAction(tr("网格 A*"));
    QAction* visibilityAct = routingSub->addAction(tr("可见性图"));
    gridAct->setCheckable(true);
    visibilityAct->setCheckable(true);
    gridAct->setChecked(d->routingStrategy == LzConnectorAlgorithm::GridAStar);
    visibilityAct->setChecked(d->routingStrategy == LzConnectorAlgorithm::VisibilityGraph);
    QMenu* prioritySub = menu.addMenu(tr("路径优先"));
    QAction* pathShortestAct = prioritySub->addAction(tr("最短"));
    QAction* pathFewerBendsAct = prioritySub->addAction(tr("少拐弯"));
    QAction* pathBalancedAct = prioritySub->addAction(tr("平衡"));
    pathShortestAct->setCheckable(true);
    pathFewerBendsAct->setCheckable(true);
    pathBalancedAct->setCheckable(true);
    pathShortestAct->setChecked(d->pathPriorityMode == LzConnectorAlgorithm::PathShortest);
    pathFewerBendsAct->setChecked(d->pathPriorityMode == LzConnectorAlgorithm::PathFewerBends);
    pathBalancedAct->setChecked(d->pathPriorityMode == LzConnectorAlgorithm::PathBalanced);
    QMenu* arrowSub = menu.addMenu(tr("箭头"));
    QAction* arrowNoneAct = arrowSub->addAction(tr("无"));
    QAction* arrowEndAct = arrowSub->addAction(tr("终点"));
    QAction* arrowStartAct = arrowSub->addAction(tr("起点"));
    QAction* arrowBothAct = arrowSub->addAction(tr("双向"));
    arrowNoneAct->setCheckable(true);
    arrowEndAct->setCheckable(true);
    arrowStartAct->setCheckable(true);
    arrowBothAct->setCheckable(true);
    switch (d->arrowHead)
    {
    case ArrowNone: arrowNoneAct->setChecked(true); break;
    case ArrowEnd: arrowEndAct->setChecked(true); break;
    case ArrowStart: arrowStartAct->setChecked(true); break;
    case ArrowBoth: arrowBothAct->setChecked(true); break;
    }
    QAction* lineJumpAct = menu.addAction(tr("交叉跳线"));
    lineJumpAct->setCheckable(true);
    lineJumpAct->setChecked(d->lineJumpEnabled);
    menu.addSeparator();
    if (onLine && !onWaypoint)
    {
        QAction* addAct = menu.addAction(tr("添加路径点"));
        QAction* clearAct = menu.addAction(tr("清除路径点并重新规划"));
        menu.addSeparator();
        QAction* globalAct = menu.addAction(tr("常规菜单"));
        const auto stateBefore = LzEditConnectLinePathCommand::captureState(this);
        QAction* chosen = menu.exec(event->screenPos());
        if (chosen == reverseAct)
        {
            reverseDirection();
            lzPushConnectLinePathChange(this, stateBefore, tr("反转连接线"));
        }
        else if (chosen == sharpAct)  setCornerStyle(Sharp);
        else if (chosen == roundedAct) setCornerStyle(Rounded);
        else if (chosen == curvedAct)  setCornerStyle(Curved);
        else if (chosen == brokenLinkAct) setConnectStrategy(BROKEN_CONNECTION);
        else if (chosen == automaticLinkAct) setConnectStrategy(AUTOMATIC_CONNECTION);
        else if (chosen == obstacleLinkAct) setConnectStrategy(OBSTACLE_AVOIDANCE_CONNECTION);
        else if (chosen == directLinkAct) setConnectStrategy(DIRECT_CONNECTION);
        else if (chosen == gridAct)   setRoutingStrategy(LzConnectorAlgorithm::GridAStar);
        else if (chosen == visibilityAct)    setRoutingStrategy(LzConnectorAlgorithm::VisibilityGraph);
        else if (chosen == pathShortestAct) setPathPriorityMode(LzConnectorAlgorithm::PathShortest);
        else if (chosen == pathFewerBendsAct) setPathPriorityMode(LzConnectorAlgorithm::PathFewerBends);
        else if (chosen == pathBalancedAct)  setPathPriorityMode(LzConnectorAlgorithm::PathBalanced);
        else if (chosen == arrowNoneAct) setArrowHead(ArrowNone);
        else if (chosen == arrowEndAct) setArrowHead(ArrowEnd);
        else if (chosen == arrowStartAct) setArrowHead(ArrowStart);
        else if (chosen == arrowBothAct) setArrowHead(ArrowBoth);
        else if (chosen == lineJumpAct) setLineJumpEnabled(!d->lineJumpEnabled);
        else if (chosen == addAct)
        {
            insertWaypointAtScenePos(scenePos);
            lzPushConnectLinePathChange(this, stateBefore, tr("添加路径点"));
        }
        else if (chosen == clearAct)
        {
            clearWaypointsAndReplan();
            lzPushConnectLinePathChange(this, stateBefore, tr("清除路径点并重新规划"));
        }
        else if (chosen == globalAct && scene())
        {
            if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
                sc->execContextMenu(event->screenPos());
        }
        return;
    }
    if (canRemoveWaypoint)
    {
        QAction* removeAct = menu.addAction(tr("删除路径点"));
        QAction* clearAct = menu.addAction(tr("清除路径点并重新规划"));
        menu.addSeparator();
        QAction* globalAct = menu.addAction(tr("常规菜单"));
        const auto stateBefore = LzEditConnectLinePathCommand::captureState(this);
        QAction* chosen = menu.exec(event->screenPos());
        if (chosen == reverseAct)
        {
            reverseDirection();
            lzPushConnectLinePathChange(this, stateBefore, tr("反转连接线"));
        }
        else if (chosen == sharpAct)  setCornerStyle(Sharp);
        else if (chosen == roundedAct) setCornerStyle(Rounded);
        else if (chosen == curvedAct)  setCornerStyle(Curved);
        else if (chosen == brokenLinkAct) setConnectStrategy(BROKEN_CONNECTION);
        else if (chosen == automaticLinkAct) setConnectStrategy(AUTOMATIC_CONNECTION);
        else if (chosen == obstacleLinkAct) setConnectStrategy(OBSTACLE_AVOIDANCE_CONNECTION);
        else if (chosen == directLinkAct) setConnectStrategy(DIRECT_CONNECTION);
        else if (chosen == gridAct)   setRoutingStrategy(LzConnectorAlgorithm::GridAStar);
        else if (chosen == visibilityAct)    setRoutingStrategy(LzConnectorAlgorithm::VisibilityGraph);
        else if (chosen == pathShortestAct) setPathPriorityMode(LzConnectorAlgorithm::PathShortest);
        else if (chosen == pathFewerBendsAct) setPathPriorityMode(LzConnectorAlgorithm::PathFewerBends);
        else if (chosen == pathBalancedAct)  setPathPriorityMode(LzConnectorAlgorithm::PathBalanced);
        else if (chosen == arrowNoneAct) setArrowHead(ArrowNone);
        else if (chosen == arrowEndAct) setArrowHead(ArrowEnd);
        else if (chosen == arrowStartAct) setArrowHead(ArrowStart);
        else if (chosen == arrowBothAct) setArrowHead(ArrowBoth);
        else if (chosen == lineJumpAct) setLineJumpEnabled(!d->lineJumpEnabled);
        else if (chosen == removeAct)
        {
            removeWaypointAt(waypointIndex);
            lzPushConnectLinePathChange(this, stateBefore, tr("删除路径点"));
        }
        else if (chosen == clearAct)
        {
            clearWaypointsAndReplan();
            lzPushConnectLinePathChange(this, stateBefore, tr("清除路径点并重新规划"));
        }
        else if (chosen == globalAct && scene())
        {
            if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
                sc->execContextMenu(event->screenPos());
        }
        return;
    }
    QAction* clearAct = menu.addAction(tr("清除路径点并重新规划"));
    QAction* globalAct = menu.addAction(tr("常规菜单"));
    const auto stateBefore = LzEditConnectLinePathCommand::captureState(this);
    QAction* chosen = menu.exec(event->screenPos());
    if (chosen == reverseAct)
    {
        reverseDirection();
        lzPushConnectLinePathChange(this, stateBefore, tr("反转连接线"));
    }
    else if (chosen == sharpAct)  setCornerStyle(Sharp);
    else if (chosen == roundedAct) setCornerStyle(Rounded);
    else if (chosen == curvedAct)  setCornerStyle(Curved);
    else if (chosen == brokenLinkAct) setConnectStrategy(BROKEN_CONNECTION);
    else if (chosen == automaticLinkAct) setConnectStrategy(AUTOMATIC_CONNECTION);
    else if (chosen == obstacleLinkAct) setConnectStrategy(OBSTACLE_AVOIDANCE_CONNECTION);
    else if (chosen == directLinkAct) setConnectStrategy(DIRECT_CONNECTION);
    else if (chosen == gridAct)   setRoutingStrategy(LzConnectorAlgorithm::GridAStar);
    else if (chosen == visibilityAct)    setRoutingStrategy(LzConnectorAlgorithm::VisibilityGraph);
    else if (chosen == pathShortestAct) setPathPriorityMode(LzConnectorAlgorithm::PathShortest);
    else if (chosen == pathFewerBendsAct) setPathPriorityMode(LzConnectorAlgorithm::PathFewerBends);
    else if (chosen == pathBalancedAct)  setPathPriorityMode(LzConnectorAlgorithm::PathBalanced);
    else if (chosen == arrowNoneAct) setArrowHead(ArrowNone);
    else if (chosen == arrowEndAct) setArrowHead(ArrowEnd);
    else if (chosen == arrowStartAct) setArrowHead(ArrowStart);
    else if (chosen == arrowBothAct) setArrowHead(ArrowBoth);
    else if (chosen == lineJumpAct) setLineJumpEnabled(!d->lineJumpEnabled);
    else if (chosen == clearAct)
    {
        clearWaypointsAndReplan();
        lzPushConnectLinePathChange(this, stateBefore, tr("清除路径点并重新规划"));
    }
    else if (chosen == globalAct && scene())
    {
        if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
            sc->execContextMenu(event->screenPos());
    }
}

void LzConnectLine::move(const QPointF& delta)
{
    LzScene* scene_ = dynamic_cast<LzScene*>(scene());
    if (!scene_)
        return;

    if (shouldMoveConnectLineAsSelectedBlock(this))
    {
        this->moveBy(delta.x(), delta.y());
        return;
    }

    const auto& selections = scene_->getSelections();

    bool onlyOneOfStartOrEndSelected =
        (selections.contains(d_ptr->startItem) ^ selections.contains(d_ptr->endItem));

    if (onlyOneOfStartOrEndSelected && !selections.contains(this))
    {
        this->moveBy(delta.x(), delta.y());
        updateConnect(delta);
    }
}

void LzConnectLine::setConnectStrategy(CONNECTION_STRATEGY stratgey)
{
    if (d_ptr->linkMode == stratgey)
        return;
    d_ptr->linkMode = stratgey;
    if (d_ptr->pathRoutingMode == ConnectorPathRoutingMode::Manual)
    {
        update();
        return;
    }
    recomputePathFromCurrentEndpoints();
}

CONNECTION_STRATEGY LzConnectLine::getConnectStrategy() const
{
    return d_ptr->linkMode;
}

int LzConnectLine::connectStrategyProperty() const
{
    return static_cast<int>(getConnectStrategy());
}

void LzConnectLine::setConnectStrategyProperty(int strategy)
{
    setConnectStrategy(static_cast<CONNECTION_STRATEGY>(
        qBound(0, strategy, static_cast<int>(OBSTACLE_AVOIDANCE_CONNECTION))));
}

void LzConnectLine::addPoint(const QPointF& point)
{
    Q_D(LzConnectLine);
    d->points.append(mapFromScene(point));
    int dir = d->points.count();
    LzHandle* shr = new LzHandle(this, dir + LEFT, dir == 1 ? false : true);
    shr->setState(HANDLE_INACTIVE);
    handleStruct.handles.push_back(shr);
}

int LzConnectLine::handleCount() const
{
    return handleStruct.handles.size() + LEFT;
}

void LzConnectLine::stretch(int handle, double sx, double sy, const QPointF& origin)
{
    Q_D(LzConnectLine);
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

QList<QGraphicsItem*> LzConnectLine::collectRoutingObstacles(const LzConnectLine* line, LzScene* scene,
                                                             const QRectF& queryRect)
{
    QList<QGraphicsItem*> filteredItems;
    if (!line || !scene)
        return filteredItems;
    QSet<QGraphicsItem*> seen;
    const QList<QGraphicsItem*> allItems = queryRect.isValid() && !queryRect.isNull()
        ? scene->items(queryRect, Qt::IntersectsItemBoundingRect)
        : scene->items();
    filteredItems.reserve(allItems.size());
    for (QGraphicsItem* item : allItems)
    {
        if (!item || item == line || seen.contains(item))
            continue;
        if (qgraphicsitem_cast<LzConnectLine*>(item) || qgraphicsitem_cast<LzConnectPoint*>(item))
            continue;
        if (qgraphicsitem_cast<LzGroup*>(item))
            continue;
        TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || shape->type() <= QGraphicsItem::UserType + 1)
            continue;
        seen.insert(item);
        filteredItems.append(shape);
    }
    return filteredItems;
}

QVector<QPoint> LzConnectLine::planOrthogonalPath(const QPointF& startPoint, const QPointF& endPoint)
{
    LzScene* scene_ = dynamic_cast<LzScene*>(scene());
    if (!scene_)
        return { startPoint.toPoint(), endPoint.toPoint() };

    const qreal pad = qreal(ObstacleClearanceMargin * 4);
    QRectF queryRect(startPoint, endPoint);
    queryRect = queryRect.normalized().adjusted(-pad, -pad, pad, pad);
    QList<QGraphicsItem*> nearItems = collectRoutingObstacles(this, scene_, queryRect);
    return LzConnectorAlgorithm::planOrthogonalPath(startPoint, endPoint, nearItems,
                                                    d_ptr->routingStrategy, d_ptr->pathPriorityMode);
}

void LzConnectLine::applyEndpointDragAtScenePos(int controlIndex, const QPointF& scenePos,
                                                const QVector<QPointF>& baseScenePath)
{
    Q_D(LzConnectLine);
    const int n = d->points.size();
    if (n < 2 || controlIndex < 0 || controlIndex >= n)
        return;

    const bool isStart = (controlIndex == 0);
    const bool isEnd = (controlIndex == n - 1);
    if (!isStart && !isEnd)
        return;

    QVector<QPointF> originalScenePts =
        (!baseScenePath.isEmpty() && baseScenePath.size() >= 2) ? baseScenePath : pathInSceneCoords();
    if (d->pathRoutingMode == ConnectorPathRoutingMode::Auto)
        originalScenePts = normalizedInheritedAutoScenePath(originalScenePts, this, 0.5);
    if (originalScenePts.size() > 2)
        markPathAsUserEdited(false);
    const bool preserveMiddle = d->pathRoutingMode == ConnectorPathRoutingMode::Manual;

    if (!preserveMiddle && lzConnectStrategyUsesOrthogonalEdit(d->linkMode))
    {
        QPointF anchorS = mapToScene(d->points.first());
        QPointF anchorE = mapToScene(d->points.last());
        if (isStart)
            anchorS = scenePos;
        else
            anchorE = scenePos;
        if (!isStart && d->startItem)
            anchorS = endpointSceneForItem(d->startItem, d->startConnectPort, anchorS);
        if (!isEnd && d->endItem)
            anchorE = endpointSceneForItem(d->endItem, d->endConnectPort, anchorE);
        static const qreal kEps = 0.5;
        const QVector<QPointF> scenePts = simplifiedOrthogonalScenePathForLine(
            buildLiteOrthogonalScenePath(
                anchorS,
                anchorE,
                isStart ? nullptr : d->startConnectPort,
                isEnd ? nullptr : d->endConnectPort,
                kEps,
                this,
                true,
                false),
            this,
            kEps);
        applyScenePathFast(scenePts);
        updateHandles();
        return;
    }

    QVector<QPointF> scenePts = originalScenePts;
    if (isStart)
        scenePts[0] = scenePos;
    else
        scenePts[n - 1] = scenePos;

    if (preserveMiddle && scenePts.size() > 2)
        scenePts = preserveOrthogonalMiddleWithEndpoints(
            scenePts, scenePts.first(), isStart ? nullptr : d->startConnectPort, scenePts.last(),
            isEnd ? nullptr : d->endConnectPort, this, 0.5);
    sanitizeOrthogonalScenePath(scenePts, 0.5);
    applyScenePathFast(scenePts);
    d->lastRecomputedScenePath = scenePts;
    updateHandles();
}

void LzConnectLine::control(int dir, const QPointF& delta)
{
    Q_D(LzConnectLine);
    if (dir <= HANDLE_POINT::LEFT)
    {
        return;
    }

    // 获取当前控制点索引
    int controlIndex = dir - HANDLE_POINT::LEFT - 1;
    if (controlIndex < 0 || controlIndex >= d->points.size())
    {
        return;
    }
    // handleEdit 传入场景坐标；中间折点再映射到图元局部
    const QPointF scenePos = delta;
    QPointF newPos = mapFromScene(delta);
    
    // 判断是否为起点或终点（终点为最后一个点）
    bool isStartPoint = (controlIndex == 0);
    bool isEndPoint = (controlIndex == d->points.size() - 1);

    LzScene* scene_ = dynamic_cast<LzScene*>(scene());
    if (!scene_)
        return;

    if (d->linkMode == CONNECTION_STRATEGY::DIRECT_CONNECTION)
    {
        d->points[controlIndex] = newPos;
        if (!isStartPoint && !isEndPoint)
            markPathAsUserEdited();
    }
    else
    {
        if (isStartPoint || isEndPoint)
        {
            if (scene_->getPaintState())
            {
                setEndpointPreview(dir, scenePos);
                return;
            }
            if (d->points.size() > 2)
                markPathAsUserEdited(false);
            applyEndpointDragAtScenePos(controlIndex, scenePos);
            prepareGeometryChange();
            QRectF bounds = d->points.boundingRect();
            itemPosition = bounds;
            itemWidth = bounds.width();
            itemHeight = bounds.height();
            d->initialPoints = d->points;
            return;
        }
        const QPointF prev = d->points[controlIndex - 1];
        const QPointF next = d->points[controlIndex + 1];
        d->points[controlIndex] = LzConnectorAlgorithm::orthogonalCornerPosition(prev, next, newPos);
        markPathAsUserEdited(false);
    }

    prepareGeometryChange();
    QRectF bounds = d->points.boundingRect();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    d->initialPoints = d->points;
    updateHandles();
}

void LzConnectLine::setEndpointPreview(int dir, const QPointF& scenePos)
{
    Q_D(LzConnectLine);
    if (dir <= HANDLE_POINT::LEFT)
        return;
    const int controlIndex = dir - HANDLE_POINT::LEFT - 1;
    if (controlIndex < 0 || controlIndex >= d->points.size())
        return;
    if (!lzConnectStrategyUsesOrthogonalEdit(d->linkMode))
        return;

    const bool isStart = (controlIndex == 0);
    const bool isEnd = (controlIndex == d->points.size() - 1);
    if (!isStart && !isEnd)
        return;

    if (d->pathRoutingMode == ConnectorPathRoutingMode::Manual)
    {
        const QVector<QPointF> previewScenePts =
            previewOrthogonalEndpointDragPath(pathInSceneCoords(), isStart, scenePos, 0.5);
        if (previewScenePts.size() >= 2)
            applyScenePathFast(previewScenePts);
        return;
    }

    static const qreal kEps = 0.5;
    QPointF anchorS = mapToScene(d->points.first());
    if (d->startItem)
        anchorS = endpointSceneForItem(d->startItem, d->startConnectPort, anchorS);
    QPointF anchorE = mapToScene(d->points.last());
    if (d->endItem)
        anchorE = endpointSceneForItem(d->endItem, d->endConnectPort, anchorE);
    if (isStart)
        anchorS = scenePos;
    else
        anchorE = scenePos;
    const QVector<QPointF> scenePts = simplifiedOrthogonalScenePathForLine(
        buildLiteOrthogonalScenePath(
        anchorS,
        anchorE,
        isStart ? nullptr : d->startConnectPort,
        isEnd ? nullptr : d->endConnectPort,
        kEps,
        this,
        true,
        false),
        this,
        kEps);
    applyScenePathFast(scenePts);
}

void LzConnectLine::setWireDragPreviewEnd(const QPointF& endScene)
{
    Q_D(LzConnectLine);
    if (d->points.isEmpty())
        return;

    static const qreal kEps = 0.5;
    QPointF anchorS = mapToScene(d->points.first());
    if (d->startItem)
        anchorS = endpointSceneForItem(d->startItem, d->startConnectPort, anchorS);
    const QPointF anchorE = endScene;
    const QVector<QPointF> scenePts = simplifiedOrthogonalScenePathForLine(
        buildLiteOrthogonalScenePath(
            anchorS, anchorE, d->startConnectPort, nullptr, kEps, this, true, false),
        this,
        kEps);
    applyScenePathFast(scenePts);
}

int LzConnectLine::collidesWithHandle(const QPointF& scenePoint) const
{
    Q_D(const LzConnectLine);
    const int segmentIndex = segmentIndexAtScenePos(scenePoint);
    if (segmentIndex >= 0 && segmentIndex + 1 < d->points.size())
    {
        const QVector<QPointF> scenePts = pathInSceneCoords();
        const QPointF a = scenePts[segmentIndex];
        const QPointF b = scenePts[segmentIndex + 1];
        const QPointF ab = b - a;
        const qreal lengthSq = ab.x() * ab.x() + ab.y() * ab.y();
        if (lengthSq > 0.01)
        {
            qreal t = ((scenePoint.x() - a.x()) * ab.x() + (scenePoint.y() - a.y()) * ab.y()) / lengthSq;
            t = qBound<qreal>(0.0, t, 1.0);

            // Short segments can sit entirely inside the two endpoint handle hit areas.
            // Keep handle picking near the actual corners, but let the segment win elsewhere
            // so every non-zero segment still supports drag-to-reroute.
            static const qreal kStrictPointHitPx = 4.0;
            static const qreal kEndpointPickRatio = 0.18;
            const bool nearStartPoint = QLineF(scenePoint, a).length() <= kStrictPointHitPx;
            const bool nearEndPoint = QLineF(scenePoint, b).length() <= kStrictPointHitPx;
            const bool nearSegmentEndpoint = t <= kEndpointPickRatio || t >= 1.0 - kEndpointPickRatio;
            if (!nearStartPoint && !nearEndPoint && !nearSegmentEndpoint)
                return HANDLE_POINT::HANDLE_NONE;
        }
    }

    const qreal hitRadius = 12.0;
    int bestDir = HANDLE_POINT::HANDLE_NONE;
    qreal bestDist = hitRadius;
    for (int i = 0; i < d->points.size(); ++i)
    {
        QPointF scenePos = mapToScene(d->points.at(i));
        qreal dist = QLineF(scenePos, scenePoint).length();
        if (dist < bestDist)
        {
            bestDist = dist;
            bestDir = LEFT + i + 1;
        }
    }
    return bestDir;
}

QVariant LzConnectLine::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSceneHasChanged && scene())
        setZValue(LzConnectLineLayerZ);
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        setZValue(value.toBool() ? LzConnectLineHighlightLayerZ : LzConnectLineLayerZ);
        update();
        QGraphicsItemGroup* g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
        {
            setState(value.toBool() ? HANDLE_ACTIVE : HANDLE_OFF);
        }
        else
        {
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    if (change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        QGraphicsItem* g = dynamic_cast<QGraphicsItem*>(parentItem());
        if (!g)
        {
            LzScene* scene_ = dynamic_cast<LzScene*>(scene());
            if (!scene_)
                return QGraphicsItem::itemChange(change, value);
            if (scene_->isInteractiveTransformActive())
                return QGraphicsItem::itemChange(change, value);
            if (scene_->getSelections().count() == 1)
            {
                if (scene_->getSelections().contains(this))
                {
                    emit scene_->itemPropertyChanged();
                }
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QString LzConnectLine::topologyEdgeStableId() const
{
    Q_D(const LzConnectLine);
    return d->edgeStableId;
}

void LzConnectLine::setTopologyEdgeStableId(const QString& id)
{
    Q_D(LzConnectLine);
    d->edgeStableId = id;
}

QString LzConnectLine::ensureTopologyEdgeStableId()
{
    Q_D(LzConnectLine);
    if (d->edgeStableId.isEmpty())
        d->edgeStableId = LzSnowflakeId::instance().nextIdString();
    return d->edgeStableId;
}

TopologyRelationType LzConnectLine::topologyRelationType() const
{
    Q_D(const LzConnectLine);
    return d->relationType;
}

void LzConnectLine::setTopologyRelationType(TopologyRelationType t)
{
    Q_D(LzConnectLine);
    d->relationType = t;
}

QString LzConnectLine::topologyStartNodeStableId() const
{
    Q_D(const LzConnectLine);
    return d->startNodeStableId;
}

QString LzConnectLine::topologyEndNodeStableId() const
{
    Q_D(const LzConnectLine);
    return d->endNodeStableId;
}

QString LzConnectLine::topologyStartPortKey() const
{
    Q_D(const LzConnectLine);
    return d->startPortKey;
}

QString LzConnectLine::topologyEndPortKey() const
{
    Q_D(const LzConnectLine);
    return d->endPortKey;
}

void LzConnectLine::setTopologyStartPortKey(const QString& key)
{
    Q_D(LzConnectLine);
    d->startPortKey = key;
}

void LzConnectLine::setTopologyEndPortKey(const QString& key)
{
    Q_D(LzConnectLine);
    d->endPortKey = key;
}

void LzConnectLine::syncTopologyEndpointsFromItems()
{
    Q_D(LzConnectLine);
    d->startNodeStableId = stableTopologyNodeIdForItem(d->startItem);
    d->endNodeStableId = stableTopologyNodeIdForItem(d->endItem);
    const int startOrdinal = connectPointOrdinalOnItem(d->startItem, d->startConnectPort);
    const int endOrdinal = connectPointOrdinalOnItem(d->endItem, d->endConnectPort);
    d->startPortKey = startOrdinal >= 0 ? QString::number(startOrdinal) : QString();
    d->endPortKey = endOrdinal >= 0 ? QString::number(endOrdinal) : QString();
    const bool startTopo = isTopologyGraphNodeItem(d->startItem);
    const bool endTopo = isTopologyGraphNodeItem(d->endItem);
    d->topologyRelationLink = startTopo && endTopo;
    if (d->topologyRelationLink && !d->startNodeStableId.isEmpty() && !d->endNodeStableId.isEmpty())
        ensureTopologyEdgeStableId();
}

bool LzConnectLine::isTopologyRelationLink() const
{
    Q_D(const LzConnectLine);
    return d->topologyRelationLink;
}

QGraphicsItem* LzConnectLine::duplicate()
{
    const LzConnectLinePrivate* d = d_ptr.data();
    auto* c = new LzConnectLine(QRectF(0, 0, 1, 1));
    LzConnectLinePrivate* cd = c->d_ptr.data();

    cd->points = d->points;
    cd->initialPoints = d->initialPoints;
    cd->lastRecomputedScenePath = d->lastRecomputedScenePath;
    cd->interactiveBaseScenePath = d->interactiveBaseScenePath;
    cd->linkMode = d->linkMode;
    cd->direction = d->direction;
    cd->cornerStyle = d->cornerStyle;
    cd->routingStrategy = d->routingStrategy;
    cd->pathPriorityMode = d->pathPriorityMode;
    cd->pathRoutingMode = d->pathRoutingMode;
    cd->arrowHead = d->arrowHead;
    cd->lineJumpEnabled = d->lineJumpEnabled;
    cd->relationType = d->relationType;
    cd->startPortKey = d->startPortKey;
    cd->endPortKey = d->endPortKey;
    cd->startNodeStableId = d->startNodeStableId;
    cd->endNodeStableId = d->endNodeStableId;
    cd->startItem = nullptr;
    cd->endItem = nullptr;
    cd->edgeStableId.clear();
    cd->topologyRelationLink = false;

    c->setUsePen(getUsePen());
    c->setItemPenColor(getItemPenColor());
    c->setItemPenWidth(getItemPenWidth());
    c->setItemPenStyle(getItemPenStyle());
    c->setUseBrush(getUseBrush());
    c->setItemBrushColor(getItemBrushColor());
    c->setItemBrushStyle(getItemBrushStyle());
    c->setUseFont(getUseFont());
    c->setItemFont(getItemFont());

    c->setPos(pos());
    c->setTransform(transform());
    c->setTransformOriginPoint(transformOriginPoint());
    c->setRotation(rotation());
    c->setScale(scale());
    c->setZValue(LzConnectLineLayerZ);

    for (LzHandle* h : c->handleStruct.handles) {
        delete h;
    }
    c->handleStruct.handles.clear();
    for (int k = 0; k < cd->points.size(); ++k) {
        const int dir = k + 1;
        LzHandle* shr = new LzHandle(c, dir + LEFT, dir == 1 ? false : true);
        shr->setState(HANDLE_INACTIVE);
        c->handleStruct.handles.push_back(shr);
    }

    c->prepareGeometryChange();
    c->itemPosition = cd->points.boundingRect();
    c->itemWidth = c->itemPosition.width();
    c->itemHeight = c->itemPosition.height();
    c->itemInitialRect = c->itemPosition;
    c->updateHandles();
    c->setLzObjectId(QString());
    return c;
}

REGISTER_OBJECT_CREATOR(LzConnectLine, connectline)

