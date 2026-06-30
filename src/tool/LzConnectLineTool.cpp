#include <QPainter>

#include "LzScene.h"
#include "LzView.h"
#include "Item/LzConnectLine.h"
#include "Item/LzConnectPoint.h"
#include "Command/LzAddCommand.h"
#include "LzConnectLineTool.h"
#include "LzConnectConfig.h"
#include "LzWiringItemBase.h"
#include "LzPowerBusbarSectionItem.h"
#include "Item/TmpBase.h"
#include "wiring/LzShapeEdgeConnect.h"

namespace {

void lzCleanupBusbarPortAfterWireAbort(TmpShape* startItem, LzConnectPoint* port)
{
    if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(startItem))
        bus->removeConnectPointIfUnused(port);
}

} // namespace

// 拖动时仅当光标移动超过该像素数才做完整 A* 路径，否则用简单 L 形预览，减轻卡顿
static constexpr qreal kDragPathUpdateDistance = 32.0;
/** 松手到空白处生成悬空边的最小长度（像素） */
static constexpr qreal kMinFloatingWireLengthPx = 12.0;

bool lzWiringConnectEndpointsCompatible(TmpShape* start, TmpShape* end)
{
    if (!start || !end)
        return false;
    // 接线图 SLD（含母线）可与断路器/刀闸等拓扑图元相连
    if (dynamic_cast<LzWiringItemBase*>(start) || dynamic_cast<LzWiringItemBase*>(end))
        return true;
    return start->isTopologyGraphNode() == end->isTopologyGraphNode();
}

class LzConnectLineToolPrivate
{
public:
    LzConnectLineToolPrivate(LzConnectLineTool* tool)
        :q_ptr(tool)
    {

    }
    TmpShape* startItem = nullptr;
    TmpShape* endItem = nullptr;
    LzConnectLine* connectLine = nullptr;
    LzConnectPoint* startConnectPoint = nullptr;  // 起始连接点，用于 draw.io 风格：释放在同一点则取消
    QPointF downPoint = QPointF();
    QPointF lastPoint = QPointF();
    QPointF lastFullPathUpdatePos = QPointF();  // 上次做完整路径规划时的光标位置，用于节流
    LzConnectLineTool* q_ptr;
};

static QPointF wireReleaseScenePos(LzConnectLineToolPrivate* d, const QPointF& releasePos,
                                   Qt::KeyboardModifiers mods)
{
    if (!d || !d->startConnectPoint)
        return releasePos;
    QPointF pos = releasePos;
    if (mods & Qt::ShiftModifier)
    {
        const QPointF origin = d->startConnectPoint->connectionCenterInScene();
        const qreal dx = qAbs(pos.x() - origin.x());
        const qreal dy = qAbs(pos.y() - origin.y());
        if (dx >= dy)
            pos.setY(origin.y());
        else
            pos.setX(origin.x());
    }
    return pos;
}

static void commitFinishedConnectLine(LzConnectLineToolPrivate* d, LzScene* scene,
                                      bool switchToSelection)
{
    if (!d || !scene || !d->connectLine)
        return;
    QList<QGraphicsItem*> list = scene->getSelections();
    scene->setPaintState(false);
    d->startItem = nullptr;
    d->endItem = nullptr;
    d->startConnectPoint = nullptr;
    LzView* view = scene->getView();
    if (view)
    {
        if (QWidget* vp = view->viewport(); vp && QWidget::mouseGrabber() == vp)
            vp->releaseMouse();
        LzAddCommand* command = new LzAddCommand(d->connectLine, scene, list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
    d->connectLine = nullptr;
    if (switchToSelection)
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
}

LzConnectLineTool::LzConnectLineTool(QObject* parent) 
    : LzTool(parent),d_ptr(new LzConnectLineToolPrivate(this))
{
    name = tr("connectLine");
    createMode = CREATE_MODE_PRESS;
}

LzConnectLineTool::~LzConnectLineTool()
{

}

bool LzConnectLineTool::hasActiveWire() const
{
    return d_ptr->connectLine != nullptr;
}

void LzConnectLineTool::abortCurrentWire(LzScene* scene, bool switchToSelection)
{
    if (!scene || !d_ptr->connectLine)
        return;

    scene->removeSelection(d_ptr->connectLine);
    if (d_ptr->startItem)
        d_ptr->startItem->removeConnect(d_ptr->connectLine);
    scene->removeSceneItem(d_ptr->connectLine);
    lzCleanupBusbarPortAfterWireAbort(d_ptr->startItem, d_ptr->startConnectPoint);
    delete d_ptr->connectLine;
    d_ptr->connectLine = nullptr;
    d_ptr->startItem = nullptr;
    d_ptr->endItem = nullptr;
    d_ptr->startConnectPoint = nullptr;
    scene->setPaintState(false);
    currentSelectMode = MODE_NONE;
    currentDragHandle = HANDLE_NONE;

    if (LzView* view = scene->getView())
    {
        if (QWidget* vp = view->viewport(); vp && QWidget::mouseGrabber() == vp)
            vp->releaseMouse();
        view->unsetCursor();
    }

    if (switchToSelection)
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
}

LzDrawTypeId LzConnectLineTool::getDrawType() const
{
    return LZ_CONNECT_LINE;
}

QGraphicsItem* LzConnectLineTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    d_ptr->connectLine = new LzConnectLine(*pos);
    d_ptr->connectLine->setDrawTool(this);
    if (d_ptr->connectLine)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                d_ptr->connectLine->setSelected(true);
            }
        }
    }
    return d_ptr->connectLine;
}

QIcon LzConnectLineTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    p.setRenderHint(QPainter::Antialiasing);
    LzConnectLine::paintToolboxIcon(&p, QRectF(0, 0, size.width(), size.height()));

    return QIcon(map);
}

void LzConnectLineTool::beginWireFromConnectPoint(LzScene* scene, LzConnectPoint* clickedShape,
                                                  const QPointF& scenePos)
{
    if (!scene || !clickedShape || d_ptr->connectLine)
        return;

    d_ptr->downPoint = scenePos;
    const QPointF startCenter = clickedShape->connectionCenterInScene();
    QRectF rc(startCenter, QSizeF(0, 0));
    createObject(scene, &rc);
    d_ptr->connectLine->setPos(0, 0);
    d_ptr->startItem = qgraphicsitem_cast<TmpShape*>(clickedShape->parentItem());
    if (!d_ptr->startItem)
    {
        delete d_ptr->connectLine;
        d_ptr->connectLine = nullptr;
        return;
    }
    d_ptr->connectLine->setStartItem(d_ptr->startItem);
    d_ptr->connectLine->setStartConnectPort(clickedShape);
    d_ptr->connectLine->setConnectStrategy(CONNECTION_STRATEGY::BROKEN_CONNECTION);
    d_ptr->startItem->addConnect(d_ptr->connectLine);
    d_ptr->startConnectPoint = clickedShape;
    scene->addItem(d_ptr->connectLine);
    d_ptr->connectLine->addPoint(startCenter);
    scene->addSelection(d_ptr->connectLine);
    currentSelectMode = MODE_SIZE;
    currentDragHandle = d_ptr->connectLine->handleCount();
    d_ptr->lastFullPathUpdatePos = startCenter;
    scene->setPaintState(true);
    if (LzView* view = scene->getView())
    {
        view->setCursor(Qt::ArrowCursor);
        if (QWidget* vp = view->viewport(); vp && QWidget::mouseGrabber() != vp)
            vp->grabMouse();
    }
}

void LzConnectLineTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    d_ptr->downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(d_ptr->connectLine)
        {
            LzConnectPoint* clickedShape = findPointAtPosition(scene, d_ptr->downPoint, true);
            if (!clickedShape)
            {
                abortCurrentWire(scene, false);
                return;
            }
            d_ptr->endItem = qgraphicsitem_cast<TmpShape*>(clickedShape->parentItem());
            if (!d_ptr->endItem)
            {
                abortCurrentWire(scene, false);
                return;
            }
            if (!lzWiringConnectEndpointsCompatible(d_ptr->startItem, d_ptr->endItem))
            {
                abortCurrentWire(scene, false);
                return;
            }
            d_ptr->connectLine->attachEndAndRecompute(clickedShape->parentItem(), clickedShape);
            commitFinishedConnectLine(d_ptr, scene, true);
        }
        currentSelectMode = MODE_NONE;
        scene->clearSelection();
        return;
    }
    if (!d_ptr->connectLine)
    {
        LzConnectPoint* clickedShape = findPointAtPosition(scene, d_ptr->downPoint, true);
        if (!clickedShape)
        {
            delete d_ptr->connectLine;
            d_ptr->connectLine = nullptr;
            return;
        }
        beginWireFromConnectPoint(scene, clickedShape, d_ptr->downPoint);
    }
    else
    {
        LzConnectPoint* clickedShape = findPointAtPosition(scene, d_ptr->downPoint, true);
        if (!clickedShape)
        {
            abortCurrentWire(scene, false);
            return;
        }
        d_ptr->endItem = qgraphicsitem_cast<TmpShape*>(clickedShape->parentItem());
        if (!d_ptr->endItem)
        {
            abortCurrentWire(scene, false);
            return;
        }
        if (!lzWiringConnectEndpointsCompatible(d_ptr->startItem, d_ptr->endItem))
        {
            abortCurrentWire(scene, false);
            return;
        }
        d_ptr->connectLine->attachEndAndRecompute(clickedShape->parentItem(), clickedShape);
        commitFinishedConnectLine(d_ptr, scene, true);
        currentSelectMode = MODE_NONE;
    }
}

void LzConnectLineTool::onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    // draw.io 风格：拖拽到终点释放则完成连线，否则取消；完成后切回选择工具
    if (!d_ptr->connectLine)
        return;

    const QPointF releasePos = wireReleaseScenePos(d_ptr, evt->scenePos(), evt->modifiers());
    LzConnectPoint* endPoint =
        findPointAtPosition(scene, releasePos, true, d_ptr->startConnectPoint);

    if (endPoint && endPoint != d_ptr->startConnectPoint)
    {
        TmpShape* endShape = qgraphicsitem_cast<TmpShape*>(endPoint->parentItem());
        if (endShape && endShape != d_ptr->startItem)
        {
            if (!lzWiringConnectEndpointsCompatible(d_ptr->startItem, endShape))
            {
                abortCurrentWire(scene);
                return;
            }
            d_ptr->connectLine->attachEndAndRecompute(endPoint->parentItem(), endPoint);
            commitFinishedConnectLine(d_ptr, scene, true);
            currentSelectMode = MODE_NONE;
            return;
        }
    }

    const QPointF origin = d_ptr->startConnectPoint->connectionCenterInScene();
    if (QLineF(origin, releasePos).length() < kMinFloatingWireLengthPx)
    {
        abortCurrentWire(scene);
        return;
    }

    d_ptr->connectLine->finalizeWireWithFloatingEnd(releasePos);
    commitFinishedConnectLine(d_ptr, scene, true);
    currentSelectMode = MODE_NONE;
}

void LzConnectLineTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        return;
    }
    if (d_ptr->connectLine)
    {
        QPointF cursorPos = evt->scenePos();
        // 预览阶段仅吸附已有端口，勿每帧创建沿边动态点（否则 pointStruct 易产生悬空指针崩溃）
        if (LzConnectPoint* snapPort = findPointAtPosition(
                scene, cursorPos, false, d_ptr->startConnectPoint))
        {
            cursorPos = snapPort->connectionCenterInScene();
        }

        QPointF previewEnd = cursorPos;
        // Shift：相对起点锁定水平/垂直（Ctrl 已用于「从图元拉线」，勿再约束轨迹）
        if (evt->modifiers() & Qt::ShiftModifier && d_ptr->startConnectPoint)
        {
            const QPointF origin = d_ptr->startConnectPoint->connectionCenterInScene();
            const qreal dx = qAbs(previewEnd.x() - origin.x());
            const qreal dy = qAbs(previewEnd.y() - origin.y());
            if (dx >= dy)
                previewEnd.setY(origin.y());
            else
                previewEnd.setX(origin.x());
        }

        constexpr qreal kPreviewMinMovePx = 4.0;
        const qreal dist = QLineF(d_ptr->lastFullPathUpdatePos, previewEnd).length();
        if (dist >= kPreviewMinMovePx)
        {
            d_ptr->connectLine->setWireDragPreviewEnd(previewEnd);
            d_ptr->lastFullPathUpdatePos = previewEnd;
        }
    }
}
