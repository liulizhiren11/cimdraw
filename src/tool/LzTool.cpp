#include <QMap>
#include <QList>
#include <QPointF>

#include "LzListWidgetItem.h"
#include "LzSelectTool.h"
#include "LzRectTool.h"
#include "LzTopologyNodeTool.h"
#include "LzPowerBusbarSectionTool.h"
#include "LzPowerCircuitBreakerTool.h"
#include "LzPowerDisconnectorTool.h"
#include "LzPowerTransformer2wTool.h"
#include "LzPowerFeederTool.h"
#include "LzPowerGeneratorTool.h"
#include "LzPowerMotorTool.h"
#include "LzPowerLoadTool.h"
#include "LzPowerGroundTool.h"
#include "LzPowerCapacitorTool.h"
#include "LzPowerReactorTool.h"
#include "LzPowerArresterTool.h"
#include "LzPowerArcCoilTool.h"
#include "LzPowerCableTool.h"
#include "LzPowerLoadBreakTool.h"
#include "LzPowerEarthSwitchTool.h"
#include "LzPowerGridTool.h"
#include "LzPowerSvgCompTool.h"
#include "LzPowerPvInfeedTool.h"
#include "LzPowerEssTool.h"
#include "LzPowerWindInfeedTool.h"
#include "LzPowerStationXfmrTool.h"
#include "LzPowerTransformer3wTool.h"
#include "LzPowerEarthingXfmrTool.h"
#include "LzPowerAutoXfmrTool.h"
#include "LzPowerSplitReactorTool.h"
#include "LzPowerCurrentTransformerTool.h"
#include "LzPowerVoltageTransformerTool.h"
#include "LzPowerFuseTool.h"
#include "LzPowerJunctionTool.h"
#include "LzArcTool.h"
#include "LzBraceCalloutTool.h"
#include "LzImageTool.h"
#include "LzRoundRectTool.h"
#include "LzEllipseTool.h"
#include "LzConnectLineTool.h"
#include "LzLineTool.h"
#include "LzPolylineTool.h"
#include "LzPolygonTool.h"
#include "LzBezierTool.h"
#include "LzTextTool.h"
#include "LzTool.h"
#include "LzScene.h"
#include "Item/LzConnectPoint.h"
#include "Item/LzHandle.h"
#include "LzPowerBusbarSectionItem.h"
#include "wiring/LzShapeEdgeConnect.h"
#include "Item/LzConnectLine.h"
#include "LzConnectConfig.h"
#include "Item/LzConnectLine.h"

LzTool::LzTool(QObject* parent)
    :QObject(parent),scene(nullptr)
{
    createMode = CREATE_MODE_NONE;
}

LzTool::~LzTool()
{

}

QIcon LzTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(size);
    Q_UNUSED(para);

    return QIcon();
}

QString LzTool::getName() const
{
    return name;
}

LzDrawTypeId LzTool::getDrawType() const
{
    return drawType;
}

CREATE_MODE LzTool::getCreateMode() const
{
    return createMode;
}

void LzTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent *evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void LzTool::onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent *evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void LzTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void LzTool::onMouseDoubleClick(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    Q_UNUSED(scene);
    Q_UNUSED(evt);
}

void LzTool::setScene(LzScene* scene)
{
    this->scene = scene;
}
LzScene* LzTool::getScene() const
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
            if (!lzIsHandle(parentShape))
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

TmpShape* LzTool::findShapeAtPosition(LzScene* scene, const QPointF& position)
{
    if (!scene)
        return nullptr;

    TmpShape* topShape = nullptr;
    LzConnectLine* topLine = nullptr;
    const QList<QGraphicsItem*> stack = scene->items(position);
    for (QGraphicsItem* item : stack)
    {
        if (lzIsHandle(item) || qgraphicsitem_cast<LzConnectPoint*>(item))
            continue;
        if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(item))
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

TmpShape* LzTool::findShapeBehindAtPosition(LzScene* scene, const QPointF& position)
{
    if (!scene)
        return nullptr;

    const QList<QGraphicsItem*> stack = scene->items(position);
    for (int i = stack.size() - 1; i >= 0; --i)
    {
        QGraphicsItem* item = stack[i];
        if (lzIsHandle(item) || qgraphicsitem_cast<LzConnectPoint*>(item))
            continue;
        if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(item))
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

bool LzTool::shapeHitByScenePoint(TmpShape* shape, const QPointF& position)
{
    return shapeAcceptsScenePoint(shape, position);
}

bool LzTool::shapeHitBySceneRect(TmpShape* shape, const QRectF& rect, bool requireContained)
{
    return shapeAcceptsSceneRect(shape, rect, requireContained);
}

namespace {

/** 命中 8 向缩放手柄（LzHandle dir 1..7）时勿当作接线点；勿对 TmpShape 做 dynamic_cast，避免 MSVC 下误匹配崩溃 */
bool sceneHasResizeHandleAt(const QList<QGraphicsItem*>& items, const QPointF& scenePos)
{
    for (QGraphicsItem* item : items)
    {
        LzHandle* handle = lzHandleFromItem(item);
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

bool LzTool::hitResizeHandleAt(LzScene* scene, const QPointF& position)
{
    if (!scene)
        return false;
    return sceneHasResizeHandleAt(scene->items(position), position);
}

namespace {

LzConnectPoint* nearestConnectPointInScene(LzScene* scene, const QPointF& scenePos,
                                           qreal maxDist, LzConnectPoint* exclude)
{
    if (!scene || maxDist <= 0.0)
        return nullptr;

    const QRectF searchRect(scenePos.x() - maxDist, scenePos.y() - maxDist,
                            maxDist * 2.0, maxDist * 2.0);
    LzConnectPoint* best = nullptr;
    qreal bestDist = maxDist;
    const QList<QGraphicsItem*> candidates =
        scene->items(searchRect, Qt::IntersectsItemBoundingRect);

    for (QGraphicsItem* item : candidates)
    {
        LzConnectPoint* cp = qgraphicsitem_cast<LzConnectPoint*>(item);
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

LzConnectPoint* busbarConnectPointNear(LzScene* scene, const QPointF& scenePos, qreal maxSnapDist)
{
    if (!scene || maxSnapDist <= 0.0)
        return nullptr;

    const QRectF searchRect(scenePos.x() - maxSnapDist, scenePos.y() - maxSnapDist,
                            maxSnapDist * 2.0, maxSnapDist * 2.0);
    LzPowerBusbarSectionItem* bestBus = nullptr;
    QPointF bestAnchor;
    qreal bestDist = maxSnapDist;

    for (QGraphicsItem* item : scene->items(searchRect, Qt::IntersectsItemBoundingRect))
    {
        auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(item);
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

LzConnectPoint* shapeEdgeConnectPointNear(LzScene* scene, const QPointF& scenePos, qreal maxSnapDist,
                                          LzConnectPoint* excludeConnectPoint)
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
        if (!item || item->type() == LzConnectLine::Type)
            continue;
        if (item == excludeShape)
            continue;
        if (qgraphicsitem_cast<LzConnectPoint*>(item))
            continue;
        if (dynamic_cast<LzPowerBusbarSectionItem*>(item))
            continue;

        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || shape->parentItem())
            continue;

        QPointF anchor;
        if (!lzSnapTmpShapeSceneToRectEdge(shape, scenePos, maxSnapDist, anchor))
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
    return lzEnsureTmpShapeConnectPointAtScene(bestShape, bestAnchor);
}

} // namespace

LzConnectPoint* LzTool::findPointAtPosition(LzScene* scene, const QPointF& position,
                                            bool createBusbarConnectPoint,
                                            LzConnectPoint* excludeConnectPoint)
{
    if (!scene)
        return nullptr;

    const QList<QGraphicsItem*> items = scene->items(position);

    if (sceneHasResizeHandleAt(items, position))
        return nullptr;

    for (QGraphicsItem* item : items)
    {
        if (LzConnectPoint* point = qgraphicsitem_cast<LzConnectPoint*>(item))
        {
            if (point != excludeConnectPoint)
                return point;
        }
    }

    if (LzConnectPoint* nearPoint = nearestConnectPointInScene(
            scene, position, LzConnectPointSnapRadiusPx, excludeConnectPoint))
    {
        return nearPoint;
    }

    if (!createBusbarConnectPoint)
        return nullptr;

    if (LzConnectPoint* busPort = busbarConnectPointNear(scene, position, LzBusbarSnapRadiusPx))
        return busPort;

    return nullptr;
}

LzConnectPoint* LzTool::nearestConnectPointOnShape(TmpShape* shape, const QPointF& scenePos,
                                                   qreal maxDist) const
{
    if (!shape || maxDist <= 0.0)
        return nullptr;
    LzConnectPoint* best = nullptr;
    qreal bestDist = maxDist;
    for (LzConnectPoint* cp : shape->wiringConnectPorts())
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
LzTool* LzToolManager::currentTool = nullptr;

LzToolManager::LzToolManager() : QObject(nullptr)
{
    QList<LzTool*> regList;
    regList << new LzSelectTool(this) << new LzRectTool(this);
    regList << new LzTopologyNodeTool(this);
    regList << new LzPowerBusbarSectionTool(this) << new LzPowerCircuitBreakerTool(this) << new LzPowerDisconnectorTool(this)
        << new LzPowerTransformer2wTool(this) << new LzPowerFeederTool(this) << new LzPowerGeneratorTool(this)
        << new LzPowerMotorTool(this)
        << new LzPowerLoadTool(this) << new LzPowerGroundTool(this)
        << new LzPowerCapacitorTool(this) << new LzPowerReactorTool(this)
        << new LzPowerArresterTool(this) << new LzPowerArcCoilTool(this)
        << new LzPowerCableTool(this) << new LzPowerLoadBreakTool(this)
        << new LzPowerEarthSwitchTool(this)
        << new LzPowerGridTool(this) << new LzPowerSvgCompTool(this) << new LzPowerPvInfeedTool(this)
        << new LzPowerEssTool(this) << new LzPowerWindInfeedTool(this) << new LzPowerStationXfmrTool(this)
        << new LzPowerTransformer3wTool(this) << new LzPowerEarthingXfmrTool(this) << new LzPowerAutoXfmrTool(this)
        << new LzPowerSplitReactorTool(this) << new LzPowerCurrentTransformerTool(this)
        << new LzPowerVoltageTransformerTool(this) << new LzPowerFuseTool(this)
        << new LzPowerJunctionTool(this);
    regList << new LzArcTool(this) << new LzImageTool(this);
    regList << new LzRoundRectTool(this) << new LzEllipseTool(this);
    regList << new LzConnectLineTool(this) << new LzLineTool(this);
    regList << new LzPolylineTool(this) << new LzPolygonTool(this);
    regList << new LzBezierTool(this) << new LzBraceCalloutTool(this) << new LzTextTool(this);
    for (auto tool : regList)
    {
        registerMap.insert(tool->getName(), tool);
    }
}

LzToolManager::~LzToolManager()
{
    currentTool = nullptr;

    for (auto it = registerMap.begin(); it != registerMap.end(); it++)
    {
        LzTool* tool = it.value();
        delete tool;
    }

    registerMap.clear();
}

LzToolManager* LzToolManager::getInstance()
{
    static std::once_flag s_once;
    static std::unique_ptr<LzToolManager> s_instance = nullptr;
    std::call_once(s_once, []() {
        s_instance.reset(new LzToolManager());
    });
    return s_instance.get();
}

LzTool* LzToolManager::changeTool(const QString &typeName)
{
    auto it = registerMap.find(typeName);
    if (it == registerMap.end())
        return nullptr;

    currentTool = it.value();
    return currentTool;
}

LzTool* LzToolManager::changeTool(LzDrawTypeId type)
{
    for (auto it = registerMap.begin(); it != registerMap.end(); it++)
    {
        LzTool* tool = it.value();
        if (tool->getDrawType() == type)
        {
            currentTool = tool;
            return currentTool;
        }
    }

    return nullptr;
}

LzTool* LzToolManager::getCurrentTool() const
{
    return currentTool;
}

void LzToolManager::setCurrentTool(LzTool* tool)
{
    currentTool = tool;
}
