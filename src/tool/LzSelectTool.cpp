#include <QPainter>
#include <QPen>
#include <QApplication>
#include <QDebug>
#include <QSet>
#include <QTimer>
#include "LzTool.h"
#include "LzScene.h"
#include "LzView.h"
#include "LzSelectTool.h"
#include "LzConnectLineTool.h"
#include "LzPowerBusbarSectionItem.h"
#include "LzConnectConfig.h"
#include "Item/LzConnectLine.h"
#include "Item/LzConnectPoint.h"
#include "Item/LzItem.h"
#include "LzCursorManager.h"
#include "LzCursorTypeRegistry.h"
#include "command/LzMoveCommand.h"
#include "command/LzReSizeCommand.h"
#include "command/LzEditConnectLinePathCommand.h"

namespace {

constexpr qreal kBusbarWireDragThresholdPx = 2.0;
/** 距母线上下沿在此范围内：Ctrl + 点击母线沿边拉线 */
constexpr qreal kBusbarEdgeWireIntentPx = 16.0;

bool lzBusbarGeometryEditGesture(Qt::KeyboardModifiers mods)
{
    return (mods & Qt::ShiftModifier) != 0;
}

LzPowerBusbarSectionItem* lzBusbarFromConnectPoint(LzConnectPoint* point)
{
    return point ? dynamic_cast<LzPowerBusbarSectionItem*>(point->parentItem()) : nullptr;
}

QList<QGraphicsItem*> expandMoveSelectionWithFullySelectedConnectLines(const QList<QGraphicsItem*>& selections)
{
    QList<QGraphicsItem*> moveItems = selections;
    QSet<QGraphicsItem*> selectionSet(selections.begin(), selections.end());
    QSet<QGraphicsItem*> moveSet(moveItems.begin(), moveItems.end());

    for (QGraphicsItem* item : selections)
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(item);
        if (!line)
            continue;

        QGraphicsItem* startItem = line->getStartItem();
        QGraphicsItem* endItem = line->getEndItem();
        const bool startSelected = !startItem || selectionSet.contains(startItem);
        const bool endSelected = !endItem || selectionSet.contains(endItem);
        if (!startSelected || !endSelected)
            continue;

        if (startItem && !moveSet.contains(startItem))
        {
            moveItems.append(startItem);
            moveSet.insert(startItem);
        }
        if (endItem && !moveSet.contains(endItem))
        {
            moveItems.append(endItem);
            moveSet.insert(endItem);
        }
    }

    for (QGraphicsItem* item : selections)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape)
            continue;
        for (QGraphicsItem* connectedItem : shape->connectedItems())
        {
            auto* line = qgraphicsitem_cast<LzConnectLine*>(connectedItem);
            if (!line || !selectionSet.contains(line) || moveSet.contains(line))
                continue;

            QGraphicsItem* startItem = line->getStartItem();
            QGraphicsItem* endItem = line->getEndItem();
            const bool startSelected = !startItem || selectionSet.contains(startItem);
            const bool endSelected = !endItem || selectionSet.contains(endItem);
            if (!startSelected || !endSelected)
                continue;

            moveItems.append(line);
            moveSet.insert(line);
        }
    }

    return moveItems;
}
}

LzSelectTool::LzSelectTool(QObject* parent) : LzTool(parent)
{
    name = tr("select");
}

LzSelectTool::~LzSelectTool()
{

}

LzDrawTypeId LzSelectTool::getDrawType() const
{
    return LZ_SELECTION;
}

QGraphicsItem* LzSelectTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    Q_UNUSED(scene);
    Q_UNUSED(pos);
    Q_UNUSED(addToScene);

    return nullptr;
}

QIcon LzSelectTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(size);
    Q_UNUSED(para);

    return QIcon();
}

void LzSelectTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        scene->cleanSelection();
        emit scene->currentObjectChanged({});  // 清空时触发信号
        return;
    }
    const QPointF pressPos = evt->scenePos();
    const bool wiringGesture = lzWiringGestureActive(evt->modifiers());
    const bool busbarGeomEdit = lzBusbarGeometryEditGesture(evt->modifiers());

    auto tryStartWireFromPort = [&](LzConnectPoint* port) -> bool {
        if (!port || (busbarGeomEdit && lzBusbarFromConnectPoint(port)))
            return false;
        LzTool* connectTool = LzToolManager::getInstance()->changeTool(LZ_CONNECT_LINE);
        if (!connectTool)
            return false;
        connectTool->onMousePress(scene, evt);
        return true;
    };

    // Ctrl + 点端口/母线边：连线；普通点击优先用于选择图元/连接线
    if (wiringGesture && !busbarGeomEdit && !LzTool::hitResizeHandleAt(scene, pressPos))
    {
        if (LzConnectPoint* connectPoint = findPointAtPosition(scene, pressPos, false))
        {
            if (tryStartWireFromPort(connectPoint))
                return;
        }
    }

    // Ctrl + 端口：从该端口拉线；普通点击不抢选中
    if (wiringGesture)
    {
        if (LzConnectPoint* connectPoint = findPointAtPosition(scene, pressPos, false))
        {
            if (tryStartWireFromPort(connectPoint))
                return;
        }
    }

    resetState();
    lastPoint = pressPos;
    downPoint = pressPos;

    TmpShape* clickedShape = (evt->modifiers() & Qt::AltModifier)
        ? findShapeBehindAtPosition(scene, lastPoint)
        : findShapeAtPosition(scene, lastPoint);

    // Ctrl + 母线沿边 / Ctrl + 图元本体：从固定端口或母线新端口拉线；Shift 仅几何编辑
    if (wiringGesture && !busbarGeomEdit && !hitResizeHandleAt(scene, pressPos))
    {
        if (auto* bus = dynamic_cast<LzPowerBusbarSectionItem*>(clickedShape))
        {
            QPointF anchor;
            if (bus->snapSceneToEdgePort(pressPos, kBusbarEdgeWireIntentPx, anchor))
            {
                pendingBusbarWire = true;
                pendingBusbarWireDown = pressPos;
                pendingBusbar = bus;
                pendingBusbarTouchPort = nullptr;
                pendingBusbarPortNeedsCleanup = false;
            }
        }
        else if (clickedShape)
        {
            if (LzConnectPoint* port =
                    nearestConnectPointOnShape(clickedShape, pressPos, LzConnectPointSnapRadiusPx))
            {
                if (tryStartWireFromPort(port))
                    return;
            }
        }
    }

    const bool addToSelection = (evt->modifiers() & Qt::ShiftModifier) != 0;

    if (clickedShape)
    {
        bool wasSelected = clickedShape->isSelected();

        if (addToSelection)
        {
            if (wasSelected)
                scene->removeSelection(clickedShape);
            else
                scene->addSelection(clickedShape);
        }
        else
        {
            if (!wasSelected)
            {
                scene->cleanSelection();
                scene->addSelection(clickedShape);
            }
        }

        QList<QGraphicsItem*> selectedItems = scene->getSelections();
        emit scene->currentObjectChanged(scene->getSelections());

        if (selectedItems.count() >= 1)
        {
            if (selectedItems.count() == 1)
            {
                currentDragHandle = clickedShape->collidesWithHandle(lastPoint);
                if (currentDragHandle != HANDLE_NONE)
                {
                    if (currentDragHandle >= HANDLE_POINT::LEFT_TOP
                        && currentDragHandle <= HANDLE_POINT::LEFT)
                    {
                        currentSelectMode = MODE_SIZE;
                        opposite = clickedShape->opposite(currentDragHandle);
                        opposite = QPointF(
                                    opposite.x() == 0 ? 1 : opposite.x(),
                                    opposite.y() == 0 ? 1 : opposite.y()
                                    );
                    }
                else
                {
                    currentSelectMode = MODE_EDITOR;
                    if (auto* line = qgraphicsitem_cast<LzConnectLine*>(clickedShape))
                    {
                        beginConnectLinePathEdit(line);
                        const int lastHandleDir = line->handleCount();
                        if (currentDragHandle == HANDLE_POINT::LEFT + 1)
                            line->setStartConnectPort(nullptr);
                        else if (currentDragHandle == lastHandleDir)
                            line->setEndConnectPort(nullptr);
                    }
                }
                }
                else
                {
                    // 点击在线段上时，进入线段拖拽模式
                    LzConnectLine* connectLine = qgraphicsitem_cast<LzConnectLine*>(clickedShape);
                    if (connectLine && LzTool::shapeHitByScenePoint(connectLine, lastPoint))
                    {
                        currentSelectMode = MODE_SEGMENT_DRAG;
                        beginConnectLinePathEdit(connectLine);
                        setupSegmentDrag(connectLine, downPoint);
                    }
                    else
                    {
                        currentSelectMode = MODE_MOVE;
                        moveLineSnapshotsBefore.clear();
                        LzGroup::collectConnectLineSnapshots(selectedItems, moveLineSnapshotsBefore);
                    }
                }
            }
            else
            {
                currentSelectMode = MODE_MOVE;
                moveLineSnapshotsBefore.clear();
                LzGroup::collectConnectLineSnapshots(selectedItems, moveLineSnapshotsBefore);
            }
            isDragging = true;  // 只要点击了选中的图形，就允许拖动
            if (currentSelectMode == MODE_MOVE
                || currentSelectMode == MODE_SIZE
                || currentSelectMode == MODE_EDITOR
                || currentSelectMode == MODE_SEGMENT_DRAG)
            {
                scene->setInteractiveTransformActive(true);
            }
        }
    }
    else
    {
        scene->cleanSelection();
        emit scene->currentObjectChanged({});
        currentSelectMode = MODE_NET_SELECT;
        selectionRect = new QGraphicsRectItem();
        selectionRect->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        selectionRect->setBrush(QBrush(QColor(0, 0, 255, 50)));
        scene->addItem(selectionRect);
        selectionRect->setRect(QRectF(downPoint, downPoint));
        isDragging = true;
    }

    updateCursor(scene);
}

void LzSelectTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    QPointF currentPos = evt->scenePos();
    QPointF delta = currentPos - lastPoint;

    if (pendingBusbarWire && pendingBusbar
        && QLineF(pendingBusbarWireDown, currentPos).length() >= kBusbarWireDragThresholdPx)
    {
        LzConnectPoint* port =
            pendingBusbar->ensureConnectPointNearScene(currentPos, LzBusbarSnapRadiusPx);
        if (port && port != pendingBusbarTouchPort && pendingBusbarPortNeedsCleanup)
            pendingBusbar->removeConnectPointIfUnused(pendingBusbarTouchPort);

        const QPointF wireDown = currentPos;
        pendingBusbarWire = false;
        pendingBusbarPortNeedsCleanup = false;
        pendingBusbar.clear();
        pendingBusbarTouchPort = nullptr;

        if (port)
        {
            LzTool* connectTool = LzToolManager::getInstance()->changeTool(LZ_CONNECT_LINE);
            if (auto* lineTool = qobject_cast<LzConnectLineTool*>(connectTool))
            {
                lineTool->beginWireFromConnectPoint(scene, port, wireDown);
                lineTool->onMouseMove(scene, evt);
                return;
            }
            pendingBusbar->removeConnectPointIfUnused(port);
        }
    }

    switch (currentSelectMode)
    {
    case MODE_NONE:
        break;
    case MODE_NET_SELECT:
        if (selectionRect)
        {
            QRectF rect = QRectF(downPoint, currentPos).normalized();
            selectionRect->setRect(rect);
            const bool requireContained = currentPos.x() >= downPoint.x();

            scene->cleanSelection();
            const QList<QGraphicsItem*> intersectedItems = scene->items(rect);
            QList<TmpShape*> toSelect;
            toSelect.reserve(intersectedItems.size());
            for (QGraphicsItem* item : intersectedItems)
            {
                if (!item || item->scene() != scene)
                    continue;
                if (lzIsHandle(item))
                    continue;
                TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
                if (!shape || shape->parentItem())
                    continue;
                if (!LzTool::shapeHitBySceneRect(shape, rect, requireContained))
                    continue;
                if (!toSelect.contains(shape))
                    toSelect.append(shape);
            }
            for (TmpShape* shape : toSelect)
                scene->addSelection(shape);
        }
        break;
    case MODE_MOVE:
        if (!isDragging || pendingBusbarWire)
            return;
        handleMove(scene, delta);
        lastPoint = currentPos;
        break;
    case MODE_SIZE:
        if(!isDragging)
            return;
        handleResize(scene, currentPos);
        lastPoint = currentPos;
        break;
    case MODE_EDITOR:
        if(!isDragging)
            return;
        handleEdit(scene, currentPos);
        lastPoint = currentPos;
        break;
    case MODE_SEGMENT_DRAG:
        if (!isDragging)
            return;
        if (scene->getSelections().count() == 1)
        {
            LzConnectLine* connectLine =
                qgraphicsitem_cast<LzConnectLine*>(scene->getSelection());
            if (connectLine && connectLine->scene() == scene)
            {
                if (segmentDragSegmentIndex >= 0 && !segmentDragBasePath.isEmpty())
                {
                    connectLinePathEditChanged = true;
                    connectLine->applySegmentDragFromBase(segmentDragBasePath,
                                                          segmentDragSegmentIndex,
                                                          currentPos - downPoint);
                }
                else if (segmentDragPending)
                {
                    const int insertIndex = connectLine->insertWaypointAtScenePos(currentPos);
                    if (insertIndex >= 0)
                    {
                        connectLinePathEditChanged = true;
                        currentDragHandle = LEFT + insertIndex + 1;
                        currentSelectMode = MODE_EDITOR;
                        segmentDragPending = false;
                        segmentDragSegmentIndex = -1;
                        segmentDragBasePath.clear();
                        handleEdit(scene, currentPos);
                    }
                }
            }
        }
        lastPoint = currentPos;
        break;
    default:
        break;
    }

    if (currentSelectMode != MODE_NET_SELECT)
    {
        lastPoint = currentPos;
    }

    if (currentSelectMode == MODE_NONE && !isDragging && scene->views().size() > 0)
    {
        int cursorType = LzCursorTypeRegistry::CURSOR_DEFAULT;
        if (TmpShape* hover = findShapeAtPosition(scene, currentPos))
        {
            const int h = hover->collidesWithHandle(currentPos);
            if (h >= HANDLE_POINT::LEFT_TOP && h <= HANDLE_POINT::LEFT)
                cursorType = LzCursorTypeRegistry::CURSOR_RESIZE;
        }
        scene->views().first()->setCursor(
            LzCursorManager::instance()->getCursor(cursorType));
    }
}

void LzSelectTool::onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() != Qt::LeftButton)
        return;

    if (pendingBusbarPortNeedsCleanup && pendingBusbar && pendingBusbarTouchPort)
        pendingBusbar->removeConnectPointIfUnused(pendingBusbarTouchPort);

    pendingBusbarWire = false;
    pendingBusbarPortNeedsCleanup = false;
    pendingBusbar.clear();
    pendingBusbarTouchPort = nullptr;

    const int releaseSelectMode = currentSelectMode;
    if (releaseSelectMode == MODE_EDITOR && scene->getSelections().count() == 1)
    {
        if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(scene->getSelection()))
            attachConnectLineEndpointIfNearPort(scene, line, currentDragHandle, evt->scenePos());
    }
    endConnectLinePathEdit(scene);

    isDragging = false;
    scene->setInteractiveTransformActive(false);
    bool propertyPanelAfterPath = false;
    QPointF currentPos = evt->scenePos();
    if (currentSelectMode == MODE_NET_SELECT && selectionRect)
    {
        scene->removeSceneItem(selectionRect);
        delete selectionRect;
        selectionRect = nullptr;
    }
    if(currentSelectMode == MODE_MOVE)
    {
        if(currentPos != downPoint)
        {
            QPointF totalDelta = currentPos - downPoint;
            LzView* view = scene->getView();
            const QList<QGraphicsItem*> moveItems =
                expandMoveSelectionWithFullySelectedConnectLines(scene->getSelections());
            QSet<LzConnectLine*> linesToRefresh;
            for (QGraphicsItem* item : moveItems)
            {
                TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
                if (!shape)
                    continue;
                const QVector<QGraphicsItem*> connected = shape->connectedItems();
                for (QGraphicsItem* connectedItem : connected)
                {
                    if (auto* line = qgraphicsitem_cast<LzConnectLine*>(connectedItem))
                        linesToRefresh.insert(line);
                }
            }
            if (view)
                view->setUpdatesEnabled(false);
            for (LzConnectLine* line : linesToRefresh)
            {
                if (!line)
                    continue;
                line->cancelDeferredPathRecompute();
            }
            if (view)
                view->setUpdatesEnabled(true);
            scene->requestDeferredItemPropertyPanel();
            propertyPanelAfterPath = true;
            QVector<LzConnectLinePathSnapshot> moveLineSnapshotsAfter;
            LzGroup::collectConnectLineSnapshots(moveItems, moveLineSnapshotsAfter);
            const QVector<LzConnectLinePathSnapshot> moveLineSnapshotsBeforeCopy = moveLineSnapshotsBefore;
            QTimer::singleShot(0, view, [view, moveItems, scene, totalDelta,
                                         moveLineSnapshotsBeforeCopy, moveLineSnapshotsAfter]() {
                view->getStack()->push(new LzMoveCommand(moveItems, scene, totalDelta,
                                                         moveLineSnapshotsBeforeCopy,
                                                         moveLineSnapshotsAfter));
            });
        }
    }
    if(currentSelectMode == MODE_SIZE)
    {
        if (scene->selectedItems().count() == 1)
        {
            if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(scene->getSelection()))
            {
                if(currentPos != downPoint)
                {
                    if (opposite.isNull())
                    {
                        opposite = shape->opposite(currentDragHandle);
                        if (opposite.x() == 0)
                        {
                            opposite.setX(1);
                        }
                        if (opposite.y() == 0)
                        {
                            opposite.setY(1);
                        }
                    }

                    QPointF new_delta = shape->mapFromScene(currentPos) - opposite;
                    QPointF initial_delta = shape->mapFromScene(downPoint) - opposite;

                    qreal sx = new_delta.x() / initial_delta.x();
                    qreal sy = new_delta.y() / initial_delta.y();
                    shape->updateCoordinate();
                    LzView* view = scene->getView();
                    QUndoCommand* command = new LzResizeCommand(shape,scene, currentDragHandle, QPointF(sx,sy));
                    view->getStack()->push(command);
                }
            }
        }
    }

    currentSelectMode = MODE_NONE;
    currentDragHandle = HANDLE_NONE;
    opposite = QPointF();
    segmentDragPending = false;
    segmentDragSegmentIndex = -1;
    segmentDragBasePath.clear();
    moveLineSnapshotsBefore.clear();
    if (!propertyPanelAfterPath)
        emit scene->itemPropertyChanged();
    updateCursor(scene);
}

void LzSelectTool::handleMove(LzScene* scene, const QPointF& delta)
{
    const QList<QGraphicsItem*> moveItems =
        expandMoveSelectionWithFullySelectedConnectLines(scene->getSelections());
    foreach(QGraphicsItem* item, moveItems)
    {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!lzIsHandle(item))
            {
                shape->move(delta);
            }
        }
    }
}

void LzSelectTool::handleResize(LzScene* scene, const QPointF& pos)
{
    if (scene->getSelections().count() != 1)
        return;

    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(scene->getSelection());
    if (shape && !lzIsHandle(scene->getSelection()))
    {
        if (opposite.isNull())
        {
            opposite = shape->opposite(currentDragHandle);
            if (opposite.x() == 0)
            {
                opposite.setX(1);
            }
            if (opposite.y() == 0)
            {
                opposite.setY(1);
            }
        }

        QPointF new_delta = shape->mapFromScene(lastPoint) - opposite;
        QPointF initial_delta = shape->mapFromScene(downPoint) - opposite;

        qreal sx = new_delta.x() / initial_delta.x();
        qreal sy = new_delta.y() / initial_delta.y();

        shape->stretch(currentDragHandle, sx, sy, opposite);
    }
}

void LzSelectTool::setupSegmentDrag(LzConnectLine* line, const QPointF& scenePressPos)
{
    segmentDragSegmentIndex = -1;
    segmentDragBasePath.clear();
    segmentDragPending = true;
    if (!line)
        return;
    const QVector<QPointF> editablePath = line->editablePathInSceneCoords();
    const int seg = line->segmentIndexAtScenePosForPath(editablePath, scenePressPos);
    if (seg >= 0)
    {
        segmentDragSegmentIndex = seg;
        segmentDragBasePath = editablePath;
        segmentDragPending = false;
    }
}

void LzSelectTool::beginConnectLinePathEdit(LzConnectLine* line)
{
    if (!line)
        return;
    connectLinePathEditTarget = line;
    connectLineEditStateBefore = LzEditConnectLinePathCommand::captureState(line);
    connectLineEditBasePath = line->editablePathInSceneCoords();
    connectLinePathEditChanged = false;
}

void LzSelectTool::endConnectLinePathEdit(LzScene* scene)
{
    if (!connectLinePathEditTarget || !scene)
    {
        connectLinePathEditTarget.clear();
        connectLineEditBasePath.clear();
        connectLinePathEditChanged = false;
        return;
    }
    LzConnectLine* line = connectLinePathEditTarget.data();
    connectLinePathEditTarget.clear();
    connectLineEditBasePath.clear();
    const bool pathChanged = connectLinePathEditChanged;
    connectLinePathEditChanged = false;
    if (!line || line->scene() != scene)
        return;
    if (!pathChanged)
        return;
    LzView* view = scene->getView();
    if (!view)
        return;
    LzEditConnectLinePathCommand::pushIfChanged(
        view,
        line,
        connectLineEditStateBefore,
        LzEditConnectLinePathCommand::captureState(line),
        tr("编辑连接线路径"));
}

void LzSelectTool::attachConnectLineEndpointIfNearPort(LzScene* scene, LzConnectLine* line,
    int handleDir, const QPointF& scenePos)
{
    if (!scene || !line || handleDir <= HANDLE_POINT::LEFT)
        return;

    const int lastHandleDir = line->handleCount();
    if (lastHandleDir <= HANDLE_POINT::LEFT)
        return;

    const bool isStart = (handleDir == HANDLE_POINT::LEFT + 1);
    const bool isEnd = (handleDir == lastHandleDir);
    if (!isStart && !isEnd)
        return;

    LzConnectPoint* port = findPointAtPosition(scene, scenePos, true);
    if (!port)
        return;

    TmpShape* target = qgraphicsitem_cast<TmpShape*>(port->parentItem());
    if (!target)
        return;

    TmpShape* otherEnd = isStart
        ? qgraphicsitem_cast<TmpShape*>(line->getEndItem())
        : qgraphicsitem_cast<TmpShape*>(line->getStartItem());
    if (target == otherEnd)
        return;
    if (otherEnd && !lzWiringConnectEndpointsCompatible(target, otherEnd))
        return;

    const QPointF center = port->connectionCenterInScene();
    line->control(handleDir, center);
    line->updateCoordinate();

    if (isStart)
    {
        if (TmpShape* old = qgraphicsitem_cast<TmpShape*>(line->getStartItem()))
            old->removeConnect(line);
        line->setStartItem(target);
        line->setStartConnectPort(port);
        target->addConnect(line);
    }
    else
    {
        if (TmpShape* old = qgraphicsitem_cast<TmpShape*>(line->getEndItem()))
            old->removeConnect(line);
        line->setEndItem(target);
        line->setEndConnectPort(port);
        target->addConnect(line);
    }
    line->refreshEndpointsFromAttachedItems();
}

void LzSelectTool::handleEdit(LzScene* scene, const QPointF& pos)
{
    if (scene->getSelections().count() != 1)
        return;

    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(scene->getSelection());
    if (shape)
    {
        if (!lzIsHandle(shape))
        {
            QPointF editPos = pos;
            if (LzConnectLine* line = qgraphicsitem_cast<LzConnectLine*>(shape))
            {
                const int lastHandleDir = line->handleCount();
                const bool isEndpointHandle =
                    currentDragHandle == HANDLE_POINT::LEFT + 1
                    || currentDragHandle == lastHandleDir;
                const bool freeEndpointDrag = lzWiringGestureActive(QApplication::keyboardModifiers());
                if (isEndpointHandle && !freeEndpointDrag)
                {
                    if (LzConnectPoint* snap = findPointAtPosition(scene, pos, true))
                        editPos = snap->connectionCenterInScene();
                }
                if (isEndpointHandle && !connectLineEditBasePath.isEmpty())
                {
                    connectLinePathEditChanged = true;
                    line->applyEndpointDragAtScenePos(
                        currentDragHandle - HANDLE_POINT::LEFT - 1,
                        editPos,
                        connectLineEditBasePath);
                    return;
                }
            }
            shape->control(currentDragHandle, editPos);
        }
    }
}

void LzSelectTool::updateCursor(LzScene* scene)
{
    int cursorType = LzCursorTypeRegistry::CURSOR_DEFAULT;

    switch (currentSelectMode)
    {
    case MODE_MOVE:
        cursorType = LzCursorTypeRegistry::CURSOR_MOVE;
        break;
    case MODE_SIZE:
        cursorType = LzCursorTypeRegistry::CURSOR_RESIZE;
        break;
    case MODE_EDITOR:
        cursorType = LzCursorTypeRegistry::CURSOR_EDIT;
        break;
    case MODE_SEGMENT_DRAG:
        cursorType = LzCursorTypeRegistry::CURSOR_EDIT;
        break;
    case MODE_NET_SELECT:
        cursorType = LzCursorTypeRegistry::CURSOR_SELECT;
        break;
    default:
        break;
    }

    scene->views().first()->setCursor
            (
                LzCursorManager::instance()->getCursor(cursorType)
                );
}

void LzSelectTool::updateHoverState(LzScene* scene, TmpShape* hoverShape)
{
    if (hoverShape)
    {
        scene->views().first()->setCursor
                (
                    LzCursorManager::instance()->getCursor(LzCursorTypeRegistry::CURSOR_SELECT)
                    );

        const QList<QGraphicsItem*> sceneItems = scene->items();
        for (QGraphicsItem* item : sceneItems)
        {
            if (!item || item->scene() != scene || lzIsHandle(item))
                continue;
            TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
            if (!shape || scene->getSelections().contains(shape))
                continue;
            shape->setState(shape == hoverShape ? HANDLE_ACTIVE : HANDLE_INACTIVE);
        }
    }
    else
    {
        scene->views().first()->setCursor(
                    LzCursorManager::instance()->getCursor(LzCursorTypeRegistry::CURSOR_DEFAULT)
                    );

        const QList<QGraphicsItem*> sceneItems = scene->items();
        for (QGraphicsItem* item : sceneItems)
        {
            if (!item || item->scene() != scene || lzIsHandle(item))
                continue;
            TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item);
            if (!shape || scene->getSelections().contains(shape))
                continue;
            shape->setState(HANDLE_INACTIVE);
        }
    }
}

void LzSelectTool::onMouseDoubleClick(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (!scene || evt->button() != Qt::LeftButton)
        return;
    QList<QGraphicsItem*> selections = scene->getSelections();
    if (selections.size() != 1)
        return;
    QGraphicsItem* sel = selections.first();
    if (!sel || !scene->items().contains(sel) || sel->type() != LzConnectLine::Type)
        return;
    LzConnectLine* connectLine = qgraphicsitem_cast<LzConnectLine*>(sel);
    if (!connectLine)
        return;
    if (connectLine->insertWaypointAtScenePos(evt->scenePos()) >= 0)
        return;
}

void LzSelectTool::resetState()
{
    currentSelectMode = MODE_NONE;
    currentDragHandle = HANDLE_NONE;
    opposite = QPointF();
    isDragging = false;
    segmentDragPending = false;
    segmentDragSegmentIndex = -1;
    segmentDragBasePath.clear();
    pendingBusbarWire = false;
    pendingBusbarPortNeedsCleanup = false;
    pendingBusbar.clear();
    pendingBusbarTouchPort = nullptr;
    connectLinePathEditTarget.clear();
    connectLineEditBasePath.clear();
    connectLinePathEditChanged = false;
}

void LzSelectTool::enableRubberBandSelection(LzScene* scene)
{
    currentSelectMode = MODE_NET_SELECT;
    if (QGraphicsView* view = scene->getView())
    {
        view->setDragMode(QGraphicsView::RubberBandDrag);
        view->setCursor(LzCursorManager::instance()->getCursor(LzCursorTypeRegistry::CURSOR_SELECT));
    }
}
