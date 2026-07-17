#include "CimdrawShapeEdgeConnect.h"

#include <algorithm>
#include <limits>

#include <QLineF>
#include <QGraphicsItem>

#include "Item/TmpBase.h"
#include "Item/CimdrawConnectPoint.h"
#include "Item/CimdrawConnectLine.h"
#include "CimdrawConnectConfig.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"

namespace {

static constexpr qreal kReuseAlongEdgeScenePx = 12.0;

bool closestPointOnRectPerimeter(const QRectF& rect, const QPointF& local,
                                 QPointF& onEdge, qreal& distance)
{
    if (!rect.isValid())
        return false;

    const qreal left = rect.left();
    const qreal right = rect.right();
    const qreal top = rect.top();
    const qreal bottom = rect.bottom();

    const QPointF candidates[4] = {
        QPointF(qBound(left, local.x(), right), top),
        QPointF(right, qBound(top, local.y(), bottom)),
        QPointF(qBound(left, local.x(), right), bottom),
        QPointF(left, qBound(top, local.y(), bottom)),
    };

    distance = std::numeric_limits<qreal>::max();
    onEdge = candidates[0];
    for (const QPointF& c : candidates)
    {
        const qreal d = QLineF(local, c).length();
        if (d < distance)
        {
            distance = d;
            onEdge = c;
        }
    }
    return distance < std::numeric_limits<qreal>::max();
}

CONNECT_DIRECTION directionForRectEdge(const QRectF& rect, const QPointF& onEdge)
{
    if (qAbs(onEdge.y() - rect.top()) < 0.5)
        return TOP_DIRECTION;
    if (qAbs(onEdge.x() - rect.right()) < 0.5)
        return RIGHT_DIRECTION;
    if (qAbs(onEdge.y() - rect.bottom()) < 0.5)
        return BOTTOM_DIRECTION;
    return LEFT_DIRECTION;
}

bool isLiveConnectPointOnShape(const CimdrawConnectPoint* port, const QGraphicsItem* shape)
{
    return port && port->scene() && port->parentItem() == shape;
}

bool isDynamicWiringPort(const CimdrawConnectPoint* port)
{
    return isLiveConnectPointOnShape(port, port ? port->parentItem() : nullptr)
           && port->dir() > LEFT_DIRECTION;
}

} // namespace

void cimdrawPruneStaleConnectPointsOnShape(TmpBase<QGraphicsItem>* shape)
{
    if (!shape)
        return;

    QVector<CimdrawConnectPoint*> livePoints;
    livePoints.reserve(shape->childItems().size());
    for (QGraphicsItem* child : shape->childItems())
    {
        CimdrawConnectPoint* point = qgraphicsitem_cast<CimdrawConnectPoint*>(child);
        if (!point || point->parentItem() != shape || !point->scene())
            continue;
        livePoints.push_back(point);
    }

    shape->pointStruct.points = livePoints;
    shape->pointStruct.count = static_cast<ConnectCount>(shape->pointStruct.points.size());
}

bool cimdrawSnapTmpShapeSceneToRectEdge(const TmpBase<QGraphicsItem>* shape, const QPointF& scenePos,
                                   qreal maxDist,
                                   QPointF& anchorScene)
{
    if (!shape || maxDist <= 0.0)
        return false;
    if (dynamic_cast<const CimdrawPowerBusbarSectionItem*>(shape))
        return false;

    const QRectF body = shape->boundingRect();
    if (!body.isValid() || body.width() < 1.0 || body.height() < 1.0)
        return false;

    const QPointF local = shape->mapFromScene(scenePos);
    QPointF onEdgeLocal;
    qreal localDist = 0.0;
    if (!closestPointOnRectPerimeter(body, local, onEdgeLocal, localDist))
        return false;

    anchorScene = shape->mapToScene(onEdgeLocal);
    const qreal sceneDist = QLineF(scenePos, anchorScene).length();
    return sceneDist <= maxDist && localDist <= maxDist * 2.0;
}

CimdrawConnectPoint* cimdrawEnsureTmpShapeConnectPointAtScene(TmpBase<QGraphicsItem>* shape, const QPointF& scenePos)
{
    if (!shape)
        return nullptr;

    cimdrawPruneStaleConnectPointsOnShape(shape);

    QPointF anchorScene;
    if (!cimdrawSnapTmpShapeSceneToRectEdge(shape, scenePos, CimdrawConnectPointSnapRadiusPx, anchorScene))
        return nullptr;

    const QRectF body = shape->boundingRect();
    const QPointF onEdgeLocal = shape->mapFromScene(anchorScene);
    const CONNECT_DIRECTION dir = directionForRectEdge(body, onEdgeLocal);

    const QPointF center = body.center();
    const qreal w = qMax<qreal>(1.0, body.width());
    const qreal h = qMax<qreal>(1.0, body.height());
    const QPointF norm((onEdgeLocal.x() - center.x()) / w, (onEdgeLocal.y() - center.y()) / h);

    for (CimdrawConnectPoint* p : shape->wiringConnectPorts())
    {
        if (!isLiveConnectPointOnShape(p, shape))
            continue;
        if (p->getDirection() == dir
            && QLineF(p->connectionCenterInScene(), anchorScene).length() <= kReuseAlongEdgeScenePx)
        {
            return p;
        }
    }

    auto* point = new CimdrawConnectPoint(shape);
    point->setDir(static_cast<int>(shape->wiringConnectPorts().size()));
    point->setDirection(dir);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(shape, norm);
    shape->pointStruct.points.push_back(point);
    shape->pointStruct.count = static_cast<ConnectCount>(shape->pointStruct.points.size());
    shape->updatePoints();
    point->update();
    return point;
}

void cimdrawRemoveTmpShapeDynamicConnectPointIfUnused(TmpBase<QGraphicsItem>* shape, CimdrawConnectPoint* port)
{
    if (!shape || !isLiveConnectPointOnShape(port, shape) || !isDynamicWiringPort(port))
        return;

    for (QGraphicsItem* item : shape->connectStruct.connects)
    {
        const auto* line = qgraphicsitem_cast<const CimdrawConnectLine*>(item);
        if (!line)
            continue;
        if (line->startConnectPort() == port || line->endConnectPort() == port)
            return;
    }

    if (!shape->pointStruct.points.removeOne(port))
        return;
    shape->pointStruct.count = static_cast<ConnectCount>(shape->pointStruct.points.size());
    delete port;
    shape->updatePoints();
    shape->update();
}

void cimdrawCleanupAfterConnectLineRemoved(CimdrawConnectLine* line)
{
    if (!line)
        return;

    QGraphicsItem* const startItem = line->getStartItem();
    QGraphicsItem* const endItem = line->getEndItem();
    CimdrawConnectPoint* const startPort = line->startConnectPort();
    CimdrawConnectPoint* const endPort = line->endConnectPort();

    if (TmpShape* startShape = qgraphicsitem_cast<TmpShape*>(startItem))
        startShape->removeConnect(line);
    if (TmpShape* endShape = qgraphicsitem_cast<TmpShape*>(endItem))
        endShape->removeConnect(line);

    const auto prunePort = [line](QGraphicsItem* item, CimdrawConnectPoint* port) {
        if (!port || !item)
            return;
        if (auto* bus = dynamic_cast<CimdrawPowerBusbarSectionItem*>(item))
        {
            bus->removeConnectPointIfUnused(port);
            if (line->startConnectPort() == port)
                line->setStartConnectPort(nullptr);
            if (line->endConnectPort() == port)
                line->setEndConnectPort(nullptr);
            return;
        }
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            cimdrawRemoveTmpShapeDynamicConnectPointIfUnused(shape, port);
            if (line->startConnectPort() == port)
                line->setStartConnectPort(nullptr);
            if (line->endConnectPort() == port)
                line->setEndConnectPort(nullptr);
        }
    };

    prunePort(startItem, startPort);
    prunePort(endItem, endPort);
}
