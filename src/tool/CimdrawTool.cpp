#include <QMap>
#include <QList>
#include <QPointF>

#include "CimdrawListWidgetItem.h"
#include "CimdrawSelectTool.h"
#include "CimdrawRectTool.h"
#include "CimdrawTopologyNodeTool.h"
#include "CimdrawPowerBusbarSectionTool.h"
#include "CimdrawPowerCircuitBreakerTool.h"
#include "CimdrawPowerDisconnectorTool.h"
#include "CimdrawPowerTransformer2wTool.h"
#include "CimdrawPowerGeneratorTool.h"
#include "CimdrawPowerMotorTool.h"
#include "CimdrawPowerLoadTool.h"
#include "CimdrawPowerGroundTool.h"
#include "CimdrawPowerCapacitorTool.h"
#include "CimdrawPowerReactorTool.h"
#include "CimdrawPowerArresterTool.h"
#include "CimdrawPowerArcCoilTool.h"
#include "CimdrawPowerCableTool.h"
#include "CimdrawPowerLoadBreakTool.h"
#include "CimdrawPowerEarthSwitchTool.h"
#include "CimdrawPowerGridTool.h"
#include "CimdrawPowerSvgCompTool.h"
#include "CimdrawPowerPvInfeedTool.h"
#include "CimdrawPowerEssTool.h"
#include "CimdrawPowerWindInfeedTool.h"
#include "CimdrawPowerStationXfmrTool.h"
#include "CimdrawPowerTransformer3wTool.h"
#include "CimdrawPowerEarthingXfmrTool.h"
#include "CimdrawPowerAutoXfmrTool.h"
#include "CimdrawPowerSplitReactorTool.h"
#include "CimdrawPowerCurrentTransformerTool.h"
#include "CimdrawPowerVoltageTransformerTool.h"
#include "CimdrawPowerFuseTool.h"
#include "CimdrawPowerJunctionTool.h"
#include "CimdrawArcTool.h"
#include "CimdrawBraceCalloutTool.h"
#include "CimdrawImageTool.h"
#include "CimdrawRoundRectTool.h"
#include "CimdrawEllipseTool.h"
#include "CimdrawConnectLineTool.h"
#include "CimdrawLineTool.h"
#include "CimdrawPolylineTool.h"
#include "CimdrawPolygonTool.h"
#include "CimdrawBezierTool.h"
#include "CimdrawTextTool.h"
#include "CimdrawTool.h"
#include "CimdrawScene.h"
#include "Item/CimdrawConnectPoint.h"
#include "Item/CimdrawHandle.h"
#include "CimdrawPowerBusbarSectionItem.h"
#include "wiring/CimdrawShapeEdgeConnect.h"
#include "Item/CimdrawConnectLine.h"
#include "CimdrawConnectConfig.h"
#include "Item/CimdrawConnectLine.h"

CimdrawTool::CimdrawTool(QObject* parent)
    :QObject(parent),scene(nullptr)
{
    createMode = CREATE_MODE_NONE;
}

CimdrawTool::~CimdrawTool()
{

}

QIcon CimdrawTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(size);
    Q_UNUSED(para);

    return QIcon();
}

QString CimdrawTool::getName() const
{
    return name;
}

CimdrawDrawTypeId CimdrawTool::getDrawType() const
{
    return drawType;
}

CREATE_MODE CimdrawTool::getCreateMode() const
{
    return createMode;
}

void CimdrawTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void CimdrawTool::onMouseRelease(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void CimdrawTool::onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void CimdrawTool::onMouseDoubleClick(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void CimdrawTool::setScene(CimdrawScene* scene)
{
    this->scene = scene;
}
CimdrawScene* CimdrawTool::getScene() const
{
    return scene;
}

static TmpShape* normalizePickedShape(TmpShape* shape)
{
    if (!shape)
        return nullptr;
    QGraphicsItem* parentItem = shape->parentItem();
    while (parentItem)
    {
        if (TmpShape* parentShape = qgraphicsitem_cast<TmpShape*>(parentItem))
        {
            if (!cimdrawIsHandle(parentShape))
                shape = parentShape;
        }
        parentItem = parentItem->parentItem();
    }
    return shape;
}

static bool shapeAcceptsScenePoint(TmpShape* shape, const QPointF& scenePos)
{
    if (!shape)
        return false;
    const QPointF local = shape->mapFromScene(scenePos);
    return shape->shape().contains(local);
}

static bool shapeAcceptsSceneRect(TmpShape* shape, const QRectF& sceneRect, bool requireContained)
{
    if (!shape || !sceneRect.isValid())
        return false;
    QPainterPath scenePath;
    scenePath.addRect(sceneRect.normalized());
    const QPainterPath localPath = shape->mapFromScene(scenePath);
    const QPainterPath itemShape = shape->shape();
    return requireContained ? localPath.contains(itemShape) : localPath.intersects(itemShape);
}

TmpShape* CimdrawTool::findShapeAtPosition(CimdrawScene* scene, const QPointF& position)
{
    if (!scene)
        return nullptr;

    TmpShape* topShape = nullptr;
    CimdrawConnectLine* topLine = nullptr;
    const QList<QGraphicsItem*> stack = scene->items(position);
    for (QGraphicsItem* item : stack)
    {
        if (cimdrawIsHandle(item) || qgraphicsitem_cast<CimdrawConnectPoint*>(item))
            continue;
        if (CimdrawConnectLine* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            if (shapeAcceptsScenePoint(line, position))
                topLine = line;
            continue;
        }
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!topShape && shapeAcceptsScenePoint(shape, position))
                topShape = normalizePickedShape(shape);
        }
    }
    if (topShape)
        return topShape;
    return topLine;
}

TmpShape* CimdrawTool::findShapeBehindAtPosition(CimdrawScene* scene, const QPointF& position)
{
    if (!scene)
        return nullptr;

    const QList<QGraphicsItem*> stack = scene->items(position);
    for (int i = stack.size() - 1; i >= 0; --i)
    {
        QGraphicsItem* item = stack[i];
        if (cimdrawIsHandle(item) || qgraphicsitem_cast<CimdrawConnectPoint*>(item))
            continue;
        if (CimdrawConnectLine* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            if (shapeAcceptsScenePoint(line, position))
                return line;
            continue;
        }
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (shapeAcceptsScenePoint(shape, position))
                return normalizePickedShape(shape);
        }
    }
    return nullptr;
}

bool CimdrawTool::shapeHitByScenePoint(TmpShape* shape, const QPointF& position)
{
    return shapeAcceptsScenePoint(shape, position);
}

bool CimdrawTool::shapeHitBySceneRect(TmpShape* shape, const QRectF& rect, bool requireContained)
{
    return shapeAcceptsSceneRect(shape, rect, requireContained);
}

namespace {

/** 命中 8 向缩放手柄（CimdrawHandle dir 1..7）时勿当作接线点；勿对 TmpShape 做 dynamic_cast，避免 MSVC 下误匹配崩溃 */
bool sceneHasResizeHandleAt(const QList<QGraphicsItem*>& items, const QPointF& scenePos)
{
    for (QGraphicsItem* item : items)
    {
        CimdrawHandle* handle = cimdrawHandleFromItem(item);
        if (!handle || !handle->scene())
            continue;
        const int d = handle->dir();
        if (d < HANDLE_POINT::LEFT_TOP || d > HANDLE_POINT::LEFT)
            continue;
        const QPointF local = handle->mapFromScene(scenePos);
        if (handle->contains(local))
            return true;
    }
    return false;
}

} // namespace

bool CimdrawTool::hitResizeHandleAt(CimdrawScene* scene, const QPointF& position)
{
    if (!scene)
        return false;
    return sceneHasResizeHandleAt(scene->items(position), position);
}

namespace {

CimdrawConnectPoint* nearestConnectPointInScene(CimdrawScene* scene, const QPointF& scenePos,
                                           qreal maxDist, CimdrawConnectPoint* exclude)
{
    if (!scene || maxDist <= 0.0)
        return nullptr;

    const QRectF searchRect(scenePos.x() - maxDist, scenePos.y() - maxDist,
                            maxDist * 2.0, maxDist * 2.0);
    CimdrawConnectPoint* best = nullptr;
    qreal bestDist = maxDist;
    const QList<QGraphicsItem*> candidates =
        scene->items(searchRect, Qt::IntersectsItemBoundingRect);

    for (QGraphicsItem* item : candidates)
    {
        CimdrawConnectPoint* cp = qgraphicsitem_cast<CimdrawConnectPoint*>(item);
        if (!cp || cp == exclude || !cp->isVisible())
            continue;
        const qreal dist = QLineF(cp->connectionCenterInScene(), scenePos).length();
        if (dist <= bestDist)
        {
            bestDist = dist;
            best = cp;
        }
    }
    return best;
}

CimdrawConnectPoint* busbarConnectPointNear(CimdrawScene* scene, const QPointF& scenePos, qreal maxSnapDist)
{
    if (!scene || maxSnapDist <= 0.0)
        return nullptr;

    const QRectF searchRect(scenePos.x() - maxSnapDist, scenePos.y() - maxSnapDist,
                            maxSnapDist * 2.0, maxSnapDist * 2.0);
    CimdrawPowerBusbarSectionItem* bestBus = nullptr;
    QPointF bestAnchor;
    qreal bestDist = maxSnapDist;

    for (QGraphicsItem* item : scene->items(searchRect, Qt::IntersectsItemBoundingRect))
    {
        auto* bus = dynamic_cast<CimdrawPowerBusbarSectionItem*>(item);
        if (!bus)
            continue;
        QPointF anchor;
        if (!bus->snapSceneToEdgePort(scenePos, maxSnapDist, anchor))
            continue;
        const qreal dist = QLineF(scenePos, anchor).length();
        if (dist <= bestDist)
        {
            bestDist = dist;
            bestBus = bus;
            bestAnchor = anchor;
        }
    }

    if (!bestBus)
        return nullptr;
    return bestBus->ensureConnectPointAtScene(bestAnchor);
}

CimdrawConnectPoint* shapeEdgeConnectPointNear(CimdrawScene* scene, const QPointF& scenePos, qreal maxSnapDist,
                                          CimdrawConnectPoint* excludeConnectPoint)
{
    if (!scene || maxSnapDist <= 0.0)
        return nullptr;

    QGraphicsItem* excludeShape =
        excludeConnectPoint ? excludeConnectPoint->parentItem() : nullptr;

    const QRectF searchRect(scenePos.x() - maxSnapDist, scenePos.y() - maxSnapDist,
                            maxSnapDist * 2.0, maxSnapDist * 2.0);
    TmpShape* bestShape = nullptr;
    QPointF bestAnchor;
    qreal bestDist = maxSnapDist;

    const QList<QGraphicsItem*> candidates =
        scene->items(searchRect, Qt::IntersectsItemBoundingRect);
    for (QGraphicsItem* item : candidates)
    {
        if (!item || item->type() == CimdrawConnectLine::Type)
            continue;
        if (item == excludeShape)
            continue;
        if (qgraphicsitem_cast<CimdrawConnectPoint*>(item))
            continue;
        if (dynamic_cast<CimdrawPowerBusbarSectionItem*>(item))
            continue;

        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || shape->parentItem())
            continue;

        QPointF anchor;
        if (!cimdrawSnapTmpShapeSceneToRectEdge(shape, scenePos, maxSnapDist, anchor))
            continue;
        const qreal dist = QLineF(scenePos, anchor).length();
        if (dist <= bestDist)
        {
            bestDist = dist;
            bestShape = shape;
            bestAnchor = anchor;
        }
    }

    if (!bestShape)
        return nullptr;
    return cimdrawEnsureTmpShapeConnectPointAtScene(bestShape, bestAnchor);
}

} // namespace

CimdrawConnectPoint* CimdrawTool::findPointAtPosition(CimdrawScene* scene, const QPointF& position,
                                            bool createBusbarConnectPoint,
                                            CimdrawConnectPoint* excludeConnectPoint)
{
    if (!scene)
        return nullptr;

    const QList<QGraphicsItem*> items = scene->items(position);

    if (sceneHasResizeHandleAt(items, position))
        return nullptr;

    for (QGraphicsItem* item : items)
    {
        if (CimdrawConnectPoint* point = qgraphicsitem_cast<CimdrawConnectPoint*>(item))
        {
            if (point != excludeConnectPoint)
                return point;
        }
    }

    if (CimdrawConnectPoint* nearPoint = nearestConnectPointInScene(
            scene, position, CimdrawConnectPointSnapRadiusPx, excludeConnectPoint))
    {
        return nearPoint;
    }

    if (!createBusbarConnectPoint)
        return nullptr;

    if (CimdrawConnectPoint* busPort = busbarConnectPointNear(scene, position, CimdrawBusbarSnapRadiusPx))
        return busPort;

    if (CimdrawConnectPoint* shapePort = shapeEdgeConnectPointNear(
            scene, position, CimdrawBusbarSnapRadiusPx, excludeConnectPoint))
        return shapePort;

    return nullptr;
}

CimdrawConnectPoint* CimdrawTool::nearestConnectPointOnShape(TmpShape* shape, const QPointF& scenePos,
                                                   qreal maxDist) const
{
    if (!shape || maxDist <= 0.0)
        return nullptr;
    CimdrawConnectPoint* best = nullptr;
    qreal bestDist = maxDist;
    for (CimdrawConnectPoint* cp : shape->wiringConnectPorts())
    {
        if (!cp || !cp->scene() || cp->parentItem() != shape || !cp->isVisible())
            continue;
        const qreal dist = QLineF(cp->connectionCenterInScene(), scenePos).length();
        if (dist <= bestDist)
        {
            bestDist = dist;
            best = cp;
        }
    }
    if (best)
        return best;
    return nullptr;
}

//***********************************************************************//
CimdrawTool* CimdrawToolManager::currentTool = nullptr;

CimdrawToolManager::CimdrawToolManager() : QObject(nullptr)
{
    QList<CimdrawTool*> regList;
    regList << new CimdrawSelectTool(this) << new CimdrawRectTool(this);
    regList << new CimdrawTopologyNodeTool(this);
    regList << new CimdrawPowerBusbarSectionTool(this) << new CimdrawPowerCircuitBreakerTool(this)
        << new CimdrawPowerDisconnectorTool(this) << new CimdrawPowerTransformer2wTool(this)
        << new CimdrawPowerGeneratorTool(this) << new CimdrawPowerMotorTool(this)
        << new CimdrawPowerLoadTool(this) << new CimdrawPowerGroundTool(this)
        << new CimdrawPowerCapacitorTool(this) << new CimdrawPowerReactorTool(this)
        << new CimdrawPowerArresterTool(this) << new CimdrawPowerArcCoilTool(this)
        << new CimdrawPowerCableTool(this) << new CimdrawPowerLoadBreakTool(this)
        << new CimdrawPowerEarthSwitchTool(this) << new CimdrawPowerGridTool(this)
        << new CimdrawPowerSvgCompTool(this) << new CimdrawPowerPvInfeedTool(this)
        << new CimdrawPowerEssTool(this) << new CimdrawPowerWindInfeedTool(this)
        << new CimdrawPowerStationXfmrTool(this) << new CimdrawPowerTransformer3wTool(this)
        << new CimdrawPowerEarthingXfmrTool(this) << new CimdrawPowerAutoXfmrTool(this)
        << new CimdrawPowerSplitReactorTool(this) << new CimdrawPowerCurrentTransformerTool(this)
        << new CimdrawPowerVoltageTransformerTool(this) << new CimdrawPowerFuseTool(this)
        << new CimdrawPowerJunctionTool(this);
    regList << new CimdrawArcTool(this) << new CimdrawImageTool(this);
    regList << new CimdrawRoundRectTool(this) << new CimdrawEllipseTool(this);
    regList << new CimdrawConnectLineTool(this) << new CimdrawLineTool(this);
    regList << new CimdrawPolylineTool(this) << new CimdrawPolygonTool(this);
    regList << new CimdrawBezierTool(this) << new CimdrawBraceCalloutTool(this) << new CimdrawTextTool(this);
    for (auto tool : regList)
    {
        registerMap.insert(tool->getName(), tool);
    }
}

CimdrawToolManager::~CimdrawToolManager()
{
    currentTool = nullptr;

    for (auto it = registerMap.begin(); it != registerMap.end(); it++)
    {
        CimdrawTool* tool = it.value();
        delete tool;
    }

    registerMap.clear();
}

CimdrawToolManager* CimdrawToolManager::getInstance()
{
    static std::once_flag s_once;
    static std::unique_ptr<CimdrawToolManager> s_instance = nullptr;
    std::call_once(s_once, []() {
        s_instance.reset(new CimdrawToolManager());
    });
    return s_instance.get();
}

CimdrawTool* CimdrawToolManager::changeTool(const QString &typeName)
{
    auto it = registerMap.find(typeName);
    if (it == registerMap.end())
        return nullptr;

    currentTool = it.value();
    return currentTool;
}

CimdrawTool* CimdrawToolManager::changeTool(CimdrawDrawTypeId type)
{
    for (auto it = registerMap.begin(); it != registerMap.end(); it++)
    {
        CimdrawTool* tool = it.value();
        if (tool->getDrawType() == type)
        {
            currentTool = tool;
            return currentTool;
        }
    }

    return nullptr;
}

CimdrawTool* CimdrawToolManager::getCurrentTool() const
{
    return currentTool;
}

void CimdrawToolManager::setCurrentTool(CimdrawTool* tool)
{
    currentTool = tool;
}
