#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDomDocument>
#include <QHash>
#include <QSet>
#include <algorithm>
#include <memory>

#include "Item/CimdrawGroup.h"
#include "Item/CimdrawHandle.h"
#include "Item/CimdrawConnectLine.h"
#include "CimdrawPowerBusbarSectionItem.h"
#include "wiring/CimdrawShapeEdgeConnect.h"
#include "Tool/CimdrawTool.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawView.h"
#include "CimdrawScene.h"
#include "CimdrawFile.h"
#include "CimdrawConnectConfig.h"
#include "algorithm/CimdrawConnectorAlgorithm.h"
#include "algorithm/CimdrawAStar.h"
#include "cim/behavior/CimBehaviorResultVariant.h"
#include "cim/query/CimQueryFacade.h"
#include "scene/CimdrawSceneContextMenuController.h"
#include "scene/CimdrawSceneConnectorLayoutController.h"
#include "scene/CimdrawSceneDeferredUpdateController.h"
#include "scene/CimdrawSceneDocumentController.h"
#include "scene/CimdrawSceneDocumentSlotWorkbenchController.h"
#include "scene/CimdrawSceneDocumentWorkbenchController.h"
#include "scene/CimdrawSceneEditController.h"
#include "scene/CimdrawSceneEditSlotWorkbenchController.h"
#include "scene/CimdrawSceneEditWorkbenchController.h"
#include "scene/CimdrawSceneGroupController.h"
#include "scene/CimdrawSceneInteractionController.h"
#include "scene/CimdrawSceneInteractionWorkbenchController.h"
#include "scene/CimdrawSceneItemLifecycleController.h"
#include "scene/CimdrawScenePowerTopologyAnalysisController.h"
#include "scene/CimdrawSceneSelectionManager.h"
#include "scene/CimdrawSceneSelectionSlotWorkbenchController.h"
#include "scene/CimdrawSceneSelectionWorkbenchController.h"
#include "scene/CimdrawSceneStateSlotWorkbenchController.h"
#include "scene/CimdrawSceneStateWorkbenchController.h"
#include "scene/CimdrawSceneTopologySlotWorkbenchController.h"
#include "scene/CimdrawSceneTopologyStateController.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"
#include "scene/CimdrawSceneViewportController.h"
#include "topology/TopologyGraphIndex.h"


class CimdrawScenePrivate
{
    Q_DECLARE_PUBLIC(CimdrawScene)
public:
    CimdrawScenePrivate()
        :view(nullptr), paintState(false)
    {

    }
    CimdrawView*                    view;
    QList<QGraphicsItem* >     selectList;
    QString                    fileName;
    //判断是否处于绘制状态
    bool					   paintState;
    bool interactiveTransformActive = false;
    CimdrawSceneDeferredUpdateController deferredUpdateController;
    QSizeF					   size;
    FILE_TYPE                  sceneType;

    QMenu* contextMenu = nullptr;

    QHash<QString, QVariant> wiringData;
    CimdrawSceneTopologyStateController topologyStateController;

    CimdrawScene* q_ptr;
};

namespace {

const CimQueryFacade& cimQueryFacade()
{
    static const CimQueryFacade facade;
    return facade;
}

} // namespace

CimdrawScene::CimdrawScene()
    :d_ptr(new CimdrawScenePrivate())
{
    d_ptr->paintState = false;
    d_ptr->sceneType = FILE_TYPE::CIMDRAW_DRAW;
    CimdrawSceneContextMenuController controller;
    d_ptr->contextMenu = controller.ensureMenu(d_ptr->contextMenu, this);
}

CimdrawScene::~CimdrawScene()
{
    Q_D(CimdrawScene);
    delete d->contextMenu;
    d->contextMenu = nullptr;
}

CimGraphicVisualSummary CimdrawScene::buildGraphicVisualSummary(
    const CimModel& model,
    const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().buildGraphicVisualSummary(model, queryState);
}

CimdrawView* CimdrawScene::getView()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this, CimdrawSceneStateSlotWorkbenchController::ViewAction::Get);
}

void CimdrawScene::setView(CimdrawView* view)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.setView(this, view);
}

bool CimdrawScene::load(const QString& filename, QString* inout_error_msg)
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(this,
                               CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::Load,
                               filename,
                               inout_error_msg);
}

bool CimdrawScene::toDomDocument(const QString& filename, QDomDocument * dom, QString* inout_error_msg)
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(
        CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
        filename,
        dom,
        inout_error_msg);
}

QRectF CimdrawScene::getContentsRect()
{
    CimdrawSceneViewportController controller;
    return controller.contentsRect(this);
}

void CimdrawScene::onLeftAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignLeft);
}

void CimdrawScene::onRightAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignRight);
}

void CimdrawScene::onTopAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignTop);
}

void CimdrawScene::onBottomAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignBottom);
}

void CimdrawScene::onVcenterAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignVCenter);
}

void CimdrawScene::onHcenterAlign()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AlignHCenter);
}

void CimdrawScene::onAutoCol()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AutoCol);
}

void CimdrawScene::onAutoRow()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::AutoRow);
}

void CimdrawScene::onSameWidth()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::SameWidth);
}

void CimdrawScene::onSameHeight()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::SameHeight);
}

void CimdrawScene::onSameSize()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::SameSize);
}

void CimdrawScene::onBringFont()
{
    editToTop();
}

void CimdrawScene::onBringBack()
{
    editToBottom();
}

void CimdrawScene::onShortcutLeft(QPointF delta)
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Left, delta);
}

void CimdrawScene::onShortcutRight(QPointF delta)
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Right, delta);
}

void CimdrawScene::onShortcutUp(QPointF delta)
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Up, delta);
}

void CimdrawScene::onShortcutDown(QPointF delta)
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatchShortcut(this, CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Down, delta);
}

void CimdrawScene::editCut()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Cut);
}

void CimdrawScene::editCopy()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Copy);
}

void CimdrawScene::editPaste()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Paste);
}

void CimdrawScene::editDelete()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Remove);
}

void CimdrawScene::editLevelUp()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::LevelUp);
}

void CimdrawScene::editLevelDown()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::LevelDown);
}

void CimdrawScene::editToTop()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::ToTop);
}

void CimdrawScene::editToBottom()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::ToBottom);
}

void CimdrawScene::itemRotate(int value)
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.rotate(this, value);
}

void CimdrawScene::editGroup()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Group);
}

void CimdrawScene::editUnGroup()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneEditSlotWorkbenchController::Action::Ungroup);
}

QGraphicsItem *CimdrawScene::getSelection()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    return controller.dispatch(this, CimdrawSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection);
}

QList<QGraphicsItem*> CimdrawScene::getSelections()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    return controller.dispatch(this, CimdrawSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections);
}

bool CimdrawScene::canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene)
{
    return CimdrawSceneSelectionManager::canTouchSelectionItem(item, scene);
}

void CimdrawScene::pruneSelectionList()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneSelectionSlotWorkbenchController::SelectionAction::Prune);
}

void CimdrawScene::setSelections(const QList<QGraphicsItem*>& selectios)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::ReplaceSelection,
                        selectios);
}

void CimdrawScene::cleanSelection()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneSelectionSlotWorkbenchController::SelectionAction::Clear);
}

void CimdrawScene::addSelection(QGraphicsItem* item)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::AddSelection,
                        item);
}

void CimdrawScene::removeSelection(QGraphicsItem* item)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSelection,
                        item);
}

void CimdrawScene::removeSceneItem(QGraphicsItem* item)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSceneItem,
                        item);
}

void CimdrawScene::insertSelection(QGraphicsItem* item)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::InsertSelection,
                        item);
}

void CimdrawScene::deleteSelection(QGraphicsItem* item)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::DeleteSelection,
                        item);
}

void CimdrawScene::addSelections(QList<QGraphicsItem*> items)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                        items);
}

void CimdrawScene::removeSelections(QList<QGraphicsItem*> items)
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::RemoveSelections,
                        items);
}

QList<QGraphicsItem*>& CimdrawScene::selectionStorage()
{
    Q_D(CimdrawScene);
    return d->selectList;
}

const QList<QGraphicsItem*>& CimdrawScene::selectionStorage() const
{
    Q_D(const CimdrawScene);
    return d->selectList;
}

CimdrawView*& CimdrawScene::viewStorage()
{
    Q_D(CimdrawScene);
    return d->view;
}

CimdrawView* CimdrawScene::viewStorage() const
{
    Q_D(const CimdrawScene);
    return d->view;
}

bool& CimdrawScene::paintStateStorage()
{
    Q_D(CimdrawScene);
    return d->paintState;
}

bool CimdrawScene::paintStateStorage() const
{
    Q_D(const CimdrawScene);
    return d->paintState;
}

bool& CimdrawScene::interactiveTransformStorage()
{
    Q_D(CimdrawScene);
    return d->interactiveTransformActive;
}

bool CimdrawScene::interactiveTransformStorage() const
{
    Q_D(const CimdrawScene);
    return d->interactiveTransformActive;
}

CimdrawSceneDeferredUpdateController& CimdrawScene::deferredUpdateStorage()
{
    Q_D(CimdrawScene);
    return d->deferredUpdateController;
}

const CimdrawSceneDeferredUpdateController& CimdrawScene::deferredUpdateStorage() const
{
    Q_D(const CimdrawScene);
    return d->deferredUpdateController;
}

FILE_TYPE& CimdrawScene::sceneTypeStorage()
{
    Q_D(CimdrawScene);
    return d->sceneType;
}

FILE_TYPE CimdrawScene::sceneTypeStorage() const
{
    Q_D(const CimdrawScene);
    return d->sceneType;
}

QHash<QString, QVariant>& CimdrawScene::wiringDataStorage()
{
    Q_D(CimdrawScene);
    return d->wiringData;
}

const QHash<QString, QVariant>& CimdrawScene::wiringDataStorage() const
{
    Q_D(const CimdrawScene);
    return d->wiringData;
}

CimdrawSceneTopologyStateController& CimdrawScene::topologyStateStorage()
{
    Q_D(CimdrawScene);
    return d->topologyStateController;
}

const CimdrawSceneTopologyStateController& CimdrawScene::topologyStateStorage() const
{
    Q_D(const CimdrawScene);
    return d->topologyStateController;
}

bool CimdrawScene::getPaintState()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    return controller.flagState(this, CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState);
}

void CimdrawScene::setPaintState(bool state)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                        state);
}

bool CimdrawScene::isInteractiveTransformActive() const
{
    CimdrawSceneStateSlotWorkbenchController controller;
    return controller.flagState(this,
                                CimdrawSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive);
}

void CimdrawScene::setInteractiveTransformActive(bool active)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive,
                        active);
}

void CimdrawScene::scheduleTopologyRebuild()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild);
}

void CimdrawScene::invalidateTopologySnapshots()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::InvalidateTopologySnapshots);
}

void CimdrawScene::cancelConnectLinePathRecompute(CimdrawConnectLine* line)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::CancelConnectLinePathRecompute,
                        line);
}

bool CimdrawScene::isConnectLinePathUpdatePending() const
{
    CimdrawSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this,
                               CimdrawSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending);
}

void CimdrawScene::requestDeferredItemPropertyPanel()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::RequestDeferredItemPropertyPanel);
}

void CimdrawScene::flushDeferredItemPropertyPanel()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::FlushDeferredItemPropertyPanel);
}

void CimdrawScene::scheduleConnectLinesPathRecompute(const QList<CimdrawConnectLine*>& lines)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
                        lines);
}

void CimdrawScene::scheduleConnectorPostprocess(const QList<CimdrawConnectLine*>& affectedLines)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectorPostprocess,
                        affectedLines);
}

QGraphicsItemGroup* CimdrawScene::createGroup(QList<QGraphicsItem* > items)
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(
        this,
        CimdrawSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        items);
}

FILE_TYPE CimdrawScene::getSceneType()
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    return controller.dispatchSceneType(this, CimdrawSceneDocumentSlotWorkbenchController::SceneTypeAction::Get);
}
void CimdrawScene::setSceneType(FILE_TYPE type)
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    controller.dispatchSceneType(this, CimdrawSceneDocumentSlotWorkbenchController::SceneTypeAction::Set, type);
}

bool CimdrawScene::save(const QString& pFileName)
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    return controller.dispatch(this,
                               CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::Save,
                               pFileName);
}

void CimdrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    CimdrawSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, CimdrawSceneInteractionWorkbenchController::MouseAction::Press, mouseEvent);
}

void CimdrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    CimdrawSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, CimdrawSceneInteractionWorkbenchController::MouseAction::Move, mouseEvent);
}

void CimdrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    CimdrawSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, CimdrawSceneInteractionWorkbenchController::MouseAction::Release, mouseEvent);
}

void CimdrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    CimdrawSceneInteractionWorkbenchController controller;
    controller.dispatchMouse(this, CimdrawSceneInteractionWorkbenchController::MouseAction::DoubleClick, mouseEvent);
}

void CimdrawScene::slotMenuClicked(QAction* action)
{
    CimdrawSceneInteractionWorkbenchController controller;
    controller.dispatchMenuAction(this, action);
}

QAction* CimdrawScene::execContextMenu(const QPoint& screenPos)
{
    Q_D(CimdrawScene);
    CimdrawSceneInteractionWorkbenchController controller;
    return controller.dispatchContextMenu(this, d->contextMenu, screenPos);
}

void CimdrawScene::applyConnectorNudging(const QList<CimdrawConnectLine*>& seedLines)
{
    CimdrawSceneConnectorLayoutController controller;
    controller.applyConnectorNudging(this, seedLines);
}

void CimdrawScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    execContextMenu(event->screenPos());
    event->accept();
}

void CimdrawScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    CimdrawSceneInteractionWorkbenchController controller;
    if (controller.dispatchDrag(this,
                                CimdrawSceneInteractionWorkbenchController::DragAction::Enter,
                                event->mimeData()))
    {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
    return;
}

void CimdrawScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    CimdrawSceneInteractionWorkbenchController controller;
    if (controller.dispatchDrag(this,
                                CimdrawSceneInteractionWorkbenchController::DragAction::Move,
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


void CimdrawScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    CimdrawSceneInteractionWorkbenchController controller;
    if (!controller.dispatchDrag(this,
                                 CimdrawSceneInteractionWorkbenchController::DragAction::Drop,
                                 event->mimeData(),
                                 event->scenePos()))
    {
        QGraphicsScene::dropEvent(event);
    }
}

void CimdrawScene::rebuildTopologyIndex()
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex);
}

void CimdrawScene::highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c)
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Node,
                        nodeStableId,
                        on,
                        c);
}

void CimdrawScene::highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c)
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Edge,
                        edgeStableId,
                        on,
                        c);
}

void CimdrawScene::highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c)
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                        edgeStableIds,
                        on,
                        c);
}

void CimdrawScene::clearTopologyHighlights()
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    controller.dispatch(this, CimdrawSceneTopologySlotWorkbenchController::SceneAction::ClearTopologyHighlights);
}

QGraphicsItem* CimdrawScene::graphicSceneItemByMrid(const QString& mrid,
                                                    const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().sceneItemByMrid(mrid, this, queryState.queryContext);
}

QString CimdrawScene::graphicMridForSceneItem(QGraphicsItem* item,
                                              const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().mridForSceneItem(item, queryState.queryContext);
}

bool CimdrawScene::activateGraphicObjectByMrid(const QString& mrid,
                                               const CimGraphicQueryState& queryState)
{
    QGraphicsItem* item = graphicSceneItemByMrid(mrid, queryState);
    if (!item || item->scene() != this)
        return false;

    cleanSelection();
    addSelection(item);
    if (CimdrawView* view = getView())
        view->centerOn(item);
    return true;
}

CimGraphicObjectSummary CimdrawScene::graphicObjectSummaryForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().graphicObjectSummaryForSceneItem(item);
}

CimGraphicObjectSummary CimdrawScene::graphicObjectSummaryForMrid(const QString& mrid,
                                                                  const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().graphicObjectSummaryForMrid(mrid, this, queryState);
}

CimGraphicRenderStateSource CimdrawScene::graphicRenderStateSourceForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().graphicRenderStateSourceForSceneItem(item);
}

CimGraphicRenderStateSource CimdrawScene::graphicRenderStateSourceForMrid(
    const QString& mrid,
    const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().graphicRenderStateSourceForMrid(mrid, this, queryState);
}

CimGraphicRenderState CimdrawScene::graphicRenderStateForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().graphicRenderStateForSceneItem(item);
}

CimGraphicRenderState CimdrawScene::graphicRenderStateForMrid(const QString& mrid,
                                                              const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().graphicRenderStateForMrid(mrid, this, queryState);
}

CimGraphicObjectSummary CimdrawScene::selectedGraphicObjectSummary(const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().selectedGraphicObjectSummary(this, queryState);
}

CimGraphicRenderStateSource CimdrawScene::selectedGraphicRenderStateSource(const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().selectedGraphicRenderStateSource(this, queryState);
}

CimGraphicRenderState CimdrawScene::selectedGraphicRenderState(const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().selectedGraphicRenderState(this, queryState);
}

CimTopologyObjectSummary CimdrawScene::topologyObjectSummaryForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().topologyObjectSummaryForSceneItem(item);
}

CimTopologyObjectSummary CimdrawScene::topologyObjectSummaryForMrid(
    const QString& mrid,
    const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().topologyObjectSummaryForMrid(mrid, this, queryState);
}

CimTopologyHighlightSummary CimdrawScene::topologyHighlightSummaryForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().topologyHighlightSummaryForSceneItem(item);
}

QStringList CimdrawScene::topologySummaryLinesForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().topologySummaryLinesForSceneItem(item);
}

CimTopologyObjectSummary CimdrawScene::selectedTopologyObjectSummary(
    const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().selectedTopologyObjectSummary(this, queryState);
}

CimTopologyHighlightSummary CimdrawScene::selectedTopologyHighlightSummary()
{
    const QList<QGraphicsItem*> selections = getSelections();
    if (selections.count() != 1)
        return {};
    return topologyHighlightSummaryForSceneItem(selections.first());
}

QStringList CimdrawScene::selectedTopologySummaryLines()
{
    const QList<QGraphicsItem*> selections = getSelections();
    if (selections.count() != 1)
        return {};
    return topologySummaryLinesForSceneItem(selections.first());
}

CimBehaviorResult CimdrawScene::behaviorResultForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().behaviorResultForSceneItem(item);
}

CimBehaviorResult CimdrawScene::behaviorResultForMrid(const QString& mrid,
                                                      const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().behaviorResultForMrid(mrid, this, queryState);
}

CimBehaviorResult CimdrawScene::selectedBehaviorResult(const CimGraphicQueryState& queryState)
{
    return cimQueryFacade().selectedBehaviorResult(this, queryState);
}

CimBehaviorResult CimdrawScene::behaviorResultForWiringKey(const QString& wiringDataKey)
{
    return cimQueryFacade().behaviorResultForWiringKey(this, wiringDataKey);
}

TopologyNodeRelationInfo CimdrawScene::relationNodeInfoValue(const QString& nodeStableId)
{
    return cimQueryFacade().relationNodeInfoValue(this, nodeStableId);
}

QVariantMap CimdrawScene::relationNodeInfo(const QString& nodeStableId)
{
    return cimQueryFacade().relationNodeInfo(this, nodeStableId);
}

TopologyEdgeMeta CimdrawScene::relationEdgeMetaForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().relationEdgeMetaForSceneItem(item);
}

TopologyEdgeMeta CimdrawScene::relationEdgeMeta(const QString& edgeStableId)
{
    return cimQueryFacade().relationEdgeMeta(this, edgeStableId);
}

QVector<TopologyEdgeMeta> CimdrawScene::relationEdgeMetas()
{
    return cimQueryFacade().relationEdgeMetas(this);
}

TopologyEdgeMeta CimdrawScene::resolveRelationEdgeMeta(const TopologyEdgeLookupHint& lookupHint)
{
    return cimQueryFacade().resolveRelationEdgeMeta(this, lookupHint);
}

CimRelationEdgeSummary CimdrawScene::relationEdgeSummaryForSceneItem(QGraphicsItem* item)
{
    return cimQueryFacade().relationEdgeSummaryForSceneItem(item);
}

CimRelationEdgeSummary CimdrawScene::selectedRelationEdgeSummary()
{
    return cimQueryFacade().selectedRelationEdgeSummary(this);
}

TopologyEdgeMeta CimdrawScene::selectedRelationEdgeMeta()
{
    return cimQueryFacade().selectedRelationEdgeMeta(this);
}

QVariantMap CimdrawScene::relationBindingSnapshot()
{
    return cimQueryFacade().relationBindingSnapshot(this);
}

QVariantMap CimdrawScene::relationEdgeInfo(const TopologyEdgeLookupHint& lookupHint)
{
    return cimQueryFacade().relationEdgeInfo(this, lookupHint);
}

CimGeneratedTopologySourceSummary CimdrawScene::generatedTopologySourceSummaryForSceneItem(
    QGraphicsItem* item,
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot)
{
    return cimQueryFacade().generatedTopologySourceSummaryForSceneItem(item, runtimeSnapshot);
}

QVector<QPair<const QGraphicsItem*, CimGeneratedTopologySourceSummary>>
CimdrawScene::generatedTopologySourceSummaries(
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot,
    bool ensureNodeIds)
{
    return cimQueryFacade().generatedTopologySourceSummaries(this, runtimeSnapshot, ensureNodeIds);
}

CimGeneratedTopologyGraphSummary CimdrawScene::generatedTopologyGraphSummary(
    CimdrawTopologyDomain domainFilter)
{
    return cimQueryFacade().generatedTopologyGraphSummary(this, domainFilter);
}

QVector<CimGeneratedTopologyRelationSummary> CimdrawScene::generatedTopologyRelationSummaries(
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId)
{
    return cimQueryFacade().generatedTopologyRelationSummaries(this, sourceSummariesByNodeId);
}

CimGeneratedTopologySceneBuildResult CimdrawScene::renderGeneratedTopologyGraphSummary(
    const CimGeneratedTopologyGraphSummary& graphSummary)
{
    return cimRenderGeneratedTopologyGraphSummary(this, graphSummary);
}

PowerTopologyProjection CimdrawScene::runtimePowerTopologyProjection()
{
    return cimQueryFacade().runtimePowerTopologyProjection(this);
}

PowerTopologyAnalysisSnapshot CimdrawScene::buildPowerTopologySnapshot()
{
    return cimQueryFacade().runtimePowerTopologySnapshot(this);
}

PowerTopologyDocumentExport CimdrawScene::buildPowerTopologyDocumentExport()
{
    return cimQueryFacade().runtimePowerTopologyDocumentExport(this);
}

QVector<int> CimdrawScene::powerDeviceNodeIds(const QString& deviceId)
{
    return cimQueryFacade().powerDeviceNodeIds(this, deviceId);
}

QStringList CimdrawScene::powerDeviceConductorIds(const QString& deviceId)
{
    return cimQueryFacade().powerDeviceConductorIds(this, deviceId);
}

QStringList CimdrawScene::connectedPowerDevices(const QString& deviceId)
{
    return cimQueryFacade().connectedPowerDevices(this, deviceId);
}

QStringList CimdrawScene::busbarAttachedPowerDevices(const QString& deviceId)
{
    return cimQueryFacade().busbarAttachedPowerDevices(this, deviceId);
}

QStringList CimdrawScene::reachablePowerDevices(const QString& deviceId)
{
    return cimQueryFacade().reachablePowerDevices(this, deviceId);
}

QStringList CimdrawScene::directedReachablePowerDevices(const QString& sourceDeviceId)
{
    return cimQueryFacade().directedReachablePowerDevices(this, sourceDeviceId);
}

PowerTopologyIslandAnalysis CimdrawScene::analyzePowerIslands()
{
    return cimQueryFacade().analyzePowerIslands(this);
}

PowerTopologyBranchAnalysis CimdrawScene::analyzePowerBranches(const QString& sourceDeviceId)
{
    return cimQueryFacade().analyzePowerBranches(this, sourceDeviceId);
}

PowerTopologyLoopAnalysis CimdrawScene::analyzePowerLoops()
{
    return cimQueryFacade().analyzePowerLoops(this);
}

PowerTopologyPath CimdrawScene::shortestPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId)
{
    return cimQueryFacade().shortestPowerSupplyPath(this, sourceDeviceId, targetDeviceId);
}

PowerTopologyPath CimdrawScene::shortestDirectedPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId)
{
    return cimQueryFacade().shortestDirectedPowerSupplyPath(this, sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath CimdrawScene::shortestPowerGroundPath(const QString& sourceDeviceId)
{
    return cimQueryFacade().shortestPowerGroundPath(this, sourceDeviceId);
}

PowerTopologySwitchChangePreview CimdrawScene::previewPowerSwitchTopologyChange(const QString& deviceId, int toSwitchPosition)
{
    return cimQueryFacade().previewPowerSwitchTopologyChange(this, deviceId, toSwitchPosition);
}

PowerTopologyProtectionRangePreview CimdrawScene::previewPowerProtectionRange(const QString& deviceId,
                                                                          int toSwitchPosition,
                                                                          const QString& sourceDeviceId)
{
    return cimQueryFacade().previewPowerProtectionRange(this, deviceId, toSwitchPosition, sourceDeviceId);
}

PowerTopologyOperationPreview CimdrawScene::previewPowerSwitchOperation(const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& supplySourceDeviceId,
                                                                    const QString& supplyTargetDeviceId,
                                                                    const QString& groundSourceDeviceId)
{
    return cimQueryFacade().previewPowerSwitchOperation(
        this,
        deviceId,
        toSwitchPosition,
        supplySourceDeviceId,
        supplyTargetDeviceId,
        groundSourceDeviceId);
}

QVariantMap CimdrawScene::buildPowerTopologyBindingSnapshot()
{
    return cimQueryFacade().powerTopologyBindingSnapshot(this);
}

QVariantMap CimdrawScene::buildTopologyBindingSnapshot()
{
    return cimQueryFacade().topologyBindingSnapshot(this);
}

QVariantMap CimdrawScene::queryTopology(const QVariantMap& request)
{
    return cimQueryFacade().topologyQuery(this, request);
}

QVariantMap CimdrawScene::queryTopology(const QString& domain,
                                        const QString& op,
                                        const QVariantMap& extra)
{
    return cimQueryFacade().topologyQuery(this, domain, op, extra);
}

QVariant CimdrawScene::queryTopologyData(const QString& domain,
                                         const QString& op,
                                         const QVariantMap& extra)
{
    return cimQueryFacade().topologyQueryData(this, domain, op, extra);
}

QStringList CimdrawScene::queryTopologyStringList(const QString& domain,
                                                  const QString& op,
                                                  const QVariantMap& extra)
{
    return cimQueryFacade().topologyQueryStringList(this, domain, op, extra);
}

QVector<int> CimdrawScene::queryTopologyIntVector(const QString& domain,
                                                  const QString& op,
                                                  const QVariantMap& extra)
{
    return cimQueryFacade().topologyQueryIntVector(this, domain, op, extra);
}

QVariantMap CimdrawScene::queryTopologyMap(const QString& domain,
                                           const QString& op,
                                           const QVariantMap& extra)
{
    return cimQueryFacade().topologyQueryMap(this, domain, op, extra);
}

void CimdrawScene::setWiringData(const QString& key, const QVariant& value)
{
    CimdrawSceneStateSlotWorkbenchController controller;
    if (!controller.dispatchWiringData(this, key, value))
        return;
    emit wiringDataChanged(key, value);
}

QVariant CimdrawScene::wiringData(const QString& key) const
{
    CimdrawSceneStateSlotWorkbenchController controller;
    return controller.dispatch(this, CimdrawSceneStateSlotWorkbenchController::WiringDataAction::Get, key);
}

void CimdrawScene::setWiringBehaviorResult(const QString& key, const CimBehaviorResult& result)
{
    setWiringData(key, cimBehaviorResultToVariantMap(result));
}

CimBehaviorResult CimdrawScene::wiringBehaviorResult(const QString& key) const
{
    const QVariantMap map = wiringData(key).toMap();
    if (!map.isEmpty() && isCimBehaviorResultVariantMap(map))
        return cimBehaviorResultFromVariantMap(map);
    return {};
}
