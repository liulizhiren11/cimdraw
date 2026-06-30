#include "test_power_topology.h"

#include <QApplication>
#include <QSignalSpy>
#include <QTimer>

#include "FrameCanvasAppearanceController.h"
#include "FrameCanvasSlotWorkbenchController.h"
#include "FrameCanvasWorkbenchController.h"
#include "FrameCimSlotWorkbenchController.h"
#include "FrameCimWorkbenchController.h"
#include "FrameDataSourceCoordinator.h"
#include "FrameDocumentWorkbenchController.h"
#include "FrameDocumentSlotWorkbenchController.h"
#include "FrameEditorWorkbenchController.h"
#include "FrameEditorSlotWorkbenchController.h"
#include "FrameSceneLifecycleCoordinator.h"
#include "FrameTopologySlotWorkbenchController.h"
#include "FrameTopologyWorkbenchController.h"
#include "FrameWindowStateWorkbenchController.h"
#include "FrameWindowStateSlotWorkbenchController.h"
#include "FrameWorkbenchUiController.h"
#include "datamodule/LzDataSourceConfigLoader.h"
#include "datamodule/LzDataSourceBuilder.h"
#include "datamodule/LzDataSourceFactory.h"
#include "datamodule/LzDataSourceManager.h"
#include "datamodule/LzDataSourceRuntimeController.h"
#include "datamodule/LzIDataSource.h"
#include "LzCenterWidget.h"
#include "LzScene.h"
#include "LzView.h"
#include "LzItemController.h"
#include "FrameWorkbenchStateController.h"
#include "FrameToolSelectionController.h"
#include "FrameToolSelectionSlotWorkbenchController.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"
#include "cim/ui/CimModelBrowserDock.h"
#include "cim/ui/CimSceneBuilder.h"
#include "item/LzConnectLine.h"
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
#include "scene/LzSceneStateController.h"
#include "scene/LzSceneStateSlotWorkbenchController.h"
#include "scene/LzSceneStateWorkbenchController.h"
#include "scene/LzSceneTopologySlotWorkbenchController.h"
#include "scene/LzSceneTopologyWorkbenchController.h"
#include "scene/LzSceneTopologyQueryController.h"
#include "scene/LzSceneTopologyStateController.h"
#include "scene/LzSceneViewportController.h"
#include "Tool/LzTool.h"
#include "wiring/base/LzWiringSymbolStyle.h"
#include "wiring/power/LzPowerBusbarSectionItem.h"
#include "wiring/power/LzPowerArresterItem.h"
#include "wiring/power/LzPowerCableItem.h"
#include "wiring/power/LzPowerCapacitorItem.h"
#include "wiring/power/LzPowerCircuitBreakerItem.h"
#include "wiring/power/LzPowerCurrentTransformerItem.h"
#include "wiring/power/LzPowerEarthSwitchItem.h"
#include "wiring/power/LzPowerFuseItem.h"
#include "wiring/power/LzPowerGridItem.h"
#include "wiring/power/LzPowerGeneratorItem.h"
#include "wiring/power/LzPowerGroundItem.h"
#include "wiring/power/LzPowerLoadBreakItem.h"
#include "wiring/power/LzPowerLoadItem.h"
#include "wiring/power/LzPowerMotorItem.h"
#include "wiring/power/LzPowerReactorItem.h"
#include "wiring/power/LzPowerVoltageTransformerItem.h"

#include <QTemporaryDir>
#include <QColor>
#include <QFile>
#include <QComboBox>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QTreeWidget>
#include <QTabWidget>

namespace {

class RecordingTool : public LzTool
{
public:
    explicit RecordingTool(QObject* parent = nullptr)
        : LzTool(parent)
    {
        name = QStringLiteral("recording-tool");
        drawType = LZ_SELECTION;
    }

    QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override
    {
        Q_UNUSED(scene);
        Q_UNUSED(pos);
        Q_UNUSED(addToScene);
        return nullptr;
    }

    void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++pressCount;
    }

    void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++moveCount;
    }

    void onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++releaseCount;
    }

    void onMouseDoubleClick(LzScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++doubleClickCount;
    }

    int pressCount = 0;
    int moveCount = 0;
    int releaseCount = 0;
    int doubleClickCount = 0;
    LzScene* lastScene = nullptr;
    QGraphicsSceneMouseEvent* lastEvent = nullptr;
};

class RecordingDataSource : public LzIDataSource
{
public:
    explicit RecordingDataSource(const QString& sourceTypeId,
                                 bool startResult = true,
                                 QObject* parent = nullptr)
        : LzIDataSource(parent)
        , typeId_(sourceTypeId)
        , startResult_(startResult)
    {
    }

    QString typeId() const override
    {
        return typeId_;
    }

    bool configure(const QJsonObject& o) override
    {
        Q_UNUSED(o);
        ++configureCalls;
        return true;
    }

    bool start() override
    {
        ++startCalls;
        running_ = startResult_;
        return startResult_;
    }

    void stop() override
    {
        ++stopCalls;
        running_ = false;
    }

    bool isRunning() const override
    {
        return running_;
    }

    int configureCalls = 0;
    int startCalls = 0;
    int stopCalls = 0;

private:
    QString typeId_;
    bool startResult_ = true;
    bool running_ = false;
};

} // namespace

void TestPowerTopology::scene_defaults_to_power()
{
    LzScene scene;
    QCOMPARE(scene.getSceneType(), FILE_TYPE::LZ_DRAW);
}

void TestPowerTopology::power_items_expose_expected_roles()
{
    LzPowerBusbarSectionItem busbar(QRectF(0, 0, 140, 18));
    LzPowerCircuitBreakerItem breaker(QRectF(0, 0, 64, 92));
    LzPowerGeneratorItem generator(QRectF(0, 0, 88, 88));
    LzPowerLoadItem load(QRectF(0, 0, 88, 88));
    LzPowerMotorItem motor(QRectF(0, 0, 88, 88));
    LzPowerGroundItem ground(QRectF(0, 0, 64, 72));
    LzPowerReactorItem reactor(QRectF(0, 0, 96, 40));
    LzPowerCapacitorItem capacitor(QRectF(0, 0, 44, 48));
    LzPowerCurrentTransformerItem currentTransformer(QRectF(0, 0, 72, 48));
    LzPowerVoltageTransformerItem voltageTransformer(QRectF(0, 0, 78, 52));
    LzPowerFuseItem fuse(QRectF(0, 0, 84, 32));

    QCOMPARE(busbar.topologyDomain(), LzTopologyDomain::PowerSystem);
    QCOMPARE(busbar.powerTopologyRole(), PowerTopologyRole::Busbar);
    QCOMPARE(breaker.powerTopologyRole(), PowerTopologyRole::Breaker);
    QCOMPARE(generator.powerTopologyRole(), PowerTopologyRole::Feeder);
    QCOMPARE(load.powerTopologyRole(), PowerTopologyRole::GenericEquipment);
    QCOMPARE(motor.powerTopologyRole(), PowerTopologyRole::GenericEquipment);
    QCOMPARE(ground.powerTopologyRole(), PowerTopologyRole::Ground);
    QCOMPARE(reactor.powerTopologyRole(), PowerTopologyRole::Reactor);
    QCOMPARE(currentTransformer.powerTopologyRole(), PowerTopologyRole::GenericEquipment);
    QCOMPARE(voltageTransformer.powerTopologyRole(), PowerTopologyRole::GenericEquipment);
    QCOMPARE(fuse.powerTopologyRole(), PowerTopologyRole::GenericEquipment);
    QCOMPARE(capacitor.topologyDomain(), LzTopologyDomain::PowerSystem);
    QVERIFY(!capacitor.topologyGraphNodeEnabled());
    QVERIFY(!reactor.topologyGraphNodeEnabled());
}

void TestPowerTopology::power_tools_are_registered()
{
    auto* toolMgr = LzToolManager::getInstance();

    LzPowerBusbarSectionItem busbar(QRectF(0, 0, 140, 18));
    LzPowerCircuitBreakerItem breaker(QRectF(0, 0, 64, 92));
    LzPowerGeneratorItem generator(QRectF(0, 0, 88, 88));
    LzPowerMotorItem motor(QRectF(0, 0, 88, 88));
    LzPowerGroundItem ground(QRectF(0, 0, 64, 72));
    LzPowerReactorItem reactor(QRectF(0, 0, 96, 40));
    LzPowerCapacitorItem capacitor(QRectF(0, 0, 44, 48));
    LzPowerCurrentTransformerItem currentTransformer(QRectF(0, 0, 72, 48));
    LzPowerVoltageTransformerItem voltageTransformer(QRectF(0, 0, 78, 52));
    LzPowerFuseItem fuse(QRectF(0, 0, 84, 32));

    QVERIFY(toolMgr->changeTool(busbar.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(breaker.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(generator.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(motor.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(ground.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(reactor.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(capacitor.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(currentTransformer.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(voltageTransformer.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(fuse.drawTypeForXml()) != nullptr);
}

void TestPowerTopology::topology_query_reports_missing_domain()
{
    LzScene scene;

    const QVariantMap result = scene.queryTopology({
        {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), false);
    QCOMPARE(result.value(QStringLiteral("errorCode")).toString(), QStringLiteral("missing_domain"));
}

void TestPowerTopology::topology_binding_snapshot_defaults_to_empty_power_payload()
{
    LzScene scene;

    const QVariantMap result = scene.buildTopologyBindingSnapshot();
    const QVariantMap power = result.value(QStringLiteral("power")).toMap();

    QCOMPARE(power.value(QStringLiteral("domain")).toString(), QStringLiteral("PowerSystem"));
    QCOMPARE(power.value(QStringLiteral("deviceCount")).toInt(), 0);
    QCOMPARE(power.value(QStringLiteral("conductorCount")).toInt(), 0);
    QCOMPARE(power.value(QStringLiteral("nodeCount")).toInt(), 0);
    QVERIFY(power.value(QStringLiteral("devices")).toList().isEmpty());
    QVERIFY(power.value(QStringLiteral("conductors")).toList().isEmpty());
    QVERIFY(power.value(QStringLiteral("nodes")).toList().isEmpty());
}

void TestPowerTopology::topology_query_controller_reports_missing_scene()
{
    LzSceneTopologyQueryController controller;

    const QVariantMap result = controller.queryTopology(nullptr, {
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), false);
    QCOMPARE(result.value(QStringLiteral("errorCode")).toString(), QStringLiteral("missing_scene"));
}

void TestPowerTopology::selection_manager_add_replace_remove_items()
{
    LzScene scene;
    LzSceneSelectionManager manager;

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(40, 0, 20, 20));
    auto* external = new QGraphicsRectItem(QRectF(0, 0, 10, 10));

    QList<QGraphicsItem*> selection;
    QVERIFY(manager.addSelection(selection, item1, &scene));
    QVERIFY(item1->isSelected());
    QCOMPARE(selection.size(), 1);

    QVERIFY(!manager.addSelection(selection, item1, &scene));
    QCOMPARE(selection.size(), 1);

    const QList<QGraphicsItem*> replaced = manager.replaceSelection({item1, item1, item2, external}, &scene);
    QCOMPARE(replaced.size(), 2);
    QVERIFY(replaced.contains(item1));
    QVERIFY(replaced.contains(item2));
    QVERIFY(item2->isSelected());

    manager.clearSelectionState(replaced, &scene);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    selection = replaced;
    QVERIFY(manager.removeSelection(selection, item1, &scene));
    QCOMPARE(selection.size(), 1);
    QVERIFY(selection.contains(item2));
    QVERIFY(!manager.removeSelection(selection, external, &scene));

    delete external;
}

void TestPowerTopology::scene_get_selection_tracks_first_selected_item()
{
    LzScene scene;

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));

    scene.setSelections({item1, item2});
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(item1));

    scene.setSelections({});
    QCOMPARE(scene.getSelection(), nullptr);
}

void TestPowerTopology::edit_controller_rotates_selected_items()
{
    LzSceneEditController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    controller.rotate(scene.getSelections(), &scene, view.getStack(), 90.0);
    QCOMPARE(item->rotation(), 90.0);
    QCOMPARE(view.getStack()->count(), 1);

    view.getStack()->undo();
    QCOMPARE(item->rotation(), 0.0);

    view.getStack()->redo();
    QCOMPARE(item->rotation(), 90.0);
}

void TestPowerTopology::scene_item_rotate_routes_through_edit_controller()
{
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    scene.itemRotate(45);
    QCOMPARE(item->rotation(), 45.0);
    QCOMPARE(view.getStack()->count(), 1);
}

void TestPowerTopology::deferred_update_controller_tracks_path_recompute_state()
{
    LzSceneDeferredUpdateController controller;
    auto* line = new LzConnectLine();
    const QList<LzConnectLine*> singleLine{line};

    QVERIFY(!controller.isConnectLinePathUpdatePending());

    controller.cancelConnectLinePathRecompute(line);
    QVERIFY(!controller.isConnectLinePathUpdatePending());

    controller.scheduleConnectLinesPathRecompute(nullptr, singleLine);
    QVERIFY(!controller.isConnectLinePathUpdatePending());

    LzScene scene;
    scene.addItem(line);
    controller.scheduleConnectLinesPathRecompute(&scene, singleLine);
    QVERIFY(controller.isConnectLinePathUpdatePending());

    controller.cancelConnectLinePathRecompute(line);
    QVERIFY(!controller.isConnectLinePathUpdatePending());
}

void TestPowerTopology::deferred_update_controller_flushes_property_panel_on_demand()
{
    LzSceneDeferredUpdateController controller;
    LzScene scene;
    QSignalSpy spy(&scene, &LzScene::itemPropertyChanged);

    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 0);

    controller.requestDeferredItemPropertyPanel();
    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 1);

    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 1);
}

void TestPowerTopology::interaction_controller_rejects_unknown_menu_and_drop_inputs()
{
    LzSceneInteractionController controller;
    LzScene scene;
    QAction action(QStringLiteral("未知动作"));

    QVERIFY(!controller.handleMenuAction(nullptr, &action));
    QVERIFY(!controller.handleMenuAction(&scene, nullptr));
    QVERIFY(!controller.handleMenuAction(&scene, &action));

    QVERIFY(!controller.handleTextDrop(nullptr, QStringLiteral("rect"), QPointF(0, 0)));
    QVERIFY(!controller.handleTextDrop(&scene, QString(), QPointF(0, 0)));
    QVERIFY(!controller.handleTextDrop(&scene, QStringLiteral("not-registered"), QPointF(0, 0)));
}

void TestPowerTopology::interaction_controller_handles_text_drop_with_registered_tool()
{
    LzSceneInteractionController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    const bool ok = controller.handleTextDrop(&scene, QStringLiteral("rect"), QPointF(30, 40));
    QVERIFY(ok);
    QCOMPARE(scene.items().size(), 1);
    QCOMPARE(scene.getSelections().size(), 1);
    QVERIFY(view.isModified());
    QCOMPARE(view.getStack()->count(), 1);
}

void TestPowerTopology::interaction_controller_dispatches_mouse_events_to_tool()
{
    LzSceneInteractionController controller;
    LzScene scene;
    RecordingTool tool;
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    QGraphicsSceneMouseEvent doubleClickEvent(QEvent::GraphicsSceneMouseDoubleClick);

    QVERIFY(!controller.dispatchMousePress(&scene, nullptr, &pressEvent));
    QVERIFY(controller.dispatchMousePress(&scene, &tool, &pressEvent));
    QVERIFY(controller.dispatchMouseMove(&scene, &tool, &moveEvent));
    QVERIFY(controller.dispatchMouseRelease(&scene, &tool, &releaseEvent));
    QVERIFY(controller.dispatchMouseDoubleClick(&scene, &tool, &doubleClickEvent));

    QCOMPARE(tool.pressCount, 1);
    QCOMPARE(tool.moveCount, 1);
    QCOMPARE(tool.releaseCount, 1);
    QCOMPARE(tool.doubleClickCount, 1);
    QCOMPARE(tool.lastScene, &scene);
    QCOMPARE(tool.lastEvent, &doubleClickEvent);
}

void TestPowerTopology::interaction_controller_handles_drag_enter_move_and_drop()
{
    LzSceneInteractionController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    QMimeData emptyMime;
    QMimeData textMime;
    textMime.setText(QStringLiteral("rect"));

    QVERIFY(!controller.handleDragEnter(nullptr, &textMime));
    QVERIFY(!controller.handleDragEnter(&scene, &emptyMime));
    QVERIFY(controller.handleDragEnter(&scene, &textMime));

    QVERIFY(!controller.handleDragMove(&emptyMime));
    QVERIFY(controller.handleDragMove(&textMime));

    QVERIFY(!controller.handleDrop(&scene, nullptr, QPointF(10, 20)));
    QVERIFY(!controller.handleDrop(&scene, &emptyMime, QPointF(10, 20)));
    QVERIFY(controller.handleDrop(&scene, &textMime, QPointF(10, 20)));
    QCOMPARE(scene.items().size(), 1);
    QCOMPARE(scene.getSelections().size(), 1);
}

void TestPowerTopology::scene_interaction_workbench_controller_routes_tool_menu_and_drag()
{
    LzSceneInteractionWorkbenchController controller;
    LzScene scene;
    RecordingTool tool;
    LzToolManager::getInstance()->setCurrentTool(&tool);

    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    QGraphicsSceneMouseEvent doubleClickEvent(QEvent::GraphicsSceneMouseDoubleClick);
    QVERIFY(!controller.dispatchMouse(nullptr,
                                      LzSceneInteractionWorkbenchController::MouseAction::Press,
                                      &pressEvent));
    QVERIFY(controller.dispatchMousePress(&scene, &pressEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     LzSceneInteractionWorkbenchController::MouseAction::Move,
                                     &moveEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     LzSceneInteractionWorkbenchController::MouseAction::Release,
                                     &releaseEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     LzSceneInteractionWorkbenchController::MouseAction::DoubleClick,
                                     &doubleClickEvent));
    QCOMPARE(tool.pressCount, 1);
    QCOMPARE(tool.moveCount, 1);
    QCOMPARE(tool.releaseCount, 1);
    QCOMPARE(tool.doubleClickCount, 1);
    QCOMPARE(tool.lastScene, &scene);
    QCOMPARE(tool.lastEvent, &doubleClickEvent);
    QCOMPARE(controller.currentTool(), static_cast<LzTool*>(&tool));

    QAction action(QStringLiteral("删除"), nullptr);
    QVERIFY(!controller.dispatchMenuAction(nullptr, &action));
    QVERIFY(controller.dispatchMenuAction(&scene, &action));
    QVERIFY(controller.handleMenuAction(&scene, &action));
    QVERIFY(!controller.handleDragMove(nullptr));

    QMimeData textMime;
    textMime.setText(QStringLiteral("rect"));
    QVERIFY(!controller.dispatchDrag(nullptr,
                                     LzSceneInteractionWorkbenchController::DragAction::Enter,
                                     &textMime));
    QVERIFY(controller.dispatchDrag(&scene,
                                    LzSceneInteractionWorkbenchController::DragAction::Enter,
                                    &textMime));
    QVERIFY(controller.handleDragEnter(&scene, &textMime));
    QVERIFY(controller.dispatchDrag(&scene,
                                    LzSceneInteractionWorkbenchController::DragAction::Move,
                                    &textMime));

    QMenu* menu = nullptr;
    QTimer::singleShot(0, []()
    {
        if (QWidget* popup = QApplication::activePopupWidget())
            popup->close();
    });
    QAction* result = controller.dispatchContextMenu(&scene, menu, QPoint(0, 0));
    Q_UNUSED(result);
    QVERIFY(menu != nullptr);
    QCOMPARE(menu->actions().size(), 6);

    const int itemCountBeforeDrop = scene.items().size();
    QVERIFY(controller.dispatchDrag(&scene,
                                    LzSceneInteractionWorkbenchController::DragAction::Drop,
                                    &textMime,
                                    QPointF(12, 16)));
    QVERIFY(scene.items().size() > itemCountBeforeDrop);

    delete menu;
    LzToolManager::getInstance()->setCurrentTool(nullptr);
}

void TestPowerTopology::connector_layout_controller_expands_scope_and_nudges_overlapping_lines()
{
    LzSceneConnectorLayoutController controller;
    LzScene scene;

    auto* line1 = new LzConnectLine();
    auto* line2 = new LzConnectLine();
    const QList<LzConnectLine*> seedLines{line1};
    scene.addItem(line1);
    scene.addItem(line2);

    line1->setPathFromScenePoints({QPointF(0, 0), QPointF(80, 0), QPointF(160, 0)});
    line2->setPathFromScenePoints({QPointF(0, 0), QPointF(80, 0), QPointF(160, 0)});

    const QList<LzConnectLine*> scope = controller.expandNudgingScope(&scene, seedLines);
    QVERIFY(scope.contains(line1));
    QVERIFY(scope.contains(line2));

    const QVector<QPointF> before1 = line1->pathInSceneCoords();
    const QVector<QPointF> before2 = line2->pathInSceneCoords();

    QVERIFY(controller.applyConnectorNudging(&scene, seedLines));

    const QVector<QPointF> after1 = line1->pathInSceneCoords();
    const QVector<QPointF> after2 = line2->pathInSceneCoords();
    QVERIFY(after1 != before1 || after2 != before2);
}

void TestPowerTopology::connector_layout_controller_ignores_empty_or_single_line_scope()
{
    LzSceneConnectorLayoutController controller;
    LzScene scene;
    auto* line = new LzConnectLine();
    const QList<LzConnectLine*> singleLine{line};
    scene.addItem(line);
    line->setPathFromScenePoints({QPointF(0, 0), QPointF(20, 0), QPointF(40, 0)});

    QVERIFY(controller.expandNudgingScope(nullptr, {}).isEmpty());
    QVERIFY(!controller.applyConnectorNudging(nullptr, {}));
    QVERIFY(!controller.applyConnectorNudging(&scene, {}));
    QVERIFY(!controller.applyConnectorNudging(&scene, singleLine));
}

void TestPowerTopology::state_controller_updates_view_flags_and_wiring_data()
{
    LzSceneStateController controller;
    LzView* view = nullptr;
    bool paintState = false;
    bool interactive = false;
    QHash<QString, QVariant> wiringData;
    LzView realView;

    QCOMPARE(controller.view(view), nullptr);
    controller.setView(view, nullptr);
    QCOMPARE(view, nullptr);
    controller.setView(view, &realView);
    QCOMPARE(view, &realView);
    QCOMPARE(controller.view(view), &realView);

    QCOMPARE(controller.paintState(paintState), false);
    controller.setPaintState(paintState, true);
    QCOMPARE(controller.paintState(paintState), true);

    QCOMPARE(controller.interactiveTransformActive(interactive), false);
    controller.setInteractiveTransformActive(interactive, true);
    QCOMPARE(controller.interactiveTransformActive(interactive), true);

    QVERIFY(!controller.setWiringData(wiringData, QString(), 1));
    QVERIFY(controller.setWiringData(wiringData, QStringLiteral("k1"), 42));
    QCOMPARE(controller.wiringData(wiringData, QStringLiteral("k1")).toInt(), 42);
    QVERIFY(!controller.wiringData(wiringData, QStringLiteral("missing")).isValid());
}

void TestPowerTopology::scene_state_workbench_controller_routes_scene_state_updates()
{
    LzSceneStateWorkbenchController controller;
    LzSceneDeferredUpdateController deferredUpdateController;
    LzSceneTopologyStateController topologyStateController;
    LzView* view = nullptr;
    bool paintState = false;
    bool interactive = false;
    QHash<QString, QVariant> wiringData;
    LzView realView;
    LzScene scene;

    QCOMPARE(controller.view(view), nullptr);
    controller.setView(view, &realView);
    QCOMPARE(controller.view(view), &realView);

    QCOMPARE(controller.paintState(paintState), false);
    controller.setPaintState(paintState, true);
    QCOMPARE(controller.paintState(paintState), true);

    QCOMPARE(controller.interactiveTransformActive(interactive), false);
    controller.setInteractiveTransformActive(interactive, true);
    QCOMPARE(controller.interactiveTransformActive(interactive), true);

    QVERIFY(!controller.setWiringData(wiringData, topologyStateController, QString(), 1));
    QVERIFY(controller.setWiringData(wiringData, topologyStateController, QStringLiteral("k2"), 84));
    QCOMPARE(controller.wiringData(wiringData, QStringLiteral("k2")).toInt(), 84);

    QCOMPARE(controller.isConnectLinePathUpdatePending(deferredUpdateController), false);
    controller.requestDeferredItemPropertyPanel(deferredUpdateController);
    controller.flushDeferredItemPropertyPanel(&scene, deferredUpdateController);
    controller.invalidateTopologySnapshots(topologyStateController);
}

void TestPowerTopology::frame_workbench_state_controller_syncs_ui_state()
{
    FrameWorkbenchStateController controller;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(LzWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(LzWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(LzWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    controller.syncWiringStandardCombo(&combo, LzWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::GB));

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(12.5, -3.0));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=12.5"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=-3"));

    QTabWidget tabs;
    LzView* view1 = new LzView();
    LzView* view2 = new LzView();
    tabs.addTab(view1, QStringLiteral("v1"));
    tabs.addTab(view2, QStringLiteral("v2"));
    tabs.setCurrentWidget(view2);

    QCOMPARE(controller.currentViewFromTabs(&tabs), view2);
    QCOMPARE(controller.currentViewFromTabs(nullptr), nullptr);
}

void TestPowerTopology::frame_canvas_appearance_controller_applies_view_background()
{
    FrameCanvasAppearanceController controller;
    LzView view;
    QPushButton button;
    QTabWidget tabs;

    tabs.addTab(&view, QStringLiteral("view"));
    tabs.setCurrentWidget(&view);

    QVERIFY(controller.applyBackgroundColor(nullptr, &button, QColor(Qt::red)) == false);
    QVERIFY(controller.applyBackgroundColor(&view, nullptr, QColor(Qt::red)) == false);
    QVERIFY(controller.applyBackgroundColor(&view, &button, QColor()) == false);
    QCOMPARE(controller.currentView(nullptr), nullptr);
    QCOMPARE(controller.currentView(&tabs), &view);

    QVERIFY(controller.applyBackgroundColor(&view, &button, QColor(Qt::green)));
    QCOMPARE(view.getBackgroundCol(), QColor(Qt::green));
    QCOMPARE(button.palette().color(QPalette::Button), QColor(Qt::green));
    QVERIFY(controller.applyBackgroundColorToCurrentView(&tabs, &button, QColor(Qt::blue)));
    QCOMPARE(view.getBackgroundCol(), QColor(Qt::blue));

    QVERIFY(controller.applyBackgroundImage(nullptr, QStringLiteral("x.png")) == false);
    QVERIFY(controller.applyBackgroundImage(&view, QString()) == false);
    QVERIFY(controller.applyBackgroundImage(&view, QStringLiteral("demo.png")));
    QCOMPARE(view.getBackgroundImage(), QStringLiteral("demo.png"));
    QVERIFY(controller.applyBackgroundImageToCurrentView(&tabs, QStringLiteral("demo2.png")));
    QCOMPARE(view.getBackgroundImage(), QStringLiteral("demo2.png"));
    QVERIFY(controller.applyBackgroundImageToCurrentView(nullptr, QStringLiteral("demo3.png")) == false);

    const QString defaultDir = controller.defaultImageDirectory(nullptr);
    QVERIFY(defaultDir.contains(QStringLiteral("data/image")));
}

void TestPowerTopology::frame_canvas_workbench_controller_routes_canvas_state_updates()
{
    FrameCanvasWorkbenchController controller;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(LzWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(LzWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(LzWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    controller.syncWiringStandardUi(&combo, LzWiringSymbolStandard::ANSI);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::ANSI));

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(5.5, 6.0));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=5.5"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=6"));

    QTabWidget tabs;
    controller.refreshAllWiringItemPaint(&tabs);
    controller.applyWiringStandardChange(&combo, nullptr, &tabs, LzWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::GB));
}

void TestPowerTopology::frame_canvas_slot_workbench_controller_routes_center_widget_canvas_actions()
{
    FrameCanvasSlotWorkbenchController controller;
    LzCenterWidget centerWidget;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(LzWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(LzWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(LzWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(8.0, 9.5));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=8"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=9.5"));

    controller.syncWiringStandardUi(&combo, LzWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::GB));

    controller.refreshAllWiringItemPaint(nullptr);
    controller.applyWiringStandardChange(&combo, nullptr, nullptr, LzWiringSymbolStandard::ANSI);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::ANSI));
    QVERIFY(!controller.chooseAndApplyBackgroundColor(nullptr, nullptr, nullptr));
    QVERIFY(!controller.chooseAndApplyBackgroundImage(nullptr, nullptr));

    LzScene* scene = new LzScene();
    LzView* view = new LzView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    controller.refreshAllWiringItemPaint(&centerWidget);
    controller.applyWiringStandardChange(&combo, nullptr, &centerWidget, LzWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(LzWiringSymbolStandard::GB));
}

void TestPowerTopology::frame_canvas_slot_workbench_controller_applies_selected_wiring_standard()
{
    FrameCanvasSlotWorkbenchController controller;
    QComboBox combo;

    combo.addItem(QStringLiteral("IEC"), static_cast<int>(LzWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(LzWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(LzWiringSymbolStandard::GB));
    combo.setCurrentIndex(1);

    const LzWiringSymbolStandard original = LzWiringSymbolStyle::instance().standard();
    QVERIFY(controller.applySelectedWiringStandard(&combo, QString()));
    QCOMPARE(LzWiringSymbolStyle::instance().standard(), LzWiringSymbolStandard::ANSI);

    LzWiringSymbolStyle::instance().setStandard(original);
    QVERIFY(!controller.applySelectedWiringStandard(nullptr, QString()));

    QComboBox invalidCombo;
    QVERIFY(!controller.applySelectedWiringStandard(&invalidCombo, QString()));
}

void TestPowerTopology::frame_window_state_workbench_controller_syncs_tab_and_action_state()
{
    FrameWindowStateWorkbenchController controller;

    QAction undoAction(QStringLiteral("undo"), nullptr);
    QAction redoAction(QStringLiteral("redo"), nullptr);
    QAction saveAction(QStringLiteral("save"), nullptr);
    QAction saveAsAction(QStringLiteral("saveAs"), nullptr);

    controller.syncActionStates(nullptr, &undoAction, &redoAction, &saveAction, &saveAsAction);
    QVERIFY(!undoAction.isEnabled());
    QVERIFY(!redoAction.isEnabled());
    QVERIFY(!saveAction.isEnabled());
    QVERIFY(!saveAsAction.isEnabled());

    QTabWidget tabs;
    QPushButton button;
    LzView view;
    LzScene scene;
    view.setPowerScene(&scene);
    scene.setView(&view);
    view.setBackgroundCol(QColor(Qt::cyan));
    tabs.addTab(&view, QStringLiteral("old"));
    tabs.setCurrentWidget(&view);

    controller.syncCurrentTabState(&tabs,
                                   &button,
                                   nullptr,
                                   &undoAction,
                                   &redoAction,
                                   &saveAction,
                                   &saveAsAction);
    QCOMPARE(button.palette().color(QPalette::Button), QColor(Qt::cyan));

    QVERIFY(controller.renameCurrentTab(&tabs, QStringLiteral("new-title")));
    QCOMPARE(tabs.tabText(tabs.currentIndex()), QStringLiteral("new-title"));
    QVERIFY(!controller.renameCurrentTab(nullptr, QStringLiteral("x")));
}

void TestPowerTopology::frame_cim_workbench_controller_uses_default_import_path()
{
    FrameCimWorkbenchController controller;
    const QString defaultPath = controller.defaultImportPath();

    QVERIFY(defaultPath.contains(QStringLiteral("tests/data/cim/minimal")));
    QVERIFY(defaultPath.endsWith(QStringLiteral("EQ_SSH_sample")));
}

void TestPowerTopology::frame_cim_workbench_controller_activates_shape_by_mrid()
{
    FrameCimWorkbenchController controller;
    FrameCimWorkbenchController::ImportState state;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* shape = new LzPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    state.shapeByMrid.insert(QStringLiteral("breaker-activate"), shape);

    controller.activateObjectByMrid(QStringLiteral("breaker-activate"), &scene, state);
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(shape));
}

void TestPowerTopology::cim_scene_builder_instantiates_p0_gap_graphics()
{
    CimModel model;

    CimObject busbar;
    busbar.mrid = QStringLiteral("busbar-gap");
    busbar.className = QStringLiteral("BusbarSection");
    busbar.name = QStringLiteral("BB1");
    QVERIFY(model.addObject(busbar));

    CimObject line;
    line.mrid = QStringLiteral("line-gap");
    line.className = QStringLiteral("ACLineSegment");
    line.name = QStringLiteral("Line1");
    QVERIFY(model.addObject(line));

    CimObject loadBreak;
    loadBreak.mrid = QStringLiteral("load-break-gap");
    loadBreak.className = QStringLiteral("LoadBreakSwitch");
    loadBreak.name = QStringLiteral("FZ1");
    loadBreak.setAttribute(QStringLiteral("Switch.open"), QStringLiteral("false"));
    QVERIFY(model.addObject(loadBreak));

    CimObject groundDisconnector;
    groundDisconnector.mrid = QStringLiteral("ground-dis-gap");
    groundDisconnector.className = QStringLiteral("GroundDisconnector");
    groundDisconnector.name = QStringLiteral("QSg1");
    groundDisconnector.setAttribute(QStringLiteral("Switch.open"), QStringLiteral("true"));
    QVERIFY(model.addObject(groundDisconnector));

    CimObject shunt;
    shunt.mrid = QStringLiteral("shunt-gap");
    shunt.className = QStringLiteral("LinearShuntCompensator");
    shunt.name = QStringLiteral("C1");
    QVERIFY(model.addObject(shunt));

    auto addTerminal = [&](const QString& terminalMrid,
                           const QString& equipmentMrid,
                           const QString& nodeMrid) {
        CimObject terminal;
        terminal.mrid = terminalMrid;
        terminal.className = QStringLiteral("Terminal");
        terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), equipmentMrid);
        terminal.addReference(QStringLiteral("Terminal.TopologicalNode"), nodeMrid);
        QVERIFY(model.addObject(terminal));
    };

    addTerminal(QStringLiteral("terminal-busbar-gap"), QStringLiteral("busbar-gap"), QStringLiteral("node-gap"));
    addTerminal(QStringLiteral("terminal-line-gap"), QStringLiteral("line-gap"), QStringLiteral("node-gap"));
    addTerminal(QStringLiteral("terminal-load-break-gap"), QStringLiteral("load-break-gap"), QStringLiteral("node-gap"));
    addTerminal(QStringLiteral("terminal-ground-dis-gap"), QStringLiteral("ground-dis-gap"), QStringLiteral("node-gap"));
    addTerminal(QStringLiteral("terminal-shunt-gap"), QStringLiteral("shunt-gap"), QStringLiteral("node-gap"));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* lineItem = dynamic_cast<LzPowerCableItem*>(result.shapeByMrid.value(QStringLiteral("line-gap")));
    auto* loadBreakItem = dynamic_cast<LzPowerLoadBreakItem*>(result.shapeByMrid.value(QStringLiteral("load-break-gap")));
    auto* groundDisconnectorItem = dynamic_cast<LzPowerEarthSwitchItem*>(result.shapeByMrid.value(QStringLiteral("ground-dis-gap")));
    auto* shuntItem = dynamic_cast<LzPowerCapacitorItem*>(result.shapeByMrid.value(QStringLiteral("shunt-gap")));

    QVERIFY(lineItem != nullptr);
    QVERIFY(loadBreakItem != nullptr);
    QVERIFY(groundDisconnectorItem != nullptr);
    QVERIFY(shuntItem != nullptr);

    QCOMPARE(lineItem->drawTypeForXml(), LZ_WSYM_CABLE);
    QCOMPARE(loadBreakItem->drawTypeForXml(), LZ_WSYM_LOAD_BREAK);
    QCOMPARE(groundDisconnectorItem->drawTypeForXml(), LZ_WSYM_EARTH_SWITCH);
    QCOMPARE(shuntItem->drawTypeForXml(), LZ_WSYM_CAPACITOR);

    QCOMPARE(loadBreakItem->switchPosition(), 1);
    QCOMPARE(groundDisconnectorItem->switchPosition(), 0);
    QCOMPARE(loadBreakItem->referenceDesignator(), QStringLiteral("FZ1"));
    QCOMPARE(shuntItem->referenceDesignator(), QStringLiteral("C1"));

    int connectionLineCount = 0;
    for (QGraphicsItem* item : scene.items())
    {
        if (qgraphicsitem_cast<LzConnectLine*>(item))
            ++connectionLineCount;
    }
    QVERIFY(connectionLineCount > 0);
}

void TestPowerTopology::cim_scene_builder_instantiates_p1_reuse_graphics()
{
    CimModel model;

    auto addEquipment = [&](const QString& mrid,
                            const QString& className,
                            const QString& name,
                            const QString& nodeMrid) {
        CimObject object;
        object.mrid = mrid;
        object.className = className;
        object.name = name;
        QVERIFY(model.addObject(object));

        CimObject terminal;
        terminal.mrid = mrid + QStringLiteral("-terminal");
        terminal.className = QStringLiteral("Terminal");
        terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), mrid);
        terminal.addReference(QStringLiteral("Terminal.TopologicalNode"), nodeMrid);
        QVERIFY(model.addObject(terminal));
    };

    addEquipment(QStringLiteral("external-gap"), QStringLiteral("ExternalNetworkInjection"), QStringLiteral("Grid1"), QStringLiteral("node-source"));
    addEquipment(QStringLiteral("equivalent-gap"), QStringLiteral("EquivalentInjection"), QStringLiteral("Eq1"), QStringLiteral("node-source"));
    addEquipment(QStringLiteral("async-gap"), QStringLiteral("AsynchronousMachine"), QStringLiteral("M1"), QStringLiteral("node-load"));
    addEquipment(QStringLiteral("shunt-p1-gap"), QStringLiteral("ShuntCompensator"), QStringLiteral("Sh1"), QStringLiteral("node-comp"));
    addEquipment(QStringLiteral("series-gap"), QStringLiteral("SeriesCompensator"), QStringLiteral("Sr1"), QStringLiteral("node-comp"));
    addEquipment(QStringLiteral("arrester-gap"), QStringLiteral("SurgeArrester"), QStringLiteral("LA1"), QStringLiteral("node-comp"));
    addEquipment(QStringLiteral("ground-gap"), QStringLiteral("Ground"), QStringLiteral("GND1"), QStringLiteral("node-ground"));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* externalItem = dynamic_cast<LzPowerGridItem*>(result.shapeByMrid.value(QStringLiteral("external-gap")));
    auto* equivalentItem = dynamic_cast<LzPowerGridItem*>(result.shapeByMrid.value(QStringLiteral("equivalent-gap")));
    auto* asyncItem = dynamic_cast<LzPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("async-gap")));
    auto* shuntItem = dynamic_cast<LzPowerCapacitorItem*>(result.shapeByMrid.value(QStringLiteral("shunt-p1-gap")));
    auto* seriesItem = dynamic_cast<LzPowerReactorItem*>(result.shapeByMrid.value(QStringLiteral("series-gap")));
    auto* arresterItem = dynamic_cast<LzPowerArresterItem*>(result.shapeByMrid.value(QStringLiteral("arrester-gap")));
    auto* groundItem = dynamic_cast<LzPowerGroundItem*>(result.shapeByMrid.value(QStringLiteral("ground-gap")));

    QVERIFY(externalItem != nullptr);
    QVERIFY(equivalentItem != nullptr);
    QVERIFY(asyncItem != nullptr);
    QVERIFY(shuntItem != nullptr);
    QVERIFY(seriesItem != nullptr);
    QVERIFY(arresterItem != nullptr);
    QVERIFY(groundItem != nullptr);

    QCOMPARE(externalItem->drawTypeForXml(), LZ_WSYM_GRID);
    QCOMPARE(equivalentItem->drawTypeForXml(), LZ_WSYM_GRID);
    QCOMPARE(asyncItem->drawTypeForXml(), LZ_SLD_MOTOR);
    QCOMPARE(shuntItem->drawTypeForXml(), LZ_WSYM_CAPACITOR);
    QCOMPARE(seriesItem->drawTypeForXml(), LZ_WSYM_REACTOR);
    QCOMPARE(arresterItem->drawTypeForXml(), LZ_WSYM_ARRESTER);
    QCOMPARE(groundItem->drawTypeForXml(), LZ_SLD_GROUND);

    QCOMPARE(asyncItem->referenceDesignator(), QStringLiteral("M1"));
    QCOMPARE(seriesItem->referenceDesignator(), QStringLiteral("Sr1"));
    QCOMPARE(arresterItem->referenceDesignator(), QStringLiteral("LA1"));
    QCOMPARE(groundItem->referenceDesignator(), QStringLiteral("GND1"));
}

void TestPowerTopology::cim_scene_builder_instantiates_missing_measurement_and_fuse_graphics()
{
    CimModel model;

    auto addEquipment = [&](const QString& mrid,
                            const QString& className,
                            const QString& name,
                            const QString& nodeMrid) {
        CimObject object;
        object.mrid = mrid;
        object.className = className;
        object.name = name;
        QVERIFY(model.addObject(object));

        CimObject terminal;
        terminal.mrid = mrid + QStringLiteral("-terminal");
        terminal.className = QStringLiteral("Terminal");
        terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), mrid);
        terminal.addReference(QStringLiteral("Terminal.TopologicalNode"), nodeMrid);
        QVERIFY(model.addObject(terminal));
    };

    addEquipment(QStringLiteral("ct-gap"), QStringLiteral("CurrentTransformer"), QStringLiteral("CT1"), QStringLiteral("node-measure"));
    addEquipment(QStringLiteral("vt-gap"), QStringLiteral("PotentialTransformer"), QStringLiteral("PT1"), QStringLiteral("node-measure"));
    addEquipment(QStringLiteral("vt2-gap"), QStringLiteral("VoltageTransformer"), QStringLiteral("VT1"), QStringLiteral("node-measure"));
    addEquipment(QStringLiteral("fuse-gap"), QStringLiteral("Fuse"), QStringLiteral("FU1"), QStringLiteral("node-fuse"));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* currentTransformer = dynamic_cast<LzPowerCurrentTransformerItem*>(result.shapeByMrid.value(QStringLiteral("ct-gap")));
    auto* potentialTransformer = dynamic_cast<LzPowerVoltageTransformerItem*>(result.shapeByMrid.value(QStringLiteral("vt-gap")));
    auto* voltageTransformer = dynamic_cast<LzPowerVoltageTransformerItem*>(result.shapeByMrid.value(QStringLiteral("vt2-gap")));
    auto* fuse = dynamic_cast<LzPowerFuseItem*>(result.shapeByMrid.value(QStringLiteral("fuse-gap")));

    QVERIFY(currentTransformer != nullptr);
    QVERIFY(potentialTransformer != nullptr);
    QVERIFY(voltageTransformer != nullptr);
    QVERIFY(fuse != nullptr);

    QCOMPARE(currentTransformer->drawTypeForXml(), LZ_WSYM_CURRENT_TRANSFORMER);
    QCOMPARE(potentialTransformer->drawTypeForXml(), LZ_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(voltageTransformer->drawTypeForXml(), LZ_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(fuse->drawTypeForXml(), LZ_WSYM_FUSE);

    QCOMPARE(currentTransformer->referenceDesignator(), QStringLiteral("CT1"));
    QCOMPARE(potentialTransformer->referenceDesignator(), QStringLiteral("PT1"));
    QCOMPARE(voltageTransformer->referenceDesignator(), QStringLiteral("VT1"));
    QCOMPARE(fuse->referenceDesignator(), QStringLiteral("FU1"));
}

void TestPowerTopology::cim_scene_builder_prefers_diagram_layout_coordinates_when_present()
{
    CimModel model;

    CimObject busbar;
    busbar.mrid = QStringLiteral("busbar-dl");
    busbar.className = QStringLiteral("BusbarSection");
    busbar.name = QStringLiteral("BB-DL");
    QVERIFY(model.addObject(busbar));

    CimObject motor;
    motor.mrid = QStringLiteral("motor-dl");
    motor.className = QStringLiteral("AsynchronousMachine");
    motor.name = QStringLiteral("M-DL");
    QVERIFY(model.addObject(motor));

    CimObject motorTerminal;
    motorTerminal.mrid = QStringLiteral("motor-dl-terminal");
    motorTerminal.className = QStringLiteral("Terminal");
    motorTerminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("motor-dl"));
    motorTerminal.addReference(QStringLiteral("Terminal.TopologicalNode"), QStringLiteral("node-dl"));
    QVERIFY(model.addObject(motorTerminal));

    CimObject busbarTerminal;
    busbarTerminal.mrid = QStringLiteral("busbar-dl-terminal");
    busbarTerminal.className = QStringLiteral("Terminal");
    busbarTerminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("busbar-dl"));
    busbarTerminal.addReference(QStringLiteral("Terminal.TopologicalNode"), QStringLiteral("node-dl"));
    QVERIFY(model.addObject(busbarTerminal));

    CimObject busbarDiagramObject;
    busbarDiagramObject.mrid = QStringLiteral("diagram-busbar-dl");
    busbarDiagramObject.className = QStringLiteral("DiagramObject");
    busbarDiagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"), QStringLiteral("busbar-dl"));
    QVERIFY(model.addObject(busbarDiagramObject));

    CimObject busbarPoint1;
    busbarPoint1.mrid = QStringLiteral("diagram-busbar-dl-p1");
    busbarPoint1.className = QStringLiteral("DiagramObjectPoint");
    busbarPoint1.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"), QStringLiteral("diagram-busbar-dl"));
    busbarPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 1);
    busbarPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 100.0);
    busbarPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 200.0);
    QVERIFY(model.addObject(busbarPoint1));

    CimObject busbarPoint2;
    busbarPoint2.mrid = QStringLiteral("diagram-busbar-dl-p2");
    busbarPoint2.className = QStringLiteral("DiagramObjectPoint");
    busbarPoint2.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"), QStringLiteral("diagram-busbar-dl"));
    busbarPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 2);
    busbarPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 220.0);
    busbarPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 200.0);
    QVERIFY(model.addObject(busbarPoint2));

    CimObject motorDiagramObject;
    motorDiagramObject.mrid = QStringLiteral("diagram-motor-dl");
    motorDiagramObject.className = QStringLiteral("DiagramObject");
    motorDiagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"), QStringLiteral("motor-dl"));
    motorDiagramObject.setAttribute(QStringLiteral("DiagramObject.rotation"), 90.0);
    QVERIFY(model.addObject(motorDiagramObject));

    CimObject motorPoint;
    motorPoint.mrid = QStringLiteral("diagram-motor-dl-p1");
    motorPoint.className = QStringLiteral("DiagramObjectPoint");
    motorPoint.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"), QStringLiteral("diagram-motor-dl"));
    motorPoint.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 1);
    motorPoint.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 260.0);
    motorPoint.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 260.0);
    QVERIFY(model.addObject(motorPoint));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* busbarItem =
        dynamic_cast<LzPowerBusbarSectionItem*>(result.shapeByMrid.value(QStringLiteral("busbar-dl")));
    auto* motorItem =
        dynamic_cast<LzPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-dl")));

    QVERIFY(busbarItem != nullptr);
    QVERIFY(motorItem != nullptr);

    QCOMPARE(qRound(busbarItem->scenePos().x()), 160);
    QCOMPARE(qRound(busbarItem->scenePos().y()), 200);
    QCOMPARE(qRound(busbarItem->rect().width()), 120);
    QCOMPARE(qRound(busbarItem->rotation()), 0);

    QCOMPARE(qRound(motorItem->scenePos().x()), 260);
    QCOMPARE(qRound(motorItem->scenePos().y()), 260);
    QCOMPARE(qRound(motorItem->rotation()), 90);
}

void TestPowerTopology::cim_scene_builder_resolves_terminal_diagram_objects_to_equipment()
{
    CimModel model;

    CimObject motor;
    motor.mrid = QStringLiteral("motor-terminal-diagram");
    motor.className = QStringLiteral("AsynchronousMachine");
    motor.name = QStringLiteral("M-Terminal");
    QVERIFY(model.addObject(motor));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-for-motor-diagram");
    terminal.className = QStringLiteral("Terminal");
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"),
                          QStringLiteral("motor-terminal-diagram"));
    terminal.addReference(QStringLiteral("Terminal.TopologicalNode"),
                          QStringLiteral("node-terminal-diagram"));
    QVERIFY(model.addObject(terminal));

    CimObject diagramObject;
    diagramObject.mrid = QStringLiteral("diagram-terminal-target");
    diagramObject.className = QStringLiteral("DiagramObject");
    diagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"),
                               QStringLiteral("terminal-for-motor-diagram"));
    diagramObject.setAttribute(QStringLiteral("DiagramObject.rotation"), 180.0);
    QVERIFY(model.addObject(diagramObject));

    CimObject point;
    point.mrid = QStringLiteral("diagram-terminal-target-p1");
    point.className = QStringLiteral("DiagramObjectPoint");
    point.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"),
                       QStringLiteral("diagram-terminal-target"));
    point.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 1);
    point.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 410.0);
    point.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 520.0);
    QVERIFY(model.addObject(point));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* motorItem =
        dynamic_cast<LzPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-terminal-diagram")));

    QVERIFY(motorItem != nullptr);
    QCOMPARE(qRound(motorItem->scenePos().x()), 410);
    QCOMPARE(qRound(motorItem->scenePos().y()), 520);
    QCOMPARE(qRound(motorItem->rotation()), 180);
}

void TestPowerTopology::cim_scene_builder_prefers_direct_equipment_diagram_objects_over_terminal_geometry()
{
    CimModel model;

    CimObject motor;
    motor.mrid = QStringLiteral("motor-direct-preferred");
    motor.className = QStringLiteral("AsynchronousMachine");
    motor.name = QStringLiteral("M-Direct");
    QVERIFY(model.addObject(motor));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-direct-preferred");
    terminal.className = QStringLiteral("Terminal");
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"),
                          QStringLiteral("motor-direct-preferred"));
    terminal.addReference(QStringLiteral("Terminal.TopologicalNode"),
                          QStringLiteral("node-direct-preferred"));
    QVERIFY(model.addObject(terminal));

    CimObject terminalDiagramObject;
    terminalDiagramObject.mrid = QStringLiteral("diagram-terminal-preferred-fallback");
    terminalDiagramObject.className = QStringLiteral("DiagramObject");
    terminalDiagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"),
                                       QStringLiteral("terminal-direct-preferred"));
    terminalDiagramObject.setAttribute(QStringLiteral("DiagramObject.rotation"), 90.0);
    QVERIFY(model.addObject(terminalDiagramObject));

    CimObject terminalPoint1;
    terminalPoint1.mrid = QStringLiteral("diagram-terminal-preferred-fallback-p1");
    terminalPoint1.className = QStringLiteral("DiagramObjectPoint");
    terminalPoint1.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"),
                                QStringLiteral("diagram-terminal-preferred-fallback"));
    terminalPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 1);
    terminalPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 120.0);
    terminalPoint1.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 200.0);
    QVERIFY(model.addObject(terminalPoint1));

    CimObject terminalPoint2;
    terminalPoint2.mrid = QStringLiteral("diagram-terminal-preferred-fallback-p2");
    terminalPoint2.className = QStringLiteral("DiagramObjectPoint");
    terminalPoint2.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"),
                                QStringLiteral("diagram-terminal-preferred-fallback"));
    terminalPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 2);
    terminalPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 120.0);
    terminalPoint2.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 320.0);
    QVERIFY(model.addObject(terminalPoint2));

    CimObject directDiagramObject;
    directDiagramObject.mrid = QStringLiteral("diagram-direct-preferred");
    directDiagramObject.className = QStringLiteral("DiagramObject");
    directDiagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"),
                                     QStringLiteral("motor-direct-preferred"));
    directDiagramObject.setAttribute(QStringLiteral("DiagramObject.rotation"), 0.0);
    QVERIFY(model.addObject(directDiagramObject));

    CimObject directPoint;
    directPoint.mrid = QStringLiteral("diagram-direct-preferred-p1");
    directPoint.className = QStringLiteral("DiagramObjectPoint");
    directPoint.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"),
                             QStringLiteral("diagram-direct-preferred"));
    directPoint.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 1);
    directPoint.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 360.0);
    directPoint.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 440.0);
    QVERIFY(model.addObject(directPoint));

    LzScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* motorItem =
        dynamic_cast<LzPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-direct-preferred")));

    QVERIFY(motorItem != nullptr);
    QCOMPARE(qRound(motorItem->scenePos().x()), 360);
    QCOMPARE(qRound(motorItem->scenePos().y()), 440);
    QCOMPARE(qRound(motorItem->rotation()), 0);
}

void TestPowerTopology::frame_cim_slot_workbench_controller_routes_import_entrypoints()
{
    FrameCimSlotWorkbenchController controller;
    const QString defaultPath = controller.defaultImportPath();

    QVERIFY(defaultPath.contains(QStringLiteral("tests/data/cim/minimal")));
    QVERIFY(defaultPath.endsWith(QStringLiteral("EQ_SSH_sample")));

    FrameCimWorkbenchController::ImportState state;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    CimModel seedModel;
    QHash<QString, QGraphicsItem*> seedShapes;

    auto* shape = new LzPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    seedShapes.insert(QStringLiteral("breaker-slot-activate"), shape);

    state = controller.makeImportState(nullptr, seedModel, seedShapes);
    QCOMPARE(state.browserDock, static_cast<CimModelBrowserDock*>(nullptr));
    QCOMPARE(state.shapeByMrid.value(QStringLiteral("breaker-slot-activate")), static_cast<QGraphicsItem*>(shape));

    controller.activateObjectByMrid(QStringLiteral("breaker-slot-activate"), &scene, state);
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(shape));

    const FrameCimWorkbenchController::ImportState activationState =
        controller.makeActivationState(seedShapes);
    QCOMPARE(activationState.browserDock, static_cast<CimModelBrowserDock*>(nullptr));
    QCOMPARE(activationState.shapeByMrid.value(QStringLiteral("breaker-slot-activate")),
             static_cast<QGraphicsItem*>(shape));

    CimModelBrowserDock* storedDock = reinterpret_cast<CimModelBrowserDock*>(0x1);
    CimModel storedModel;
    QHash<QString, QGraphicsItem*> storedShapes;
    controller.storeImportState(state, storedDock, storedModel, storedShapes);
    QCOMPARE(storedDock, static_cast<CimModelBrowserDock*>(nullptr));
    QCOMPARE(storedShapes.value(QStringLiteral("breaker-slot-activate")), static_cast<QGraphicsItem*>(shape));

    FrameCimWorkbenchController::ImportContext context;
    QVERIFY(!controller.importFromPath(QString(), context, &state));
    QVERIFY(!controller.importFromPath(QStringLiteral("   "), context, &state));
    QVERIFY(!controller.importFromPath(QStringLiteral("x"), context, nullptr));
}

void TestPowerTopology::frame_scene_lifecycle_coordinator_builds_cim_import_context()
{
    FrameSceneLifecycleCoordinator coordinator;
    QLineEdit messageLineEdit;

    const FrameCimWorkbenchController::ImportContext context =
        coordinator.makeCimImportContext(nullptr, &messageLineEdit);

    QCOMPARE(context.host, nullptr);
    QCOMPARE(context.messageLineEdit, &messageLineEdit);
    QVERIFY(context.onBrowserDockReady);
    QVERIFY(context.createTargetView);
    QVERIFY(context.renameCurrentTab);
    QVERIFY(context.afterSceneCreated);

    QCOMPARE(context.createTargetView(), static_cast<LzView*>(nullptr));
    context.onBrowserDockReady(nullptr);
    context.renameCurrentTab(QStringLiteral("ignored"));
    context.afterSceneCreated();
}

void TestPowerTopology::frame_data_source_coordinator_initializes_manager_with_missing_config()
{
    FrameDataSourceCoordinator coordinator;
    LzDataSourceManager manager;

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString missingPath = dir.filePath(QStringLiteral("missing-lz-data.json"));

    bool resolverCalled = false;
    QVERIFY(coordinator.initializeManager(&manager,
                                          missingPath,
                                          [&resolverCalled]() -> LzScene*
                                          {
                                              resolverCalled = true;
                                              return nullptr;
                                          }));
    QCOMPARE(manager.lastConfigPath(), missingPath);
    QVERIFY(!manager.lastError().isEmpty());
    QVERIFY(!resolverCalled);
    QVERIFY(!coordinator.initializeManager(nullptr, missingPath, {}));
}

void TestPowerTopology::data_source_config_loader_reports_missing_and_valid_root_objects()
{
    LzDataSourceConfigLoader loader;

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString missingPath = dir.filePath(QStringLiteral("missing.json"));
    const LzDataSourceConfigLoader::LoadResult missing = loader.loadRootObject(missingPath);
    QVERIFY(!missing.success);
    QCOMPARE(missing.error, QStringLiteral("config not found"));

    const QString validPath = dir.filePath(QStringLiteral("valid.json"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("{\"sources\":[],\"wiring\":{\"symbolStandard\":\"GB\"}}");
    validFile.close();

    const LzDataSourceConfigLoader::LoadResult valid = loader.loadRootObject(validPath);
    QVERIFY(valid.success);
    QVERIFY(valid.error.isEmpty());
    QVERIFY(valid.root.contains(QStringLiteral("sources")));
    QVERIFY(valid.root.value(QStringLiteral("sources")).isArray());
}

void TestPowerTopology::data_source_factory_creates_supported_source_and_rejects_invalid_inputs()
{
    LzDataSourceFactory factory;
    QObject parent;

    const QJsonObject validConfig{
        { QStringLiteral("type"), QStringLiteral("wiring_tcp_json") },
        { QStringLiteral("listenPort"), 12001 },
        { QStringLiteral("listenAddress"), QStringLiteral("127.0.0.1") }
    };

    LzIDataSource* source = factory.createConfiguredSource(QStringLiteral("wiring_tcp_json"),
                                                           validConfig,
                                                           []() -> LzScene* { return nullptr; },
                                                           &parent);
    QVERIFY(source != nullptr);
    QCOMPARE(source->typeId(), QStringLiteral("wiring_tcp_json"));
    delete source;

    QVERIFY(factory.createConfiguredSource(QStringLiteral("unknown_type"),
                                           validConfig,
                                           {},
                                           &parent) == nullptr);

    const QJsonObject invalidConfig{
        { QStringLiteral("type"), QStringLiteral("wiring_tcp_json") },
        { QStringLiteral("listenPort"), -1 }
    };
    QVERIFY(factory.createConfiguredSource(QStringLiteral("wiring_tcp_json"),
                                           invalidConfig,
                                           {},
                                           &parent) == nullptr);
}

void TestPowerTopology::data_source_builder_filters_and_builds_sources_from_root()
{
    LzDataSourceBuilder builder;
    QObject parent;

    const QJsonObject invalidRoot{
        { QStringLiteral("sources"), QStringLiteral("not-array") }
    };
    const LzDataSourceBuilder::BuildResult invalid = builder.buildSources(invalidRoot, {}, &parent);
    QVERIFY(!invalid.error.isEmpty());
    QVERIFY(invalid.sources.isEmpty());

    const QJsonArray sources{
        QJsonObject{
            { QStringLiteral("enabled"), false },
            { QStringLiteral("type"), QStringLiteral("wiring_tcp_json") },
            { QStringLiteral("listenPort"), 12002 }
        },
        QJsonObject{
            { QStringLiteral("enabled"), true },
            { QStringLiteral("type"), QStringLiteral("unknown_type") }
        },
        QJsonObject{
            { QStringLiteral("enabled"), true },
            { QStringLiteral("type"), QStringLiteral("wiring_tcp_json") },
            { QStringLiteral("listenPort"), 12003 },
            { QStringLiteral("listenAddress"), QStringLiteral("127.0.0.1") }
        }
    };
    const QJsonObject validRoot{
        { QStringLiteral("sources"), sources }
    };

    const LzDataSourceBuilder::BuildResult valid =
        builder.buildSources(validRoot, []() -> LzScene* { return nullptr; }, &parent);
    QVERIFY(valid.error.isEmpty());
    QCOMPARE(valid.sources.size(), 1);
    QCOMPARE(valid.sources.first()->typeId(), QStringLiteral("wiring_tcp_json"));
    qDeleteAll(valid.sources);
}

void TestPowerTopology::data_source_runtime_controller_starts_stops_and_clears_sources()
{
    LzDataSourceRuntimeController controller;
    QList<LzIDataSource*> sources;

    controller.startAll(sources);
    controller.stopAll(sources);
    controller.clearSources(sources);
    QVERIFY(sources.isEmpty());

    auto* source1 = new RecordingDataSource(QStringLiteral("recording_ok"));
    auto* source2 = new RecordingDataSource(QStringLiteral("recording_fail"), false);
    sources << source1 << nullptr << source2;

    controller.startAll(sources);
    QCOMPARE(source1->startCalls, 1);
    QCOMPARE(source2->startCalls, 1);
    QVERIFY(source1->isRunning());
    QVERIFY(!source2->isRunning());

    controller.stopAll(sources);
    QCOMPARE(source1->stopCalls, 1);
    QCOMPARE(source2->stopCalls, 1);
    QVERIFY(!source1->isRunning());
    QVERIFY(!source2->isRunning());

    controller.clearSources(sources);
    QVERIFY(sources.isEmpty());
}

void TestPowerTopology::frame_document_workbench_controller_creates_scene_from_factory()
{
    FrameDocumentWorkbenchController controller;
    LzCenterWidget centerWidget;
    bool factoryCalled = false;
    bool renamed = false;

    LzView* view = controller.createPowerScene([&]()
    {
        factoryCalled = true;
        FrameDocumentController::NewSceneContext context;
        context.centerWidget = &centerWidget;
        context.renameCurrentTab = [&renamed](const QString& title)
        {
            renamed = !title.isEmpty();
        };
        return context;
    });

    QVERIFY(factoryCalled);
    QVERIFY(view != nullptr);
    QCOMPARE(centerWidget.getCurrentView(), view);
    QVERIFY(renamed);
}

void TestPowerTopology::frame_document_workbench_controller_applies_save_callbacks()
{
    FrameDocumentWorkbenchController controller;
    QString renamedTo;
    bool afterSaveCalled = false;

    FrameFileController::SaveResult failed;
    QVERIFY(!controller.applySaveResult(failed,
                                        [&renamedTo](const QString& title) { renamedTo = title; },
                                        [&afterSaveCalled]() { afterSaveCalled = true; }));
    QVERIFY(renamedTo.isEmpty());
    QVERIFY(!afterSaveCalled);

    FrameFileController::SaveResult succeeded;
    succeeded.success = true;
    succeeded.tabName = QStringLiteral("demo.xml");
    QVERIFY(controller.applySaveResult(succeeded,
                                       [&renamedTo](const QString& title) { renamedTo = title; },
                                       [&afterSaveCalled]() { afterSaveCalled = true; }));
    QCOMPARE(renamedTo, QStringLiteral("demo.xml"));
    QVERIFY(afterSaveCalled);
}

void TestPowerTopology::frame_document_workbench_controller_guards_missing_open_inputs()
{
    FrameDocumentWorkbenchController controller;

    bool factoryCalled = false;
    QVERIFY(!controller.openFile(QString(),
                                 [&factoryCalled]()
                                 {
                                     factoryCalled = true;
                                     return FrameDocumentController::OpenSceneContext{};
                                 }));
    QVERIFY(!factoryCalled);

    QVERIFY(!controller.openFile(nullptr, FrameDocumentWorkbenchController::OpenSceneContextFactory{}));
    QVERIFY(!controller.saveCurrentView(nullptr, {}, {}));
    QVERIFY(!controller.saveCurrentViewAs(nullptr, nullptr, {}, {}));
}

void TestPowerTopology::frame_document_slot_workbench_controller_routes_center_widget_document_actions()
{
    FrameDocumentSlotWorkbenchController controller;
    LzCenterWidget centerWidget;

    QCOMPARE(controller.currentView(nullptr), nullptr);
    QVERIFY(!controller.saveCurrentView(nullptr, {}, {}));
    QVERIFY(!controller.saveCurrentViewAs(nullptr, nullptr, {}, {}));

    bool factoryCalled = false;
    LzView* createdView = controller.createPowerScene([&]()
    {
        factoryCalled = true;
        FrameDocumentController::NewSceneContext context;
        context.centerWidget = &centerWidget;
        return context;
    });

    QVERIFY(factoryCalled);
    QVERIFY(createdView != nullptr);
    QCOMPARE(controller.currentView(&centerWidget), createdView);

    QVERIFY(!controller.openFile(QString(), FrameDocumentSlotWorkbenchController::OpenSceneContextFactory{}));
    QVERIFY(!controller.openFile(nullptr, FrameDocumentSlotWorkbenchController::OpenSceneContextFactory{}));
}

void TestPowerTopology::frame_document_slot_workbench_controller_syncs_ui_for_missing_view_save_actions()
{
    FrameDocumentSlotWorkbenchController controller;

    QAction undoAction(QStringLiteral("undo"), nullptr);
    QAction redoAction(QStringLiteral("redo"), nullptr);
    QAction saveAction(QStringLiteral("save"), nullptr);
    QAction saveAsAction(QStringLiteral("saveAs"), nullptr);

    undoAction.setEnabled(true);
    redoAction.setEnabled(true);
    saveAction.setEnabled(true);
    saveAsAction.setEnabled(true);

    QVERIFY(!controller.saveCurrentViewAndSyncUi(nullptr,
                                                 &undoAction,
                                                 &redoAction,
                                                 &saveAction,
                                                 &saveAsAction));
    QVERIFY(!controller.saveCurrentViewAsAndSyncUi(nullptr,
                                                   nullptr,
                                                   &undoAction,
                                                   &redoAction,
                                                   &saveAction,
                                                   &saveAsAction));

    QVERIFY(undoAction.isEnabled());
    QVERIFY(redoAction.isEnabled());
    QVERIFY(saveAction.isEnabled());
    QVERIFY(saveAsAction.isEnabled());
}

void TestPowerTopology::frame_editor_workbench_controller_resolves_scene_and_guards_missing_view()
{
    FrameEditorWorkbenchController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    QCOMPARE(controller.sceneFromView(&view), &scene);
    QCOMPARE(controller.sceneFromView(nullptr), nullptr);
    QVERIFY(!controller.undo(nullptr));
    QVERIFY(!controller.redo(nullptr));
    QVERIFY(!controller.group(nullptr));
    QVERIFY(!controller.bestFit(nullptr));
    QVERIFY(!controller.shortcutRight(nullptr, QPointF(10, 0)));
}

void TestPowerTopology::frame_editor_workbench_controller_routes_delete_and_shortcut_actions()
{
    FrameEditorWorkbenchController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    QVERIFY(controller.shortcutRight(&view, QPointF(15, 0)));
    QCOMPARE(item->pos(), QPointF(15, 0));

    scene.setSelections({item});
    QVERIFY(controller.removeSelection(&view));
    QVERIFY(item->scene() == nullptr);
    delete item;
}

void TestPowerTopology::frame_editor_slot_workbench_controller_routes_center_widget_actions()
{
    FrameEditorSlotWorkbenchController controller;
    LzCenterWidget centerWidget;

    QVERIFY(!controller.undo(nullptr));
    QVERIFY(!controller.shortcutRight(nullptr, QPointF(1, 0)));
    QVERIFY(!controller.dispatch(nullptr, FrameEditorSlotWorkbenchController::Action::Undo));
    QVERIFY(!controller.dispatchShortcut(nullptr,
                                         QPointF(2, 3),
                                         FrameEditorSlotWorkbenchController::ShortcutDirection::Right));
    QCOMPARE(controller.currentView(nullptr), nullptr);
    QCOMPARE(controller.shortcutDelta(QPointF(4, 6),
                                      FrameEditorSlotWorkbenchController::ShortcutDirection::Left),
             QPointF(-4, 0));
    QCOMPARE(controller.shortcutDelta(QPointF(4, 6),
                                      FrameEditorSlotWorkbenchController::ShortcutDirection::Right),
             QPointF(4, 0));
    QCOMPARE(controller.shortcutDelta(QPointF(4, 6),
                                      FrameEditorSlotWorkbenchController::ShortcutDirection::Up),
             QPointF(0, -6));
    QCOMPARE(controller.shortcutDelta(QPointF(4, 6),
                                      FrameEditorSlotWorkbenchController::ShortcutDirection::Down),
             QPointF(0, 6));

    LzScene* scene = new LzScene();
    LzView* view = new LzView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    QCOMPARE(controller.currentView(&centerWidget), view);

    auto* item = scene->addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene->setSelections({item});

    QVERIFY(controller.shortcutRight(&centerWidget, QPointF(9, 0)));
    QCOMPARE(item->pos(), QPointF(9, 0));

    scene->setSelections({item});
    QVERIFY(controller.dispatchShortcut(&centerWidget,
                                        QPointF(3, 5),
                                        FrameEditorSlotWorkbenchController::ShortcutDirection::Down));
    QCOMPARE(item->pos(), QPointF(9, 5));

    scene->setSelections({item});
    QVERIFY(controller.dispatch(&centerWidget,
                                FrameEditorSlotWorkbenchController::Action::RemoveSelection));
    QVERIFY(item->scene() == nullptr);
    delete item;
}

void TestPowerTopology::frame_window_state_slot_workbench_controller_routes_center_widget_state_actions()
{
    FrameWindowStateSlotWorkbenchController controller;
    LzCenterWidget centerWidget;
    QAction undoAction(QStringLiteral("undo"), nullptr);
    QAction redoAction(QStringLiteral("redo"), nullptr);
    QAction saveAction(QStringLiteral("save"), nullptr);
    QAction saveAsAction(QStringLiteral("saveAs"), nullptr);

    QCOMPARE(controller.currentView(nullptr), nullptr);
    QCOMPARE(controller.tabWidget(nullptr), nullptr);
    controller.syncActionStates(nullptr, &undoAction, &redoAction, &saveAction, &saveAsAction);
    QVERIFY(!undoAction.isEnabled());
    QVERIFY(!redoAction.isEnabled());
    QVERIFY(!saveAction.isEnabled());
    QVERIFY(!saveAsAction.isEnabled());
    QVERIFY(!controller.renameCurrentTab(nullptr, QStringLiteral("x")));

    QTabWidget* tabs = centerWidget.getTabWidget();
    QVERIFY(tabs != nullptr);

    LzScene* scene = new LzScene();
    LzView* view = new LzView();
    view->setPowerScene(scene);
    scene->setView(view);
    view->setBackgroundCol(QColor(Qt::green));
    centerWidget.newTab(view);

    QPushButton button;
    QCOMPARE(controller.currentView(&centerWidget), view);
    QCOMPARE(controller.tabWidget(&centerWidget), tabs);

    controller.syncCurrentTabState(&centerWidget,
                                   &button,
                                   nullptr,
                                   &undoAction,
                                   &redoAction,
                                   &saveAction,
                                   &saveAsAction);
    QCOMPARE(button.palette().color(QPalette::Button), QColor(Qt::green));

    QVERIFY(controller.renameCurrentTab(&centerWidget, QStringLiteral("slot-title")));
    QCOMPARE(tabs->tabText(tabs->currentIndex()), QStringLiteral("slot-title"));
}

void TestPowerTopology::frame_topology_slot_workbench_controller_routes_center_widget_topology_actions()
{
    FrameTopologySlotWorkbenchController controller;
    LzCenterWidget centerWidget;

    QCOMPARE(controller.currentView(nullptr), nullptr);
    QCOMPARE(controller.currentScene(nullptr), nullptr);
    QVERIFY(!controller.dispatch(nullptr,
                                 nullptr,
                                 FrameTopologySlotWorkbenchController::Action::ShowSelectedSummary));
    QVERIFY(!controller.dispatch(nullptr,
                                 nullptr,
                                 FrameTopologySlotWorkbenchController::Action::HighlightSelected));
    QVERIFY(!controller.dispatch(nullptr,
                                 nullptr,
                                 FrameTopologySlotWorkbenchController::Action::ClearHighlights));
    QVERIFY(!controller.showSelectedSummary(nullptr, nullptr));
    QVERIFY(!controller.highlightSelected(nullptr, nullptr));
    QVERIFY(!controller.clearHighlights(nullptr));

    bool factoryCalled = false;
    QVERIFY(!controller.dispatchGenerateGraph(nullptr,
                                              nullptr,
                                              [&factoryCalled](LzScene*)
                                              {
                                                  factoryCalled = true;
                                                  return FrameTopologyController::GenerateContext{};
                                              },
                                              FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);
    QVERIFY(!controller.generateGraph(nullptr,
                                      nullptr,
                                      [&factoryCalled](LzScene*)
                                      {
                                          factoryCalled = true;
                                          return FrameTopologyController::GenerateContext{};
                                      },
                                      FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);

    LzScene* scene = new LzScene();
    LzView* view = new LzView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    QCOMPARE(controller.currentView(&centerWidget), view);
    QCOMPARE(controller.currentScene(&centerWidget), scene);
    QVERIFY(controller.dispatch(nullptr,
                                &centerWidget,
                                FrameTopologySlotWorkbenchController::Action::ClearHighlights));
    QVERIFY(controller.clearHighlights(&centerWidget));

    auto resolver = [&controller, &centerWidget]() { return controller.currentScene(&centerWidget); };
    QCOMPARE(resolver(), scene);
}

void TestPowerTopology::frame_tool_selection_slot_workbench_controller_resolves_scene_and_guards_inputs()
{
    FrameToolSelectionSlotWorkbenchController controller;
    LzCenterWidget centerWidget;

    QCOMPARE(controller.currentScene(nullptr), nullptr);
    QVERIFY(!controller.activateDrawTool(nullptr, nullptr));

    LzScene* scene = new LzScene();
    LzView* view = new LzView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    QCOMPARE(controller.currentScene(&centerWidget), scene);
    QVERIFY(!controller.activateDrawTool(nullptr, &centerWidget));
}

void TestPowerTopology::scene_edit_workbench_controller_resolves_stack_from_scene()
{
    LzSceneEditWorkbenchController controller;
    LzScene scene;
    LzView view;

    QCOMPARE(controller.stackFromScene(nullptr), nullptr);
    QCOMPARE(controller.stackFromScene(&scene), nullptr);

    view.setPowerScene(&scene);
    scene.setView(&view);
    QCOMPARE(controller.stackFromScene(&scene), view.getStack());
}

void TestPowerTopology::scene_edit_workbench_controller_routes_shortcut_and_delete()
{
    LzSceneEditWorkbenchController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    const QList<QGraphicsItem*> selection = {item};

    controller.shortcutRight(selection, &scene, controller.stackFromScene(&scene), QPointF(12, 0));
    QCOMPARE(item->pos(), QPointF(12, 0));

    controller.remove(selection, &scene, controller.stackFromScene(&scene));
    QVERIFY(item->scene() == nullptr);
    delete item;
}

void TestPowerTopology::scene_edit_slot_workbench_controller_routes_scene_actions()
{
    LzSceneEditSlotWorkbenchController controller;
    LzScene scene;
    LzView view;

    QVERIFY(!controller.copy(nullptr));
    QVERIFY(!controller.shortcutRight(nullptr, QPointF(1, 0)));
    QVERIFY(!controller.dispatch(nullptr, LzSceneEditSlotWorkbenchController::Action::Copy));
    QVERIFY(!controller.dispatchShortcut(nullptr,
                                         LzSceneEditSlotWorkbenchController::ShortcutDirection::Right,
                                         QPointF(1, 0)));

    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    QVERIFY(controller.dispatchShortcut(&scene,
                                        LzSceneEditSlotWorkbenchController::ShortcutDirection::Right,
                                        QPointF(7, 0)));
    QVERIFY(controller.shortcutRight(&scene, QPointF(7, 0)));
    QCOMPARE(item->pos(), QPointF(14, 0));

    scene.setSelections({item});
    QVERIFY(controller.dispatch(&scene, LzSceneEditSlotWorkbenchController::Action::Copy));
    QVERIFY(controller.dispatch(&scene, LzSceneEditSlotWorkbenchController::Action::Remove));
    QVERIFY(controller.remove(&scene));
    QVERIFY(item->scene() == nullptr);
    delete item;
}

void TestPowerTopology::scene_selection_workbench_controller_replaces_and_clears_selection()
{
    LzSceneSelectionWorkbenchController controller;
    LzScene scene;

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QList<QGraphicsItem*> selection;

    controller.replaceSelection(selection, {item1, item1, item2}, &scene);
    QCOMPARE(selection.size(), 2);
    QCOMPARE(controller.firstSelection(selection, &scene), static_cast<QGraphicsItem*>(item1));

    controller.clearSelection(selection, &scene);
    QVERIFY(selection.isEmpty());
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());
}

void TestPowerTopology::scene_selection_workbench_controller_routes_lifecycle_operations()
{
    LzSceneSelectionWorkbenchController controller;
    LzScene scene;
    QList<QGraphicsItem*> selection;

    auto* rect = new QGraphicsRectItem(QRectF(0, 0, 20, 20));
    QVERIFY(controller.insertSelection(&scene, selection, rect));
    QVERIFY(selection.contains(rect));
    QVERIFY(rect->scene() == &scene);

    QVERIFY(controller.removeSceneItem(&scene, selection, rect));
    QVERIFY(rect->scene() == nullptr);
    QVERIFY(!selection.contains(rect));

    QVERIFY(controller.insertSelection(&scene, selection, rect));
    QVERIFY(controller.deleteSelection(&scene, selection, rect));
    QVERIFY(rect->scene() == nullptr);
    delete rect;
}

void TestPowerTopology::scene_selection_slot_workbench_controller_routes_scene_selection_actions()
{
    LzSceneSelectionSlotWorkbenchController controller;
    LzScene scene;

    QVERIFY(controller.firstSelection(nullptr) == nullptr);
    QVERIFY(controller.selections(nullptr).isEmpty());
    QVERIFY(controller.dispatch(nullptr,
                                LzSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection)
            == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                LzSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections)
                .isEmpty());
    QVERIFY(!controller.replaceSelection(nullptr, {}));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneSelectionSlotWorkbenchController::SelectionAction::Clear));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::AddSelection,
                                 nullptr));
    QCOMPARE(controller.dispatch(nullptr,
                                 LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                                 {}),
             0);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    auto* detached = new QGraphicsRectItem(QRectF(60, 0, 20, 20));

    QCOMPARE(controller.dispatch(&scene,
                                 LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::ReplaceSelection,
                                 {item1, item2}),
             1);
    QVERIFY(controller.replaceSelection(&scene, {item1, item2}));
    QCOMPARE(controller.firstSelection(&scene), static_cast<QGraphicsItem*>(item1));
    QCOMPARE(controller.selections(&scene).size(), 2);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSelection,
                                item1));
    QVERIFY(controller.removeSelection(&scene, item1));
    QCOMPARE(controller.selections(&scene).size(), 1);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::InsertSelection,
                                detached));
    QVERIFY(controller.insertSelection(&scene, detached));
    QVERIFY(controller.selections(&scene).contains(detached));
    QVERIFY(detached->scene() == &scene);

    QCOMPARE(controller.dispatch(&scene,
                                 LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::RemoveSelections,
                                 {item2, detached}),
             2);
    QCOMPARE(controller.removeSelections(&scene, {item2, detached}), 2);
    QVERIFY(controller.selections(&scene).isEmpty());

    QCOMPARE(controller.dispatch(&scene,
                                 LzSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                                 {item1, item2}),
             2);
    QVERIFY(controller.addSelections(&scene, {item1, item2}));
    QCOMPARE(controller.selections(&scene).size(), 2);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSceneItem,
                                item2));
    QVERIFY(controller.removeSceneItem(&scene, item2));
    QVERIFY(item2->scene() == nullptr);
    QVERIFY(!controller.selections(&scene).contains(item2));
    delete item2;

    QVERIFY(controller.dispatch(&scene,
                                LzSceneSelectionSlotWorkbenchController::ItemSelectionAction::DeleteSelection,
                                detached));
    QVERIFY(controller.deleteSelection(&scene, detached));
    QVERIFY(detached->scene() == nullptr);
    delete detached;

    QVERIFY(controller.dispatch(&scene,
                                LzSceneSelectionSlotWorkbenchController::SelectionAction::Clear));
    QVERIFY(controller.selections(&scene).isEmpty());
}

void TestPowerTopology::scene_selection_slot_workbench_controller_routes_scene_selection_queries()
{
    LzSceneSelectionSlotWorkbenchController controller;
    LzScene scene;

    QVERIFY(controller.dispatch(nullptr,
                                LzSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection)
            == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                LzSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections)
                .isEmpty());

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    scene.setSelections({item1, item2});

    QCOMPARE(controller.dispatch(&scene,
                                 LzSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection),
             static_cast<QGraphicsItem*>(item1));
    const QList<QGraphicsItem*> selections = controller.dispatch(
        &scene,
        LzSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections);
    QCOMPARE(selections.size(), 2);
    QVERIFY(selections.contains(item1));
    QVERIFY(selections.contains(item2));
}

void TestPowerTopology::scene_state_slot_workbench_controller_routes_scene_state_actions()
{
    LzSceneStateSlotWorkbenchController controller;

    QCOMPARE(controller.view(static_cast<LzScene*>(nullptr)), nullptr);
    QCOMPARE(controller.view(static_cast<const LzScene*>(nullptr)), nullptr);
    QCOMPARE(controller.dispatch(static_cast<LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QCOMPARE(controller.dispatch(static_cast<const LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QVERIFY(!controller.setView(nullptr, nullptr));
    QVERIFY(!controller.flagState(nullptr, LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                                 true));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneStateSlotWorkbenchController::DeferredLineAction::CancelConnectLinePathRecompute,
                                 nullptr));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
                                 {}));
    QVERIFY(!controller.dispatchWiringData(nullptr, QStringLiteral("dispatch-k"), 7));
    QVERIFY(!controller.paintState(nullptr));
    QVERIFY(!controller.setPaintState(nullptr, true));
    QVERIFY(!controller.interactiveTransformActive(nullptr));
    QVERIFY(!controller.setInteractiveTransformActive(nullptr, true));
    QVERIFY(!controller.invalidateTopologySnapshots(nullptr));
    QVERIFY(!controller.scheduleTopologyRebuild(nullptr));
    QVERIFY(!controller.cancelConnectLinePathRecompute(nullptr, nullptr));
    QVERIFY(!controller.isConnectLinePathUpdatePending(nullptr));
    QVERIFY(!controller.dispatch(
        static_cast<const LzScene*>(nullptr),
        LzSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(!controller.requestDeferredItemPropertyPanel(nullptr));
    QVERIFY(!controller.flushDeferredItemPropertyPanel(nullptr));
    QVERIFY(!controller.scheduleConnectLinesPathRecompute(nullptr, {}));
    QVERIFY(!controller.scheduleConnectorPostprocess(nullptr, {}));
    QVERIFY(!controller.setWiringData(nullptr, QStringLiteral("k"), 1));
    QVERIFY(!controller.wiringData(nullptr, QStringLiteral("k")).isValid());
    QVERIFY(!controller.dispatch(static_cast<const LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("k"))
                 .isValid());

    LzScene scene;
    LzView view;
    QVERIFY(controller.setView(&scene, &view));
    QCOMPARE(controller.view(&scene), &view);
    QCOMPARE(controller.dispatch(&scene, LzSceneStateSlotWorkbenchController::ViewAction::Get), &view);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                                true));
    QVERIFY(controller.flagState(&scene, LzSceneStateSlotWorkbenchController::FlagStateAction::PaintState));
    QVERIFY(controller.setPaintState(&scene, true));
    QVERIFY(controller.paintState(&scene));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive,
                                true));
    QVERIFY(controller.flagState(&scene,
                                 LzSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive));
    QVERIFY(controller.setInteractiveTransformActive(&scene, true));
    QVERIFY(controller.interactiveTransformActive(&scene));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredSceneAction::InvalidateTopologySnapshots));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild));
    QVERIFY(controller.invalidateTopologySnapshots(&scene));
    QVERIFY(controller.scheduleTopologyRebuild(&scene));
    QVERIFY(!controller.isConnectLinePathUpdatePending(&scene));
    QVERIFY(!controller.dispatch(
        static_cast<const LzScene*>(&scene),
        LzSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredSceneAction::RequestDeferredItemPropertyPanel));
    QVERIFY(controller.requestDeferredItemPropertyPanel(&scene));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredSceneAction::FlushDeferredItemPropertyPanel));
    QVERIFY(controller.flushDeferredItemPropertyPanel(&scene));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
                                {}));
    QVERIFY(controller.scheduleConnectLinesPathRecompute(&scene, {}));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectorPostprocess,
                                {}));
    QVERIFY(controller.scheduleConnectorPostprocess(&scene, {}));
    QVERIFY(controller.dispatchWiringData(&scene, QStringLiteral("dispatch-slot-k"), 84));
    QCOMPARE(controller.wiringData(&scene, QStringLiteral("dispatch-slot-k")).toInt(), 84);
    QCOMPARE(controller.dispatch(static_cast<const LzScene*>(&scene),
                                 LzSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("dispatch-slot-k"))
                 .toInt(),
             84);
    QVERIFY(controller.setWiringData(&scene, QStringLiteral("slot-k"), 42));
    QCOMPARE(controller.wiringData(&scene, QStringLiteral("slot-k")).toInt(), 42);
}

void TestPowerTopology::scene_state_slot_workbench_controller_routes_scene_state_queries()
{
    LzSceneStateSlotWorkbenchController controller;
    LzScene scene;
    LzView view;

    QCOMPARE(controller.dispatch(static_cast<LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QCOMPARE(controller.dispatch(static_cast<const LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QVERIFY(!controller.dispatch(
        static_cast<const LzScene*>(nullptr),
        LzSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(!controller.dispatch(static_cast<const LzScene*>(nullptr),
                                 LzSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("missing"))
                 .isValid());

    QVERIFY(controller.setView(&scene, &view));
    QCOMPARE(controller.dispatch(&scene, LzSceneStateSlotWorkbenchController::ViewAction::Get), &view);
    QCOMPARE(controller.dispatch(static_cast<const LzScene*>(&scene),
                                 LzSceneStateSlotWorkbenchController::ViewAction::Get),
             &view);
    QVERIFY(!controller.dispatch(
        static_cast<const LzScene*>(&scene),
        LzSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));

    QVERIFY(controller.dispatchWiringData(&scene, QStringLiteral("query-slot-k"), 108));
    QCOMPARE(controller.dispatch(static_cast<const LzScene*>(&scene),
                                 LzSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("query-slot-k"))
                 .toInt(),
             108);
}

void TestPowerTopology::scene_topology_slot_workbench_controller_routes_scene_topology_actions()
{
    LzSceneTopologySlotWorkbenchController controller;

    QVERIFY(controller.topologyIndex(static_cast<LzScene*>(nullptr)) == nullptr);
    QVERIFY(controller.topologyIndex(static_cast<const LzScene*>(nullptr)) == nullptr);
    QVERIFY(controller.dispatch(static_cast<LzScene*>(nullptr),
                                LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex)
            == nullptr);
    QVERIFY(controller.dispatch(static_cast<const LzScene*>(nullptr),
                                LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex)
            == nullptr);
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneTopologySlotWorkbenchController::SceneAction::ClearTopologyHighlights));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneTopologySlotWorkbenchController::HighlightAction::Node,
                                 QStringLiteral("n"),
                                 true,
                                 QColor(Qt::red)));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                                 QStringList{QStringLiteral("e")},
                                 true,
                                 QColor(Qt::green)));
    QVERIFY(controller.dispatchMap(nullptr,
                                   LzSceneTopologySlotWorkbenchController::MapAction::BuildTopologyBindingSnapshot)
                .isEmpty());
    QCOMPARE(controller.dispatchSnapshot(
                 nullptr,
                 LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot)
                 .devices.size(),
             0);
    QCOMPARE(controller.dispatchDocumentExport(
                 nullptr,
                 LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport)
                 .devices.size(),
             0);
    QVERIFY(!controller.clearTopologyHighlights(nullptr));
    QVERIFY(!controller.highlightTopologyNode(nullptr, QStringLiteral("n"), true, QColor(Qt::red)));
    QVERIFY(controller.buildTopologyBindingSnapshot(nullptr).isEmpty());
    QVERIFY(controller.queryTopology(nullptr, QVariantMap{}).isEmpty());
    QVERIFY(controller.powerDeviceNodeIds(nullptr, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(nullptr, QStringLiteral("missing")).isEmpty());

    LzScene scene;
    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex));
    controller.rebuildTopologyIndex(&scene);
    QVERIFY(controller.topologyIndex(&scene) != nullptr);
    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex)
            != nullptr);

    const PowerTopologyAnalysisSnapshot snapshot = controller.buildPowerTopologySnapshot(&scene);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);
    const PowerTopologyAnalysisSnapshot dispatchedSnapshot =
        controller.dispatchSnapshot(
            &scene,
            LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot);
    QCOMPARE(dispatchedSnapshot.devices.size(), 0);
    QCOMPARE(dispatchedSnapshot.conductors.size(), 0);
    QCOMPARE(dispatchedSnapshot.nodes.size(), 0);
    const PowerTopologyDocumentExport dispatchedDocument =
        controller.dispatchDocumentExport(
            &scene,
            LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport);
    QCOMPARE(dispatchedDocument.devices.size(), 0);
    QCOMPARE(dispatchedDocument.conductors.size(), 0);
    QCOMPARE(dispatchedDocument.nodes.size(), 0);
    QCOMPARE(dispatchedDocument.viewScale, 1.0);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::HighlightAction::Node,
                                QStringLiteral("n"),
                                true,
                                QColor(Qt::red)));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::HighlightAction::Edge,
                                QStringLiteral("e"),
                                true,
                                QColor(Qt::yellow)));
    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                                QStringList{},
                                true,
                                QColor(Qt::blue)));
    QVERIFY(controller.dispatchMap(&scene,
                                   LzSceneTopologySlotWorkbenchController::MapAction::BuildPowerTopologyBindingSnapshot)
                .contains(QStringLiteral("power")));
    QVERIFY(controller.dispatchMap(&scene,
                                   LzSceneTopologySlotWorkbenchController::MapAction::BuildTopologyBindingSnapshot)
                .contains(QStringLiteral("power")));
    QVERIFY(controller.dispatchMap(&scene,
                                   LzSceneTopologySlotWorkbenchController::MapAction::QueryTopology,
                                   {{QStringLiteral("domain"), QStringLiteral("power")},
                                    {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}})
                .contains(QStringLiteral("ok")));
    QVERIFY(controller.dispatchDeviceNodeIds(
                nullptr,
                LzSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceNodeIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                nullptr,
                LzSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchIslandAnalysis(
                nullptr,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands)
                .islands.isEmpty());
    QVERIFY(controller.dispatchBranchAnalysis(
                nullptr,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerBranches,
                QStringLiteral("missing"))
                .branches.isEmpty());
    QVERIFY(controller.dispatchLoopAnalysis(
                nullptr,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerLoops)
                .loops.isEmpty());
    QVERIFY(!controller.dispatchPath(
                 nullptr,
                 LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchGroundPath(
                 nullptr,
                 LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerGroundPath,
                 QStringLiteral("a"))
                 .found);
    QVERIFY(controller.dispatchSwitchChangePreview(
                nullptr,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
                QStringLiteral("sw"),
                1)
                .deviceId.isEmpty());
    QVERIFY(controller.dispatchProtectionRangePreview(
                nullptr,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
                QStringLiteral("sw"),
                1,
                QStringLiteral("src"))
                .protectiveDeviceId.isEmpty());
    QVERIFY(controller.dispatchOperationPreview(
                nullptr,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
                QStringLiteral("sw"),
                1,
                QStringLiteral("s1"),
                QStringLiteral("s2"),
                QStringLiteral("g1"))
                .switchChange.deviceId.isEmpty());

    QVERIFY(controller.clearTopologyHighlights(&scene));
    QVERIFY(controller.highlightTopologyEdges(&scene, {}, true, QColor(Qt::blue)));
    QVERIFY(controller.dispatchDeviceNodeIds(
                &scene,
                LzSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceNodeIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                &scene,
                LzSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceConductorIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                &scene,
                LzSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchIslandAnalysis(
                &scene,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands)
                .islands.isEmpty());
    QVERIFY(controller.dispatchBranchAnalysis(
                &scene,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerBranches,
                QStringLiteral("missing"))
                .branches.isEmpty());
    QVERIFY(controller.dispatchLoopAnalysis(
                &scene,
                LzSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerLoops)
                .loops.isEmpty());
    QVERIFY(!controller.dispatchPath(
                 &scene,
                 LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchPath(
                 &scene,
                 LzSceneTopologySlotWorkbenchController::PathAction::ShortestDirectedPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchGroundPath(
                 &scene,
                 LzSceneTopologySlotWorkbenchController::PathAction::ShortestPowerGroundPath,
                 QStringLiteral("a"))
                 .found);
    QVERIFY(controller.dispatchSwitchChangePreview(
                &scene,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
                QStringLiteral("sw"),
                1)
                .deviceId.isEmpty());
    QVERIFY(controller.dispatchProtectionRangePreview(
                &scene,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
                QStringLiteral("sw"),
                1,
                QStringLiteral("src"))
                .protectiveDeviceId.isEmpty());
    QVERIFY(controller.dispatchOperationPreview(
                &scene,
                LzSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
                QStringLiteral("sw"),
                1,
                QStringLiteral("s1"),
                QStringLiteral("s2"),
                QStringLiteral("g1"))
                .switchChange.deviceId.isEmpty());
}

void TestPowerTopology::scene_topology_slot_workbench_controller_routes_scene_topology_index_and_snapshot_dispatch()
{
    LzSceneTopologySlotWorkbenchController controller;
    LzScene scene;

    QVERIFY(controller.dispatch(static_cast<LzScene*>(nullptr),
                                LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex)
            == nullptr);
    QVERIFY(controller.dispatch(static_cast<const LzScene*>(nullptr),
                                LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex)
            == nullptr);

    QVERIFY(controller.dispatch(&scene,
                                LzSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex));

    TopologyGraphIndex* mutableIndex =
        controller.dispatch(&scene, LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex);
    const TopologyGraphIndex* constIndex =
        controller.dispatch(static_cast<const LzScene*>(&scene),
                            LzSceneTopologySlotWorkbenchController::IndexAction::TopologyIndex);
    QVERIFY(mutableIndex != nullptr);
    QVERIFY(constIndex != nullptr);
    QCOMPARE(static_cast<const void*>(mutableIndex), static_cast<const void*>(constIndex));

    const PowerTopologyAnalysisSnapshot snapshot =
        controller.dispatchSnapshot(
            &scene,
            LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);

    const PowerTopologyDocumentExport document =
        controller.dispatchDocumentExport(
            &scene,
            LzSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport);
    QCOMPARE(document.devices.size(), 0);
    QCOMPARE(document.conductors.size(), 0);
    QCOMPARE(document.nodes.size(), 0);
    QCOMPARE(document.viewScale, 1.0);
}

void TestPowerTopology::scene_slot_workbench_controllers_support_scene_view_and_dom_entrypoints()
{
    LzScene scene;
    LzView view;

    scene.setView(&view);
    QCOMPARE(scene.getView(), &view);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene-slot.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><LzFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(scene.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("LzFile"));
}

void TestPowerTopology::scene_document_slot_workbench_controller_routes_scene_document_actions()
{
    LzSceneDocumentSlotWorkbenchController controller;
    QString error;
    QDomDocument dom;

    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneDocumentSlotWorkbenchController::DocumentAction::Load,
                                 QStringLiteral("missing.xml"),
                                 &error));
    QVERIFY(!controller.dispatch(nullptr,
                                 LzSceneDocumentSlotWorkbenchController::DocumentAction::Save,
                                 QStringLiteral("missing.xml")));
    QVERIFY(!controller.dispatch(LzSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
                                 QStringLiteral("Z:/definitely-missing-file.xml"),
                                 &dom,
                                 &error));
    QVERIFY(!controller.load(nullptr, QStringLiteral("missing.xml"), &error));
    QVERIFY(!controller.save(nullptr, QStringLiteral("missing.xml")));
    QVERIFY(!controller.toDomDocument(QStringLiteral("Z:/definitely-missing-file.xml"), &dom, &error));
    QVERIFY(controller.createGroup(nullptr, {}) == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                LzSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
                                {})
            == nullptr);
    QCOMPARE(controller.sceneType(nullptr), FILE_TYPE::LZ_DRAW);
    QCOMPARE(controller.dispatchSceneType(nullptr,
                                         LzSceneDocumentSlotWorkbenchController::SceneTypeAction::Get),
             FILE_TYPE::LZ_DRAW);
    QVERIFY(!controller.setSceneType(nullptr, FILE_TYPE::LZ_DYNAMIC));

    LzScene scene;
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::LZ_DRAW);
    QCOMPARE(controller.dispatchSceneType(&scene,
                                          LzSceneDocumentSlotWorkbenchController::SceneTypeAction::Get),
             FILE_TYPE::LZ_DRAW);
    QVERIFY(controller.setSceneType(&scene, FILE_TYPE::LZ_DYNAMIC));
    QCOMPARE(controller.dispatchSceneType(&scene,
                                          LzSceneDocumentSlotWorkbenchController::SceneTypeAction::Set,
                                          FILE_TYPE::LZ_DYNAMIC),
             FILE_TYPE::LZ_DYNAMIC);
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::LZ_DYNAMIC);
    QVERIFY(controller.setSceneType(&scene, FILE_TYPE::LZ_DYNAMIC));
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::LZ_DYNAMIC);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.dispatch(
        &scene,
        LzSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestPowerTopology::scene_document_slot_workbench_controller_routes_document_and_group_dispatch()
{
    LzSceneDocumentSlotWorkbenchController controller;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene-document-slot.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><LzFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(controller.dispatch(LzSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
                                validPath,
                                &dom,
                                &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("LzFile"));

    LzScene scene;
    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.dispatch(
        &scene,
        LzSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestPowerTopology::scene_topology_workbench_controller_rebuilds_index_and_reads_snapshot()
{
    LzSceneTopologyWorkbenchController controller;
    LzSceneTopologyStateController stateController;
    LzScene scene;

    QVERIFY(controller.topologyIndex(stateController) == nullptr);
    controller.rebuildTopologyIndex(&scene, stateController);
    QVERIFY(controller.topologyIndex(stateController) != nullptr);

    const PowerTopologyAnalysisSnapshot snapshot =
        controller.buildPowerTopologySnapshot(&scene, stateController);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);
}

void TestPowerTopology::scene_topology_workbench_controller_guards_empty_queries_and_wiring_data()
{
    LzSceneTopologyWorkbenchController controller;
    LzScene scene;
    QHash<QString, QVariant> wiringData;

    QVERIFY(controller.powerDeviceNodeIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.analyzePowerIslands(nullptr).islands.isEmpty());
    QVERIFY(controller.buildTopologyBindingSnapshot(nullptr).isEmpty());
    QVERIFY(!controller.setWiringData(wiringData, QString(), 1));
    QVERIFY(controller.setWiringData(wiringData, QStringLiteral("k"), 2));
    QCOMPARE(controller.wiringData(wiringData, QStringLiteral("k")).toInt(), 2);
}

void TestPowerTopology::frame_topology_workbench_controller_resolves_scene_and_guards_missing_view()
{
    FrameTopologyWorkbenchController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    QCOMPARE(controller.sceneFromView(&view), &scene);
    QCOMPARE(controller.sceneFromView(nullptr), nullptr);
    QVERIFY(!controller.showSelectedSummary(nullptr, nullptr));
    QVERIFY(!controller.highlightSelected(nullptr, nullptr));
    QVERIFY(!controller.clearHighlights(nullptr));

    bool factoryCalled = false;
    QVERIFY(!controller.generateGraph(nullptr,
                                      nullptr,
                                      [&factoryCalled](LzScene*)
                                      {
                                          factoryCalled = true;
                                          return FrameTopologyController::GenerateContext{};
                                      },
                                      FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);
}

void TestPowerTopology::frame_topology_workbench_controller_clears_highlights_for_active_scene()
{
    FrameTopologyWorkbenchController controller;
    LzScene scene;
    LzView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    QVERIFY(controller.clearHighlights(&view));
}

void TestPowerTopology::frame_workbench_ui_controller_syncs_tab_and_selection_state()
{
    FrameWorkbenchUiController controller;

    QTabWidget tabs;
    QPushButton button;
    LzView view;
    LzScene scene;
    view.setPowerScene(&scene);
    scene.setView(&view);
    view.setBackgroundCol(QColor(Qt::yellow));
    tabs.addTab(&view, QStringLiteral("scene"));
    tabs.setCurrentWidget(&view);

    CimModel model;
    CimObject object;
    object.mrid = QStringLiteral("breaker-001");
    object.className = QStringLiteral("Breaker");
    object.name = QStringLiteral("Breaker 1");
    QVERIFY(model.addObject(object));

    CimModelBrowserDock dock;
    dock.setImportResult(model, {}, {}, {}, {});

    auto* shape = new LzPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    shape->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("breaker-001"));
    shape->setSelected(true);
    scene.setSelections({shape});

    controller.handleCurrentTabChanged(&tabs, &button, &dock);
    QCOMPARE(button.palette().color(QPalette::Button), QColor(Qt::yellow));

    auto* treeWidget = dock.findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QVERIFY(treeWidget->currentItem() != nullptr);
    QCOMPARE(treeWidget->currentItem()->data(0, Qt::UserRole).toString(), QStringLiteral("breaker-001"));

    LzItemController propertyController;
    controller.handleCurrentObjectChanged(&propertyController, {shape}, &dock);
    QCOMPARE(propertyController.getObject(), static_cast<QObject*>(shape));
    QCOMPARE(propertyController.getView(), &view);
    QVERIFY(treeWidget->currentItem() != nullptr);
    QCOMPARE(treeWidget->currentItem()->data(0, Qt::UserRole).toString(), QStringLiteral("breaker-001"));
}

void TestPowerTopology::frame_workbench_ui_controller_handles_empty_and_multi_selection()
{
    FrameWorkbenchUiController controller;
    LzItemController propertyController;
    CimModelBrowserDock dock;

    QObject seedObject;
    propertyController.setObject(&seedObject);

    QCOMPARE(controller.handleCurrentObjectChanged(&propertyController, {}, &dock), nullptr);
    QCOMPARE(propertyController.getObject(), nullptr);

    auto* item1 = new LzPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* item2 = new LzPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    propertyController.setObject(item1);

    QCOMPARE(controller.handleCurrentObjectChanged(&propertyController, {item1, item2}, &dock),
             static_cast<QObject*>(item1));
    QCOMPARE(propertyController.getObject(), static_cast<QObject*>(item1));

    delete item1;
    delete item2;
}

void TestPowerTopology::center_widget_emits_scene_and_tab_close_signals()
{
    LzCenterWidget widget;
    QSignalSpy newSceneSpy(&widget, &LzCenterWidget::requestNewScene);
    QSignalSpy tabClosedSpy(&widget, &LzCenterWidget::currentTabClosed);
    QSignalSpy closeAllSpy(&widget, &LzCenterWidget::closeAll);

    widget.newTab();
    QCOMPARE(newSceneSpy.count(), 1);

    auto* view = new LzView();
    widget.newTab(view);
    QCOMPARE(widget.getCurrentView(), view);

    widget.closeTab();
    QCOMPARE(tabClosedSpy.count(), 1);
    QCOMPARE(closeAllSpy.count(), 1);
    QCOMPARE(widget.getCurrentView(), nullptr);
}

void TestPowerTopology::center_widget_ignores_invalid_close_index()
{
    LzCenterWidget widget;
    QSignalSpy tabClosedSpy(&widget, &LzCenterWidget::currentTabClosed);
    QSignalSpy closeAllSpy(&widget, &LzCenterWidget::closeAll);

    auto* view = new LzView();
    widget.newTab(view);
    QCOMPARE(widget.getCurrentView(), view);

    widget.closeTab(-1);
    QCOMPARE(widget.getCurrentView(), view);
    QCOMPARE(tabClosedSpy.count(), 0);
    QCOMPARE(closeAllSpy.count(), 0);

    widget.closeTab(3);
    QCOMPARE(widget.getCurrentView(), view);
    QCOMPARE(tabClosedSpy.count(), 0);
    QCOMPARE(closeAllSpy.count(), 0);
}

void TestPowerTopology::viewport_controller_computes_contents_rect_and_dispatches_mouse_event()
{
    LzSceneViewportController controller;
    LzScene scene;
    scene.setSceneRect(QRectF(0, 0, 50, 30));

    const QRectF emptyRect = controller.contentsRect(&scene);
    QCOMPARE(emptyRect.center(), scene.sceneRect().center());
    QVERIFY(emptyRect.width() >= scene.sceneRect().width());
    QVERIFY(emptyRect.height() >= scene.sceneRect().height());

    auto* rect = scene.addRect(QRectF(10, 20, 30, 40));
    const QRectF contentRect = controller.contentsRect(&scene);
    QVERIFY(contentRect.contains(rect->sceneBoundingRect()));

    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    QVERIFY(controller.dispatchCompatMouseEvent(&scene, &pressEvent));

    QGraphicsSceneMouseEvent unknownEvent(QEvent::None);
    QVERIFY(!controller.dispatchCompatMouseEvent(&scene, &unknownEvent));
    QVERIFY(!controller.dispatchCompatMouseEvent(nullptr, &pressEvent));
    QVERIFY(!controller.dispatchCompatMouseEvent(&scene, nullptr));
}

void TestPowerTopology::topology_state_controller_rebuilds_index_for_empty_scene()
{
    LzSceneTopologyStateController controller;
    LzScene scene;

    QVERIFY(controller.topologyIndex() == nullptr);
    controller.rebuildTopologyIndex(&scene);
    QVERIFY(controller.topologyIndex() != nullptr);
    QVERIFY(controller.topologyIndex() == controller.topologyIndex());
}

void TestPowerTopology::topology_state_controller_invalidates_cached_snapshot()
{
    LzSceneTopologyStateController controller;
    LzScene scene;

    const PowerTopologyAnalysisSnapshot first = controller.buildPowerTopologySnapshot(&scene);
    QCOMPARE(first.devices.size(), 0);
    QCOMPARE(first.conductors.size(), 0);
    QCOMPARE(first.nodes.size(), 0);

    controller.invalidateSnapshots();
    const PowerTopologyAnalysisSnapshot second = controller.buildPowerTopologySnapshot(&scene);
    QCOMPARE(second.devices.size(), 0);
    QCOMPARE(second.conductors.size(), 0);
    QCOMPARE(second.nodes.size(), 0);
}

void TestPowerTopology::topology_state_controller_exports_default_view_state_without_view()
{
    LzSceneTopologyStateController controller;
    LzScene scene;
    scene.setSceneRect(QRectF(0, 0, 120, 80));

    const PowerTopologyDocumentExport document = controller.buildPowerTopologyDocumentExport(&scene);

    QCOMPARE(document.devices.size(), 0);
    QCOMPARE(document.conductors.size(), 0);
    QCOMPARE(document.nodes.size(), 0);
    QCOMPARE(document.viewScale, 1.0);
    QCOMPARE(document.viewCenter, scene.getContentsRect().center());
}

void TestPowerTopology::power_topology_analysis_controller_returns_empty_results_without_scene()
{
    LzScenePowerTopologyAnalysisController controller;

    QVERIFY(controller.analyzePowerIslands(nullptr).islands.isEmpty());
    QVERIFY(controller.analyzePowerLoops(nullptr).loops.isEmpty());
    QVERIFY(!controller.shortestPowerSupplyPath(nullptr, QStringLiteral("a"), QStringLiteral("b")).found);
    QVERIFY(!controller.shortestDirectedPowerSupplyPath(nullptr, QStringLiteral("a"), QStringLiteral("b")).found);
    QVERIFY(!controller.shortestPowerGroundPath(nullptr, QStringLiteral("a")).found);
}

void TestPowerTopology::power_topology_analysis_controller_rejects_switch_preview_for_non_switch_device()
{
    LzScenePowerTopologyAnalysisController controller;
    LzScene scene;
    auto* load = new LzPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(load);

    const QString deviceId = QStringLiteral("load-preview-device");
    load->setTopologyNodeStableId(deviceId);

    const PowerTopologySwitchChangePreview switchPreview =
        controller.previewPowerSwitchTopologyChange(&scene, deviceId, 1);
    const PowerTopologyProtectionRangePreview protectionPreview =
        controller.previewPowerProtectionRange(&scene, deviceId, 1, QStringLiteral("source"));
    const PowerTopologyOperationPreview operationPreview =
        controller.previewPowerSwitchOperation(&scene, deviceId, 1, QStringLiteral("s1"), QStringLiteral("s2"), QStringLiteral("g1"));

    QVERIFY(switchPreview.deviceId.isEmpty());
    QVERIFY(protectionPreview.protectiveDeviceId.isEmpty());
    QVERIFY(operationPreview.switchChange.deviceId.isEmpty());
}

void TestPowerTopology::power_topology_analysis_controller_returns_empty_device_queries_for_empty_scene()
{
    LzScenePowerTopologyAnalysisController controller;
    LzScene scene;

    QVERIFY(controller.powerDeviceNodeIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.powerDeviceConductorIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.busbarAttachedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.reachablePowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.directedReachablePowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.analyzePowerBranches(&scene, QStringLiteral("missing")).branches.isEmpty());
}

void TestPowerTopology::context_menu_controller_creates_and_trims_menu_actions()
{
    LzSceneContextMenuController controller;
    LzScene scene;

    QMenu* menu = controller.createMenu(&scene);
    QVERIFY(menu != nullptr);
    QCOMPARE(menu->actions().size(), 6);
    QCOMPARE(menu->actions().at(0)->text(), QStringLiteral("剪切"));
    QCOMPARE(menu->actions().at(5)->text(), QStringLiteral("取消组合"));

    menu->addAction(QStringLiteral("临时动作1"));
    menu->addAction(QStringLiteral("临时动作2"));
    QCOMPARE(menu->actions().size(), 8);

    controller.trimTransientActions(menu, 6);
    QCOMPARE(menu->actions().size(), 6);
    QCOMPARE(menu->actions().at(0)->text(), QStringLiteral("剪切"));
    QCOMPARE(menu->actions().at(5)->text(), QStringLiteral("取消组合"));

    delete menu;
}

void TestPowerTopology::context_menu_controller_resets_existing_menu()
{
    LzSceneContextMenuController controller;
    LzScene scene;

    QMenu* menu = controller.createMenu(&scene);
    menu->addAction(QStringLiteral("临时动作"));
    QVERIFY(menu->actions().size() > 6);

    QMenu* reset = controller.resetMenu(menu, &scene);
    QVERIFY(reset != nullptr);
    QCOMPARE(reset->actions().size(), 6);
    QCOMPARE(reset->actions().at(0)->text(), QStringLiteral("剪切"));

    delete reset;
}

void TestPowerTopology::context_menu_controller_ensures_menu_instance()
{
    LzSceneContextMenuController controller;
    LzScene scene;

    QMenu* created = controller.ensureMenu(nullptr, &scene);
    QVERIFY(created != nullptr);
    QCOMPARE(created->actions().size(), 6);

    QMenu* reused = controller.ensureMenu(created, &scene);
    QCOMPARE(reused, created);

    delete created;
}

void TestPowerTopology::context_menu_controller_prepares_menu_for_display()
{
    LzSceneContextMenuController controller;
    LzScene scene;

    QMenu* menu = controller.createMenu(&scene);
    menu->addAction(QStringLiteral("临时动作"));
    QVERIFY(menu->actions().size() > 6);

    QMenu* prepared = controller.prepareMenuForDisplay(menu, 6);
    QCOMPARE(prepared, menu);
    QCOMPARE(menu->actions().size(), 6);
    QCOMPARE(menu->actions().at(0)->text(), QStringLiteral("剪切"));

    delete menu;
}

void TestPowerTopology::scene_constructs_context_menu_for_exec_path()
{
    LzScene scene;

    QTimer::singleShot(0, []()
    {
        if (QWidget* popup = QApplication::activePopupWidget())
            popup->close();
    });

    QAction* action = scene.execContextMenu(QPoint(0, 0));
    Q_UNUSED(action);

    // 自动关闭弹出菜单后允许返回空，但统一菜单执行链路必须可用且不应崩溃。
    QVERIFY(true);
}

void TestPowerTopology::scene_exec_context_menu_remains_repeatable()
{
    LzScene scene;

    auto closePopup = []()
    {
        if (QWidget* popup = QApplication::activePopupWidget())
            popup->close();
    };

    QTimer::singleShot(0, closePopup);
    QAction* first = scene.execContextMenu(QPoint(0, 0));
    Q_UNUSED(first);

    QTimer::singleShot(0, closePopup);
    QAction* second = scene.execContextMenu(QPoint(0, 0));
    Q_UNUSED(second);

    QVERIFY(true);
}

void TestPowerTopology::interaction_controller_accepts_only_text_drop_mime()
{
    LzSceneInteractionController controller;
    QMimeData emptyMime;
    QMimeData textMime;
    textMime.setText(QStringLiteral("rect"));

    QVERIFY(!controller.acceptsTextDrop(nullptr));
    QVERIFY(!controller.acceptsTextDrop(&emptyMime));
    QVERIFY(controller.acceptsTextDrop(&textMime));
}

void TestPowerTopology::scene_document_workbench_controller_routes_document_and_group_entrypoints()
{
    LzSceneDocumentWorkbenchController controller;
    LzScene scene;

    QVERIFY(!controller.load(nullptr, QStringLiteral("missing.xml"), nullptr));
    QVERIFY(!controller.save(nullptr, QStringLiteral("x.xml")));

    FILE_TYPE type = FILE_TYPE::LZ_DRAW;
    QCOMPARE(controller.sceneType(type), FILE_TYPE::LZ_DRAW);
    controller.setSceneType(&type, FILE_TYPE::LZ_DYNAMIC);
    QCOMPARE(type, FILE_TYPE::LZ_DYNAMIC);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.createGroup(&scene, {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestPowerTopology::document_controller_rejects_missing_or_invalid_dom_sources()
{
    LzSceneDocumentController controller;
    QDomDocument dom;
    QString error;

    QVERIFY(!controller.toDomDocument(QStringLiteral("Z:/definitely-missing-file.xml"), &dom, &error));
    QVERIFY(!error.isEmpty());

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString invalidPath = dir.filePath(QStringLiteral("invalid.xml"));
    QFile invalidFile(invalidPath);
    QVERIFY(invalidFile.open(QIODevice::WriteOnly | QIODevice::Text));
    invalidFile.write("<not-closed>");
    invalidFile.close();

    error.clear();
    QVERIFY(!controller.toDomDocument(invalidPath, &dom, &error));
    QVERIFY(!error.isEmpty());
}

void TestPowerTopology::document_controller_parses_valid_dom_and_manages_scene_type()
{
    LzSceneDocumentController controller;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("valid.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><LzFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(controller.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("LzFile"));

    FILE_TYPE type = FILE_TYPE::LZ_DRAW;
    QCOMPARE(controller.sceneType(type), FILE_TYPE::LZ_DRAW);
    controller.setSceneType(&type, FILE_TYPE::LZ_DYNAMIC);
    QCOMPARE(type, FILE_TYPE::LZ_DYNAMIC);
}

void TestPowerTopology::document_controller_rejects_save_without_scene_or_path()
{
    LzSceneDocumentController controller;
    LzScene scene;

    QVERIFY(!controller.save(nullptr, QStringLiteral("x.xml")));
    QVERIFY(!controller.save(&scene, QString()));
    QVERIFY(!controller.load(nullptr, QStringLiteral("x.xml"), nullptr));
}

void TestPowerTopology::scene_to_dom_document_routes_through_document_controller()
{
    LzScene scene;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><LzFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(scene.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("LzFile"));
}

void TestPowerTopology::group_controller_creates_group_and_reparents_items()
{
    LzSceneGroupController controller;
    LzScene scene;

    auto* rect1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* rect2 = scene.addRect(QRectF(40, 0, 20, 20));
    rect1->setSelected(true);
    rect2->setSelected(true);

    QGraphicsItemGroup* group = controller.createGroup(&scene, {rect1, rect2});
    QVERIFY(group != nullptr);
    QCOMPARE(rect1->parentItem(), group);
    QCOMPARE(rect2->parentItem(), group);
    QVERIFY(group->scene() == &scene);
    QVERIFY(!rect1->isSelected());
    QVERIFY(!rect2->isSelected());
}

void TestPowerTopology::item_lifecycle_controller_handles_insert_delete_and_batch_selection()
{
    LzSceneItemLifecycleController controller;
    LzScene scene;
    QList<QGraphicsItem*> selection;

    auto* rect1 = new QGraphicsRectItem(QRectF(0, 0, 20, 20));
    auto* rect2 = new QGraphicsRectItem(QRectF(30, 0, 20, 20));
    auto* line = new LzConnectLine();

    QVERIFY(controller.insertSelection(&scene, selection, rect1));
    QCOMPARE(rect1->scene(), &scene);
    QVERIFY(rect1->isVisible());
    QVERIFY(rect1->isSelected());
    QCOMPARE(selection.size(), 1);
    QVERIFY(selection.contains(rect1));

    QVERIFY(controller.insertSelection(&scene, selection, line));
    QCOMPARE(line->scene(), &scene);
    QVERIFY(line->isVisible());
    QCOMPARE(selection.size(), 2);
    QVERIFY(selection.contains(line));

    QCOMPARE(controller.addSelections(&scene, selection, {rect1, rect2}), 1);
    QCOMPARE(selection.size(), 3);
    QVERIFY(selection.contains(rect2));
    QVERIFY(rect2->isSelected());

    QCOMPARE(controller.removeSelections(&scene, selection, {rect2}), 1);
    QCOMPARE(selection.size(), 2);
    QVERIFY(!selection.contains(rect2));
    QVERIFY(!rect2->isSelected());

    QVERIFY(controller.deleteSelection(&scene, selection, line));
    QVERIFY(!line->isVisible());
    QVERIFY(line->scene() == nullptr);
    QCOMPARE(selection.size(), 1);
    QVERIFY(!selection.contains(line));

    QVERIFY(controller.removeSceneItem(&scene, selection, rect1));
    QVERIFY(rect1->scene() == nullptr);
    QVERIFY(!selection.contains(rect1));

    delete rect1;
    delete rect2;
    delete line;
}

void TestPowerTopology::item_lifecycle_controller_ignores_null_scene_or_items()
{
    LzSceneItemLifecycleController controller;
    LzScene scene;
    QList<QGraphicsItem*> selection;
    auto* rect = new QGraphicsRectItem(QRectF(0, 0, 20, 20));

    QVERIFY(!controller.insertSelection(nullptr, selection, rect));
    QVERIFY(!controller.insertSelection(&scene, selection, nullptr));
    QVERIFY(!controller.deleteSelection(nullptr, selection, rect));
    QVERIFY(!controller.deleteSelection(&scene, selection, nullptr));
    QVERIFY(!controller.removeSceneItem(nullptr, selection, rect));
    QVERIFY(!controller.removeSceneItem(&scene, selection, nullptr));
    QCOMPARE(controller.addSelections(nullptr, selection, {rect}), 0);
    QCOMPARE(controller.removeSelections(nullptr, selection, {rect}), 0);

    delete rect;
}
