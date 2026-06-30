#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QHash>
#include <QSet>
#include <algorithm>
#include <memory>

#include "Item/LzGroup.h"
#include "Item/LzHandle.h"
#include "Item/LzConnectLine.h"
#include "LzPowerBusbarSectionItem.h"
#include "wiring/LzShapeEdgeConnect.h"
#include "Tool/LzTool.h"
#include "Command/LzAddCommand.h"
#include "LzView.h"
#include "LzScene.h"
#include "LzFile.h"
#include "LzConnectConfig.h"
#include "algorithm/LzConnectorAlgorithm.h"
#include "algorithm/LzAStar.h"
#include "scene/LzSceneContextMenuController.h"
#include "scene/LzSceneConnectorLayoutController.h"
#include "scene/LzSceneDeferredUpdateController.h"
#include "scene/LzSceneDocumentController.h"
#include "scene/LzSceneDocumentSlotWorkbenchController.h"
#include "scene/LzSceneDocumentWorkbenchController.h"
#include "scene/LzSceneEditController.h"
#include "scene/LzSceneEditSlotWorkbenchController.h"
#include "scene/LzSceneEditWorkbenchController.h"
#include "scene/LzSceneGroupController.h"
#include "scene/LzSceneInteractionController.h"
#include "scene/LzSceneInteractionWorkbenchController.h"
#include "scene/LzSceneItemLifecycleController.h"
#include "scene/LzScenePowerTopologyAnalysisController.h"
#include "scene/LzSceneSelectionManager.h"
#include "scene/LzSceneSelectionSlotWorkbenchController.h"
#include "scene/LzSceneSelectionWorkbenchController.h"
#include "scene/LzSceneStateSlotWorkbenchController.h"
#include "scene/LzSceneStateWorkbenchController.h"
#include "scene/LzSceneTopologySlotWorkbenchController.h"
#include "scene/LzSceneTopologyStateController.h"
#include "scene/LzSceneTopologyWorkbenchController.h"
#include "scene/LzSceneTopologyQueryController.h"
#include "scene/LzSceneViewportController.h"
#include "topology/TopologyGraphIndex.h"


class LzScenePrivate
{
    Q_DECLARE_PUBLIC(LzScene)
public:
    LzScenePrivate()
        :view(nullptr), paintState(false)
    {

    }
    LzView*                    view;
    QList<QGraphicsItem* >     selectList;
    QString                    fileName;
    //判断是否处于绘制状态
    bool					   paintState;
    bool interactiveTransformActive = false;
    LzSceneDeferredUpdateController deferredUpdateController;
    QSizeF					   size;
    FILE_TYPE                  sceneType;

    QMenu* contextMenu = nullptr;

    QHash<QString, QVariant> wiringData;
    LzSceneTopologyStateController topologyStateController;

    LzScene* q_ptr;
};

LzScene::LzScene()
    :d_ptr(new LzScenePrivate())
{
    d_ptr->paintState = false;
    d_ptr->sceneType = FILE_TYPE::LZ_DRAW;
    LzSceneContextMenuController controller;
    d_ptr->contextMenu = controller.ensureMenu(d_ptr->contextMenu, this);
}

LzScene::~LzScene()
{
    Q_D(LzScene);
    delete d->contextMenu;
    d->contextMenu = nullptr;
}

LzView* LzScene::getView()
{
    LzSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneStateSlotWorkbenchController::ViewAction::Get);
}

void LzScene::setView(LzView* view)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.setView(this, view);
}

bool LzScene::load(const QString& filename, QString* inout_error_msg)
{
    LzSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(this,
                               LzSceneDocumentSlotWorkbenchController::DocumentAction::Load,
                               filename,
                               inout_error_msg);
}

bool LzScene::toDomDocument(const QString& filename, QDomDocument * dom, QString* inout_error_msg)
{
    LzSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(
        LzSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
        filename,
        dom,
        inout_error_msg);
}

QRectF LzScene::getContentsRect()
{
    LzSceneViewportController controller;
    return controller.contentsRect(this);
}

void LzScene::onLeftAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignLeft);
}

void LzScene::onRightAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignRight);
}

void LzScene::onTopAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignTop);
}

void LzScene::onBottomAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignBottom);
}

void LzScene::onVcenterAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignVCenter);
}

void LzScene::onHcenterAlign()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AlignHCenter);
}

void LzScene::onAutoCol()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AutoCol);
}

void LzScene::onAutoRow()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::AutoRow);
}

void LzScene::onSameWidth()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::SameWidth);
}

void LzScene::onSameHeight()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::SameHeight);
}

void LzScene::onSameSize()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::SameSize);
}

void LzScene::onBringFont()
{
    editToTop();
}

void LzScene::onBringBack()
{
    editToBottom();
}

void LzScene::onShortcutLeft(QPointF delta)
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, LzSceneEditSlotWorkbenchController::ShortcutDirection::Left, delta);
}

void LzScene::onShortcutRight(QPointF delta)
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, LzSceneEditSlotWorkbenchController::ShortcutDirection::Right, delta);
}

void LzScene::onShortcutUp(QPointF delta)
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, LzSceneEditSlotWorkbenchController::ShortcutDirection::Up, delta);
}

void LzScene::onShortcutDown(QPointF delta)
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, LzSceneEditSlotWorkbenchController::ShortcutDirection::Down, delta);
}

void LzScene::editCut()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Cut);
}

void LzScene::editCopy()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Copy);
}

void LzScene::editPaste()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Paste);
}

void LzScene::editDelete()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Remove);
}

void LzScene::editLevelUp()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::LevelUp);
}

void LzScene::editLevelDown()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::LevelDown);
}

void LzScene::editToTop()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::ToTop);
}

void LzScene::editToBottom()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::ToBottom);
}

void LzScene::itemRotate(int value)
{
    LzSceneEditSlotWorkbenchController controller;
    controller.rotate(this, value);
}

void LzScene::editGroup()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Group);
}

void LzScene::editUnGroup()
{
    LzSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneEditSlotWorkbenchController::Action::Ungroup);
}

QGraphicsItem *LzScene::getSelection()
{
    LzSceneSelectionSlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection);
}

QList<QGraphicsItem*> LzScene::getSelections()
{
    LzSceneSelectionSlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections);
}

bool LzScene::canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene)
{
    return LzSceneSelectionManager::canTouchSelectionItem(item, scene);
}

void LzScene::pruneSelectionList()
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneSelectionSlotWorkbenchController::SelectionAction::Prune);
}

void LzScene::setSelections(const QList<QGraphicsItem*>& selectios)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::ReplaceSelection,
                        selectios);
}

void LzScene::cleanSelection()
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this, LzSceneSelectionSlotWorkbenchController::SelectionAction::Clear);
}

void LzScene::addSelection(QGraphicsItem* item)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::AddSelection,
                        item);
}

void LzScene::removeSelection(QGraphicsItem* item)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSelection,
                        item);
}

void LzScene::removeSceneItem(QGraphicsItem* item)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSceneItem,
                        item);
}

void LzScene::insertSelection(QGraphicsItem* item)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::InsertSelection,
                        item);
}

void LzScene::deleteSelection(QGraphicsItem* item)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::DeleteSelection,
                        item);
}

void LzScene::addSelections(QList<QGraphicsItem*> items)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                        items);
}

void LzScene::removeSelections(QList<QGraphicsItem*> items)
{
    LzSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::RemoveSelections,
                        items);
}

QList<QGraphicsItem*>& LzScene::selectionStorage()
{
    Q_D(LzScene);
    return d->selectList;
}

const QList<QGraphicsItem*>& LzScene::selectionStorage() const
{
    Q_D(const LzScene);
    return d->selectList;
}

LzView*& LzScene::viewStorage()
{
    Q_D(LzScene);
    return d->view;
}

LzView* LzScene::viewStorage() const
{
    Q_D(const LzScene);
    return d->view;
}

bool& LzScene::paintStateStorage()
{
    Q_D(LzScene);
    return d->paintState;
}

bool LzScene::paintStateStorage() const
{
    Q_D(const LzScene);
    return d->paintState;
}

bool& LzScene::interactiveTransformStorage()
{
    Q_D(LzScene);
    return d->interactiveTransformActive;
}

bool LzScene::interactiveTransformStorage() const
{
    Q_D(const LzScene);
    return d->interactiveTransformActive;
}

LzSceneDeferredUpdateController& LzScene::deferredUpdateStorage()
{
    Q_D(LzScene);
    return d->deferredUpdateController;
}

const LzSceneDeferredUpdateController& LzScene::deferredUpdateStorage() const
{
    Q_D(const LzScene);
    return d->deferredUpdateController;
}

FILE_TYPE& LzScene::sceneTypeStorage()
{
    Q_D(LzScene);
    return d->sceneType;
}

FILE_TYPE LzScene::sceneTypeStorage() const
{
    Q_D(const LzScene);
    return d->sceneType;
}

QHash<QString, QVariant>& LzScene::wiringDataStorage()
{
    Q_D(LzScene);
    return d->wiringData;
}

const QHash<QString, QVariant>& LzScene::wiringDataStorage() const
{
    Q_D(const LzScene);
    return d->wiringData;
}

LzSceneTopologyStateController& LzScene::topologyStateStorage()
{
    Q_D(LzScene);
    return d->topologyStateController;
}

const LzSceneTopologyStateController& LzScene::topologyStateStorage() const
{
    Q_D(const LzScene);
    return d->topologyStateController;
}

bool LzScene::getPaintState()
{
    LzSceneStateSlotWorkbenchController controller;
    return controller.flagState(this, LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState);
}

void LzScene::setPaintState(bool state)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                        state);
}

bool LzScene::isInteractiveTransformActive() const
{
    LzSceneStateSlotWorkbenchController controller;
    return controller.flagState(this,
                                LzSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive);
}

void LzScene::setInteractiveTransformActive(bool active)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive,
                        active);
}

void LzScene::scheduleTopologyRebuild()
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild);
}

void LzScene::invalidateTopologySnapshots()
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredSceneAction::InvalidateTopologySnapshots);
}

void LzScene::cancelConnectLinePathRecompute(LzConnectLine* line)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredLineAction::CancelConnectLinePathRecompute,
                        line);
}

bool LzScene::isConnectLinePathUpdatePending() const
{
    LzSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this,
                               LzSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending);
}

void LzScene::requestDeferredItemPropertyPanel()
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredSceneAction::RequestDeferredItemPropertyPanel);
}

void LzScene::flushDeferredItemPropertyPanel()
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredSceneAction::FlushDeferredItemPropertyPanel);
}

void LzScene::scheduleConnectLinesPathRecompute(const QList<LzConnectLine*>& lines)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
                        lines);
}

void LzScene::scheduleConnectorPostprocess(const QList<LzConnectLine*>& affectedLines)
{
    LzSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectorPostprocess,
                        affectedLines);
}

QGraphicsItemGroup* LzScene::createGroup(QList<QGraphicsItem* > items)
{
    LzSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(
        this,
        LzSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        items);
}

FILE_TYPE LzScene::getSceneType()
{
    LzSceneDocumentSlotWorkbenchController controller;
    return controller.dispatchSceneType(this, LzSceneDocumentSlotWorkbenchController::SceneTypeAction::Get);
}
void LzScene::setSceneType(FILE_TYPE type)
{
    LzSceneDocumentSlotWorkbenchController controller;
    controller.dispatchSceneType(this, LzSceneDocumentSlotWorkbenchController::SceneTypeAction::Set, type);
}

bool LzScene::save(const QString& pFileName)
{
    LzSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(this,
                               LzSceneDocumentSlotWorkbenchController::DocumentAction::Save,
                               pFileName);
}

void LzScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    LzSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, LzSceneInteractionWorkbenchController::MouseAction::Press, mouseEvent);
}

void LzScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    LzSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, LzSceneInteractionWorkbenchController::MouseAction::Move, mouseEvent);
}

void LzScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    LzSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, LzSceneInteractionWorkbenchController::MouseAction::Release, mouseEvent);
}

void LzScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    LzSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, LzSceneInteractionWorkbenchController::MouseAction::DoubleClick, mouseEvent);
}

void LzScene::slotMenuClicked(QAction* action)
{
    LzSceneInteractionWorkbenchController controller;
    controller.dispatchMenuAction(this, action);
}

QAction* LzScene::execContextMenu(const QPoint& screenPos)
{
    Q_D(LzScene);
    LzSceneInteractionWorkbenchController controller;
    return controller.dispatchContextMenu(this, d->contextMenu, screenPos);
}

void LzScene::applyConnectorNudging(const QList<LzConnectLine*>& seedLines)
{
    LzSceneConnectorLayoutController controller;
    controller.applyConnectorNudging(this, seedLines);
}

void LzScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    execContextMenu(event->screenPos());
    event->accept();
}

void LzScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    LzSceneInteractionWorkbenchController controller;
    if (controller.dispatchDrag(this,
                                LzSceneInteractionWorkbenchController::DragAction::Enter,
                                event->mimeData()))
    {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
    return;
}

void LzScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    LzSceneInteractionWorkbenchController controller;
    if (controller.dispatchDrag(this,
                                LzSceneInteractionWorkbenchController::DragAction::Move,
                                event->mimeData()))
    {
        event->acceptProposedAction();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


void LzScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    LzSceneInteractionWorkbenchController controller;
    if (!controller.dispatchDrag(this,
                                 LzSceneInteractionWorkbenchController::DragAction::Drop,
                                 event->mimeData(),
                                 event->scenePos()))
    {
        QGraphicsScene::dropEvent(event);
    }
}

void LzScene::rebuildTopologyIndex()
{
    LzSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this, LzSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex);
}

const TopologyGraphIndex* LzScene::topologyIndex() const
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex);
}

TopologyGraphIndex* LzScene::topologyIndex()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex);
}

void LzScene::highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c)
{
    LzSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneTopologySlotWorkbenchController::HighlightAction::Node,
                        nodeStableId,
                        on,
                        c);
}

void LzScene::highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c)
{
    LzSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneTopologySlotWorkbenchController::HighlightAction::Edge,
                        edgeStableId,
                        on,
                        c);
}

void LzScene::highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c)
{
    LzSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        LzSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                        edgeStableIds,
                        on,
                        c);
}

void LzScene::clearTopologyHighlights()
{
    LzSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this, LzSceneTopologySlotWorkbenchController::SceneAction::ClearTopologyHighlights);
}

PowerTopologyAnalysisSnapshot LzScene::buildPowerTopologySnapshot()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchSnapshot(
        this,
        LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot);
}

PowerTopologyDocumentExport LzScene::buildPowerTopologyDocumentExport()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDocumentExport(
        this,
        LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport);
}

QVector<int> LzScene::powerDeviceNodeIds(const QString& deviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceNodeIds(this,
                                            LzSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceNodeIds,
                                            deviceId);
}

QStringList LzScene::powerDeviceConductorIds(const QString& deviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceQuery(this,
                                          LzSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceConductorIds,
                                          deviceId);
}

QStringList LzScene::connectedPowerDevices(const QString& deviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceQuery(this,
                                          LzSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
                                          deviceId);
}

QStringList LzScene::busbarAttachedPowerDevices(const QString& deviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceQuery(this,
                                          LzSceneTopologySlotWorkbenchController::DeviceQueryAction::BusbarAttachedPowerDevices,
                                          deviceId);
}

QStringList LzScene::reachablePowerDevices(const QString& deviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceQuery(this,
                                          LzSceneTopologySlotWorkbenchController::DeviceQueryAction::ReachablePowerDevices,
                                          deviceId);
}

QStringList LzScene::directedReachablePowerDevices(const QString& sourceDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchDeviceQuery(this,
                                          LzSceneTopologySlotWorkbenchController::DeviceQueryAction::DirectedReachablePowerDevices,
                                          sourceDeviceId);
}

PowerTopologyIslandAnalysis LzScene::analyzePowerIslands()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchIslandAnalysis(this,
                                             LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands);
}

PowerTopologyBranchAnalysis LzScene::analyzePowerBranches(const QString& sourceDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchBranchAnalysis(this,
                                             LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerBranches,
                                             sourceDeviceId);
}

PowerTopologyLoopAnalysis LzScene::analyzePowerLoops()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchLoopAnalysis(this,
                                           LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerLoops);
}

PowerTopologyPath LzScene::shortestPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchPath(this,
                                   LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
                                   sourceDeviceId,
                                   targetDeviceId);
}

PowerTopologyPath LzScene::shortestDirectedPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchPath(this,
                                   LzSceneTopologySlotWorkbenchController::PathAction::ShortestDirectedPowerSupplyPath,
                                   sourceDeviceId,
                                   targetDeviceId);
}

PowerTopologyGroundPath LzScene::shortestPowerGroundPath(const QString& sourceDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchGroundPath(this,
                                         LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerGroundPath,
                                         sourceDeviceId);
}

PowerTopologySwitchChangePreview LzScene::previewPowerSwitchTopologyChange(const QString& deviceId, int toSwitchPosition)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchSwitchChangePreview(
        this,
        LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
        deviceId,
        toSwitchPosition);
}

PowerTopologyProtectionRangePreview LzScene::previewPowerProtectionRange(const QString& deviceId,
                                                                         int toSwitchPosition,
                                                                         const QString& sourceDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchProtectionRangePreview(
        this,
        LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
        deviceId,
        toSwitchPosition,
        sourceDeviceId);
}

PowerTopologyOperationPreview LzScene::previewPowerSwitchOperation(const QString& deviceId,
                                                                   int toSwitchPosition,
                                                                   const QString& supplySourceDeviceId,
                                                                   const QString& supplyTargetDeviceId,
                                                                   const QString& groundSourceDeviceId)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchOperationPreview(
        this,
        LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
        deviceId,
        toSwitchPosition,
        supplySourceDeviceId,
        supplyTargetDeviceId,
        groundSourceDeviceId);
}

QVariantMap LzScene::buildPowerTopologyBindingSnapshot()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchMap(this,
                                  LzSceneTopologySlotWorkbenchController::MapAction::BuildPowerTopologyBindingSnapshot);
}

QVariantMap LzScene::buildTopologyBindingSnapshot()
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchMap(this,
                                  LzSceneTopologySlotWorkbenchController::MapAction::BuildTopologyBindingSnapshot);
}

QVariantMap LzScene::queryTopology(const QVariantMap& request)
{
    LzSceneTopologySlotWorkbenchController controller;
    return controller.dispatchMap(this,
                                  LzSceneTopologySlotWorkbenchController::MapAction::QueryTopology,
                                  request);
}

void LzScene::setWiringData(const QString& key, const QVariant& value)
{
    LzSceneStateSlotWorkbenchController controller;
    if (!controller.dispatchWiringData(this, key, value))
        return;
    emit wiringDataChanged(key, value);
}

QVariant LzScene::wiringData(const QString& key) const
{
    LzSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this, LzSceneStateSlotWorkbenchController::WiringDataAction::Get, key);
}
