#include "test_cimdraw_topology.h"

#include <QApplication>
#include <QDomDocument>
#include <QGraphicsSimpleTextItem>
#include <QSignalSpy>
#include <QTimer>
#include <memory>

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
#include "topology/ui/FrameTopologyController.h"
#include "FrameTopologySlotWorkbenchController.h"
#include "FrameTopologyWorkbenchController.h"
#include "FrameWindowStateWorkbenchController.h"
#include "FrameWindowStateSlotWorkbenchController.h"
#include "FrameWorkbenchUiController.h"
#include "datamodule/CimdrawDataSourceConfigLoader.h"
#include "datamodule/CimdrawDataSourceBuilder.h"
#include "datamodule/CimdrawDataSourceFactory.h"
#include "datamodule/CimdrawDataSourceManager.h"
#include "datamodule/CimdrawDataSourceRuntimeController.h"
#include "datamodule/CimdrawIDataSource.h"
#include "CimdrawCenterWidget.h"
#include "CimdrawFile.h"
#include "CimdrawMimeData.h"
#include "CimdrawImageManager.h"
#include "CimdrawItemController.h"
#include "CimdrawToolBoxManager.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "FrameWorkbenchStateController.h"
#include "cim/query/CimGeneratedTopologyLayout.h"
#include "cim/query/CimGeneratedTopologyGraphQuery.h"
#include "cim/query/CimPowerTopologyPreviewQuery.h"
#include "cim/query/CimPowerTopologyRuntimeQuery.h"
#include "cim/query/CimTopologyCompatQuery.h"
#include "cim/query/CimTopologySummaryQuery.h"
#include "FrameToolSelectionController.h"
#include "FrameToolSelectionSlotWorkbenchController.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"
#include "cim/behavior/CimBehaviorRuntimeIngest.h"
#include "cim/behavior/CimBehaviorResultVariant.h"
#include "cim/query/CimQueryFacade.h"
#include "cim/query/CimGraphicBehaviorQuery.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"
#include "cim/query/CimGraphicRenderStateQuery.h"
#include "cim/query/CimGraphicSummaryText.h"
#include "cim/query/CimGraphicVisualSummaryQuery.h"
#include "cim/ui/CimBrowserBridge.h"
#include "cim/ui/CimModelBrowserDock.h"
#include "cim/ui/CimSceneBuilder.h"
#include "item/CimdrawConnectPoint.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawTopologyNodeItem.h"
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
#include "scene/CimdrawSceneStateController.h"
#include "scene/CimdrawSceneStateSlotWorkbenchController.h"
#include "scene/CimdrawSceneStateWorkbenchController.h"
#include "scene/CimdrawSceneTopologySlotWorkbenchController.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"
#include "scene/CimdrawSceneTopologyQueryController.h"
#include "scene/CimdrawSceneTopologyStateController.h"
#include "scene/CimdrawSceneViewportController.h"
#include "Tool/CimdrawTool.h"
#include "wiring/base/CimdrawWiringSymbolStyle.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"
#include "wiring/power/CimdrawPowerArresterItem.h"
#include "wiring/power/CimdrawPowerCableItem.h"
#include "wiring/power/CimdrawPowerCapacitorItem.h"
#include "wiring/power/CimdrawPowerCircuitBreakerItem.h"
#include "wiring/power/CimdrawPowerCurrentTransformerItem.h"
#include "wiring/power/CimdrawPowerEarthSwitchItem.h"
#include "wiring/power/CimdrawPowerFuseItem.h"
#include "wiring/power/CimdrawPowerGridItem.h"
#include "wiring/power/CimdrawPowerGeneratorItem.h"
#include "wiring/power/CimdrawPowerGroundItem.h"
#include "wiring/power/CimdrawPowerLoadBreakItem.h"
#include "wiring/power/CimdrawPowerLoadItem.h"
#include "wiring/power/CimdrawPowerMotorItem.h"
#include "wiring/power/CimdrawPowerReactorItem.h"
#include "wiring/power/CimdrawPowerVoltageTransformerItem.h"

#include <QTemporaryDir>
#include <QColor>
#include <QFile>
#include <QComboBox>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QPalette>
#include <QPushButton>
#include <QTreeWidget>
#include <QTabWidget>

namespace {

class RecordingTool : public CimdrawTool
{
public:
    explicit RecordingTool(QObject* parent = nullptr)
        : CimdrawTool(parent)
    {
        name = QStringLiteral("recording-tool");
        drawType = CIMDRAW_SELECTION;
    }

    QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override
    {
        Q_UNUSED(scene);
        Q_UNUSED(pos);
        Q_UNUSED(addToScene);
        return nullptr;
    }

    void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++pressCount;
    }

    void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++moveCount;
    }

    void onMouseRelease(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++releaseCount;
    }

    void onMouseDoubleClick(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override
    {
        lastScene = scene;
        lastEvent = evt;
        ++doubleClickCount;
    }

    int pressCount = 0;
    int moveCount = 0;
    int releaseCount = 0;
    int doubleClickCount = 0;
    CimdrawScene* lastScene = nullptr;
    QGraphicsSceneMouseEvent* lastEvent = nullptr;
};

class TestableCircuitBreakerItem : public CimdrawPowerCircuitBreakerItem
{
public:
    explicit TestableCircuitBreakerItem(const QRectF& rect)
        : CimdrawPowerCircuitBreakerItem(rect)
    {
    }

    bool visualOpenForTest() const
    {
        return breakerVisualOpen();
    }

    bool toggleSwitchPositionFromInteractionForTest()
    {
        return toggleSwitchPositionFromInteraction();
    }
};

class RecordingDataSource : public CimdrawIDataSource
{
public:
    explicit RecordingDataSource(const QString& sourceTypeId,
                                 bool startResult = true,
                                 QObject* parent = nullptr)
        : CimdrawIDataSource(parent)
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

CimdrawConnectPoint* findConnectPort(QGraphicsItem* item, CONNECT_DIRECTION direction, int ordinal = 0)
{
    if (!item)
        return nullptr;

    int matched = 0;
    for (QGraphicsItem* child : item->childItems())
    {
        auto* port = qgraphicsitem_cast<CimdrawConnectPoint*>(child);
        if (!port || port->getDirection() != direction)
            continue;
        if (matched == ordinal)
            return port;
        ++matched;
    }
    return nullptr;
}

void connectPowerItems(CimdrawScene* scene,
                       TmpShape* startShape,
                       CimdrawConnectPoint* startPort,
                       TmpShape* endShape,
                       CimdrawConnectPoint* endPort)
{
    QVERIFY(scene != nullptr);
    QVERIFY(startShape != nullptr);
    QVERIFY(startPort != nullptr);
    QVERIFY(endShape != nullptr);
    QVERIFY(endPort != nullptr);

    auto* line = new CimdrawConnectLine(QRectF(0, 0, 1, 1));
    scene->addItem(line);
    line->setConnectStrategy(BROKEN_CONNECTION);
    line->setCornerStyle(CimdrawConnectLine::Sharp);
    line->setStartItem(startShape);
    line->setStartConnectPort(startPort);
    startShape->addConnect(line);
    line->attachEndAndRecompute(endShape, endPort);
    line->recomputePathFromCurrentEndpoints(false);
    endShape->addConnect(line);
}

} // namespace

void TestCimdrawTopology::scene_defaults_to_power()
{
    CimdrawScene scene;
    QCOMPARE(scene.getSceneType(), FILE_TYPE::CIMDRAW_DRAW);
}

void TestCimdrawTopology::power_items_expose_expected_roles()
{
    CimdrawPowerBusbarSectionItem busbar(QRectF(0, 0, 140, 18));
    CimdrawPowerCircuitBreakerItem breaker(QRectF(0, 0, 64, 92));
    CimdrawPowerGeneratorItem generator(QRectF(0, 0, 88, 88));
    CimdrawPowerLoadItem load(QRectF(0, 0, 88, 88));
    CimdrawPowerMotorItem motor(QRectF(0, 0, 88, 88));
    CimdrawPowerGroundItem ground(QRectF(0, 0, 64, 72));
    CimdrawPowerReactorItem reactor(QRectF(0, 0, 96, 40));
    CimdrawPowerCapacitorItem capacitor(QRectF(0, 0, 44, 48));
    CimdrawPowerCurrentTransformerItem currentTransformer(QRectF(0, 0, 72, 48));
    CimdrawPowerVoltageTransformerItem voltageTransformer(QRectF(0, 0, 78, 52));
    CimdrawPowerFuseItem fuse(QRectF(0, 0, 84, 32));

    QCOMPARE(busbar.topologyDomain(), CimdrawTopologyDomain::PowerSystem);
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
    QCOMPARE(capacitor.topologyDomain(), CimdrawTopologyDomain::PowerSystem);
    QVERIFY(!capacitor.topologyGraphNodeEnabled());
    QVERIFY(!reactor.topologyGraphNodeEnabled());
}

void TestCimdrawTopology::power_tools_are_registered()
{
    auto* toolMgr = CimdrawToolManager::getInstance();

    CimdrawPowerBusbarSectionItem busbar(QRectF(0, 0, 140, 18));
    CimdrawPowerCircuitBreakerItem breaker(QRectF(0, 0, 64, 92));
    CimdrawPowerGeneratorItem generator(QRectF(0, 0, 88, 88));
    CimdrawPowerMotorItem motor(QRectF(0, 0, 88, 88));
    CimdrawPowerGroundItem ground(QRectF(0, 0, 64, 72));
    CimdrawPowerReactorItem reactor(QRectF(0, 0, 96, 40));
    CimdrawPowerCapacitorItem capacitor(QRectF(0, 0, 44, 48));
    CimdrawPowerCurrentTransformerItem currentTransformer(QRectF(0, 0, 72, 48));
    CimdrawPowerVoltageTransformerItem voltageTransformer(QRectF(0, 0, 78, 52));
    CimdrawPowerFuseItem fuse(QRectF(0, 0, 84, 32));

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

void TestCimdrawTopology::topology_query_reports_missing_domain()
{
    CimdrawScene scene;

    const QVariantMap result = scene.queryTopology({
        {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), false);
    QCOMPARE(result.value(QStringLiteral("errorCode")).toString(), QStringLiteral("missing_domain"));
}

void TestCimdrawTopology::topology_binding_snapshot_defaults_to_empty_power_payload()
{
    CimdrawScene scene;

    const QVariantMap result = scene.buildTopologyBindingSnapshot();
    const QVariantMap power = result.value(QStringLiteral("power")).toMap();

    QCOMPARE(power.value(QStringLiteral("domain")).toString(), QStringLiteral("PowerSystem"));
    QCOMPARE(power.value(QStringLiteral("deviceCount")).toInt(), 0);
    QCOMPARE(power.value(QStringLiteral("conductorCount")).toInt(), 0);
    QCOMPARE(power.value(QStringLiteral("nodeCount")).toInt(), 0);
    QVERIFY(power.value(QStringLiteral("devices")).toList().isEmpty());
    QVERIFY(power.value(QStringLiteral("conductors")).toList().isEmpty());
    QVERIFY(power.value(QStringLiteral("nodes")).toList().isEmpty());
    const QVariantMap projection = power.value(QStringLiteral("projection")).toMap();
    QCOMPARE(projection.value(QStringLiteral("sourceKind")).toString(), QStringLiteral("runtimeScene"));
}

void TestCimdrawTopology::topology_query_reports_runtime_projection_info()
{
    CimdrawScene scene;

    const QVariantMap result = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("projectionInfo")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap data = result.value(QStringLiteral("data")).toMap();
    QCOMPARE(data.value(QStringLiteral("sourceKind")).toString(), QStringLiteral("runtimeScene"));
    QCOMPARE(data.value(QStringLiteral("sourceLabel")).toString(), QStringLiteral("运行期场景拓扑快照"));
    QVERIFY(data.value(QStringLiteral("truthBoundary")).toString().contains(QStringLiteral("运行期连接关系")));
}

void TestCimdrawTopology::runtime_power_queries_use_unified_topology_projection()
{
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    auto* ground = new CimdrawPowerGroundItem(QRectF(0, 0, 40, 96));

    scene.addItem(grid);
    scene.addItem(breaker);
    scene.addItem(load);
    scene.addItem(ground);

    grid->setPos(0, 0);
    breaker->setPos(220, 0);
    load->setPos(420, 80);
    ground->setPos(420, 200);

    grid->setTopologyNodeStableId(QStringLiteral("grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("breaker-001"));
    load->setTopologyNodeStableId(QStringLiteral("load-001"));
    ground->setTopologyNodeStableId(QStringLiteral("ground-001"));
    grid->setFlowSign(-1);
    breaker->setFlowSign(-1);
    breaker->setSwitchPosition(1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    CimdrawConnectPoint* breakerBottom = findConnectPort(breaker, BOTTOM_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    CimdrawConnectPoint* groundLeft = findConnectPort(ground, LEFT_DIRECTION);

    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    QVERIFY(breakerBottom != nullptr);
    QVERIFY(loadLeft != nullptr);
    QVERIFY(groundLeft != nullptr);

    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);
    connectPowerItems(&scene, breaker, breakerBottom, load, loadLeft);
    connectPowerItems(&scene, breaker, breakerBottom, ground, groundLeft);

    const QStringList reachable = scene.reachablePowerDevices(QStringLiteral("breaker-001"));
    QVERIFY(reachable.contains(QStringLiteral("grid-001")));
    QVERIFY(reachable.contains(QStringLiteral("breaker-001")));
    QVERIFY(reachable.contains(QStringLiteral("load-001")));
    QVERIFY(reachable.contains(QStringLiteral("ground-001")));

    const PowerTopologyPath supplyPath =
        scene.shortestPowerSupplyPath(QStringLiteral("grid-001"), QStringLiteral("load-001"));
    QVERIFY(supplyPath.found);
    QCOMPARE(supplyPath.deviceIds,
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("load-001")}));

    const PowerTopologyGroundPath groundPath =
        scene.shortestPowerGroundPath(QStringLiteral("grid-001"));
    QVERIFY(groundPath.found);
    QCOMPARE(groundPath.deviceIds,
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("ground-001")}));

    const PowerTopologyIslandAnalysis sceneIslands = scene.analyzePowerIslands();
    QVERIFY(!sceneIslands.islands.isEmpty());
    QVERIFY(sceneIslands.islands.first().deviceIds.contains(QStringLiteral("breaker-001")));
    const PowerTopologyAnalysisSnapshot sceneSnapshot = scene.buildPowerTopologySnapshot();
    QCOMPARE(sceneSnapshot.devices.size(), 4);
    QCOMPARE(sceneSnapshot.conductors.size(), 3);
    QVERIFY(sceneSnapshot.deviceToNodeIds.contains(QStringLiteral("breaker-001")));
    const int gridIndex = sceneSnapshot.deviceIndex.value(QStringLiteral("grid-001"), -1);
    QVERIFY(gridIndex >= 0);
    QCOMPARE(sceneSnapshot.devices.at(gridIndex).flowSign, grid->effectiveFlowSign());
    QCOMPARE(sceneSnapshot.devices.at(gridIndex).flowSign, -1);
    QVERIFY(powerTopologyDeviceSupportsDirectedTerminalFlow(sceneSnapshot.devices.at(gridIndex)));
    QCOMPARE(powerTopologyDeviceForwardFromTerminalId(sceneSnapshot.devices.at(gridIndex)),
             sceneSnapshot.devices.at(gridIndex).terminalIds.last());
    QCOMPARE(powerTopologyDeviceForwardToTerminalId(sceneSnapshot.devices.at(gridIndex)),
             sceneSnapshot.devices.at(gridIndex).terminalIds.first());
    const int breakerIndex = sceneSnapshot.deviceIndex.value(QStringLiteral("breaker-001"), -1);
    QVERIFY(breakerIndex >= 0);
    QCOMPARE(sceneSnapshot.devices.at(breakerIndex).flowSign, breaker->effectiveFlowSign());
    QCOMPARE(sceneSnapshot.devices.at(breakerIndex).flowSign, -1);
    const PowerTopologyDocumentExport sceneDocument = scene.buildPowerTopologyDocumentExport();
    QCOMPARE(sceneDocument.devices.size(), sceneSnapshot.devices.size());
    QCOMPARE(sceneDocument.conductors.size(), sceneSnapshot.conductors.size());
    QCOMPARE(sceneDocument.nodes.size(), sceneSnapshot.nodes.size());
    QCOMPARE(sceneDocument.viewScale, 1.0);

    CimdrawScenePowerTopologyAnalysisController analysisController;
    const QVector<int> analysisNodeIds =
        analysisController.powerDeviceNodeIds(&scene, QStringLiteral("breaker-001"));
    QVERIFY(!analysisNodeIds.isEmpty());
    const QStringList analysisReachable =
        analysisController.reachablePowerDevices(&scene, QStringLiteral("breaker-001"));
    QVERIFY(analysisReachable.contains(QStringLiteral("grid-001")));
    QVERIFY(analysisReachable.contains(QStringLiteral("load-001")));
    QVERIFY(analysisReachable.contains(QStringLiteral("ground-001")));
    const PowerTopologyIslandAnalysis analysisIslands = analysisController.analyzePowerIslands(&scene);
    QVERIFY(!analysisIslands.islands.isEmpty());
    const PowerTopologyPath analysisSupplyPath =
        analysisController.shortestPowerSupplyPath(&scene, QStringLiteral("grid-001"), QStringLiteral("load-001"));
    QVERIFY(analysisSupplyPath.found);
    QCOMPARE(analysisSupplyPath.deviceIds,
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("load-001")}));

    const QVariantMap reachableQuery = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("reachableDevices")},
        {QStringLiteral("deviceId"), QStringLiteral("breaker-001")}
    });
    QCOMPARE(reachableQuery.value(QStringLiteral("ok")).toBool(), true);
    const QStringList reachableQueryDevices = reachableQuery.value(QStringLiteral("data")).toStringList();
    QVERIFY(reachableQueryDevices.contains(QStringLiteral("grid-001")));
    QVERIFY(reachableQueryDevices.contains(QStringLiteral("load-001")));
    QVERIFY(reachableQueryDevices.contains(QStringLiteral("ground-001")));

    const QVariantMap supplyQuery = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("shortestSupplyPath")},
        {QStringLiteral("sourceDeviceId"), QStringLiteral("grid-001")},
        {QStringLiteral("targetDeviceId"), QStringLiteral("load-001")}
    });
    QCOMPARE(supplyQuery.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap supplyQueryData = supplyQuery.value(QStringLiteral("data")).toMap();
    QCOMPARE(supplyQueryData.value(QStringLiteral("found")).toBool(), true);
    QCOMPARE(supplyQueryData.value(QStringLiteral("deviceIds")).toStringList(),
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("load-001")}));

    const QVariantMap groundQuery = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("shortestGroundPath")},
        {QStringLiteral("sourceDeviceId"), QStringLiteral("grid-001")}
    });
    QCOMPARE(groundQuery.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap groundQueryData = groundQuery.value(QStringLiteral("data")).toMap();
    QCOMPARE(groundQueryData.value(QStringLiteral("found")).toBool(), true);
    QCOMPARE(groundQueryData.value(QStringLiteral("deviceIds")).toStringList(),
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("ground-001")}));

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController scratchStateController;
    const PowerTopologyAnalysisSnapshot workbenchSnapshot =
        workbenchController.buildPowerTopologySnapshot(&scene, scratchStateController);
    QCOMPARE(workbenchSnapshot.devices.size(), sceneSnapshot.devices.size());
    QCOMPARE(workbenchSnapshot.conductors.size(), sceneSnapshot.conductors.size());
    QCOMPARE(workbenchSnapshot.nodes.size(), sceneSnapshot.nodes.size());
    const PowerTopologyDocumentExport workbenchDocument =
        workbenchController.buildPowerTopologyDocumentExport(&scene, scratchStateController);
    QCOMPARE(workbenchDocument.devices.size(), sceneSnapshot.devices.size());
    QCOMPARE(workbenchDocument.conductors.size(), sceneSnapshot.conductors.size());
    QCOMPARE(workbenchDocument.nodes.size(), sceneSnapshot.nodes.size());
    QCOMPARE(workbenchDocument.viewScale, 1.0);
    const QVector<int> workbenchNodeIds =
        workbenchController.powerDeviceNodeIds(&scene, QStringLiteral("breaker-001"));
    QVERIFY(!workbenchNodeIds.isEmpty());
    const QStringList workbenchConnected =
        workbenchController.connectedPowerDevices(&scene, QStringLiteral("breaker-001"));
    QVERIFY(workbenchConnected.contains(QStringLiteral("grid-001")));
    QVERIFY(workbenchConnected.contains(QStringLiteral("load-001")));
    QVERIFY(workbenchConnected.contains(QStringLiteral("ground-001")));

    CimdrawSceneTopologySlotWorkbenchController slotController;
    const QStringList slotConnected = slotController.dispatchDeviceQuery(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
        QStringLiteral("breaker-001"));
    QVERIFY(slotConnected.contains(QStringLiteral("grid-001")));
    QVERIFY(slotConnected.contains(QStringLiteral("load-001")));
    QVERIFY(slotConnected.contains(QStringLiteral("ground-001")));

    const PowerTopologyIslandAnalysis workbenchIslands = workbenchController.analyzePowerIslands(&scene);
    QVERIFY(!workbenchIslands.islands.isEmpty());
    QVERIFY(workbenchIslands.islands.first().deviceIds.contains(QStringLiteral("breaker-001")));
    const PowerTopologyIslandAnalysis slotIslands = slotController.dispatchIslandAnalysis(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands);
    QVERIFY(!slotIslands.islands.isEmpty());

    const PowerTopologyPath workbenchSupplyPath =
        workbenchController.shortestPowerSupplyPath(&scene, QStringLiteral("grid-001"), QStringLiteral("load-001"));
    QVERIFY(workbenchSupplyPath.found);
    QCOMPARE(workbenchSupplyPath.deviceIds,
             QStringList({QStringLiteral("grid-001"),
                          QStringLiteral("breaker-001"),
                          QStringLiteral("load-001")}));
    const PowerTopologyPath slotSupplyPath = slotController.dispatchPath(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
        QStringLiteral("grid-001"),
        QStringLiteral("load-001"));
    QVERIFY(slotSupplyPath.found);
}

void TestCimdrawTopology::topology_query_reports_relation_node_info()
{
    CimdrawScene scene;
    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));

    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setPos(0, 0);
    breaker->setPos(220, 0);

    grid->setTopologyNodeStableId(QStringLiteral("grid-rel-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("breaker-rel-001"));

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);

    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);

    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }
    QVERIFY(relationLine != nullptr);
    const TopologyEdgeMeta relationMeta = relationLine->resolvedRelationEdgeMeta();
    const TopologyEdgeLookupHint relationLookupHint = relationLine->resolvedRelationEdgeLookupHint();
    const QString relationEdgeId = relationMeta.edgeStableId;
    QVERIFY(!relationEdgeId.isEmpty());

    const TopologyNodeRelationInfo relationInfo =
        workbenchController.relationNodeInfo(&scene, stateController, QStringLiteral("grid-rel-001"));
    QCOMPARE(relationInfo.nodeStableId, QStringLiteral("grid-rel-001"));
    QCOMPARE(relationInfo.incidentEdgeCount(), 1);
    QCOMPARE(relationInfo.adjacentNodeCount(), 1);
    QVERIFY(relationInfo.incidentEdgeIds.contains(relationEdgeId));
    QVERIFY(relationInfo.adjacentNodeIds.contains(QStringLiteral("breaker-rel-001")));
    const TopologyEdgeMeta edgeMeta =
        workbenchController.relationEdgeMeta(&scene, stateController, relationEdgeId);
    QCOMPARE(edgeMeta.edgeStableId, relationEdgeId);
    QCOMPARE(edgeMeta.startNodeStableId, QStringLiteral("grid-rel-001"));
    QCOMPARE(edgeMeta.endNodeStableId, QStringLiteral("breaker-rel-001"));
    TopologyEdgeLookupHint edgeLookupHint;
    edgeLookupHint.startNodeStableId = relationMeta.startNodeStableId;
    edgeLookupHint.endNodeStableId = relationMeta.endNodeStableId;
    edgeLookupHint.startPortKey = relationMeta.startPortKey;
    edgeLookupHint.endPortKey = relationMeta.endPortKey;
    edgeLookupHint.relationType = relationMeta.relationType;
    edgeLookupHint.relationTypeSpecified = true;
    const TopologyEdgeMeta resolvedEdgeMeta =
        workbenchController.resolveRelationEdgeMeta(&scene, stateController, edgeLookupHint);
    QCOMPARE(resolvedEdgeMeta.edgeStableId, relationEdgeId);
    QCOMPARE(resolvedEdgeMeta.startNodeStableId, QStringLiteral("grid-rel-001"));
    QCOMPARE(resolvedEdgeMeta.endNodeStableId, QStringLiteral("breaker-rel-001"));

    const QVariantMap result = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("relation")},
        {QStringLiteral("op"), QStringLiteral("nodeInfo")},
        {QStringLiteral("nodeId"), QStringLiteral("grid-rel-001")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap data = result.value(QStringLiteral("data")).toMap();
    QCOMPARE(data.value(QStringLiteral("nodeId")).toString(), QStringLiteral("grid-rel-001"));
    QCOMPARE(data.value(QStringLiteral("incidentEdgeCount")).toInt(), 1);
    QCOMPARE(data.value(QStringLiteral("adjacentNodeCount")).toInt(), 1);
    QVERIFY(data.value(QStringLiteral("incidentEdgeIds")).toStringList().contains(relationEdgeId));
    QVERIFY(data.value(QStringLiteral("adjacentNodeIds")).toStringList().contains(QStringLiteral("breaker-rel-001")));

    const QVariantMap edgeResult = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("relation")},
        {QStringLiteral("op"), QStringLiteral("edgeInfo")},
        {QStringLiteral("edgeId"), relationEdgeId}
    });
    QCOMPARE(edgeResult.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap edgeData = edgeResult.value(QStringLiteral("data")).toMap();
    QCOMPARE(edgeData.value(QStringLiteral("edgeId")).toString(), relationEdgeId);
    QCOMPARE(edgeData.value(QStringLiteral("startNodeId")).toString(), QStringLiteral("grid-rel-001"));
    QCOMPARE(edgeData.value(QStringLiteral("endNodeId")).toString(), QStringLiteral("breaker-rel-001"));

    const QVariantMap resolvedEdgeResult = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("relation")},
        {QStringLiteral("op"), QStringLiteral("resolveEdgeInfo")},
        {QStringLiteral("startNodeId"), relationLookupHint.startNodeStableId},
        {QStringLiteral("endNodeId"), relationLookupHint.endNodeStableId},
        {QStringLiteral("startPortKey"), relationLookupHint.startPortKey},
        {QStringLiteral("endPortKey"), relationLookupHint.endPortKey},
        {QStringLiteral("relationType"), static_cast<int>(relationLookupHint.relationType)}
    });
    QCOMPARE(resolvedEdgeResult.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap resolvedEdgeData = resolvedEdgeResult.value(QStringLiteral("data")).toMap();
    QCOMPARE(resolvedEdgeData.value(QStringLiteral("edgeId")).toString(), relationEdgeId);
    QCOMPARE(resolvedEdgeData.value(QStringLiteral("startNodeId")).toString(), QStringLiteral("grid-rel-001"));
    QCOMPARE(resolvedEdgeData.value(QStringLiteral("endNodeId")).toString(), QStringLiteral("breaker-rel-001"));

    const QVariantMap bindingResult = scene.queryTopology({
        {QStringLiteral("domain"), QStringLiteral("relation")},
        {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}
    });
    QCOMPARE(bindingResult.value(QStringLiteral("ok")).toBool(), true);
    const QVariantMap bindingData = bindingResult.value(QStringLiteral("data")).toMap();
    QVERIFY(bindingData.value(QStringLiteral("edgeCount")).toInt() >= 1);
    bool foundRelationEdge = false;
    const QVariantList bindingEdges = bindingData.value(QStringLiteral("edges")).toList();
    for (const QVariant& entry : bindingEdges)
    {
        const QVariantMap edge = entry.toMap();
        if (edge.value(QStringLiteral("edgeId")).toString() != relationEdgeId)
            continue;
        QCOMPARE(edge.value(QStringLiteral("startNodeId")).toString(), QStringLiteral("grid-rel-001"));
        QCOMPARE(edge.value(QStringLiteral("endNodeId")).toString(), QStringLiteral("breaker-rel-001"));
        foundRelationEdge = true;
        break;
    }
    QVERIFY(foundRelationEdge);
}

void TestCimdrawTopology::topology_query_controller_reports_missing_scene()
{
    CimdrawSceneTopologyQueryController controller;

    const QVariantMap result = controller.queryTopology(nullptr, {
        {QStringLiteral("domain"), QStringLiteral("power")},
        {QStringLiteral("op"), QStringLiteral("bindingSnapshot")}
    });

    QCOMPARE(result.value(QStringLiteral("ok")).toBool(), false);
    QCOMPARE(result.value(QStringLiteral("errorCode")).toString(), QStringLiteral("missing_scene"));
}

void TestCimdrawTopology::selection_manager_add_replace_remove_items()
{
    CimdrawScene scene;
    CimdrawSceneSelectionManager manager;

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

void TestCimdrawTopology::scene_get_selection_tracks_first_selected_item()
{
    CimdrawScene scene;

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));

    scene.setSelections({item1, item2});
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(item1));

    scene.setSelections({});
    QCOMPARE(scene.getSelection(), nullptr);
}

void TestCimdrawTopology::edit_controller_rotates_selected_items()
{
    CimdrawSceneEditController controller;
    CimdrawScene scene;
    CimdrawView view;
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

void TestCimdrawTopology::scene_item_rotate_routes_through_edit_controller()
{
    CimdrawScene scene;
    CimdrawView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    scene.itemRotate(45);
    QCOMPARE(item->rotation(), 45.0);
    QCOMPARE(view.getStack()->count(), 1);
}

void TestCimdrawTopology::deferred_update_controller_tracks_path_recompute_state()
{
    CimdrawSceneDeferredUpdateController controller;
    auto* line = new CimdrawConnectLine();
    const QList<CimdrawConnectLine*> singleLine{line};

    QVERIFY(!controller.isConnectLinePathUpdatePending());

    controller.cancelConnectLinePathRecompute(line);
    QVERIFY(!controller.isConnectLinePathUpdatePending());

    controller.scheduleConnectLinesPathRecompute(nullptr, singleLine);
    QVERIFY(!controller.isConnectLinePathUpdatePending());

    CimdrawScene scene;
    scene.addItem(line);
    controller.scheduleConnectLinesPathRecompute(&scene, singleLine);
    QVERIFY(controller.isConnectLinePathUpdatePending());

    controller.cancelConnectLinePathRecompute(line);
    QVERIFY(!controller.isConnectLinePathUpdatePending());
}

void TestCimdrawTopology::connect_line_exposes_resolved_relation_edge_meta()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("binding-start"));
    end->setTopologyNodeStableId(QStringLiteral("binding-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine line;
    QVERIFY(!line.hasTopologyBindings());

    line.setStartItem(start);
    line.setEndItem(end);
    line.setStartConnectPort(startPort);
    line.setEndConnectPort(endPort);

    const TopologyEdgeMeta binding = line.resolvedRelationEdgeMeta();
    QVERIFY(line.hasTopologyBindings());
    QCOMPARE(binding.startNodeStableId, QStringLiteral("binding-start"));
    QCOMPARE(binding.endNodeStableId, QStringLiteral("binding-end"));
    QVERIFY(!binding.startPortKey.isEmpty());
    QVERIFY(!binding.endPortKey.isEmpty());
    QVERIFY(!binding.edgeStableId.isEmpty());
    QVERIFY(line.isTopologyRelationLink());
}

void TestCimdrawTopology::connect_line_exposes_relation_edge_lookup_hint()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("lookup-start"));
    end->setTopologyNodeStableId(QStringLiteral("lookup-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine line;
    line.setStartItem(start);
    line.setEndItem(end);
    line.setStartConnectPort(startPort);
    line.setEndConnectPort(endPort);

    const QString resolvedEdgeId = line.resolvedTopologyEdgeStableId();
    QVERIFY(!resolvedEdgeId.isEmpty());
    const TopologyEdgeMeta relationMeta = line.resolvedRelationEdgeMeta();
    QCOMPARE(relationMeta.edgeStableId, resolvedEdgeId);
    QCOMPARE(relationMeta.startNodeStableId, QStringLiteral("lookup-start"));
    QCOMPARE(relationMeta.endNodeStableId, QStringLiteral("lookup-end"));
    const TopologyEdgeLookupHint lookupHint = line.resolvedRelationEdgeLookupHint();
    QVERIFY(lookupHint.hasLookupFields());
    QVERIFY(lookupHint.relationTypeSpecified);
    QCOMPARE(lookupHint.startNodeStableId, QStringLiteral("lookup-start"));
    QCOMPARE(lookupHint.endNodeStableId, QStringLiteral("lookup-end"));
    QCOMPARE(lookupHint.edgeStableId, resolvedEdgeId);
    QVERIFY(!lookupHint.startPortKey.isEmpty());
    QVERIFY(!lookupHint.endPortKey.isEmpty());
}

void TestCimdrawTopology::connect_line_loads_persisted_topology_binding_snapshot()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("apply-start"));
    end->setTopologyNodeStableId(QStringLiteral("apply-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine source;
    source.setStartItem(start);
    source.setStartConnectPort(startPort);
    source.attachEndAndRecompute(end, endPort);

    TopologyEdgeMeta expectedMeta = source.resolvedRelationEdgeMeta();
    expectedMeta.edgeStableId = QStringLiteral("edge-apply-001");
    expectedMeta.relationType = TopologyRelationType::Control;
    source.applyRelationEdgeMeta(expectedMeta);

    QDomDocument document(QStringLiteral("CimdrawLineLoadXml"));
    QDomElement object = document.createElement(QStringLiteral("Object"));
    document.appendChild(object);
    QVERIFY(source.saveXml(&object));

    CimdrawConnectLine line;
    QVERIFY(line.loadXml(&object));

    const TopologyEdgeMeta applied = line.persistedRelationEdgeMeta();
    QVERIFY(line.hasTopologyBindings());
    QCOMPARE(applied.edgeStableId, expectedMeta.edgeStableId);
    QCOMPARE(applied.startNodeStableId, expectedMeta.startNodeStableId);
    QCOMPARE(applied.endNodeStableId, expectedMeta.endNodeStableId);
    QCOMPARE(applied.startPortKey, expectedMeta.startPortKey);
    QCOMPARE(applied.endPortKey, expectedMeta.endPortKey);
    QCOMPARE(applied.relationType, expectedMeta.relationType);
    QVERIFY(line.isTopologyRelationLink());
}

void TestCimdrawTopology::connect_line_restores_persisted_endpoint_items_from_binding_index()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("restore-start"));
    end->setTopologyNodeStableId(QStringLiteral("restore-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine source;
    source.setStartItem(start);
    source.setStartConnectPort(startPort);
    source.attachEndAndRecompute(end, endPort);

    QDomDocument document(QStringLiteral("CimdrawRestoreEndpointItems"));
    QDomElement object = document.createElement(QStringLiteral("Object"));
    document.appendChild(object);
    QVERIFY(source.saveXml(&object));

    CimdrawConnectLine line;
    QVERIFY(line.loadXml(&object));

    QHash<QString, QGraphicsItem*> topologyBindingIndex;
    topologyBindingIndex.insert(QStringLiteral("restore-start"), start);
    topologyBindingIndex.insert(QStringLiteral("restore-end"), end);

    QVERIFY(line.restorePersistedEndpointItems(topologyBindingIndex));
    QCOMPARE(line.getStartItem(), static_cast<QGraphicsItem*>(start));
    QCOMPARE(line.getEndItem(), static_cast<QGraphicsItem*>(end));
    QCOMPARE(line.persistedEndpointBindingId(true), QStringLiteral("restore-start"));
    QCOMPARE(line.persistedEndpointBindingId(false), QStringLiteral("restore-end"));
}

void TestCimdrawTopology::connect_line_applies_relation_edge_meta()
{
    CimdrawConnectLine line;

    TopologyEdgeMeta edgeMeta;
    edgeMeta.edgeStableId = QStringLiteral("edge-meta-001");
    edgeMeta.startNodeStableId = QStringLiteral("meta-start");
    edgeMeta.endNodeStableId = QStringLiteral("meta-end");
    edgeMeta.startPortKey = QStringLiteral("4");
    edgeMeta.endPortKey = QStringLiteral("7");
    edgeMeta.relationType = TopologyRelationType::Flow;

    line.applyRelationEdgeMeta(edgeMeta);

    const TopologyEdgeMeta applied = line.persistedRelationEdgeMeta();
    QCOMPARE(applied.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(applied.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(applied.endNodeStableId, edgeMeta.endNodeStableId);
    QCOMPARE(applied.startPortKey, edgeMeta.startPortKey);
    QCOMPARE(applied.endPortKey, edgeMeta.endPortKey);
    QCOMPARE(applied.relationType, edgeMeta.relationType);
    QVERIFY(line.isTopologyRelationLink());
}

void TestCimdrawTopology::connect_line_duplicate_detaches_topology_identity()
{
    CimdrawConnectLine line;

    TopologyEdgeMeta binding;
    binding.edgeStableId = QStringLiteral("edge-dup-001");
    binding.startNodeStableId = QStringLiteral("dup-start");
    binding.endNodeStableId = QStringLiteral("dup-end");
    binding.startPortKey = QStringLiteral("2");
    binding.endPortKey = QStringLiteral("5");
    binding.relationType = TopologyRelationType::Dependency;
    line.applyRelationEdgeMeta(binding);

    std::unique_ptr<QGraphicsItem> duplicateHolder(line.duplicate());
    auto* duplicateLine = qgraphicsitem_cast<CimdrawConnectLine*>(duplicateHolder.get());
    QVERIFY(duplicateLine != nullptr);

    const TopologyEdgeMeta duplicated = duplicateLine->persistedRelationEdgeMeta();
    QVERIFY(duplicated.edgeStableId.isEmpty());
    QCOMPARE(duplicated.startNodeStableId, binding.startNodeStableId);
    QCOMPARE(duplicated.endNodeStableId, binding.endNodeStableId);
    QCOMPARE(duplicated.startPortKey, binding.startPortKey);
    QCOMPARE(duplicated.endPortKey, binding.endPortKey);
    QCOMPARE(duplicated.relationType, binding.relationType);
    QVERIFY(!duplicateLine->isTopologyRelationLink());
}

void TestCimdrawTopology::connect_line_port_setters_refresh_topology_binding()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("setter-start"));
    end->setTopologyNodeStableId(QStringLiteral("setter-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine line;
    line.setStartItem(start);
    line.setEndItem(end);
    line.setStartConnectPort(startPort);
    line.setEndConnectPort(endPort);

    const TopologyEdgeMeta binding = line.persistedRelationEdgeMeta();
    QCOMPARE(binding.startNodeStableId, QStringLiteral("setter-start"));
    QCOMPARE(binding.endNodeStableId, QStringLiteral("setter-end"));
    QVERIFY(!binding.startPortKey.isEmpty());
    QVERIFY(!binding.endPortKey.isEmpty());
    QVERIFY(line.isTopologyRelationLink());
}

void TestCimdrawTopology::connect_line_xml_roundtrip_preserves_topology_binding_snapshot()
{
    CimdrawScene scene;

    auto* start = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* end = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(start);
    scene.addItem(end);
    start->setTopologyNodeStableId(QStringLiteral("xml-start"));
    end->setTopologyNodeStableId(QStringLiteral("xml-end"));

    CimdrawConnectPoint* startPort = findConnectPort(start, RIGHT_DIRECTION);
    CimdrawConnectPoint* endPort = findConnectPort(end, LEFT_DIRECTION);
    QVERIFY(startPort != nullptr);
    QVERIFY(endPort != nullptr);

    CimdrawConnectLine line;
    line.setStartItem(start);
    line.setStartConnectPort(startPort);
    line.attachEndAndRecompute(end, endPort);

    const TopologyEdgeMeta resolvedMeta = line.resolvedRelationEdgeMeta();
    TopologyEdgeMeta binding;
    binding.edgeStableId = QStringLiteral("xml-edge-001");
    binding.startNodeStableId = resolvedMeta.startNodeStableId;
    binding.endNodeStableId = resolvedMeta.endNodeStableId;
    binding.startPortKey = resolvedMeta.startPortKey;
    binding.endPortKey = resolvedMeta.endPortKey;
    binding.relationType = TopologyRelationType::Reference;
    line.applyRelationEdgeMeta(binding);

    QDomDocument document(QStringLiteral("CimdrawLineXmlRoundtrip"));
    QDomElement object = document.createElement(QStringLiteral("Object"));
    document.appendChild(object);
    QVERIFY(line.saveXml(&object));

    CimdrawConnectLine restored;
    QVERIFY(restored.loadXml(&object));

    const TopologyEdgeMeta restoredBinding = restored.persistedRelationEdgeMeta();
    QCOMPARE(restoredBinding.edgeStableId, binding.edgeStableId);
    QCOMPARE(restoredBinding.startNodeStableId, binding.startNodeStableId);
    QCOMPARE(restoredBinding.endNodeStableId, binding.endNodeStableId);
    QCOMPARE(restoredBinding.startPortKey, binding.startPortKey);
    QCOMPARE(restoredBinding.endPortKey, binding.endPortKey);
    QCOMPARE(restoredBinding.relationType, binding.relationType);
    QVERIFY(restored.isTopologyRelationLink());
}

void TestCimdrawTopology::deferred_update_controller_flushes_property_panel_on_demand()
{
    CimdrawSceneDeferredUpdateController controller;
    CimdrawScene scene;
    QSignalSpy spy(&scene, &CimdrawScene::itemPropertyChanged);

    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 0);

    controller.requestDeferredItemPropertyPanel();
    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 1);

    controller.flushDeferredItemPropertyPanel(&scene);
    QCOMPARE(spy.count(), 1);
}

void TestCimdrawTopology::interaction_controller_rejects_unknown_menu_and_drop_inputs()
{
    CimdrawSceneInteractionController controller;
    CimdrawScene scene;
    QAction action(QStringLiteral("未知动作"));

    QVERIFY(!controller.handleMenuAction(nullptr, &action));
    QVERIFY(!controller.handleMenuAction(&scene, nullptr));
    QVERIFY(!controller.handleMenuAction(&scene, &action));

    QVERIFY(!controller.handleTextDrop(nullptr, QStringLiteral("rect"), QPointF(0, 0)));
    QVERIFY(!controller.handleTextDrop(&scene, QString(), QPointF(0, 0)));
    QVERIFY(!controller.handleTextDrop(&scene, QStringLiteral("not-registered"), QPointF(0, 0)));
}

void TestCimdrawTopology::interaction_controller_handles_text_drop_with_registered_tool()
{
    CimdrawSceneInteractionController controller;
    CimdrawScene scene;
    CimdrawView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    const bool ok = controller.handleTextDrop(&scene, QStringLiteral("rect"), QPointF(30, 40));
    QVERIFY(ok);
    QCOMPARE(scene.items().size(), 1);
    QCOMPARE(scene.getSelections().size(), 1);
    QVERIFY(view.isModified());
    QCOMPARE(view.getStack()->count(), 1);
}

void TestCimdrawTopology::interaction_controller_dispatches_mouse_events_to_tool()
{
    CimdrawSceneInteractionController controller;
    CimdrawScene scene;
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

void TestCimdrawTopology::interaction_controller_handles_drag_enter_move_and_drop()
{
    CimdrawSceneInteractionController controller;
    CimdrawScene scene;
    CimdrawView view;
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

void TestCimdrawTopology::scene_interaction_workbench_controller_routes_tool_menu_and_drag()
{
    CimdrawSceneInteractionWorkbenchController controller;
    CimdrawScene scene;
    RecordingTool tool;
    CimdrawToolManager::getInstance()->setCurrentTool(&tool);

    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    QGraphicsSceneMouseEvent doubleClickEvent(QEvent::GraphicsSceneMouseDoubleClick);
    QVERIFY(!controller.dispatchMouse(nullptr,
                                      CimdrawSceneInteractionWorkbenchController::MouseAction::Press,
                                      &pressEvent));
    QVERIFY(controller.dispatchMousePress(&scene, &pressEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     CimdrawSceneInteractionWorkbenchController::MouseAction::Move,
                                     &moveEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     CimdrawSceneInteractionWorkbenchController::MouseAction::Release,
                                     &releaseEvent));
    QVERIFY(controller.dispatchMouse(&scene,
                                     CimdrawSceneInteractionWorkbenchController::MouseAction::DoubleClick,
                                     &doubleClickEvent));
    QCOMPARE(tool.pressCount, 1);
    QCOMPARE(tool.moveCount, 1);
    QCOMPARE(tool.releaseCount, 1);
    QCOMPARE(tool.doubleClickCount, 1);
    QCOMPARE(tool.lastScene, &scene);
    QCOMPARE(tool.lastEvent, &doubleClickEvent);
    QCOMPARE(controller.currentTool(), static_cast<CimdrawTool*>(&tool));

    QAction action(QStringLiteral("鍒犻櫎"), nullptr);
    QVERIFY(!controller.dispatchMenuAction(nullptr, &action));
    QVERIFY(controller.dispatchMenuAction(&scene, &action));
    QVERIFY(controller.handleMenuAction(&scene, &action));
    QVERIFY(!controller.handleDragMove(nullptr));

    QMimeData textMime;
    textMime.setText(QStringLiteral("rect"));
    QVERIFY(!controller.dispatchDrag(nullptr,
                                     CimdrawSceneInteractionWorkbenchController::DragAction::Enter,
                                     &textMime));
    QVERIFY(controller.dispatchDrag(&scene,
                                    CimdrawSceneInteractionWorkbenchController::DragAction::Enter,
                                    &textMime));
    QVERIFY(controller.handleDragEnter(&scene, &textMime));
    QVERIFY(controller.dispatchDrag(&scene,
                                    CimdrawSceneInteractionWorkbenchController::DragAction::Move,
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
                                    CimdrawSceneInteractionWorkbenchController::DragAction::Drop,
                                    &textMime,
                                    QPointF(12, 16)));
    QVERIFY(scene.items().size() > itemCountBeforeDrop);

    delete menu;
    CimdrawToolManager::getInstance()->setCurrentTool(nullptr);
}

void TestCimdrawTopology::connector_layout_controller_expands_scope_and_nudges_overlapping_lines()
{
    CimdrawSceneConnectorLayoutController controller;
    CimdrawScene scene;

    auto* line1 = new CimdrawConnectLine();
    auto* line2 = new CimdrawConnectLine();
    const QList<CimdrawConnectLine*> seedLines{line1};
    scene.addItem(line1);
    scene.addItem(line2);

    line1->setPathFromScenePoints({QPointF(0, 0), QPointF(80, 0), QPointF(160, 0)});
    line2->setPathFromScenePoints({QPointF(0, 0), QPointF(80, 0), QPointF(160, 0)});

    const QList<CimdrawConnectLine*> scope = controller.expandNudgingScope(&scene, seedLines);
    QVERIFY(scope.contains(line1));
    QVERIFY(scope.contains(line2));

    const QVector<QPointF> before1 = line1->pathInSceneCoords();
    const QVector<QPointF> before2 = line2->pathInSceneCoords();

    QVERIFY(controller.applyConnectorNudging(&scene, seedLines));

    const QVector<QPointF> after1 = line1->pathInSceneCoords();
    const QVector<QPointF> after2 = line2->pathInSceneCoords();
    QVERIFY(after1 != before1 || after2 != before2);
}

void TestCimdrawTopology::connector_layout_controller_ignores_empty_or_single_line_scope()
{
    CimdrawSceneConnectorLayoutController controller;
    CimdrawScene scene;
    auto* line = new CimdrawConnectLine();
    const QList<CimdrawConnectLine*> singleLine{line};
    scene.addItem(line);
    line->setPathFromScenePoints({QPointF(0, 0), QPointF(20, 0), QPointF(40, 0)});

    QVERIFY(controller.expandNudgingScope(nullptr, {}).isEmpty());
    QVERIFY(!controller.applyConnectorNudging(nullptr, {}));
    QVERIFY(!controller.applyConnectorNudging(&scene, {}));
    QVERIFY(!controller.applyConnectorNudging(&scene, singleLine));
}

void TestCimdrawTopology::state_controller_updates_view_flags_and_wiring_data()
{
    CimdrawSceneStateController controller;
    CimdrawView* view = nullptr;
    bool paintState = false;
    bool interactive = false;
    QHash<QString, QVariant> wiringData;
    CimdrawView realView;

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

void TestCimdrawTopology::scene_state_workbench_controller_routes_scene_state_updates()
{
    CimdrawSceneStateWorkbenchController controller;
    CimdrawSceneDeferredUpdateController deferredUpdateController;
    CimdrawSceneTopologyStateController topologyStateController;
    CimdrawView* view = nullptr;
    bool paintState = false;
    bool interactive = false;
    QHash<QString, QVariant> wiringData;
    CimdrawView realView;
    CimdrawScene scene;

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

void TestCimdrawTopology::frame_workbench_state_controller_syncs_ui_state()
{
    FrameWorkbenchStateController controller;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(CimdrawWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(CimdrawWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    controller.syncWiringStandardCombo(&combo, CimdrawWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::GB));

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(12.5, -3.0));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=12.5"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=-3"));

    QTabWidget tabs;
    CimdrawView* view1 = new CimdrawView();
    CimdrawView* view2 = new CimdrawView();
    tabs.addTab(view1, QStringLiteral("v1"));
    tabs.addTab(view2, QStringLiteral("v2"));
    tabs.setCurrentWidget(view2);

    QCOMPARE(controller.currentViewFromTabs(&tabs), view2);
    QCOMPARE(controller.currentViewFromTabs(nullptr), nullptr);
}

void TestCimdrawTopology::frame_canvas_appearance_controller_applies_view_background()
{
    FrameCanvasAppearanceController controller;
    CimdrawView view;
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

void TestCimdrawTopology::frame_canvas_workbench_controller_routes_canvas_state_updates()
{
    FrameCanvasWorkbenchController controller;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(CimdrawWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(CimdrawWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    controller.syncWiringStandardUi(&combo, CimdrawWiringSymbolStandard::ANSI);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(5.5, 6.0));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=5.5"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=6"));

    QTabWidget tabs;
    controller.refreshAllWiringItemPaint(&tabs);
    controller.applyWiringStandardChange(&combo, nullptr, &tabs, CimdrawWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::GB));
}

void TestCimdrawTopology::frame_canvas_slot_workbench_controller_routes_center_widget_canvas_actions()
{
    FrameCanvasSlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;

    QComboBox combo;
    combo.addItem(QStringLiteral("IEC"), static_cast<int>(CimdrawWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(CimdrawWiringSymbolStandard::GB));
    combo.setCurrentIndex(0);

    QLineEdit xLineEdit;
    QLineEdit yLineEdit;
    controller.syncPointerPosition(&xLineEdit, &yLineEdit, QPointF(8.0, 9.5));
    QCOMPARE(xLineEdit.text(), QStringLiteral("x=8"));
    QCOMPARE(yLineEdit.text(), QStringLiteral("y=9.5"));

    controller.syncWiringStandardUi(&combo, CimdrawWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::GB));

    controller.refreshAllWiringItemPaint(nullptr);
    controller.applyWiringStandardChange(&combo, nullptr, nullptr, CimdrawWiringSymbolStandard::ANSI);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));
    QVERIFY(!controller.chooseAndApplyBackgroundColor(nullptr, nullptr, nullptr));
    QVERIFY(!controller.chooseAndApplyBackgroundImage(nullptr, nullptr));

    CimdrawScene* scene = new CimdrawScene();
    CimdrawView* view = new CimdrawView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    controller.refreshAllWiringItemPaint(&centerWidget);
    controller.applyWiringStandardChange(&combo, nullptr, &centerWidget, CimdrawWiringSymbolStandard::GB);
    QCOMPARE(combo.currentData().toInt(), static_cast<int>(CimdrawWiringSymbolStandard::GB));
}

void TestCimdrawTopology::frame_canvas_slot_workbench_controller_applies_selected_wiring_standard()
{
    FrameCanvasSlotWorkbenchController controller;
    QComboBox combo;

    combo.addItem(QStringLiteral("IEC"), static_cast<int>(CimdrawWiringSymbolStandard::IEC));
    combo.addItem(QStringLiteral("ANSI"), static_cast<int>(CimdrawWiringSymbolStandard::ANSI));
    combo.addItem(QStringLiteral("GB"), static_cast<int>(CimdrawWiringSymbolStandard::GB));
    combo.setCurrentIndex(1);

    const CimdrawWiringSymbolStandard original = CimdrawWiringSymbolStyle::instance().standard();
    QVERIFY(controller.applySelectedWiringStandard(&combo, QString()));
    QCOMPARE(CimdrawWiringSymbolStyle::instance().standard(), CimdrawWiringSymbolStandard::ANSI);

    CimdrawWiringSymbolStyle::instance().setStandard(original);
    QVERIFY(!controller.applySelectedWiringStandard(nullptr, QString()));

    QComboBox invalidCombo;
    QVERIFY(!controller.applySelectedWiringStandard(&invalidCombo, QString()));
}

void TestCimdrawTopology::frame_window_state_workbench_controller_syncs_tab_and_action_state()
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
    CimdrawView view;
    CimdrawScene scene;
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

void TestCimdrawTopology::frame_cim_workbench_controller_uses_default_import_path()
{
    FrameCimWorkbenchController controller;
    const QString defaultPath = controller.defaultImportPath();

    QVERIFY(defaultPath.contains(QStringLiteral("tests/data/cim/minimal")));
    QVERIFY(defaultPath.endsWith(QStringLiteral("EQ_SSH_sample")));
}

void TestCimdrawTopology::frame_cim_workbench_controller_activates_shape_by_mrid()
{
    FrameCimWorkbenchController controller;
    FrameCimWorkbenchController::ImportState state;
    CimdrawScene scene;
    CimdrawView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* shape = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    shape->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:breaker-activate"));
    state.shapeByMrid.insert(QStringLiteral("breaker-activate"), shape);
    state.idIndex.bindGraphicItem(QStringLiteral("breaker-activate"), QStringLiteral("gfx:breaker-activate"));

    QVERIFY(scene.activateGraphicObjectByMrid(QStringLiteral("breaker-activate"), state.graphicQueryState()));
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(shape));
    QVERIFY(!scene.activateGraphicObjectByMrid(QStringLiteral("missing-graphic"), state.graphicQueryState()));

    scene.cleanSelection();
    controller.activateObjectByMrid(QStringLiteral("breaker-activate"), &scene, state);
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(shape));
}

void TestCimdrawTopology::cim_query_facade_locates_scene_item_and_mrid()
{
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;
    CimModel model;
    CimObject object;
    object.mrid = QStringLiteral("query-breaker-001");
    object.className = QStringLiteral("Breaker");
    object.name = QStringLiteral("Query Breaker");
    QVERIFY(model.addObject(object));

    auto* shape = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    shape->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("query-breaker-001"));
    shape->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:query-breaker-001"));

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("query-breaker-001"), shape);
    idIndex.bindGraphicItem(QStringLiteral("query-breaker-001"),
                            QStringLiteral("gfx:query-breaker-001"));
    const CimGraphicQueryContext queryContext{&shapeByMrid, &idIndex};
    const CimGraphicQueryState queryState{queryContext, 3};
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);

    const QSet<QString> visualizedMrids = facade.visualizedMrids(queryContext);
    const QSet<QString> stateVisualizedMrids = facade.visualizedMrids(queryState);
    QVERIFY(visualizedMrids.contains(QStringLiteral("query-breaker-001")));
    QCOMPARE(stateVisualizedMrids, visualizedMrids);
    QCOMPARE(visualSummary.visualizedMrids, visualizedMrids);
    QCOMPARE(visualSummary.totalObjects, 1);
    QCOMPARE(visualSummary.mappableObjects, 1);
    QCOMPARE(visualSummary.visualizedObjects, 1);
    QCOMPARE(visualSummary.generatedHelperLineCount, 3);
    QCOMPARE(visualSummary.objectVisualStatesByMrid.value(QStringLiteral("query-breaker-001")),
             CimGraphicObjectVisualState::Visualized);
    QVERIFY(visualSummary.objectSummariesByMrid.contains(QStringLiteral("query-breaker-001")));
    QCOMPARE(visualSummary.objectSummariesByMrid.value(QStringLiteral("query-breaker-001")).visualState,
             CimGraphicObjectVisualState::Visualized);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("query-breaker-001")).visualized);
    const CimGraphicObjectSummary itemSummary =
        facade.graphicObjectSummaryForSceneItem(shape, queryState);
    QCOMPARE(itemSummary.mrid, QStringLiteral("query-breaker-001"));
    QCOMPARE(itemSummary.objectId, shape->cimdrawObjectId());
    QCOMPARE(itemSummary.className, QStringLiteral("Breaker"));
    QCOMPARE(itemSummary.graphicClassName, QStringLiteral("CimdrawPowerCircuitBreakerItem"));
    QCOMPARE(itemSummary.displayName, QStringLiteral("Breaker"));
    QCOMPARE(itemSummary.visualState, CimGraphicObjectVisualState::Visualized);
    QVERIFY(itemSummary.visualized);
    QVERIFY(itemSummary.participatesInTopology);
    QVERIFY(itemSummary.isTopologyGraphNode);
    QVERIFY(!itemSummary.supportsDirectedFlow);
    QCOMPARE(itemSummary.topologyDomain, CimdrawTopologyDomain::PowerSystem);
    QCOMPARE(itemSummary.powerTopologyRole, PowerTopologyRole::Breaker);
    QVERIFY(itemSummary.renderState.valid);
    QCOMPARE(itemSummary.renderState.runState, CimdrawWiringRunState::Normal);
    QCOMPARE(itemSummary.renderState.switchPosition, 1);
    QCOMPARE(itemSummary.renderState.flowSign, 1);
    const CimGraphicObjectSummary mridSummary =
        facade.graphicObjectSummaryForMrid(QStringLiteral("query-breaker-001"), &scene, queryState);
    const CimGraphicObjectSummary sceneMridSummary =
        scene.graphicObjectSummaryForMrid(QStringLiteral("query-breaker-001"), queryState);
    QCOMPARE(mridSummary.mrid, QStringLiteral("query-breaker-001"));
    QCOMPARE(mridSummary.className, QStringLiteral("Breaker"));
    QCOMPARE(mridSummary.graphicClassName, QStringLiteral("CimdrawPowerCircuitBreakerItem"));
    QCOMPARE(sceneMridSummary.mrid, mridSummary.mrid);
    QCOMPARE(sceneMridSummary.className, mridSummary.className);
    QCOMPARE(sceneMridSummary.graphicClassName, mridSummary.graphicClassName);
    QVERIFY(!mridSummary.supportsDirectedFlow);
    QCOMPARE(mridSummary.powerTopologyRole, PowerTopologyRole::Breaker);
    QCOMPARE(mridSummary.visualState, CimGraphicObjectVisualState::Visualized);
    QVERIFY(mridSummary.renderState.valid);
    QCOMPARE(facade.sceneItemByMrid(QStringLiteral("query-breaker-001"), &scene, queryContext),
             static_cast<QGraphicsItem*>(shape));
    QCOMPARE(scene.graphicSceneItemByMrid(QStringLiteral("query-breaker-001"), queryState),
             static_cast<QGraphicsItem*>(shape));
    QCOMPARE(facade.mridForSceneItem(shape, queryContext), QStringLiteral("query-breaker-001"));
    QCOMPARE(scene.graphicMridForSceneItem(shape, queryState), QStringLiteral("query-breaker-001"));
}

void TestCimdrawTopology::cim_query_facade_returns_minimal_behavior_result()
{
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("behavior-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:behavior-breaker-001"));
    breaker->setSwitchPosition(0);
    breaker->setWiringRunState(CimdrawWiringRunState::Energized);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("behavior-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("behavior-breaker-001"),
                            QStringLiteral("gfx:behavior-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};

    const CimBehaviorResult itemResult = facade.behaviorResultForSceneItem(breaker);
    const CimBehaviorResult sceneItemResult = scene.behaviorResultForSceneItem(breaker);
    const CimBehaviorResult effectiveItemResult = breaker->effectiveBehaviorResult();
    QVERIFY(itemResult.valid);
    QVERIFY(itemResult.available);
    QVERIFY(!itemResult.conductivity);
    QVERIFY(itemResult.canOperate);
    QVERIFY(itemResult.energized);
    QCOMPARE(itemResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);
    QCOMPARE(sceneItemResult.available, itemResult.available);
    QCOMPARE(sceneItemResult.conductivity, itemResult.conductivity);
    QCOMPARE(sceneItemResult.canOperate, itemResult.canOperate);
    QCOMPARE(sceneItemResult.energized, itemResult.energized);
    QCOMPARE(sceneItemResult.terminalConnectivity, itemResult.terminalConnectivity);
    QVERIFY(effectiveItemResult.valid);
    QCOMPARE(effectiveItemResult.conductivity, itemResult.conductivity);
    QCOMPARE(effectiveItemResult.energized, itemResult.energized);
    QCOMPARE(effectiveItemResult.canOperate, itemResult.canOperate);
    QCOMPARE(effectiveItemResult.terminalConnectivity, itemResult.terminalConnectivity);

    const CimBehaviorResult mridResult =
        facade.behaviorResultForMrid(QStringLiteral("behavior-breaker-001"), &scene, queryState);
    const CimBehaviorResult sceneMridResult =
        scene.behaviorResultForMrid(QStringLiteral("behavior-breaker-001"), queryState);
    QVERIFY(mridResult.valid);
    QVERIFY(!mridResult.conductivity);
    QVERIFY(mridResult.canOperate);
    QVERIFY(mridResult.energized);
    QCOMPARE(sceneMridResult.valid, mridResult.valid);
    QCOMPARE(sceneMridResult.conductivity, mridResult.conductivity);
    QCOMPARE(sceneMridResult.canOperate, mridResult.canOperate);
    QCOMPARE(sceneMridResult.energized, mridResult.energized);

    const CimModel model = []()
    {
        CimModel current;
        CimObject breakerObject;
        breakerObject.className = QStringLiteral("Breaker");
        breakerObject.mrid = QStringLiteral("behavior-breaker-001");
        breakerObject.name = QStringLiteral("行为断路器");
        current.addObject(breakerObject);
        return current;
    }();
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);
    QCOMPARE(visualSummary.behaviorAvailableObjects, 1);
    QCOMPARE(visualSummary.conductiveObjects, 0);
    QCOMPARE(visualSummary.operableObjects, 1);
    QCOMPARE(visualSummary.energizedObjects, 1);
    QVERIFY(visualSummary.objectSummariesByMrid.contains(QStringLiteral("behavior-breaker-001")));
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("behavior-breaker-001"))
                .behaviorResult.valid);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("behavior-breaker-001"))
                .behaviorResult.energized);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("behavior-breaker-001"))
                .renderState.valid);
    QCOMPARE(visualSummary.objectSummariesByMrid.value(QStringLiteral("behavior-breaker-001"))
                 .renderState.runState,
             CimdrawWiringRunState::Energized);
    QVERIFY(!visualSummary.objectSummariesByMrid.value(QStringLiteral("behavior-breaker-001"))
                 .supportsDirectedFlow);
    const CimGraphicObjectSummary itemSummary =
        facade.graphicObjectSummaryForSceneItem(breaker, queryState);
    const CimGraphicObjectSummary sceneItemSummary =
        scene.graphicObjectSummaryForSceneItem(breaker);
    QCOMPARE(itemSummary.objectId, breaker->cimdrawObjectId());
    QCOMPARE(itemSummary.className, QStringLiteral("Breaker"));
    QCOMPARE(itemSummary.displayName, QStringLiteral("Breaker"));
    QVERIFY(!itemSummary.supportsDirectedFlow);
    QVERIFY(itemSummary.behaviorResult.valid);
    QVERIFY(itemSummary.behaviorResult.energized);
    QCOMPARE(itemSummary.powerTopologyRole, PowerTopologyRole::Breaker);
    QVERIFY(itemSummary.renderState.valid);
    QCOMPARE(itemSummary.renderState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(itemSummary.renderState.switchPosition, 0);
    QCOMPARE(sceneItemSummary.objectId, itemSummary.objectId);
    QCOMPARE(sceneItemSummary.className, itemSummary.className);
    QCOMPARE(sceneItemSummary.displayName, itemSummary.displayName);
    QCOMPARE(sceneItemSummary.powerTopologyRole, itemSummary.powerTopologyRole);
    QCOMPARE(sceneItemSummary.supportsDirectedFlow, itemSummary.supportsDirectedFlow);
    QCOMPARE(sceneItemSummary.renderState.runState, itemSummary.renderState.runState);
    QCOMPARE(sceneItemSummary.renderState.switchPosition, itemSummary.renderState.switchPosition);
    const CimGraphicObjectSummary mridSummary =
        facade.graphicObjectSummaryForMrid(QStringLiteral("behavior-breaker-001"), &scene, queryState);
    QCOMPARE(mridSummary.className, QStringLiteral("Breaker"));
    QVERIFY(!mridSummary.supportsDirectedFlow);
    QCOMPARE(mridSummary.powerTopologyRole, PowerTopologyRole::Breaker);
    QVERIFY(mridSummary.behaviorResult.valid);
    QVERIFY(mridSummary.behaviorResult.energized);
    QVERIFY(mridSummary.renderState.valid);
    QCOMPARE(mridSummary.renderState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(mridSummary.renderState.switchPosition, 0);
    QVERIFY(visualSummary.behaviorResultsByMrid.contains(QStringLiteral("behavior-breaker-001")));
}

void TestCimdrawTopology::cim_query_facade_exposes_graphic_render_state()
{
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("render-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:render-breaker-001"));
    breaker->setWiringDataKey(QStringLiteral("render-breaker-key"));
    breaker->setWiringRunState(CimdrawWiringRunState::Alarm);
    breaker->setSwitchPosition(1);
    breaker->setFlowSign(-1);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("render-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("render-breaker-001"),
                            QStringLiteral("gfx:render-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};

    const CimGraphicRenderState initialItemState = breaker->graphicRenderState();
    const CimGraphicRenderState initialFacadeState =
        facade.graphicRenderStateForSceneItem(breaker);
    const CimGraphicRenderState initialSceneState =
        scene.graphicRenderStateForSceneItem(breaker);
    const CimGraphicRenderStateSource initialFacadeSource =
        facade.graphicRenderStateSourceForSceneItem(breaker);
    const CimGraphicRenderStateSource initialSceneSource =
        scene.graphicRenderStateSourceForSceneItem(breaker);
    QVERIFY(initialFacadeState.valid);
    QVERIFY(initialFacadeSource.valid);
    QCOMPARE(initialFacadeSource.runtimeRunState, CimdrawWiringRunState::Alarm);
    QCOMPARE(initialFacadeSource.manualSwitchPosition, 1);
    QCOMPARE(initialFacadeSource.flowSign, -1);
    QVERIFY(!initialFacadeSource.behaviorResult.valid);
    QCOMPARE(initialFacadeSource.behaviorDrivesSwitchPosition,
             breaker->supportsSwitchToggleOnDoubleClick());
    QCOMPARE(initialFacadeState.runState, initialItemState.runState);
    QCOMPARE(initialFacadeState.switchPosition, initialItemState.switchPosition);
    QCOMPARE(initialFacadeState.flowSign, -1);
    QCOMPARE(initialSceneSource.valid, initialFacadeSource.valid);
    QCOMPARE(initialSceneSource.runtimeRunState, initialFacadeSource.runtimeRunState);
    QCOMPARE(initialSceneSource.manualSwitchPosition, initialFacadeSource.manualSwitchPosition);
    QCOMPARE(initialSceneSource.flowSign, initialFacadeSource.flowSign);
    QCOMPARE(initialSceneSource.behaviorResult.valid, initialFacadeSource.behaviorResult.valid);
    QCOMPARE(initialSceneState.valid, initialFacadeState.valid);
    QCOMPARE(initialSceneState.runState, initialFacadeState.runState);
    QCOMPARE(initialSceneState.switchPosition, initialFacadeState.switchPosition);
    QCOMPARE(initialSceneState.flowSign, initialFacadeState.flowSign);
    const CimGraphicObjectSummary initialObjectSummary =
        facade.graphicObjectSummaryForSceneItem(breaker, queryState);
    QVERIFY(initialObjectSummary.renderState.valid);
    QVERIFY(!initialObjectSummary.supportsDirectedFlow);
    QCOMPARE(initialObjectSummary.renderState.runState, initialItemState.runState);
    QCOMPARE(initialObjectSummary.renderState.flowSign, -1);

    CimBehaviorResult behaviorResult;
    behaviorResult.valid = true;
    behaviorResult.available = true;
    behaviorResult.canOperate = true;
    behaviorResult.conductivity = false;
    behaviorResult.energized = true;
    behaviorResult.terminalConnectivity = CimBehaviorTerminalConnectivity::Disconnected;
    scene.setWiringData(QStringLiteral("render-breaker-key"),
                        cimBehaviorResultToVariantMap(behaviorResult));

    const CimGraphicRenderState mridState =
        facade.graphicRenderStateForMrid(QStringLiteral("render-breaker-001"), &scene, queryState);
    const CimGraphicRenderState sceneMridState =
        scene.graphicRenderStateForMrid(QStringLiteral("render-breaker-001"), queryState);
    const CimGraphicRenderStateSource mridSource =
        facade.graphicRenderStateSourceForMrid(QStringLiteral("render-breaker-001"), &scene, queryState);
    const CimGraphicRenderStateSource sceneMridSource =
        scene.graphicRenderStateSourceForMrid(QStringLiteral("render-breaker-001"), queryState);
    QVERIFY(mridState.valid);
    QVERIFY(mridSource.valid);
    QCOMPARE(mridSource.runtimeRunState, CimdrawWiringRunState::Alarm);
    QCOMPARE(mridSource.manualSwitchPosition, 1);
    QCOMPARE(mridSource.flowSign, -1);
    QVERIFY(mridSource.behaviorResult.valid);
    QVERIFY(mridSource.behaviorResult.energized);
    QVERIFY(mridSource.behaviorDrivesSwitchPosition);
    QCOMPARE(mridState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(mridState.switchPosition, 0);
    QCOMPARE(mridState.flowSign, -1);
    QCOMPARE(sceneMridSource.valid, mridSource.valid);
    QCOMPARE(sceneMridSource.runtimeRunState, mridSource.runtimeRunState);
    QCOMPARE(sceneMridSource.manualSwitchPosition, mridSource.manualSwitchPosition);
    QCOMPARE(sceneMridSource.flowSign, mridSource.flowSign);
    QCOMPARE(sceneMridSource.behaviorResult.valid, mridSource.behaviorResult.valid);
    QCOMPARE(sceneMridSource.behaviorResult.energized, mridSource.behaviorResult.energized);
    QCOMPARE(sceneMridState.valid, mridState.valid);
    QCOMPARE(sceneMridState.runState, mridState.runState);
    QCOMPARE(sceneMridState.switchPosition, mridState.switchPosition);
    QCOMPARE(sceneMridState.flowSign, mridState.flowSign);
    QVERIFY(!mridState.alarmPulse);
    const CimGraphicObjectSummary mridSummary =
        facade.graphicObjectSummaryForMrid(QStringLiteral("render-breaker-001"), &scene, queryState);
    QVERIFY(mridSummary.renderState.valid);
    QVERIFY(!mridSummary.supportsDirectedFlow);
    QCOMPARE(mridSummary.renderState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(mridSummary.renderState.switchPosition, 0);
    QCOMPARE(mridSummary.renderState.flowSign, -1);

    const CimModel model = []()
    {
        CimModel current;
        CimObject breakerObject;
        breakerObject.className = QStringLiteral("Breaker");
        breakerObject.mrid = QStringLiteral("render-breaker-001");
        breakerObject.name = QStringLiteral("Render Breaker");
        current.addObject(breakerObject);
        return current;
    }();
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("render-breaker-001"))
                .renderState.valid);
    QCOMPARE(visualSummary.objectSummariesByMrid.value(QStringLiteral("render-breaker-001"))
                 .renderState.runState,
             CimdrawWiringRunState::Energized);
    QVERIFY(!visualSummary.objectSummariesByMrid.value(QStringLiteral("render-breaker-001"))
                 .supportsDirectedFlow);
}

void TestCimdrawTopology::cim_graphic_render_state_query_reads_effective_visual_state()
{
    CimGraphicRenderStateQuery query;
    CimdrawPowerCircuitBreakerItem breaker(QRectF(0, 0, 64, 92));
    breaker.setWiringRunState(CimdrawWiringRunState::Alarm);
    breaker.setSwitchPosition(0);
    breaker.setFlowSign(-1);

    const CimGraphicRenderStateSource source = query.sourceForItem(&breaker);
    QVERIFY(source.valid);
    QCOMPARE(source.runtimeRunState, CimdrawWiringRunState::Alarm);
    QCOMPARE(source.manualSwitchPosition, 0);
    QCOMPARE(source.flowSign, -1);

    const CimGraphicRenderState queryState = query.resultForItem(&breaker);
    const CimGraphicRenderState itemState = breaker.graphicRenderState();
    const CimGraphicRenderState sourceState = query.resultForSource(source, queryState.alarmPulse);
    QVERIFY(queryState.valid);
    QCOMPARE(queryState.runState, itemState.runState);
    QCOMPARE(queryState.switchPosition, itemState.switchPosition);
    QCOMPARE(queryState.flowSign, itemState.flowSign);
    QCOMPARE(queryState.flowLeftToRight, itemState.flowLeftToRight);
    QCOMPARE(queryState.alarmActive, itemState.alarmActive);
    QCOMPARE(queryState.switchClosed, itemState.switchClosed);
    QCOMPARE(queryState.runState, sourceState.runState);
    QCOMPARE(queryState.switchPosition, sourceState.switchPosition);
    QCOMPARE(queryState.flowSign, sourceState.flowSign);
    QCOMPARE(queryState.flowLeftToRight, sourceState.flowLeftToRight);
    QCOMPARE(queryState.alarmActive, sourceState.alarmActive);
    QCOMPARE(queryState.switchClosed, sourceState.switchClosed);
    QCOMPARE(queryState.alarmActive,
             cimGraphicAlarmActive(queryState.runState, queryState.alarmPulse));
    QCOMPARE(queryState.alarmActive, cimGraphicRenderStateAlarmActive(queryState));
    QVERIFY(cimGraphicUsesAlarmAccent(queryState.runState));
    QVERIFY(cimGraphicRenderStateUsesAlarmAccent(queryState));
    const CimGraphicRenderState forcedAlarmState = query.resultForSource(source, true);
    QCOMPARE(cimGraphicAlarmPenWidth(forcedAlarmState.runState, forcedAlarmState.alarmPulse),
             qreal(3.0));
    QCOMPARE(cimGraphicRenderStateAlarmPenWidth(forcedAlarmState), qreal(3.0));
    QCOMPARE(cimGraphicRenderStateAlarmPenWidth(forcedAlarmState, 2.0, 4.0), qreal(4.0));
    QCOMPARE(cimGraphicAlarmPenWidth(forcedAlarmState.runState, false), qreal(2.0));
    QVERIFY(!cimGraphicUsesAlarmAccent(CimdrawWiringRunState::Normal));
    QVERIFY(!queryState.flowLeftToRight);
    QVERIFY(!cimGraphicRenderStateFlowLeftToRight(queryState));
    QVERIFY(!queryState.switchClosed);
    QVERIFY(cimGraphicRenderStateSwitchOpen(queryState));
    QVERIFY(!cimGraphicRenderStateSwitchClosed(queryState));
    breaker.setWiringRunState(CimdrawWiringRunState::Normal);
    QVERIFY(!breaker.effectiveAlarmPulse());
    QCOMPARE(breaker.graphicRenderState().alarmPulse, breaker.effectiveAlarmPulse());
    QVERIFY(!query.resultForItem(nullptr).valid);
    QVERIFY(!query.sourceForItem(nullptr).valid);
    QVERIFY(!query.resultForSource({}).valid);
}

void TestCimdrawTopology::cim_graphic_render_state_query_resolves_mrid_and_selection()
{
    CimGraphicRenderStateQuery query;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("render-query-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:render-query-breaker-001"));
    breaker->setWiringRunState(CimdrawWiringRunState::Alarm);
    breaker->setSwitchPosition(0);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("render-query-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("render-query-breaker-001"),
                            QStringLiteral("gfx:render-query-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};

    QVERIFY(!query.selectedSource(&scene, queryState).valid);
    QVERIFY(!query.selectedResult(&scene, queryState).valid);
    const CimGraphicRenderStateSource mridSource =
        query.sourceForMrid(QStringLiteral("render-query-breaker-001"), &scene, queryState);
    QVERIFY(mridSource.valid);
    QCOMPARE(mridSource.runtimeRunState, CimdrawWiringRunState::Alarm);
    QCOMPARE(mridSource.manualSwitchPosition, 0);
    const CimGraphicRenderState mridState =
        query.resultForMrid(QStringLiteral("render-query-breaker-001"), &scene, queryState);
    QVERIFY(mridState.valid);
    QCOMPARE(mridState.runState, CimdrawWiringRunState::Alarm);
    QCOMPARE(mridState.switchPosition, 0);

    scene.addSelection(breaker);
    const CimGraphicRenderStateSource selectedSource = query.selectedSource(&scene, queryState);
    QVERIFY(selectedSource.valid);
    QCOMPARE(selectedSource.runtimeRunState, mridSource.runtimeRunState);
    QCOMPARE(selectedSource.manualSwitchPosition, mridSource.manualSwitchPosition);
    const CimGraphicRenderState selectedState = query.selectedResult(&scene, queryState);
    QVERIFY(selectedState.valid);
    QCOMPARE(selectedState.runState, mridState.runState);
    QCOMPARE(selectedState.switchPosition, mridState.switchPosition);
}

void TestCimdrawTopology::cim_graphic_behavior_query_reads_mrid_selection_and_wiring_results()
{
    CimGraphicBehaviorQuery query;
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("behavior-query-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:behavior-query-breaker-001"));
    breaker->setSwitchPosition(1);
    breaker->setWiringRunState(CimdrawWiringRunState::Energized);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("behavior-query-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("behavior-query-breaker-001"),
                            QStringLiteral("gfx:behavior-query-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};

    const CimBehaviorResult itemResult = query.resultForItem(breaker);
    const CimBehaviorResult mridResult =
        query.resultForMrid(QStringLiteral("behavior-query-breaker-001"), &scene, queryState);
    QVERIFY(itemResult.valid);
    QVERIFY(mridResult.valid);
    QCOMPARE(mridResult.energized, itemResult.energized);
    QCOMPARE(mridResult.canOperate, itemResult.canOperate);
    QVERIFY(!query.selectedResult(&scene, queryState).valid);

    scene.addSelection(breaker);
    const CimBehaviorResult selectedResult = query.selectedResult(&scene, queryState);
    const CimBehaviorResult facadeSelectedResult =
        facade.selectedBehaviorResult(&scene, queryState);
    const CimBehaviorResult sceneSelectedResult = scene.selectedBehaviorResult(queryState);
    QVERIFY(selectedResult.valid);
    QVERIFY(selectedResult.energized);
    QCOMPARE(facadeSelectedResult.energized, selectedResult.energized);
    QCOMPARE(sceneSelectedResult.energized, selectedResult.energized);
    QCOMPARE(sceneSelectedResult.canOperate, selectedResult.canOperate);

    scene.setWiringBehaviorResult(QStringLiteral("wiring:behavior-query-breaker-001"), itemResult);
    const CimBehaviorResult wiringResult =
        query.resultForWiringKey(&scene, QStringLiteral("wiring:behavior-query-breaker-001"));
    QVERIFY(wiringResult.valid);
    QCOMPARE(wiringResult.energized, itemResult.energized);
    QCOMPARE(wiringResult.canOperate, itemResult.canOperate);
    QVERIFY(!query.resultForWiringKey(&scene, QStringLiteral("wiring:missing")).valid);
}

void TestCimdrawTopology::cim_graphic_visual_summary_query_builds_live_summary()
{
    CimGraphicVisualSummaryQuery query;
    CimGraphicObjectSummaryQuery objectQuery;
    CimdrawScene scene;
    CimIdIndex idIndex;
    CimModel model;

    CimObject breakerObject;
    breakerObject.mrid = QStringLiteral("visual-query-breaker-001");
    breakerObject.className = QStringLiteral("Breaker");
    breakerObject.name = QStringLiteral("Visual Query Breaker");
    QVERIFY(model.addObject(breakerObject));

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("visual-query-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:visual-query-breaker-001"));
    breaker->setWiringRunState(CimdrawWiringRunState::Energized);
    breaker->setSwitchPosition(0);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("visual-query-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("visual-query-breaker-001"),
                            QStringLiteral("gfx:visual-query-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 5};

    const QSet<QString> visualizedMrids = query.visualizedMrids(queryState);
    QVERIFY(visualizedMrids.contains(QStringLiteral("visual-query-breaker-001")));
    QCOMPARE(objectQuery.mridForSceneItem(breaker, queryState.queryContext),
             QStringLiteral("visual-query-breaker-001"));

    const CimGraphicVisualSummary summary = query.buildSummary(model, queryState);
    QCOMPARE(summary.generatedHelperLineCount, 5);
    QVERIFY(summary.objectSummariesByMrid.contains(QStringLiteral("visual-query-breaker-001")));
    const CimGraphicObjectSummary objectSummary =
        summary.objectSummariesByMrid.value(QStringLiteral("visual-query-breaker-001"));
    QCOMPARE(objectSummary.className, QStringLiteral("Breaker"));
    QVERIFY(objectSummary.renderState.valid);
    QCOMPARE(objectSummary.renderState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(objectSummary.renderState.switchPosition, 0);
    QVERIFY(summary.behaviorResultsByMrid.contains(QStringLiteral("visual-query-breaker-001")));
}

void TestCimdrawTopology::cim_generated_topology_graph_query_builds_source_relation_and_graph_summaries()
{
    CimGeneratedTopologyGraphQuery query;
    CimdrawScene scene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(busbar);
    scene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("query-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("query-load-001"));
    load->setDisplayName(QStringLiteral("查询负荷"));

    CimdrawConnectPoint* busbarRight = findConnectPort(busbar, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(busbarRight != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, busbar, busbarRight, load, loadLeft);

    const PowerTopologyAnalysisSnapshot snapshot =
        PowerTopologyProjection::fromRuntimeScene(&scene).snapshot();
    const CimGeneratedTopologySourceSummary loadSummary =
        query.generatedTopologySourceSummaryForSceneItem(load, &snapshot);
    QCOMPARE(loadSummary.nodeId, QStringLiteral("query-load-001"));
    QCOMPARE(loadSummary.displayName, QStringLiteral("查询负荷"));
    QVERIFY(loadSummary.isValid());

    const auto sourceSummaries = query.generatedTopologySourceSummaries(&scene, &snapshot, true);
    QCOMPARE(sourceSummaries.size(), 2);
    QHash<QString, CimGeneratedTopologySourceSummary> summariesByNodeId;
    for (const auto& sourceEntry : sourceSummaries)
        summariesByNodeId.insert(sourceEntry.second.nodeId, sourceEntry.second);
    QVERIFY(summariesByNodeId.contains(QStringLiteral("query-busbar-001")));
    QVERIFY(summariesByNodeId.contains(QStringLiteral("query-load-001")));

    const QVector<CimGeneratedTopologyRelationSummary> relationSummaries =
        query.generatedTopologyRelationSummaries(&scene, summariesByNodeId);
    QCOMPARE(relationSummaries.size(), 1);
    QCOMPARE(relationSummaries.first().startNodeId, QStringLiteral("query-busbar-001"));
    QCOMPARE(relationSummaries.first().endNodeId, QStringLiteral("query-load-001"));

    const CimGeneratedTopologyGraphSummary graphSummary =
        query.generatedTopologyGraphSummary(&scene, CimdrawTopologyDomain::PowerSystem);
    QCOMPARE(graphSummary.nodeSummaries.size(), 2);
    QCOMPARE(graphSummary.relationSummaries.size(), 1);
}

void TestCimdrawTopology::cim_topology_summary_query_builds_object_highlight_and_summary_lines()
{
    CimTopologySummaryQuery query;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("query-summary-grid-node-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("query-summary-breaker-node-001"));
    grid->setDisplayName(QStringLiteral("查询电源"));
    breaker->setDisplayName(QStringLiteral("查询断路器"));
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("query-summary-breaker-mrid-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:query-summary-breaker-001"));
    breaker->setWiringRunState(CimdrawWiringRunState::Alarm);
    breaker->setSwitchPosition(0);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("query-summary-breaker-mrid-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("query-summary-breaker-mrid-001"),
                            QStringLiteral("gfx:query-summary-breaker-001"));

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }
    QVERIFY(relationLine != nullptr);

    const CimTopologyObjectSummary objectSummary = query.topologyObjectSummaryForSceneItem(grid);
    QCOMPARE(objectSummary.nodeId, QStringLiteral("query-summary-grid-node-001"));
    QVERIFY(objectSummary.connectedDevices.contains(QStringLiteral("query-summary-breaker-node-001")));

    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};
    const CimTopologyObjectSummary mridSummary =
        query.topologyObjectSummaryForMrid(QStringLiteral("query-summary-breaker-mrid-001"),
                                          &scene,
                                          queryState);
    QCOMPARE(mridSummary.nodeId, QStringLiteral("query-summary-breaker-node-001"));

    const CimTopologyHighlightSummary highlightSummary =
        query.topologyHighlightSummaryForSceneItem(breaker);
    QVERIFY(highlightSummary.canHighlight);
    QVERIFY(highlightSummary.nodeIds.contains(QStringLiteral("query-summary-grid-node-001")));
    QVERIFY(highlightSummary.nodeIds.contains(QStringLiteral("query-summary-breaker-node-001")));
    QCOMPARE(highlightSummary.edgeIds.size(), 1);

    const CimRelationEdgeSummary relationSummary = query.relationEdgeSummaryForSceneItem(relationLine);
    QVERIFY(relationSummary.isValid());
    QVERIFY(relationSummary.relationLink);

    const QStringList breakerLines = query.topologySummaryLinesForSceneItem(breaker);
    QVERIFY(breakerLines.contains(QStringLiteral("对象名称：查询断路器")));
    QVERIFY(breakerLines.contains(QStringLiteral("显示运行态：告警")));
    const QStringList relationLines = query.topologySummaryLinesForSceneItem(relationLine);
    QVERIFY(relationLines.contains(QStringLiteral("关系类型：物理")));
}

void TestCimdrawTopology::cim_power_topology_runtime_query_reads_projection_snapshot_document_and_paths()
{
    CimPowerTopologyRuntimeQuery query;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    auto* ground = new CimdrawPowerGroundItem(QRectF(0, 0, 40, 96));
    scene.addItem(grid);
    scene.addItem(breaker);
    scene.addItem(load);
    scene.addItem(ground);

    grid->setTopologyNodeStableId(QStringLiteral("runtime-query-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("runtime-query-breaker-001"));
    load->setTopologyNodeStableId(QStringLiteral("runtime-query-load-001"));
    ground->setTopologyNodeStableId(QStringLiteral("runtime-query-ground-001"));
    breaker->setDisplayName(QStringLiteral("Runtime Query Breaker"));
    grid->setFlowSign(-1);
    breaker->setFlowSign(-1);
    breaker->setSwitchPosition(1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    CimdrawConnectPoint* breakerBottom = findConnectPort(breaker, BOTTOM_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    CimdrawConnectPoint* groundLeft = findConnectPort(ground, LEFT_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    QVERIFY(breakerBottom != nullptr);
    QVERIFY(loadLeft != nullptr);
    QVERIFY(groundLeft != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);
    connectPowerItems(&scene, breaker, breakerBottom, load, loadLeft);
    connectPowerItems(&scene, breaker, breakerBottom, ground, groundLeft);

    const PowerTopologyProjection projection = query.runtimePowerTopologyProjection(&scene);
    QCOMPARE(projection.sourceKind(), PowerTopologyProjectionSourceKind::RuntimeScene);

    const PowerTopologyAnalysisSnapshot snapshot = query.runtimePowerTopologySnapshot(&scene);
    QCOMPARE(snapshot.devices.size(), 4);
    QVERIFY(snapshot.deviceIndex.contains(QStringLiteral("runtime-query-breaker-001")));

    const PowerTopologyDocumentExport document = query.runtimePowerTopologyDocumentExport(&scene);
    QCOMPARE(document.devices.size(), snapshot.devices.size());
    QCOMPARE(document.conductors.size(), snapshot.conductors.size());
    QCOMPARE(document.nodes.size(), snapshot.nodes.size());
    QCOMPARE(document.viewScale, 1.0);

    const QString breakerId = QStringLiteral("runtime-query-breaker-001");
    const QString gridId = QStringLiteral("runtime-query-grid-001");
    const QString loadId = QStringLiteral("runtime-query-load-001");

    QCOMPARE(query.powerDeviceNodeIds(&scene, breakerId), projection.deviceNodeIds(breakerId));
    QCOMPARE(query.powerDeviceConductorIds(&scene, breakerId),
             projection.deviceConductorIds(breakerId));
    QCOMPARE(query.connectedPowerDevices(&scene, breakerId),
             projection.connectedComponentDeviceIds(breakerId));
    QCOMPARE(query.busbarAttachedPowerDevices(&scene, breakerId),
             projection.busbarAttachedDevices(breakerId));
    QCOMPARE(query.reachablePowerDevices(&scene, breakerId),
             projection.reachableDevices(breakerId));
    QCOMPARE(query.directedReachablePowerDevices(&scene, gridId),
             projection.directedReachableDevices(gridId));

    const PowerTopologyIslandAnalysis islands = query.analyzePowerIslands(&scene);
    QCOMPARE(islands.islands.size(), projection.islandAnalysis().islands.size());
    const PowerTopologyBranchAnalysis branches = query.analyzePowerBranches(&scene, gridId);
    QCOMPARE(branches.branches.size(), projection.branchAnalysis(gridId).branches.size());
    const PowerTopologyLoopAnalysis loops = query.analyzePowerLoops(&scene);
    QCOMPARE(loops.loops.size(), projection.loopAnalysis().loops.size());

    const PowerTopologyPath supplyPath = query.shortestPowerSupplyPath(&scene, gridId, loadId);
    QCOMPARE(supplyPath.deviceIds, projection.shortestSupplyPath(gridId, loadId).deviceIds);
    const PowerTopologyPath directedSupplyPath =
        query.shortestDirectedPowerSupplyPath(&scene, gridId, loadId);
    QCOMPARE(directedSupplyPath.deviceIds,
             projection.shortestDirectedSupplyPath(gridId, loadId).deviceIds);
    const PowerTopologyGroundPath groundPath = query.shortestPowerGroundPath(&scene, gridId);
    QCOMPARE(groundPath.deviceIds, projection.shortestGroundPath(gridId).deviceIds);
}

void TestCimdrawTopology::cim_power_topology_preview_query_builds_switch_protection_and_operation_previews()
{
    CimPowerTopologyPreviewQuery query;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(grid);
    scene.addItem(breaker);
    scene.addItem(load);

    grid->setTopologyNodeStableId(QStringLiteral("preview-query-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("preview-query-breaker-001"));
    load->setTopologyNodeStableId(QStringLiteral("preview-query-load-001"));
    breaker->setSwitchPosition(1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    CimdrawConnectPoint* breakerBottom = findConnectPort(breaker, BOTTOM_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    QVERIFY(breakerBottom != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);
    connectPowerItems(&scene, breaker, breakerBottom, load, loadLeft);

    const QString breakerId = QStringLiteral("preview-query-breaker-001");
    const PowerTopologySwitchChangePreview switchPreview =
        query.previewPowerSwitchTopologyChange(&scene, breakerId, 0);
    const PowerTopologyProtectionRangePreview protectionPreview =
        query.previewPowerProtectionRange(&scene, breakerId, 0, QStringLiteral("preview-query-grid-001"));
    const PowerTopologyOperationPreview operationPreview =
        query.previewPowerSwitchOperation(&scene,
                                          breakerId,
                                          0,
                                          QStringLiteral("preview-query-grid-001"),
                                          QStringLiteral("preview-query-load-001"),
                                          QStringLiteral("preview-query-grid-001"));

    QVERIFY(switchPreview.topologyChanged);
    QCOMPARE(switchPreview.deviceId, breakerId);
    QCOMPARE(switchPreview.fromSwitchPosition, 1);
    QCOMPARE(switchPreview.toSwitchPosition, 0);
    QVERIFY(!protectionPreview.protectiveDeviceId.isEmpty());
    QCOMPARE(protectionPreview.protectiveDeviceId, breakerId);
    QCOMPARE(operationPreview.switchChange.deviceId, breakerId);
    QCOMPARE(operationPreview.switchChange.toSwitchPosition, 0);
}

void TestCimdrawTopology::cim_topology_compat_query_reads_relation_and_binding_queries()
{
    CimTopologyCompatQuery query;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("compat-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("compat-breaker-001"));

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }
    QVERIFY(relationLine != nullptr);

    const TopologyNodeRelationInfo nodeInfo = query.relationNodeInfoValue(&scene, QStringLiteral("compat-grid-001"));
    QCOMPARE(nodeInfo.nodeStableId, QStringLiteral("compat-grid-001"));
    QCOMPARE(nodeInfo.incidentEdgeCount(), 1);

    const QVector<TopologyEdgeMeta> edgeMetas = query.relationEdgeMetas(&scene);
    QCOMPARE(edgeMetas.size(), 1);
    const TopologyEdgeMeta resolvedMeta =
        query.resolveRelationEdgeMeta(&scene, relationLine->resolvedRelationEdgeLookupHint());
    QCOMPARE(resolvedMeta.edgeStableId, edgeMetas.first().edgeStableId);
    const TopologyEdgeMeta directMeta = query.relationEdgeMeta(&scene, resolvedMeta.edgeStableId);
    QCOMPARE(directMeta.edgeStableId, resolvedMeta.edgeStableId);

    const QVariantMap topologyBindingSnapshot = query.topologyBindingSnapshot(&scene);
    QVERIFY(topologyBindingSnapshot.contains(QStringLiteral("power")));
    const QVariantMap powerBindingSnapshot = query.powerTopologyBindingSnapshot(&scene);
    QCOMPARE(powerBindingSnapshot.value(QStringLiteral("domain")).toString(), QStringLiteral("PowerSystem"));

    const QVariantMap relationNodeInfo = query.relationNodeInfo(&scene, QStringLiteral("compat-grid-001"));
    QCOMPARE(relationNodeInfo.value(QStringLiteral("nodeId")).toString(), QStringLiteral("compat-grid-001"));
    const QVariantMap relationBindingSnapshot = query.relationBindingSnapshot(&scene);
    QVERIFY(!relationBindingSnapshot.value(QStringLiteral("edges")).toList().isEmpty());
    const QVariantMap relationEdgeInfo = query.relationEdgeInfo(&scene, relationLine->resolvedRelationEdgeLookupHint());
    QCOMPARE(relationEdgeInfo.value(QStringLiteral("edgeId")).toString(), resolvedMeta.edgeStableId);

    const QVector<int> nodeIds =
        query.topologyQueryIntVector(&scene,
                                     QStringLiteral("power"),
                                     QStringLiteral("deviceNodeIds"),
                                     {{QStringLiteral("deviceId"), QStringLiteral("compat-breaker-001")}});
    QVERIFY(!nodeIds.isEmpty());
}

void TestCimdrawTopology::cim_query_facade_exposes_selected_graphic_summary_and_browser_sync()
{
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("selected-breaker-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:selected-breaker-001"));
    breaker->setWiringRunState(CimdrawWiringRunState::Energized);
    breaker->setSwitchPosition(0);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("selected-breaker-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("selected-breaker-001"),
                            QStringLiteral("gfx:selected-breaker-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};

    QVERIFY(facade.selectedGraphicObjectSummary(&scene, queryState).mrid.isEmpty());
    QVERIFY(!facade.selectedGraphicRenderStateSource(&scene, queryState).valid);
    QVERIFY(!facade.selectedGraphicRenderState(&scene, queryState).valid);
    QVERIFY(scene.selectedGraphicObjectSummary().mrid.isEmpty());
    QVERIFY(!scene.selectedGraphicRenderStateSource().valid);
    QVERIFY(!scene.selectedGraphicRenderState().valid);

    scene.addSelection(breaker);

    const CimGraphicObjectSummary selectedSummary =
        facade.selectedGraphicObjectSummary(&scene, queryState);
    const CimGraphicRenderStateSource selectedSource =
        facade.selectedGraphicRenderStateSource(&scene, queryState);
    const CimGraphicRenderState selectedRenderState =
        facade.selectedGraphicRenderState(&scene, queryState);
    const CimGraphicObjectSummary sceneSelectedSummary = scene.selectedGraphicObjectSummary(queryState);
    const CimGraphicRenderStateSource sceneSelectedSource =
        scene.selectedGraphicRenderStateSource(queryState);
    const CimGraphicRenderState sceneSelectedRenderState = scene.selectedGraphicRenderState(queryState);
    const CimBehaviorResult sceneSelectedBehaviorResult =
        scene.behaviorResultForMrid(QStringLiteral("selected-breaker-001"), queryState);

    QCOMPARE(selectedSummary.mrid, QStringLiteral("selected-breaker-001"));
    QCOMPARE(selectedSummary.className, QStringLiteral("Breaker"));
    QVERIFY(selectedSummary.renderState.valid);
    QVERIFY(selectedSource.valid);
    QCOMPARE(selectedSource.runtimeRunState, CimdrawWiringRunState::Energized);
    QCOMPARE(selectedSource.manualSwitchPosition, 0);
    QCOMPARE(selectedSource.flowSign, 1);
    QVERIFY(selectedRenderState.valid);
    QCOMPARE(selectedRenderState.runState, CimdrawWiringRunState::Energized);
    QCOMPARE(selectedRenderState.switchPosition, 0);
    QCOMPARE(sceneSelectedSummary.mrid, selectedSummary.mrid);
    QCOMPARE(sceneSelectedSummary.className, selectedSummary.className);
    QCOMPARE(sceneSelectedSummary.renderState.runState, selectedSummary.renderState.runState);
    QCOMPARE(sceneSelectedSource.valid, selectedSource.valid);
    QCOMPARE(sceneSelectedSource.runtimeRunState, selectedSource.runtimeRunState);
    QCOMPARE(sceneSelectedSource.manualSwitchPosition, selectedSource.manualSwitchPosition);
    QCOMPARE(sceneSelectedSource.flowSign, selectedSource.flowSign);
    QCOMPARE(sceneSelectedRenderState.runState, selectedRenderState.runState);
    QCOMPARE(sceneSelectedRenderState.switchPosition, selectedRenderState.switchPosition);
    QVERIFY(sceneSelectedBehaviorResult.valid);
    QVERIFY(sceneSelectedBehaviorResult.energized);

    CimModel model;
    CimObject breakerObject;
    breakerObject.className = QStringLiteral("Breaker");
    breakerObject.mrid = QStringLiteral("selected-breaker-001");
    breakerObject.name = QStringLiteral("Selected Breaker");
    QVERIFY(model.addObject(breakerObject));

    CimModelBrowserDock dock;
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);
    dock.setImportResult(model, {}, {}, {}, {}, visualSummary);
    FrameCimWorkbenchController::ImportState importState;
    importState.browserDock = &dock;
    importState.shapeByMrid = shapeByMrid;
    importState.idIndex = idIndex;
    importState.syncBrowserSelection(&scene);

    auto* treeWidget = dock.findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QVERIFY(treeWidget->currentItem() != nullptr);
    QCOMPARE(treeWidget->currentItem()->data(0, Qt::UserRole).toString(),
             QStringLiteral("selected-breaker-001"));
}

void TestCimdrawTopology::cim_graphic_summary_text_formats_status_labels()
{
    CimGraphicObjectSummary breakerSummary;
    breakerSummary.visualState = CimGraphicObjectVisualState::Visualized;
    breakerSummary.topologyDomain = CimdrawTopologyDomain::PowerSystem;
    breakerSummary.powerTopologyRole = PowerTopologyRole::Breaker;
    breakerSummary.renderState.valid = true;
    breakerSummary.renderState.runState = CimdrawWiringRunState::Alarm;
    breakerSummary.renderState.switchPosition = 0;
    breakerSummary.renderState.switchClosed = cimGraphicSwitchClosedFromPosition(
        breakerSummary.renderState.switchPosition);
    breakerSummary.renderState.alarmPulse = true;
    breakerSummary.renderState.alarmActive = cimGraphicAlarmActive(
        breakerSummary.renderState.runState,
        breakerSummary.renderState.alarmPulse);
    breakerSummary.behaviorResult.valid = true;
    breakerSummary.behaviorResult.available = true;
    breakerSummary.behaviorResult.canOperate = true;

    QCOMPARE(cimGraphicObjectVisualStateLabel(breakerSummary.visualState),
             QStringLiteral("已可视化"));
    QCOMPARE(cimGraphicTopologyDomainLabel(breakerSummary.topologyDomain),
             QStringLiteral("电力系统拓扑"));
    QCOMPARE(cimGraphicPowerRoleLabel(breakerSummary.powerTopologyRole),
             QStringLiteral("断路器"));
    QCOMPARE(cimGraphicRenderRunStateLabel(breakerSummary.renderState.runState),
             QStringLiteral("告警"));
    QVERIFY(cimGraphicRenderStateSwitchOpen(breakerSummary.renderState));
    QCOMPARE(cimGraphicRenderSwitchPositionLabel(breakerSummary),
             QStringLiteral("分"));
    QVERIFY(cimGraphicRenderFlowDirectionLabel(breakerSummary).isEmpty());
    const QString breakerRenderLabel = cimGraphicRenderStateLabel(breakerSummary);
    QCOMPARE(breakerRenderLabel, QStringLiteral("显示:告警 / 开关:分 / 告警闪烁"));
    QVERIFY(breakerRenderLabel.contains(QStringLiteral("显示:告警")));
    QVERIFY(breakerRenderLabel.contains(QStringLiteral("开关:分")));
    QVERIFY(breakerRenderLabel.contains(QStringLiteral("告警闪烁")));
    const QString breakerStatusLabel = cimGraphicObjectStatusLabel(breakerSummary);
    QVERIFY(breakerStatusLabel.contains(QStringLiteral("已可视化")));
    QVERIFY(breakerStatusLabel.contains(QStringLiteral("显示:告警")));
    QVERIFY(breakerStatusLabel.contains(QStringLiteral("行为可用")));
    QVERIFY(breakerStatusLabel.contains(QStringLiteral("可操作")));

    CimGraphicObjectSummary gridSummary;
    gridSummary.visualState = CimGraphicObjectVisualState::Visualized;
    gridSummary.topologyDomain = CimdrawTopologyDomain::PowerSystem;
    gridSummary.powerTopologyRole = PowerTopologyRole::Feeder;
    gridSummary.supportsDirectedFlow = true;
    gridSummary.renderState.valid = true;
    gridSummary.renderState.runState = CimdrawWiringRunState::Normal;
    gridSummary.renderState.flowSign = -1;
    gridSummary.renderState.flowLeftToRight = cimGraphicFlowLeftToRightFromSign(
        gridSummary.renderState.flowSign);

    QCOMPARE(cimGraphicRenderRunStateLabel(gridSummary.renderState.runState),
             QStringLiteral("正常"));
    QVERIFY(cimGraphicRenderSwitchPositionLabel(gridSummary).isEmpty());
    QVERIFY(!cimGraphicRenderStateFlowLeftToRight(gridSummary.renderState));
    QCOMPARE(cimGraphicRenderFlowDirectionLabel(gridSummary),
             QStringLiteral("反向"));
    const QString gridStatusLabel = cimGraphicObjectStatusLabel(gridSummary);
    QVERIFY(gridStatusLabel.contains(QStringLiteral("显示:正常")));
    QVERIFY(gridStatusLabel.contains(QStringLiteral("流向:反向")));
    QVERIFY(!gridStatusLabel.contains(QStringLiteral("开关:")));

    QCOMPARE(cimBehaviorTerminalConnectivityLabel(CimBehaviorTerminalConnectivity::Partial),
             QStringLiteral("部分连通"));

    CimRelationEdgeSummary relationSummary;
    relationSummary.relationLink = true;
    relationSummary.edgeMeta.edgeStableId = QStringLiteral("edge-summary-001");
    relationSummary.edgeMeta.relationType = TopologyRelationType::Flow;
    relationSummary.edgeMeta.startNodeStableId = QStringLiteral("node-start");
    relationSummary.edgeMeta.endNodeStableId = QStringLiteral("node-end");
    relationSummary.edgeMeta.startPortKey = QStringLiteral("1");
    relationSummary.edgeMeta.endPortKey = QStringLiteral("3");
    const QStringList relationLines = cimRelationEdgeSummaryLines(relationSummary);
    QVERIFY(relationLines.contains(QStringLiteral("对象类型：连接线")));
    QVERIFY(relationLines.contains(QStringLiteral("拓扑边 ID：edge-summary-001")));
    QVERIFY(relationLines.contains(QStringLiteral("关系类型：流向")));
    QVERIFY(relationLines.contains(QStringLiteral("是否语义拓扑关系线：是")));

    breakerSummary.displayName = QStringLiteral("示例断路器");
    breakerSummary.className = QStringLiteral("CimdrawPowerCircuitBreakerItem");
    breakerSummary.objectId = QStringLiteral("object-breaker-001");
    breakerSummary.mrid = QStringLiteral("mrid-breaker-001");
    breakerSummary.participatesInTopology = true;
    CimTopologyObjectSummary topologySummary;
    topologySummary.nodeId = QStringLiteral("topology-breaker-001");
    topologySummary.conductorIds = {QStringLiteral("cond-1"), QStringLiteral("cond-2")};
    topologySummary.deviceNodeIds = {1, 2};
    topologySummary.connectedDevices = {QStringLiteral("dev-a")};
    topologySummary.reachableDevices = {QStringLiteral("dev-a"), QStringLiteral("dev-b")};
    const QStringList objectLines = cimTopologyObjectSummaryLines(breakerSummary, topologySummary);
    QVERIFY(objectLines.contains(QStringLiteral("对象名称：示例断路器")));
    QVERIFY(objectLines.contains(QStringLiteral("类名：CimdrawPowerCircuitBreakerItem")));
    QVERIFY(objectLines.contains(QStringLiteral("对象 ID：object-breaker-001")));
    QVERIFY(objectLines.contains(QStringLiteral("mRID：mrid-breaker-001")));
    QVERIFY(objectLines.contains(QStringLiteral("显示运行态：告警")));
    QVERIFY(objectLines.contains(QStringLiteral("显示开关位置：分")));
    QVERIFY(objectLines.contains(QStringLiteral("显示告警闪烁：是")));
    QVERIFY(objectLines.contains(QStringLiteral("行为可用：是")));
    QVERIFY(objectLines.contains(QStringLiteral("可操作：是")));
    QVERIFY(objectLines.contains(QStringLiteral("关联导体数：2")));
    QVERIFY(objectLines.contains(QStringLiteral("当前可达设备数：2")));
}

void TestCimdrawTopology::scene_wiring_data_behavior_payload_refreshes_wiring_item()
{
    CimdrawScene scene;
    CimQueryFacade facade;
    CimGraphicRenderStateQuery renderStateQuery;
    auto* breaker = new TestableCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setWiringDataKey(QStringLiteral("behavior-refresh-breaker"));

    CimBehaviorResult deenergizedOpenResult;
    deenergizedOpenResult.valid = true;
    deenergizedOpenResult.available = true;
    deenergizedOpenResult.canOperate = true;
    deenergizedOpenResult.conductivity = false;
    deenergizedOpenResult.energized = false;
    deenergizedOpenResult.terminalConnectivity = CimBehaviorTerminalConnectivity::Disconnected;
    scene.setWiringData(QStringLiteral("behavior-refresh-breaker"),
                        cimBehaviorResultToVariantMap(deenergizedOpenResult));

    QCOMPARE(breaker->wiringRunState(), CimdrawWiringRunState::Deenergized);
    QCOMPARE(breaker->switchPosition(), 0);
    QCOMPARE(breaker->effectiveWiringRunState(), CimdrawWiringRunState::Deenergized);
    QCOMPARE(breaker->effectiveSwitchPosition(), 0);
    const CimGraphicRenderStateSource firstSource = renderStateQuery.sourceForItem(breaker);
    const CimGraphicRenderState firstSourceState = renderStateQuery.resultForSource(firstSource);
    QVERIFY(firstSource.behaviorResult.valid);
    QCOMPARE(firstSourceState.runState, breaker->effectiveWiringRunState());
    QCOMPARE(firstSourceState.switchPosition, breaker->effectiveSwitchPosition());
    QVERIFY(!firstSourceState.switchClosed);
    QVERIFY(cimGraphicRenderStateSwitchOpen(firstSourceState, true));
    QCOMPARE(breaker->visualOpenForTest(), cimGraphicRenderStateSwitchOpen(firstSourceState, true));
    const CimBehaviorResult firstEffectiveResult = breaker->effectiveBehaviorResult();
    const CimBehaviorResult firstResult = facade.behaviorResultForSceneItem(breaker);
    QVERIFY(firstEffectiveResult.valid);
    QVERIFY(firstResult.valid);
    QVERIFY(firstResult.available);
    QVERIFY(firstResult.canOperate);
    QVERIFY(!firstResult.conductivity);
    QVERIFY(!firstResult.energized);
    QCOMPARE(firstResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);
    QCOMPARE(firstEffectiveResult.available, firstResult.available);
    QCOMPARE(firstEffectiveResult.conductivity, firstResult.conductivity);
    QCOMPARE(firstEffectiveResult.energized, firstResult.energized);
    QCOMPARE(firstEffectiveResult.canOperate, firstResult.canOperate);
    QCOMPARE(firstEffectiveResult.terminalConnectivity, firstResult.terminalConnectivity);

    CimBehaviorResult energizedClosedResult;
    energizedClosedResult.valid = true;
    energizedClosedResult.available = true;
    energizedClosedResult.canOperate = true;
    energizedClosedResult.conductivity = true;
    energizedClosedResult.energized = true;
    energizedClosedResult.terminalConnectivity = CimBehaviorTerminalConnectivity::Connected;
    scene.setWiringData(QStringLiteral("behavior-refresh-breaker"),
                        cimBehaviorResultToVariantMap(energizedClosedResult));

    QCOMPARE(breaker->wiringRunState(), CimdrawWiringRunState::Energized);
    QCOMPARE(breaker->switchPosition(), 1);
    QCOMPARE(breaker->effectiveWiringRunState(), CimdrawWiringRunState::Energized);
    QCOMPARE(breaker->effectiveSwitchPosition(), 1);
    const CimGraphicRenderStateSource secondSource = renderStateQuery.sourceForItem(breaker);
    const CimGraphicRenderState secondSourceState = renderStateQuery.resultForSource(secondSource);
    QVERIFY(secondSource.behaviorResult.valid);
    QCOMPARE(secondSourceState.runState, breaker->effectiveWiringRunState());
    QCOMPARE(secondSourceState.switchPosition, breaker->effectiveSwitchPosition());
    QVERIFY(secondSourceState.switchClosed);
    QVERIFY(cimGraphicRenderStateSwitchClosed(secondSourceState, true));
    QCOMPARE(breaker->visualOpenForTest(), cimGraphicRenderStateSwitchOpen(secondSourceState, true));
    const CimBehaviorResult secondEffectiveResult = breaker->effectiveBehaviorResult();
    const CimBehaviorResult secondResult = facade.behaviorResultForSceneItem(breaker);
    QVERIFY(secondEffectiveResult.valid);
    QVERIFY(secondResult.valid);
    QVERIFY(secondResult.available);
    QVERIFY(secondResult.canOperate);
    QVERIFY(secondResult.conductivity);
    QVERIFY(secondResult.energized);
    QCOMPARE(secondResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Connected);
    QCOMPARE(secondEffectiveResult.available, secondResult.available);
    QCOMPARE(secondEffectiveResult.conductivity, secondResult.conductivity);
    QCOMPARE(secondEffectiveResult.energized, secondResult.energized);
    QCOMPARE(secondEffectiveResult.canOperate, secondResult.canOperate);
    QCOMPARE(secondEffectiveResult.terminalConnectivity, secondResult.terminalConnectivity);

    breaker->setSwitchPosition(0);
    QCOMPARE(breaker->effectiveWiringRunState(), CimdrawWiringRunState::Energized);
    QCOMPARE(breaker->effectiveSwitchPosition(), 0);
    const CimGraphicRenderState manualSourceState =
        renderStateQuery.resultForSource(renderStateQuery.sourceForItem(breaker));
    QVERIFY(!manualSourceState.switchClosed);
    QVERIFY(cimGraphicRenderStateSwitchOpen(manualSourceState, true));
    QCOMPARE(breaker->visualOpenForTest(), cimGraphicRenderStateSwitchOpen(manualSourceState, true));
    const CimBehaviorResult manualSceneKeyResult =
        scene.behaviorResultForWiringKey(QStringLiteral("behavior-refresh-breaker"));
    QVERIFY(manualSceneKeyResult.valid);
    QVERIFY(!manualSceneKeyResult.conductivity);
    QVERIFY(manualSceneKeyResult.energized);
    QCOMPARE(manualSceneKeyResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);
    const CimBehaviorResult manualEffectiveResult = breaker->effectiveBehaviorResult();
    const CimBehaviorResult manualOverrideResult = facade.behaviorResultForSceneItem(breaker);
    QVERIFY(manualEffectiveResult.valid);
    QVERIFY(manualOverrideResult.valid);
    QVERIFY(manualOverrideResult.available);
    QVERIFY(manualOverrideResult.canOperate);
    QVERIFY(!manualOverrideResult.conductivity);
    QVERIFY(manualOverrideResult.energized);
    QCOMPARE(manualOverrideResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);
    QCOMPARE(manualEffectiveResult.available, manualOverrideResult.available);
    QCOMPARE(manualEffectiveResult.conductivity, manualOverrideResult.conductivity);
    QCOMPARE(manualEffectiveResult.energized, manualOverrideResult.energized);
    QCOMPARE(manualEffectiveResult.canOperate, manualOverrideResult.canOperate);
    QCOMPARE(manualEffectiveResult.terminalConnectivity, manualOverrideResult.terminalConnectivity);

    auto* fallbackBreaker = new TestableCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(fallbackBreaker);
    fallbackBreaker->setWiringRunState(CimdrawWiringRunState::Energized);
    fallbackBreaker->setSwitchPosition(0);
    const CimGraphicRenderState fallbackState =
        renderStateQuery.resultForSource(renderStateQuery.sourceForItem(fallbackBreaker));
    const CimBehaviorResult fallbackResult = fallbackBreaker->effectiveBehaviorResult();
    QVERIFY(cimGraphicRenderStateEnergized(fallbackState));
    QCOMPARE(fallbackResult.runState, CimBehaviorRunState::Energized);
    QCOMPARE(fallbackResult.energized, cimGraphicRenderStateEnergized(fallbackState));
    QCOMPARE(fallbackResult.conductivity, false);
    QCOMPARE(fallbackResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);

    breaker->setWiringRunState(CimdrawWiringRunState::Alarm);
    const CimBehaviorResult scenePublishedResult =
        scene.behaviorResultForWiringKey(QStringLiteral("behavior-refresh-breaker"));
    QVERIFY(scenePublishedResult.valid);
    QCOMPARE(scenePublishedResult.runState, CimBehaviorRunState::Alarm);
    QCOMPARE(breaker->wiringRunState(), CimdrawWiringRunState::Alarm);
    QCOMPARE(breaker->effectiveWiringRunState(), CimdrawWiringRunState::Alarm);
    const QVariantMap publishedMap =
        scene.wiringData(QStringLiteral("behavior-refresh-breaker")).toMap();
    QVERIFY(isCimBehaviorResultVariantMap(publishedMap));
    QCOMPARE(publishedMap.value(QStringLiteral("runState")).toString(), QStringLiteral("alarm"));

    breaker->setWiringRunState(CimdrawWiringRunState::Energized);
    breaker->setSwitchPosition(1);
    QSignalSpy togglePublishSpy(&scene, &CimdrawScene::wiringDataChanged);
    QVERIFY(breaker->toggleSwitchPositionFromInteractionForTest());
    QCOMPARE(togglePublishSpy.count(), 1);
    QCOMPARE(breaker->effectiveSwitchPosition(), 0);
    const CimBehaviorResult toggledResult =
        scene.behaviorResultForWiringKey(QStringLiteral("behavior-refresh-breaker"));
    QVERIFY(toggledResult.valid);
    QCOMPARE(toggledResult.runState, CimBehaviorRunState::Energized);
    QVERIFY(toggledResult.energized);
    QVERIFY(!toggledResult.conductivity);
    QCOMPARE(toggledResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);
}

void TestCimdrawTopology::scene_behavior_result_api_exposes_explicit_behavior_payload()
{
    CimdrawScene scene;
    CimQueryFacade facade;

    CimBehaviorResult result;
    result.objectId = QStringLiteral("behavior-api-breaker");
    result.valid = true;
    result.available = true;
    result.conductivity = true;
    result.canOperate = true;
    result.energized = true;
    result.terminalConnectivity = CimBehaviorTerminalConnectivity::Connected;

    scene.setWiringBehaviorResult(QStringLiteral("behavior-api-key"), result);

    const CimBehaviorResult sceneResult = scene.wiringBehaviorResult(QStringLiteral("behavior-api-key"));
    QCOMPARE(sceneResult.objectId, QStringLiteral("behavior-api-breaker"));
    QVERIFY(sceneResult.valid);
    QVERIFY(sceneResult.available);
    QVERIFY(sceneResult.conductivity);
    QVERIFY(sceneResult.canOperate);
    QVERIFY(sceneResult.energized);
    QCOMPARE(sceneResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Connected);

    const CimBehaviorResult sceneQueryResult =
        scene.behaviorResultForWiringKey(QStringLiteral("behavior-api-key"));
    QCOMPARE(sceneQueryResult.objectId, QStringLiteral("behavior-api-breaker"));
    QVERIFY(sceneQueryResult.valid);
    QVERIFY(sceneQueryResult.available);
    QVERIFY(sceneQueryResult.conductivity);
    QVERIFY(sceneQueryResult.canOperate);
    QVERIFY(sceneQueryResult.energized);
    QCOMPARE(sceneQueryResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Connected);

    const CimBehaviorResult facadeResult =
        facade.behaviorResultForWiringKey(&scene, QStringLiteral("behavior-api-key"));
    QCOMPARE(facadeResult.objectId, QStringLiteral("behavior-api-breaker"));
    QVERIFY(facadeResult.valid);
    QVERIFY(facadeResult.available);
    QVERIFY(facadeResult.conductivity);
    QVERIFY(facadeResult.canOperate);
    QVERIFY(facadeResult.energized);
    QCOMPARE(facadeResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Connected);

    QVERIFY(!scene.behaviorResultForWiringKey(QStringLiteral("missing-key")).valid);
    QVERIFY(!facade.behaviorResultForWiringKey(&scene, QStringLiteral("missing-key")).valid);

    CimBehaviorResult ingestResult = result;
    ingestResult.objectId = QStringLiteral("behavior-ingest-breaker");
    ingestResult.conductivity = false;
    ingestResult.terminalConnectivity = CimBehaviorTerminalConnectivity::Disconnected;

    const CimBehaviorRuntimeIngestResult behaviorIngest =
        cimApplyWiringRuntimeValue(&scene,
                                   QStringLiteral("behavior-ingest-key"),
                                   cimBehaviorResultToVariantMap(ingestResult));
    QVERIFY(behaviorIngest.accepted());
    QVERIFY(behaviorIngest.behaviorResultPayload());

    const CimBehaviorResult ingestedSceneResult =
        scene.wiringBehaviorResult(QStringLiteral("behavior-ingest-key"));
    QCOMPARE(ingestedSceneResult.objectId, QStringLiteral("behavior-ingest-breaker"));
    QVERIFY(ingestedSceneResult.valid);
    QVERIFY(!ingestedSceneResult.conductivity);
    QCOMPARE(ingestedSceneResult.terminalConnectivity, CimBehaviorTerminalConnectivity::Disconnected);

    const CimBehaviorRuntimeIngestResult legacyIntIngest =
        cimApplyWiringRuntimeValue(&scene, QStringLiteral("legacy-int-runtime-key"), 1);
    QVERIFY(legacyIntIngest.accepted());
    QVERIFY(legacyIntIngest.behaviorResultPayload());
    const CimBehaviorResult legacyIntResult =
        scene.behaviorResultForWiringKey(QStringLiteral("legacy-int-runtime-key"));
    QVERIFY(legacyIntResult.valid);
    QCOMPARE(legacyIntResult.runState, CimBehaviorRunState::Energized);
    QVERIFY(legacyIntResult.energized);
    QVERIFY(scene.wiringData(QStringLiteral("legacy-int-runtime-key")).toMap().contains(QStringLiteral("runState")));

    const CimBehaviorRuntimeIngestResult legacyStringIngest =
        cimApplyWiringRuntimeValue(&scene, QStringLiteral("legacy-string-runtime-key"), QStringLiteral("alarm"));
    QVERIFY(legacyStringIngest.accepted());
    QVERIFY(legacyStringIngest.behaviorResultPayload());
    const CimBehaviorResult legacyStringResult =
        scene.behaviorResultForWiringKey(QStringLiteral("legacy-string-runtime-key"));
    QVERIFY(legacyStringResult.valid);
    QCOMPARE(legacyStringResult.runState, CimBehaviorRunState::Alarm);
    QVERIFY(!legacyStringResult.energized);

    const CimBehaviorRuntimeIngestResult legacyJsonNumberIngest =
        cimApplyWiringRuntimeValue(&scene, QStringLiteral("legacy-json-number-runtime-key"), QVariant(2.0));
    QVERIFY(legacyJsonNumberIngest.accepted());
    QVERIFY(legacyJsonNumberIngest.behaviorResultPayload());
    const CimBehaviorResult legacyJsonNumberResult =
        scene.behaviorResultForWiringKey(QStringLiteral("legacy-json-number-runtime-key"));
    QVERIFY(legacyJsonNumberResult.valid);
    QCOMPARE(legacyJsonNumberResult.runState, CimBehaviorRunState::Deenergized);
    QVERIFY(!legacyJsonNumberResult.energized);

    const CimBehaviorRuntimeIngestResult rawIngest =
        cimApplyWiringRuntimeValue(&scene, QStringLiteral("raw-runtime-key"), 7);
    QVERIFY(rawIngest.accepted());
    QVERIFY(!rawIngest.behaviorResultPayload());
    QCOMPARE(rawIngest.kind, CimBehaviorRuntimeIngestKind::RawRuntimeValue);
    QCOMPARE(scene.wiringData(QStringLiteral("raw-runtime-key")).toInt(), 7);
    QVERIFY(!cimApplyWiringRuntimeValue(nullptr, QStringLiteral("invalid"), 1).accepted());
    QVERIFY(!cimApplyWiringRuntimeValue(&scene, QString(), 1).accepted());
}

void TestCimdrawTopology::cim_query_facade_forwards_power_topology_queries()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    const QVariantMap topologyBindingSnapshot = facade.topologyBindingSnapshot(&scene);
    QVERIFY(topologyBindingSnapshot.contains(QStringLiteral("power")));

    const QVariantMap powerBindingSnapshot = facade.powerTopologyBindingSnapshot(&scene);
    QCOMPARE(powerBindingSnapshot.value(QStringLiteral("domain")).toString(),
             QStringLiteral("PowerSystem"));

    const QVariantMap projectionInfo = facade.topologyQueryMap(&scene,
                                                               QStringLiteral("power"),
                                                               QStringLiteral("projectionInfo"));
    const QVariantMap sceneProjectionInfo = scene.queryTopologyMap(QStringLiteral("power"),
                                                                   QStringLiteral("projectionInfo"));
    QCOMPARE(projectionInfo.value(QStringLiteral("sourceKind")).toString(),
             QStringLiteral("runtimeScene"));
    QCOMPARE(sceneProjectionInfo.value(QStringLiteral("sourceKind")).toString(),
             projectionInfo.value(QStringLiteral("sourceKind")).toString());

    const QVector<int> missingNodeIds = facade.topologyQueryIntVector(&scene,
                                                                      QStringLiteral("power"),
                                                                      QStringLiteral("deviceNodeIds"),
                                                                      {{QStringLiteral("deviceId"),
                                                                        QStringLiteral("missing-device")}});
    const QVector<int> sceneMissingNodeIds = scene.queryTopologyIntVector(
        QStringLiteral("power"),
        QStringLiteral("deviceNodeIds"),
        {{QStringLiteral("deviceId"), QStringLiteral("missing-device")}});
    QVERIFY(missingNodeIds.isEmpty());
    QVERIFY(sceneMissingNodeIds.isEmpty());
    const QVariantMap sceneProjectionInfoRaw = scene.queryTopology(QStringLiteral("power"),
                                                                   QStringLiteral("projectionInfo"));
    QCOMPARE(sceneProjectionInfoRaw.value(QStringLiteral("ok")).toBool(), true);
    QCOMPARE(scene.queryTopologyData(QStringLiteral("power"),
                                     QStringLiteral("projectionInfo")).toMap().value(QStringLiteral("sourceKind")).toString(),
             QStringLiteral("runtimeScene"));
    QVERIFY(scene.queryTopologyStringList(QStringLiteral("power"),
                                          QStringLiteral("reachableDevices"),
                                          {{QStringLiteral("deviceId"), QStringLiteral("missing-device")}})
                .isEmpty());
    QVERIFY(facade.powerDeviceNodeIds(&scene, QStringLiteral("missing-device")).isEmpty());
    QVERIFY(facade.reachablePowerDevices(&scene, QStringLiteral("missing-device")).isEmpty());
    QVERIFY(facade.analyzePowerIslands(&scene).islands.isEmpty());
    QVERIFY(!facade.shortestPowerSupplyPath(&scene,
                                            QStringLiteral("missing-source"),
                                            QStringLiteral("missing-target"))
                 .found);
}

void TestCimdrawTopology::cim_query_facade_exposes_runtime_power_projection()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    auto* ground = new CimdrawPowerGroundItem(QRectF(0, 0, 40, 96));
    scene.addItem(grid);
    scene.addItem(breaker);
    scene.addItem(load);
    scene.addItem(ground);
    grid->setTopologyNodeStableId(QStringLiteral("projection-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("projection-breaker-001"));
    load->setTopologyNodeStableId(QStringLiteral("projection-load-001"));
    ground->setTopologyNodeStableId(QStringLiteral("projection-ground-001"));
    breaker->setDisplayName(QStringLiteral("Projection Breaker"));
    breaker->setFlowSign(-1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    CimdrawConnectPoint* breakerBottom = findConnectPort(breaker, BOTTOM_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    CimdrawConnectPoint* groundLeft = findConnectPort(ground, LEFT_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    QVERIFY(breakerBottom != nullptr);
    QVERIFY(loadLeft != nullptr);
    QVERIFY(groundLeft != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);
    connectPowerItems(&scene, breaker, breakerBottom, load, loadLeft);
    connectPowerItems(&scene, breaker, breakerBottom, ground, groundLeft);

    const PowerTopologyProjection projection = facade.runtimePowerTopologyProjection(&scene);
    const PowerTopologyProjection sceneProjection = scene.runtimePowerTopologyProjection();
    QCOMPARE(projection.sourceKind(), PowerTopologyProjectionSourceKind::RuntimeScene);
    QVERIFY(projection.truthBoundaryText().contains(QStringLiteral("运行期连接关系")));
    QCOMPARE(sceneProjection.sourceKind(), projection.sourceKind());
    QVERIFY(sceneProjection.truthBoundaryText().contains(QStringLiteral("运行期连接关系")));

    const PowerTopologyAnalysisSnapshot& snapshot = projection.snapshot();
    const PowerTopologyAnalysisSnapshot& sceneSnapshot = sceneProjection.snapshot();
    QCOMPARE(snapshot.devices.size(), 4);
    QCOMPARE(sceneSnapshot.devices.size(), snapshot.devices.size());
    QVERIFY(snapshot.deviceIndex.contains(QStringLiteral("projection-breaker-001")));
    QVERIFY(sceneSnapshot.deviceIndex.contains(QStringLiteral("projection-breaker-001")));
    const PowerTopologyDeviceRecord& breakerRecord =
        snapshot.devices.at(snapshot.deviceIndex.value(QStringLiteral("projection-breaker-001")));
    const PowerTopologyDeviceRecord& sceneBreakerRecord =
        sceneSnapshot.devices.at(sceneSnapshot.deviceIndex.value(QStringLiteral("projection-breaker-001")));
    QCOMPARE(breakerRecord.displayName, QStringLiteral("Projection Breaker"));
    QCOMPARE(breakerRecord.role, PowerTopologyRole::Breaker);
    QCOMPARE(breakerRecord.flowSign, -1);
    QCOMPARE(sceneBreakerRecord.displayName, breakerRecord.displayName);
    QCOMPARE(sceneBreakerRecord.role, breakerRecord.role);
    QCOMPARE(sceneBreakerRecord.flowSign, breakerRecord.flowSign);

    const PowerTopologyAnalysisSnapshot facadeSnapshot =
        facade.runtimePowerTopologySnapshot(&scene);
    QCOMPARE(facadeSnapshot.devices.size(), snapshot.devices.size());
    QCOMPARE(facadeSnapshot.conductors.size(), snapshot.conductors.size());
    QVERIFY(facadeSnapshot.deviceIndex.contains(QStringLiteral("projection-breaker-001")));

    const PowerTopologyDocumentExport document =
        facade.runtimePowerTopologyDocumentExport(&scene);
    QCOMPARE(document.devices.size(), snapshot.devices.size());
    QCOMPARE(document.conductors.size(), snapshot.conductors.size());
    QCOMPARE(document.nodes.size(), snapshot.nodes.size());
    QCOMPARE(document.viewScale, 1.0);

    const QString breakerId = QStringLiteral("projection-breaker-001");
    const QString gridId = QStringLiteral("projection-grid-001");
    const QString loadId = QStringLiteral("projection-load-001");

    QCOMPARE(facade.powerDeviceNodeIds(&scene, breakerId),
             projection.deviceNodeIds(breakerId));
    QCOMPARE(facade.powerDeviceConductorIds(&scene, breakerId),
             projection.deviceConductorIds(breakerId));
    QCOMPARE(facade.connectedPowerDevices(&scene, breakerId),
             projection.connectedComponentDeviceIds(breakerId));
    QCOMPARE(facade.busbarAttachedPowerDevices(&scene, breakerId),
             projection.busbarAttachedDevices(breakerId));
    QCOMPARE(facade.reachablePowerDevices(&scene, breakerId),
             projection.reachableDevices(breakerId));
    QCOMPARE(facade.directedReachablePowerDevices(&scene, gridId),
             projection.directedReachableDevices(gridId));

    const PowerTopologyIslandAnalysis facadeIslands = facade.analyzePowerIslands(&scene);
    const PowerTopologyIslandAnalysis projectionIslands = projection.islandAnalysis();
    QCOMPARE(facadeIslands.islands.size(), projectionIslands.islands.size());
    QCOMPARE(facadeIslands.isolatedDeviceIds, projectionIslands.isolatedDeviceIds);
    QVERIFY(!facadeIslands.islands.isEmpty());
    QCOMPARE(facadeIslands.islands.first().deviceIds,
             projectionIslands.islands.first().deviceIds);
    QCOMPARE(facadeIslands.islands.first().conductorIds,
             projectionIslands.islands.first().conductorIds);

    const PowerTopologyBranchAnalysis facadeBranches =
        facade.analyzePowerBranches(&scene, gridId);
    const PowerTopologyBranchAnalysis projectionBranches =
        projection.branchAnalysis(gridId);
    QCOMPARE(facadeBranches.sourceDeviceId, projectionBranches.sourceDeviceId);
    QCOMPARE(facadeBranches.branches.size(), projectionBranches.branches.size());

    const PowerTopologyLoopAnalysis facadeLoops = facade.analyzePowerLoops(&scene);
    const PowerTopologyLoopAnalysis projectionLoops = projection.loopAnalysis();
    QCOMPARE(facadeLoops.loops.size(), projectionLoops.loops.size());

    const PowerTopologyPath facadeSupplyPath =
        facade.shortestPowerSupplyPath(&scene, gridId, loadId);
    const PowerTopologyPath projectionSupplyPath =
        projection.shortestSupplyPath(gridId, loadId);
    QCOMPARE(facadeSupplyPath.found, projectionSupplyPath.found);
    QCOMPARE(facadeSupplyPath.deviceIds, projectionSupplyPath.deviceIds);
    QCOMPARE(facadeSupplyPath.conductorIds, projectionSupplyPath.conductorIds);

    const PowerTopologyPath facadeDirectedSupplyPath =
        facade.shortestDirectedPowerSupplyPath(&scene, gridId, loadId);
    const PowerTopologyPath projectionDirectedSupplyPath =
        projection.shortestDirectedSupplyPath(gridId, loadId);
    QCOMPARE(facadeDirectedSupplyPath.found, projectionDirectedSupplyPath.found);
    QCOMPARE(facadeDirectedSupplyPath.deviceIds, projectionDirectedSupplyPath.deviceIds);
    QCOMPARE(facadeDirectedSupplyPath.conductorIds, projectionDirectedSupplyPath.conductorIds);

    const PowerTopologyGroundPath facadeGroundPath =
        facade.shortestPowerGroundPath(&scene, gridId);
    const PowerTopologyGroundPath projectionGroundPath =
        projection.shortestGroundPath(gridId);
    QCOMPARE(facadeGroundPath.found, projectionGroundPath.found);
    QCOMPARE(facadeGroundPath.deviceIds, projectionGroundPath.deviceIds);
    QCOMPARE(facadeGroundPath.conductorIds, projectionGroundPath.conductorIds);
}

void TestCimdrawTopology::cim_query_facade_builds_generated_topology_source_summary()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("generated-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("generated-breaker-001"));
    breaker->setDisplayName(QStringLiteral("生成断路器"));
    breaker->setFlowSign(-1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    const PowerTopologyAnalysisSnapshot snapshot =
        facade.runtimePowerTopologyProjection(&scene).snapshot();
    const CimGeneratedTopologySourceSummary breakerSummary =
        facade.generatedTopologySourceSummaryForSceneItem(breaker, &snapshot);
    const CimGeneratedTopologySourceSummary sceneBreakerSummary =
        scene.generatedTopologySourceSummaryForSceneItem(breaker, &snapshot);
    QCOMPARE(breakerSummary.nodeId, QStringLiteral("generated-breaker-001"));
    QCOMPARE(breakerSummary.displayName, QStringLiteral("生成断路器"));
    QCOMPARE(breakerSummary.displayLabel, QStringLiteral("生成断路器"));
    QVERIFY(breakerSummary.participatesInTopology);
    QVERIFY(breakerSummary.isTopologyGraphNode);
    QCOMPARE(breakerSummary.topologyDomain, CimdrawTopologyDomain::PowerSystem);
    QCOMPARE(breakerSummary.powerTopologyRole, PowerTopologyRole::Breaker);
    QVERIFY(breakerSummary.supportsDirectedFlow);
    QCOMPARE(breakerSummary.flowSign, -1);
    QCOMPARE(sceneBreakerSummary.nodeId, breakerSummary.nodeId);
    QCOMPARE(sceneBreakerSummary.displayName, breakerSummary.displayName);
    QCOMPARE(sceneBreakerSummary.powerTopologyRole, breakerSummary.powerTopologyRole);
    QCOMPARE(sceneBreakerSummary.flowSign, breakerSummary.flowSign);

    const CimGeneratedTopologySourceSummary gridSummary =
        facade.generatedTopologySourceSummaryForSceneItem(grid, &snapshot);
    const CimGeneratedTopologySourceSummary sceneGridSummary =
        scene.generatedTopologySourceSummaryForSceneItem(grid, &snapshot);
    QCOMPARE(gridSummary.nodeId, QStringLiteral("generated-grid-001"));
    QVERIFY(!gridSummary.displayName.isEmpty());
    QCOMPARE(gridSummary.displayLabel, gridSummary.displayName);
    QCOMPARE(gridSummary.labelOffset, QPointF(-42.0, 44.0));
    QVERIFY(gridSummary.participatesInTopology);
    QVERIFY(gridSummary.supportsDirectedFlow);
    QCOMPARE(gridSummary.flowSign, 1);
    QCOMPARE(sceneGridSummary.nodeId, gridSummary.nodeId);
    QCOMPARE(sceneGridSummary.displayLabel, gridSummary.displayLabel);
    QCOMPARE(sceneGridSummary.flowSign, gridSummary.flowSign);

    const auto sourceSummaries =
        facade.generatedTopologySourceSummaries(&scene, &snapshot, true);
    const auto sceneSourceSummaries =
        scene.generatedTopologySourceSummaries(&snapshot, true);
    QCOMPARE(sourceSummaries.size(), 2);
    QCOMPARE(sceneSourceSummaries.size(), sourceSummaries.size());
    bool sawBreakerSummary = false;
    bool sawGridSummary = false;
    bool sceneSawBreakerSummary = false;
    bool sceneSawGridSummary = false;
    for (const auto& sourceEntry : sourceSummaries)
    {
        if (sourceEntry.second.nodeId == QStringLiteral("generated-breaker-001"))
        {
            sawBreakerSummary = true;
            QCOMPARE(sourceEntry.second.displayName, QStringLiteral("生成断路器"));
            QCOMPARE(sourceEntry.second.powerTopologyRole, PowerTopologyRole::Breaker);
            QVERIFY(sourceEntry.second.supportsDirectedFlow);
            QCOMPARE(sourceEntry.second.flowSign, -1);
        }
        if (sourceEntry.second.nodeId == QStringLiteral("generated-grid-001"))
        {
            sawGridSummary = true;
            QVERIFY(!sourceEntry.second.displayName.isEmpty());
            QVERIFY(sourceEntry.second.participatesInTopology);
            QCOMPARE(sourceEntry.second.flowSign, 1);
        }
    }
    for (const auto& sourceEntry : sceneSourceSummaries)
    {
        if (sourceEntry.second.nodeId == QStringLiteral("generated-breaker-001"))
        {
            sceneSawBreakerSummary = true;
            QCOMPARE(sourceEntry.second.displayName, QStringLiteral("生成断路器"));
            QCOMPARE(sourceEntry.second.flowSign, -1);
        }
        if (sourceEntry.second.nodeId == QStringLiteral("generated-grid-001"))
        {
            sceneSawGridSummary = true;
            QVERIFY(sourceEntry.second.participatesInTopology);
            QCOMPARE(sourceEntry.second.flowSign, 1);
        }
    }
    QVERIFY(sawBreakerSummary);
    QVERIFY(sawGridSummary);
    QVERIFY(sceneSawBreakerSummary);
    QVERIFY(sceneSawGridSummary);

    const CimGraphicObjectSummary gridObjectSummary =
        facade.graphicObjectSummaryForSceneItem(grid);
    QVERIFY(gridObjectSummary.renderState.valid);
    QVERIFY(gridObjectSummary.supportsDirectedFlow);
    QCOMPARE(gridObjectSummary.renderState.flowSign, 1);
    QCOMPARE(cimGraphicObjectSummaryFlowSign(gridObjectSummary), gridSummary.flowSign);

    const CimModel model = []()
    {
        CimModel current;
        CimObject gridObject;
        gridObject.className = QStringLiteral("EquivalentInjection");
        gridObject.mrid = QStringLiteral("generated-grid-mrid-001");
        gridObject.name = QStringLiteral("Generated Grid");
        current.addObject(gridObject);
        return current;
    }();
    grid->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("generated-grid-mrid-001"));
    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("generated-grid-mrid-001"), grid);
    const CimGraphicQueryState queryState{{&shapeByMrid, nullptr}, 0};
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("generated-grid-mrid-001"))
                .renderState.valid);
    QVERIFY(visualSummary.objectSummariesByMrid.value(QStringLiteral("generated-grid-mrid-001"))
                .supportsDirectedFlow);
}

void TestCimdrawTopology::cim_query_facade_builds_generated_topology_graph_summary()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(busbar);
    scene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("graph-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("graph-load-001"));

    CimdrawConnectPoint* busbarRight = findConnectPort(busbar, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(busbarRight != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, busbar, busbarRight, load, loadLeft);

    CimdrawConnectLine* sourceLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        sourceLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (sourceLine)
            break;
    }
    QVERIFY(sourceLine != nullptr);
    sourceLine->setArrowHead(CimdrawConnectLine::ArrowBoth);

    const CimGeneratedTopologyGraphSummary combinedSummary =
        facade.generatedTopologyGraphSummary(&scene, CimdrawTopologyDomain::None);
    QCOMPARE(combinedSummary.nodeSummaries.size(), 2);
    QCOMPARE(combinedSummary.relationSummaries.size(), 1);
    QCOMPARE(combinedSummary.layoutOrigin, QPointF(120.0, 120.0));
    QCOMPARE(combinedSummary.nodeSpacing, QPointF(220.0, 160.0));
    QCOMPARE(combinedSummary.nodeColumnCount(), 2);
    QCOMPARE(combinedSummary.nodeScenePos(0), QPointF(120.0, 120.0));
    QCOMPARE(combinedSummary.nodeScenePos(1), QPointF(340.0, 120.0));
    QCOMPARE(combinedSummary.relationSummaries.first().startNodeId,
             QStringLiteral("graph-busbar-001"));
    QCOMPARE(combinedSummary.relationSummaries.first().endNodeId,
             QStringLiteral("graph-load-001"));
    QCOMPARE(combinedSummary.relationSummaries.first().arrowHeadStyle,
             static_cast<int>(CimdrawConnectLine::ArrowBoth));

    const CimGeneratedTopologyGraphSummary powerSummary =
        facade.generatedTopologyGraphSummary(&scene, CimdrawTopologyDomain::PowerSystem);
    QCOMPARE(powerSummary.nodeSummaries.size(), combinedSummary.nodeSummaries.size());
    QCOMPARE(powerSummary.relationSummaries.size(), combinedSummary.relationSummaries.size());

    const CimGeneratedTopologyGraphSummary sceneCombinedSummary =
        scene.generatedTopologyGraphSummary(CimdrawTopologyDomain::None);
    QCOMPARE(sceneCombinedSummary.nodeSummaries.size(), combinedSummary.nodeSummaries.size());
    QCOMPARE(sceneCombinedSummary.relationSummaries.size(), combinedSummary.relationSummaries.size());
    const CimGeneratedTopologyGraphSummary scenePowerSummary =
        scene.generatedTopologyGraphSummary(CimdrawTopologyDomain::PowerSystem);
    QCOMPARE(scenePowerSummary.nodeSummaries.size(), powerSummary.nodeSummaries.size());
    QCOMPARE(scenePowerSummary.relationSummaries.size(), powerSummary.relationSummaries.size());
}

void TestCimdrawTopology::cim_query_facade_builds_generated_topology_relation_summary()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(busbar);
    scene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("generated-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("generated-load-001"));
    load->setDisplayName(QStringLiteral("生成负荷"));

    CimdrawConnectPoint* busbarRight = findConnectPort(busbar, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(busbarRight != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, busbar, busbarRight, load, loadLeft);

    CimdrawConnectLine* sourceLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        sourceLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (sourceLine)
            break;
    }
    QVERIFY(sourceLine != nullptr);
    sourceLine->setArrowHead(CimdrawConnectLine::ArrowBoth);

    const PowerTopologyAnalysisSnapshot snapshot = facade.runtimePowerTopologySnapshot(&scene);
    const auto sourceSummaries = facade.generatedTopologySourceSummaries(&scene, &snapshot, true);
    const auto sceneSourceSummaries = scene.generatedTopologySourceSummaries(&snapshot, true);
    QHash<QString, CimGeneratedTopologySourceSummary> summariesByNodeId;
    for (const auto& sourceEntry : sourceSummaries)
        summariesByNodeId.insert(sourceEntry.second.nodeId, sourceEntry.second);
    QCOMPARE(sceneSourceSummaries.size(), sourceSummaries.size());

    const QVector<CimGeneratedTopologyRelationSummary> relationSummaries =
        facade.generatedTopologyRelationSummaries(&scene, summariesByNodeId);
    const QVector<CimGeneratedTopologyRelationSummary> sceneRelationSummaries =
        scene.generatedTopologyRelationSummaries(summariesByNodeId);
    QCOMPARE(relationSummaries.size(), 1);
    QCOMPARE(sceneRelationSummaries.size(), relationSummaries.size());

    const CimGeneratedTopologyRelationSummary& relationSummary = relationSummaries.first();
    const CimGeneratedTopologyRelationSummary& sceneRelationSummary = sceneRelationSummaries.first();
    QVERIFY(relationSummary.isValid());
    QCOMPARE(relationSummary.startNodeId, QStringLiteral("generated-busbar-001"));
    QCOMPARE(relationSummary.endNodeId, QStringLiteral("generated-load-001"));
    QCOMPARE(relationSummary.relationType, TopologyRelationType::Physical);
    QCOMPARE(relationSummary.relationLabel, QStringLiteral("物理"));
    QCOMPARE(relationSummary.labelOffset, QPointF(-16.0, -18.0));
    QCOMPARE(relationSummary.arrowHeadStyle, static_cast<int>(CimdrawConnectLine::ArrowBoth));
    QCOMPARE(sceneRelationSummary.edgeStableId, relationSummary.edgeStableId);
    QCOMPARE(sceneRelationSummary.startNodeId, relationSummary.startNodeId);
    QCOMPARE(sceneRelationSummary.endNodeId, relationSummary.endNodeId);
    QCOMPARE(sceneRelationSummary.relationLabel, relationSummary.relationLabel);
}

void TestCimdrawTopology::cim_query_facade_reverses_generated_topology_relation_ports_and_arrows()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(busbar);
    scene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("reverse-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("reverse-load-001"));

    CimdrawConnectPoint* loadRight = findConnectPort(load, RIGHT_DIRECTION);
    CimdrawConnectPoint* busbarLeft = findConnectPort(busbar, LEFT_DIRECTION);
    QVERIFY(loadRight != nullptr);
    QVERIFY(busbarLeft != nullptr);
    connectPowerItems(&scene, load, loadRight, busbar, busbarLeft);

    CimdrawConnectLine* sourceLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        sourceLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (sourceLine)
            break;
    }
    QVERIFY(sourceLine != nullptr);
    sourceLine->setArrowHead(CimdrawConnectLine::ArrowEnd);

    const TopologyEdgeMeta sourceMeta = sourceLine->resolvedRelationEdgeMeta();
    QVERIFY(!sourceMeta.startPortKey.isEmpty());
    QVERIFY(!sourceMeta.endPortKey.isEmpty());

    const PowerTopologyAnalysisSnapshot snapshot = facade.runtimePowerTopologySnapshot(&scene);
    const auto sourceSummaries = facade.generatedTopologySourceSummaries(&scene, &snapshot, true);
    QHash<QString, CimGeneratedTopologySourceSummary> summariesByNodeId;
    for (const auto& sourceEntry : sourceSummaries)
        summariesByNodeId.insert(sourceEntry.second.nodeId, sourceEntry.second);

    const QVector<CimGeneratedTopologyRelationSummary> relationSummaries =
        facade.generatedTopologyRelationSummaries(&scene, summariesByNodeId);
    QCOMPARE(relationSummaries.size(), 1);

    const CimGeneratedTopologyRelationSummary& relationSummary = relationSummaries.first();
    QCOMPARE(relationSummary.startNodeId, QStringLiteral("reverse-busbar-001"));
    QCOMPARE(relationSummary.endNodeId, QStringLiteral("reverse-load-001"));
    QCOMPARE(relationSummary.startPortKey, sourceMeta.endPortKey);
    QCOMPARE(relationSummary.endPortKey, sourceMeta.startPortKey);
    QCOMPARE(relationSummary.arrowHeadStyle, static_cast<int>(CimdrawConnectLine::ArrowStart));
}

void TestCimdrawTopology::cim_query_facade_derives_generated_topology_arrows_from_flow_sign()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("flow-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("flow-breaker-001"));
    grid->setFlowSign(-1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* sourceLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        sourceLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (sourceLine)
            break;
    }
    QVERIFY(sourceLine != nullptr);
    sourceLine->setArrowHead(CimdrawConnectLine::ArrowNone);

    const TopologyEdgeMeta sourceMeta = sourceLine->resolvedRelationEdgeMeta();
    QVERIFY(!sourceMeta.startPortKey.isEmpty());
    QVERIFY(!sourceMeta.endPortKey.isEmpty());
    QCOMPARE(sourceMeta.startNodeStableId, QStringLiteral("flow-grid-001"));
    QCOMPARE(sourceMeta.endNodeStableId, QStringLiteral("flow-breaker-001"));

    const PowerTopologyAnalysisSnapshot snapshot = facade.runtimePowerTopologySnapshot(&scene);
    const auto sourceSummaries = facade.generatedTopologySourceSummaries(&scene, &snapshot, true);
    QHash<QString, CimGeneratedTopologySourceSummary> summariesByNodeId;
    for (const auto& sourceEntry : sourceSummaries)
        summariesByNodeId.insert(sourceEntry.second.nodeId, sourceEntry.second);
    QCOMPARE(summariesByNodeId.value(QStringLiteral("flow-grid-001")).flowSign, -1);

    const QVector<CimGeneratedTopologyRelationSummary> relationSummaries =
        facade.generatedTopologyRelationSummaries(&scene, summariesByNodeId);
    QCOMPARE(relationSummaries.size(), 1);

    const CimGeneratedTopologyRelationSummary& relationSummary = relationSummaries.first();
    QCOMPARE(relationSummary.startNodeId, QStringLiteral("flow-breaker-001"));
    QCOMPARE(relationSummary.endNodeId, QStringLiteral("flow-grid-001"));
    QCOMPARE(relationSummary.startPortKey, sourceMeta.endPortKey);
    QCOMPARE(relationSummary.endPortKey, sourceMeta.startPortKey);
    QCOMPARE(relationSummary.arrowHeadStyle, static_cast<int>(CimdrawConnectLine::ArrowEnd));
}

void TestCimdrawTopology::cim_query_facade_resolves_relation_edge_info()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);
    grid->setTopologyNodeStableId(QStringLiteral("query-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("query-breaker-002"));

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }

    QVERIFY(relationLine != nullptr);
    const QVariantMap relationNodeInfo = facade.relationNodeInfo(&scene, QStringLiteral("query-grid-001"));
    const QVariantMap sceneRelationNodeInfo =
        scene.relationNodeInfo(QStringLiteral("query-grid-001"));
    QCOMPARE(relationNodeInfo.value(QStringLiteral("nodeId")).toString(),
             QStringLiteral("query-grid-001"));
    QCOMPARE(relationNodeInfo.value(QStringLiteral("incidentEdgeCount")).toInt(), 1);
    QCOMPARE(sceneRelationNodeInfo.value(QStringLiteral("nodeId")).toString(),
             relationNodeInfo.value(QStringLiteral("nodeId")).toString());
    QCOMPARE(sceneRelationNodeInfo.value(QStringLiteral("incidentEdgeCount")).toInt(),
             relationNodeInfo.value(QStringLiteral("incidentEdgeCount")).toInt());
    const TopologyNodeRelationInfo relationInfoValue =
        facade.relationNodeInfoValue(&scene, QStringLiteral("query-grid-001"));
    QCOMPARE(relationInfoValue.nodeStableId, QStringLiteral("query-grid-001"));
    QCOMPARE(relationInfoValue.incidentEdgeCount(), 1);
    const TopologyNodeRelationInfo sceneRelationInfo =
        scene.relationNodeInfoValue(QStringLiteral("query-grid-001"));
    QCOMPARE(sceneRelationInfo.nodeStableId, relationInfoValue.nodeStableId);
    QCOMPARE(sceneRelationInfo.incidentEdgeCount(), relationInfoValue.incidentEdgeCount());

    const QVariantMap relationBindingSnapshot = facade.relationBindingSnapshot(&scene);
    const QVariantMap sceneRelationBindingSnapshot = scene.relationBindingSnapshot();
    QVERIFY(relationBindingSnapshot.value(QStringLiteral("edgeCount")).toInt() >= 1);
    QCOMPARE(sceneRelationBindingSnapshot.value(QStringLiteral("edgeCount")).toInt(),
             relationBindingSnapshot.value(QStringLiteral("edgeCount")).toInt());
    const QVector<TopologyEdgeMeta> relationEdgeMetas = facade.relationEdgeMetas(&scene);
    QVERIFY(!relationEdgeMetas.isEmpty());

    const QVariantMap edgeInfo = facade.relationEdgeInfo(&scene,
                                                         relationLine->resolvedRelationEdgeLookupHint());
    const QVariantMap sceneEdgeInfo =
        scene.relationEdgeInfo(relationLine->resolvedRelationEdgeLookupHint());
    QVERIFY(!edgeInfo.value(QStringLiteral("edgeId")).toString().isEmpty());
    QCOMPARE(edgeInfo.value(QStringLiteral("startNodeId")).toString(), QStringLiteral("query-grid-001"));
    QCOMPARE(edgeInfo.value(QStringLiteral("endNodeId")).toString(), QStringLiteral("query-breaker-002"));
    QCOMPARE(sceneEdgeInfo.value(QStringLiteral("edgeId")).toString(),
             edgeInfo.value(QStringLiteral("edgeId")).toString());
    QCOMPARE(sceneEdgeInfo.value(QStringLiteral("startNodeId")).toString(),
             edgeInfo.value(QStringLiteral("startNodeId")).toString());
    QCOMPARE(sceneEdgeInfo.value(QStringLiteral("endNodeId")).toString(),
             edgeInfo.value(QStringLiteral("endNodeId")).toString());
    const TopologyEdgeMeta edgeMeta =
        facade.resolveRelationEdgeMeta(&scene, relationLine->resolvedRelationEdgeLookupHint());
    QVERIFY(!edgeMeta.edgeStableId.isEmpty());
    QCOMPARE(edgeMeta.startNodeStableId, QStringLiteral("query-grid-001"));
    QCOMPARE(edgeMeta.endNodeStableId, QStringLiteral("query-breaker-002"));
    const CimRelationEdgeSummary relationSummary =
        facade.relationEdgeSummaryForSceneItem(relationLine);
    QVERIFY(relationSummary.isValid());
    QVERIFY(relationSummary.relationLink);
    QCOMPARE(relationSummary.edgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(relationSummary.edgeMeta.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(relationSummary.edgeMeta.endNodeStableId, edgeMeta.endNodeStableId);
    const CimRelationEdgeSummary sceneRelationSummary =
        scene.relationEdgeSummaryForSceneItem(relationLine);
    QVERIFY(sceneRelationSummary.isValid());
    QCOMPARE(sceneRelationSummary.relationLink, relationSummary.relationLink);
    QCOMPARE(sceneRelationSummary.edgeMeta.edgeStableId, relationSummary.edgeMeta.edgeStableId);
    const TopologyEdgeMeta lineEdgeMeta = facade.relationEdgeMetaForSceneItem(relationLine);
    QCOMPARE(lineEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(lineEdgeMeta.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(lineEdgeMeta.endNodeStableId, edgeMeta.endNodeStableId);
    QCOMPARE(lineEdgeMeta.relationType, edgeMeta.relationType);
    const TopologyEdgeMeta sceneLineEdgeMeta = scene.relationEdgeMetaForSceneItem(relationLine);
    QCOMPARE(sceneLineEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(sceneLineEdgeMeta.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(sceneLineEdgeMeta.endNodeStableId, edgeMeta.endNodeStableId);
    QCOMPARE(sceneLineEdgeMeta.relationType, edgeMeta.relationType);
    const TopologyEdgeMeta directEdgeMeta = facade.relationEdgeMeta(&scene, edgeMeta.edgeStableId);
    QCOMPARE(directEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(directEdgeMeta.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(directEdgeMeta.endNodeStableId, edgeMeta.endNodeStableId);
    QCOMPARE(directEdgeMeta.relationType, edgeMeta.relationType);
    const TopologyEdgeMeta sceneDirectEdgeMeta = scene.relationEdgeMeta(edgeMeta.edgeStableId);
    QCOMPARE(sceneDirectEdgeMeta.edgeStableId, directEdgeMeta.edgeStableId);
    QCOMPARE(sceneDirectEdgeMeta.startNodeStableId, directEdgeMeta.startNodeStableId);
    QCOMPARE(sceneDirectEdgeMeta.endNodeStableId, directEdgeMeta.endNodeStableId);
    QCOMPARE(scene.relationEdgeMetas().size(), relationEdgeMetas.size());
    const TopologyEdgeMeta sceneResolvedEdgeMeta =
        scene.resolveRelationEdgeMeta(relationLine->resolvedRelationEdgeLookupHint());
    QCOMPARE(sceneResolvedEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QVERIFY(!facade.selectedRelationEdgeSummary(&scene).isValid());
    QVERIFY(!scene.selectedRelationEdgeSummary().isValid());
    QVERIFY(facade.selectedRelationEdgeMeta(&scene).edgeStableId.isEmpty());
    QVERIFY(scene.selectedRelationEdgeMeta().edgeStableId.isEmpty());
    scene.setSelections({relationLine});
    const CimRelationEdgeSummary facadeSelectedRelationSummary =
        facade.selectedRelationEdgeSummary(&scene);
    const CimRelationEdgeSummary sceneSelectedRelationSummary =
        scene.selectedRelationEdgeSummary();
    QVERIFY(facadeSelectedRelationSummary.isValid());
    QVERIFY(sceneSelectedRelationSummary.isValid());
    QCOMPARE(facadeSelectedRelationSummary.edgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(facadeSelectedRelationSummary.relationLink, relationSummary.relationLink);
    QCOMPARE(sceneSelectedRelationSummary.edgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(sceneSelectedRelationSummary.relationLink, relationSummary.relationLink);
    const TopologyEdgeMeta facadeSelectedEdgeMeta = facade.selectedRelationEdgeMeta(&scene);
    const TopologyEdgeMeta sceneSelectedEdgeMeta = scene.selectedRelationEdgeMeta();
    QCOMPARE(facadeSelectedEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(facadeSelectedEdgeMeta.startNodeStableId, edgeMeta.startNodeStableId);
    QCOMPARE(sceneSelectedEdgeMeta.edgeStableId, edgeMeta.edgeStableId);
    QCOMPARE(sceneSelectedEdgeMeta.endNodeStableId, edgeMeta.endNodeStableId);
}

void TestCimdrawTopology::cim_query_facade_builds_topology_object_summary()
{
    CimQueryFacade facade;
    CimdrawScene scene;
    CimIdIndex idIndex;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("summary-grid-node-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("summary-breaker-node-001"));
    grid->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("summary-grid-mrid-001"));
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("summary-breaker-mrid-001"));
    grid->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:summary-grid-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:summary-breaker-001"));

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("summary-grid-mrid-001"), grid);
    shapeByMrid.insert(QStringLiteral("summary-breaker-mrid-001"), breaker);
    idIndex.bindGraphicItem(QStringLiteral("summary-grid-mrid-001"),
                            QStringLiteral("gfx:summary-grid-001"));
    idIndex.bindGraphicItem(QStringLiteral("summary-breaker-mrid-001"),
                            QStringLiteral("gfx:summary-breaker-001"));

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    const CimTopologyObjectSummary gridSummary =
        facade.topologyObjectSummaryForSceneItem(grid);
    const CimTopologyObjectSummary sceneGridSummary =
        scene.topologyObjectSummaryForSceneItem(grid);
    QCOMPARE(gridSummary.nodeId, QStringLiteral("summary-grid-node-001"));
    QCOMPARE(gridSummary.relationInfo.nodeStableId, QStringLiteral("summary-grid-node-001"));
    QCOMPARE(gridSummary.relationInfo.incidentEdgeCount(), 1);
    QCOMPARE(gridSummary.conductorIds.size(), 1);
    QVERIFY(gridSummary.connectedDevices.contains(QStringLiteral("summary-breaker-node-001")));
    QVERIFY(gridSummary.reachableDevices.contains(QStringLiteral("summary-breaker-node-001")));
    QVERIFY(!gridSummary.deviceNodeIds.isEmpty());
    QCOMPARE(sceneGridSummary.nodeId, gridSummary.nodeId);
    QCOMPARE(sceneGridSummary.relationInfo.nodeStableId, gridSummary.relationInfo.nodeStableId);
    QCOMPARE(sceneGridSummary.relationInfo.incidentEdgeCount(), gridSummary.relationInfo.incidentEdgeCount());
    QCOMPARE(sceneGridSummary.conductorIds, gridSummary.conductorIds);
    QCOMPARE(sceneGridSummary.connectedDevices, gridSummary.connectedDevices);
    QCOMPARE(sceneGridSummary.reachableDevices, gridSummary.reachableDevices);

    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};
    const CimTopologyObjectSummary breakerSummary =
        facade.topologyObjectSummaryForMrid(QStringLiteral("summary-breaker-mrid-001"),
                                           &scene,
                                           queryState);
    QVERIFY(!facade.selectedTopologyObjectSummary(&scene, queryState).isValid());
    const CimTopologyObjectSummary sceneBreakerSummary =
        scene.topologyObjectSummaryForMrid(QStringLiteral("summary-breaker-mrid-001"),
                                          queryState);
    QCOMPARE(breakerSummary.nodeId, QStringLiteral("summary-breaker-node-001"));
    QCOMPARE(breakerSummary.relationInfo.nodeStableId, QStringLiteral("summary-breaker-node-001"));
    QCOMPARE(breakerSummary.relationInfo.incidentEdgeCount(), 1);
    QCOMPARE(breakerSummary.conductorIds.size(), 1);
    QVERIFY(breakerSummary.connectedDevices.contains(QStringLiteral("summary-grid-node-001")));
    QVERIFY(breakerSummary.reachableDevices.contains(QStringLiteral("summary-grid-node-001")));
    QVERIFY(!breakerSummary.deviceNodeIds.isEmpty());
    QCOMPARE(sceneBreakerSummary.nodeId, breakerSummary.nodeId);
    QCOMPARE(sceneBreakerSummary.relationInfo.nodeStableId, breakerSummary.relationInfo.nodeStableId);
    QCOMPARE(sceneBreakerSummary.conductorIds, breakerSummary.conductorIds);
    QCOMPARE(sceneBreakerSummary.connectedDevices, breakerSummary.connectedDevices);
    QCOMPARE(sceneBreakerSummary.reachableDevices, breakerSummary.reachableDevices);

    QVERIFY(!scene.selectedTopologyObjectSummary().isValid());
    scene.setSelections({breaker});
    const CimTopologyObjectSummary facadeSelectedBreakerSummary =
        facade.selectedTopologyObjectSummary(&scene, queryState);
    const CimTopologyObjectSummary selectedBreakerSummary = scene.selectedTopologyObjectSummary(queryState);
    QCOMPARE(facadeSelectedBreakerSummary.nodeId, breakerSummary.nodeId);
    QCOMPARE(facadeSelectedBreakerSummary.relationInfo.nodeStableId, breakerSummary.relationInfo.nodeStableId);
    QCOMPARE(facadeSelectedBreakerSummary.conductorIds, breakerSummary.conductorIds);
    QCOMPARE(facadeSelectedBreakerSummary.connectedDevices, breakerSummary.connectedDevices);
    QCOMPARE(selectedBreakerSummary.nodeId, breakerSummary.nodeId);
    QCOMPARE(selectedBreakerSummary.relationInfo.nodeStableId, breakerSummary.relationInfo.nodeStableId);
    QCOMPARE(selectedBreakerSummary.conductorIds, breakerSummary.conductorIds);
    QCOMPARE(selectedBreakerSummary.connectedDevices, breakerSummary.connectedDevices);
}

void TestCimdrawTopology::cim_query_facade_builds_topology_highlight_summary()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(busbar);
    scene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("highlight-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("highlight-load-001"));

    CimdrawConnectPoint* busbarRight = findConnectPort(busbar, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(busbarRight != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, busbar, busbarRight, load, loadLeft);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }
    QVERIFY(relationLine != nullptr);

    const CimTopologyHighlightSummary loadHighlightSummary =
        facade.topologyHighlightSummaryForSceneItem(load);
    QVERIFY(loadHighlightSummary.canHighlight);
    QVERIFY(loadHighlightSummary.isValid());
    QVERIFY(loadHighlightSummary.nodeIds.contains(QStringLiteral("highlight-load-001")));
    QVERIFY(loadHighlightSummary.nodeIds.contains(QStringLiteral("highlight-busbar-001")));
    QCOMPARE(loadHighlightSummary.edgeIds.size(), 1);

    const CimTopologyHighlightSummary lineHighlightSummary =
        facade.topologyHighlightSummaryForSceneItem(relationLine);
    QVERIFY(lineHighlightSummary.canHighlight);
    QVERIFY(lineHighlightSummary.isValid());
    QCOMPARE(lineHighlightSummary.edgeIds.size(), 1);
    QVERIFY(lineHighlightSummary.nodeIds.contains(QStringLiteral("highlight-busbar-001")));
    QVERIFY(lineHighlightSummary.nodeIds.contains(QStringLiteral("highlight-load-001")));

    const CimTopologyHighlightSummary sceneLoadHighlightSummary =
        scene.topologyHighlightSummaryForSceneItem(load);
    QCOMPARE(sceneLoadHighlightSummary.canHighlight, loadHighlightSummary.canHighlight);
    QCOMPARE(sceneLoadHighlightSummary.nodeIds, loadHighlightSummary.nodeIds);
    QCOMPARE(sceneLoadHighlightSummary.edgeIds, loadHighlightSummary.edgeIds);

    scene.setSelections({load});
    const CimTopologyHighlightSummary selectedHighlightSummary =
        scene.selectedTopologyHighlightSummary();
    QCOMPARE(selectedHighlightSummary.canHighlight, loadHighlightSummary.canHighlight);
    QCOMPARE(selectedHighlightSummary.nodeIds, loadHighlightSummary.nodeIds);
    QCOMPARE(selectedHighlightSummary.edgeIds, loadHighlightSummary.edgeIds);
}

void TestCimdrawTopology::cim_query_facade_builds_topology_summary_lines()
{
    CimQueryFacade facade;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(grid);
    scene.addItem(breaker);

    grid->setTopologyNodeStableId(QStringLiteral("summary-lines-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("summary-lines-breaker-001"));
    grid->setDisplayName(QStringLiteral("示例电源"));
    breaker->setDisplayName(QStringLiteral("示例断路器"));
    breaker->setWiringRunState(CimdrawWiringRunState::Alarm);
    breaker->setSwitchPosition(0);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);

    CimdrawConnectLine* relationLine = nullptr;
    for (QGraphicsItem* item : scene.items())
    {
        relationLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (relationLine)
            break;
    }
    QVERIFY(relationLine != nullptr);

    const QStringList breakerLines = facade.topologySummaryLinesForSceneItem(breaker);
    QVERIFY(breakerLines.contains(QStringLiteral("对象名称：示例断路器")));
    QVERIFY(breakerLines.contains(QStringLiteral("显示运行态：告警")));
    QVERIFY(breakerLines.contains(QStringLiteral("显示开关位置：分")));

    const QStringList relationLines = facade.topologySummaryLinesForSceneItem(relationLine);
    QVERIFY(relationLines.contains(QStringLiteral("对象类型：连接线")));
    QVERIFY(relationLines.contains(QStringLiteral("关系类型：物理")));

    const QStringList sceneBreakerLines = scene.topologySummaryLinesForSceneItem(breaker);
    QCOMPARE(sceneBreakerLines, breakerLines);

    scene.setSelections({breaker});
    const QStringList selectedBreakerLines = scene.selectedTopologySummaryLines();
    QCOMPARE(selectedBreakerLines, breakerLines);
}

void TestCimdrawTopology::scene_renders_generated_topology_graph_summary()
{
    CimdrawScene sourceScene;
    CimdrawScene targetScene;

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    sourceScene.addItem(busbar);
    sourceScene.addItem(load);

    busbar->setTopologyNodeStableId(QStringLiteral("render-busbar-001"));
    load->setTopologyNodeStableId(QStringLiteral("render-load-001"));
    busbar->setDisplayName(QStringLiteral("渲染母线"));
    load->setDisplayName(QStringLiteral("渲染负荷"));

    CimdrawConnectPoint* busbarRight = findConnectPort(busbar, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(busbarRight != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&sourceScene, busbar, busbarRight, load, loadLeft);

    const CimGeneratedTopologyGraphSummary graphSummary =
        sourceScene.generatedTopologyGraphSummary(CimdrawTopologyDomain::None);
    const CimGeneratedTopologySceneBuildResult buildResult =
        targetScene.renderGeneratedTopologyGraphSummary(graphSummary);
    QCOMPARE(buildResult.generatedNodeCount, 2);
    QCOMPARE(buildResult.generatedLineCount, 1);

    int generatedNodeCount = 0;
    int generatedLineCount = 0;
    for (QGraphicsItem* item : targetScene.items())
    {
        if (qgraphicsitem_cast<CimdrawTopologyNodeItem*>(item))
            ++generatedNodeCount;
        if (qgraphicsitem_cast<CimdrawConnectLine*>(item))
            ++generatedLineCount;
    }

    QCOMPARE(generatedNodeCount, buildResult.generatedNodeCount);
    QCOMPARE(generatedLineCount, buildResult.generatedLineCount);
}

void TestCimdrawTopology::cim_scene_builder_instantiates_p0_gap_graphics()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* lineItem = dynamic_cast<CimdrawPowerCableItem*>(result.shapeByMrid.value(QStringLiteral("line-gap")));
    auto* loadBreakItem = dynamic_cast<CimdrawPowerLoadBreakItem*>(result.shapeByMrid.value(QStringLiteral("load-break-gap")));
    auto* groundDisconnectorItem = dynamic_cast<CimdrawPowerEarthSwitchItem*>(result.shapeByMrid.value(QStringLiteral("ground-dis-gap")));
    auto* shuntItem = dynamic_cast<CimdrawPowerCapacitorItem*>(result.shapeByMrid.value(QStringLiteral("shunt-gap")));

    QVERIFY(lineItem != nullptr);
    QVERIFY(loadBreakItem != nullptr);
    QVERIFY(groundDisconnectorItem != nullptr);
    QVERIFY(shuntItem != nullptr);
    QVERIFY(result.idIndex.hasBinding(QStringLiteral("line-gap")));
    const QString lineGraphicItemId = result.idIndex.graphicItemIdForMrid(QStringLiteral("line-gap"));
    QVERIFY(!lineGraphicItemId.isEmpty());
    QCOMPARE(result.idIndex.mridForGraphicItemId(lineGraphicItemId), QStringLiteral("line-gap"));
    QCOMPARE(lineItem->data(CimSceneBuilder::CimGraphicItemIdDataKey).toString(), lineGraphicItemId);

    QCOMPARE(lineItem->drawTypeForXml(), CIMDRAW_WSYM_CABLE);
    QCOMPARE(loadBreakItem->drawTypeForXml(), CIMDRAW_WSYM_LOAD_BREAK);
    QCOMPARE(groundDisconnectorItem->drawTypeForXml(), CIMDRAW_WSYM_EARTH_SWITCH);
    QCOMPARE(shuntItem->drawTypeForXml(), CIMDRAW_WSYM_CAPACITOR);

    QCOMPARE(loadBreakItem->switchPosition(), 1);
    QCOMPARE(groundDisconnectorItem->switchPosition(), 0);
    QCOMPARE(loadBreakItem->referenceDesignator(), QStringLiteral("FZ1"));
    QCOMPARE(shuntItem->referenceDesignator(), QStringLiteral("C1"));

    int connectionLineCount = 0;
    for (QGraphicsItem* item : scene.items())
    {
        if (qgraphicsitem_cast<CimdrawConnectLine*>(item))
            ++connectionLineCount;
    }
    QVERIFY(connectionLineCount > 0);
}

void TestCimdrawTopology::cim_scene_builder_instantiates_p1_reuse_graphics()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* externalItem = dynamic_cast<CimdrawPowerGridItem*>(result.shapeByMrid.value(QStringLiteral("external-gap")));
    auto* equivalentItem = dynamic_cast<CimdrawPowerGridItem*>(result.shapeByMrid.value(QStringLiteral("equivalent-gap")));
    auto* asyncItem = dynamic_cast<CimdrawPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("async-gap")));
    auto* shuntItem = dynamic_cast<CimdrawPowerCapacitorItem*>(result.shapeByMrid.value(QStringLiteral("shunt-p1-gap")));
    auto* seriesItem = dynamic_cast<CimdrawPowerReactorItem*>(result.shapeByMrid.value(QStringLiteral("series-gap")));
    auto* arresterItem = dynamic_cast<CimdrawPowerArresterItem*>(result.shapeByMrid.value(QStringLiteral("arrester-gap")));
    auto* groundItem = dynamic_cast<CimdrawPowerGroundItem*>(result.shapeByMrid.value(QStringLiteral("ground-gap")));

    QVERIFY(externalItem != nullptr);
    QVERIFY(equivalentItem != nullptr);
    QVERIFY(asyncItem != nullptr);
    QVERIFY(shuntItem != nullptr);
    QVERIFY(seriesItem != nullptr);
    QVERIFY(arresterItem != nullptr);
    QVERIFY(groundItem != nullptr);

    QCOMPARE(externalItem->drawTypeForXml(), CIMDRAW_WSYM_GRID);
    QCOMPARE(equivalentItem->drawTypeForXml(), CIMDRAW_WSYM_GRID);
    QCOMPARE(asyncItem->drawTypeForXml(), CIMDRAW_SLD_MOTOR);
    QCOMPARE(shuntItem->drawTypeForXml(), CIMDRAW_WSYM_CAPACITOR);
    QCOMPARE(seriesItem->drawTypeForXml(), CIMDRAW_WSYM_REACTOR);
    QCOMPARE(arresterItem->drawTypeForXml(), CIMDRAW_WSYM_ARRESTER);
    QCOMPARE(groundItem->drawTypeForXml(), CIMDRAW_SLD_GROUND);

    QCOMPARE(asyncItem->referenceDesignator(), QStringLiteral("M1"));
    QCOMPARE(seriesItem->referenceDesignator(), QStringLiteral("Sr1"));
    QCOMPARE(arresterItem->referenceDesignator(), QStringLiteral("LA1"));
    QCOMPARE(groundItem->referenceDesignator(), QStringLiteral("GND1"));
}

void TestCimdrawTopology::cim_scene_builder_instantiates_missing_measurement_and_fuse_graphics()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* currentTransformer = dynamic_cast<CimdrawPowerCurrentTransformerItem*>(result.shapeByMrid.value(QStringLiteral("ct-gap")));
    auto* potentialTransformer = dynamic_cast<CimdrawPowerVoltageTransformerItem*>(result.shapeByMrid.value(QStringLiteral("vt-gap")));
    auto* voltageTransformer = dynamic_cast<CimdrawPowerVoltageTransformerItem*>(result.shapeByMrid.value(QStringLiteral("vt2-gap")));
    auto* fuse = dynamic_cast<CimdrawPowerFuseItem*>(result.shapeByMrid.value(QStringLiteral("fuse-gap")));

    QVERIFY(currentTransformer != nullptr);
    QVERIFY(potentialTransformer != nullptr);
    QVERIFY(voltageTransformer != nullptr);
    QVERIFY(fuse != nullptr);

    QCOMPARE(currentTransformer->drawTypeForXml(), CIMDRAW_WSYM_CURRENT_TRANSFORMER);
    QCOMPARE(potentialTransformer->drawTypeForXml(), CIMDRAW_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(voltageTransformer->drawTypeForXml(), CIMDRAW_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(fuse->drawTypeForXml(), CIMDRAW_WSYM_FUSE);

    QCOMPARE(currentTransformer->referenceDesignator(), QStringLiteral("CT1"));
    QCOMPARE(potentialTransformer->referenceDesignator(), QStringLiteral("PT1"));
    QCOMPARE(voltageTransformer->referenceDesignator(), QStringLiteral("VT1"));
    QCOMPARE(fuse->referenceDesignator(), QStringLiteral("FU1"));
}

void TestCimdrawTopology::cim_scene_builder_prefers_diagram_layout_coordinates_when_present()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* busbarItem =
        dynamic_cast<CimdrawPowerBusbarSectionItem*>(result.shapeByMrid.value(QStringLiteral("busbar-dl")));
    auto* motorItem =
        dynamic_cast<CimdrawPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-dl")));

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

void TestCimdrawTopology::cim_scene_builder_resolves_terminal_diagram_objects_to_equipment()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* motorItem =
        dynamic_cast<CimdrawPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-terminal-diagram")));

    QVERIFY(motorItem != nullptr);
    QCOMPARE(qRound(motorItem->scenePos().x()), 410);
    QCOMPARE(qRound(motorItem->scenePos().y()), 520);
    QCOMPARE(qRound(motorItem->rotation()), 180);
}

void TestCimdrawTopology::cim_scene_builder_prefers_direct_equipment_diagram_objects_over_terminal_geometry()
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

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult result = builder.populateSceneFromCimModel(&scene, model);

    auto* motorItem =
        dynamic_cast<CimdrawPowerMotorItem*>(result.shapeByMrid.value(QStringLiteral("motor-direct-preferred")));

    QVERIFY(motorItem != nullptr);
    QCOMPARE(qRound(motorItem->scenePos().x()), 360);
    QCOMPARE(qRound(motorItem->scenePos().y()), 440);
    QCOMPARE(qRound(motorItem->rotation()), 0);
}

void TestCimdrawTopology::frame_cim_slot_workbench_controller_routes_import_entrypoints()
{
    FrameCimSlotWorkbenchController controller;
    const QString defaultPath = controller.defaultImportPath();

    QVERIFY(defaultPath.contains(QStringLiteral("tests/data/cim/minimal")));
    QVERIFY(defaultPath.endsWith(QStringLiteral("EQ_SSH_sample")));

    FrameCimWorkbenchController::ImportState state;
    CimdrawScene scene;
    CimdrawView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    CimModel seedModel;
    QHash<QString, QGraphicsItem*> seedShapes;
    CimIdIndex seedIndex;
    CimSceneBuilder::BuildResult seedBuildResult;
    CimObject seedObject;
    seedObject.mrid = QStringLiteral("breaker-slot-activate");
    seedObject.className = QStringLiteral("Breaker");
    seedObject.name = QStringLiteral("Slot Breaker");
    QVERIFY(seedModel.addObject(seedObject));

    auto* shape = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    shape->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("breaker-slot-activate"));
    shape->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:breaker-slot-activate"));
    seedShapes.insert(QStringLiteral("breaker-slot-activate"), shape);
    seedIndex.bindGraphicItem(QStringLiteral("breaker-slot-activate"), QStringLiteral("gfx:breaker-slot-activate"));
    seedBuildResult.shapeByMrid = seedShapes;
    seedBuildResult.idIndex = seedIndex;
    seedBuildResult.generatedHelperLineCount = 7;

    state = controller.makeImportState(nullptr, seedModel, seedBuildResult);
    QCOMPARE(state.browserDock, static_cast<CimModelBrowserDock*>(nullptr));
    QCOMPARE(state.shapeByMrid.value(QStringLiteral("breaker-slot-activate")), static_cast<QGraphicsItem*>(shape));
    QCOMPARE(state.idIndex.graphicItemIdForMrid(QStringLiteral("breaker-slot-activate")),
             QStringLiteral("gfx:breaker-slot-activate"));
    QCOMPARE(state.generatedHelperLineCount, 7);
    QCOMPARE(state.graphicQueryState().generatedHelperLineCount, 7);
    QCOMPARE(state.graphicQueryState().queryContext.idIndex, &state.idIndex);
    CimQueryFacade facade;
    const CimGraphicVisualSummary sceneStaticVisualSummary =
        CimdrawScene::buildGraphicVisualSummary(seedModel, state.graphicQueryState());
    const CimGraphicVisualSummary stateVisualSummary = state.buildGraphicVisualSummary(seedModel);
    const CimGraphicVisualSummary staticVisualSummary =
        FrameCimWorkbenchController::ImportState::buildGraphicVisualSummaryForQueryState(seedModel);
    const CimGraphicVisualSummary facadeStateVisualSummary =
        facade.buildGraphicVisualSummary(seedModel, state.graphicQueryState());
    const CimGraphicVisualSummary facadeStaticVisualSummary =
        facade.buildGraphicVisualSummary(seedModel, CimGraphicQueryState{});
    QCOMPARE(stateVisualSummary.generatedHelperLineCount, 7);
    QCOMPARE(stateVisualSummary.generatedHelperLineCount, facadeStateVisualSummary.generatedHelperLineCount);
    QCOMPARE(stateVisualSummary.generatedHelperLineCount, sceneStaticVisualSummary.generatedHelperLineCount);
    QVERIFY(stateVisualSummary.objectSummariesByMrid.contains(QStringLiteral("breaker-slot-activate")));
    QCOMPARE(stateVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid,
             QStringLiteral("breaker-slot-activate"));
    QCOMPARE(staticVisualSummary.generatedHelperLineCount, 0);
    QCOMPARE(staticVisualSummary.generatedHelperLineCount, facadeStaticVisualSummary.generatedHelperLineCount);
    QVERIFY(staticVisualSummary.objectSummariesByMrid.contains(QStringLiteral("breaker-slot-activate")));
    QCOMPARE(stateVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid,
             facadeStateVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid);
    QCOMPARE(stateVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid,
             sceneStaticVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid);
    QCOMPARE(staticVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid,
             facadeStaticVisualSummary.objectSummariesByMrid.value(QStringLiteral("breaker-slot-activate")).mrid);

    controller.activateObjectByMrid(QStringLiteral("breaker-slot-activate"), &scene, state);
    QCOMPARE(scene.getSelection(), static_cast<QGraphicsItem*>(shape));

    const FrameCimWorkbenchController::ImportState emptyActivationState =
        controller.makeActivationState(CimSceneBuilder::BuildResult{});
    QVERIFY(emptyActivationState.shapeByMrid.isEmpty());

    CimSceneBuilder::BuildResult activationBuildResult = seedBuildResult;
    activationBuildResult.generatedHelperLineCount = 9;
    const FrameCimWorkbenchController::ImportState populatedActivationState =
        controller.makeActivationState(activationBuildResult);
    QCOMPARE(populatedActivationState.browserDock, static_cast<CimModelBrowserDock*>(nullptr));
    QCOMPARE(populatedActivationState.shapeByMrid.value(QStringLiteral("breaker-slot-activate")),
             static_cast<QGraphicsItem*>(shape));
    QCOMPARE(populatedActivationState.idIndex.mridForGraphicItemId(QStringLiteral("gfx:breaker-slot-activate")),
             QStringLiteral("breaker-slot-activate"));
    QCOMPARE(populatedActivationState.generatedHelperLineCount, 9);
    QCOMPARE(state.lastImportedModel.objectCount(), seedModel.objectCount());

    FrameCimWorkbenchController::ImportContext context;
    QVERIFY(!controller.importFromPath(QString(), context, &state));
    QVERIFY(!controller.importFromPath(QStringLiteral("   "), context, &state));
    QVERIFY(!controller.importFromPath(QStringLiteral("x"), context, nullptr));
}

void TestCimdrawTopology::frame_scene_lifecycle_coordinator_builds_cim_import_context()
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

    QCOMPARE(context.createTargetView(), static_cast<CimdrawView*>(nullptr));
    context.onBrowserDockReady(nullptr);
    context.renameCurrentTab(QStringLiteral("ignored"));
    context.afterSceneCreated();
}

void TestCimdrawTopology::frame_data_source_coordinator_initializes_manager_with_missing_config()
{
    FrameDataSourceCoordinator coordinator;
    CimdrawDataSourceManager manager;

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString missingPath = dir.filePath(QStringLiteral("missing-cimdraw-data.json"));

    bool resolverCalled = false;
    QVERIFY(coordinator.initializeManager(&manager,
                                          missingPath,
                                          [&resolverCalled]() -> CimdrawScene*
                                          {
                                              resolverCalled = true;
                                              return nullptr;
                                          }));
    QCOMPARE(manager.lastConfigPath(), missingPath);
    QVERIFY(!manager.lastError().isEmpty());
    QVERIFY(!resolverCalled);
    QVERIFY(!coordinator.initializeManager(nullptr, missingPath, {}));
}

void TestCimdrawTopology::data_source_config_loader_reports_missing_and_valid_root_objects()
{
    CimdrawDataSourceConfigLoader loader;

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString missingPath = dir.filePath(QStringLiteral("missing.json"));
    const CimdrawDataSourceConfigLoader::LoadResult missing = loader.loadRootObject(missingPath);
    QVERIFY(!missing.success);
    QCOMPARE(missing.error, QStringLiteral("config not found"));

    const QString validPath = dir.filePath(QStringLiteral("valid.json"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("{\"sources\":[],\"wiring\":{\"symbolStandard\":\"GB\"}}");
    validFile.close();

    const CimdrawDataSourceConfigLoader::LoadResult valid = loader.loadRootObject(validPath);
    QVERIFY(valid.success);
    QVERIFY(valid.error.isEmpty());
    QVERIFY(valid.root.contains(QStringLiteral("sources")));
    QVERIFY(valid.root.value(QStringLiteral("sources")).isArray());
}

void TestCimdrawTopology::data_source_factory_creates_supported_source_and_rejects_invalid_inputs()
{
    CimdrawDataSourceFactory factory;
    QObject parent;

    const QJsonObject validConfig{
        { QStringLiteral("type"), QStringLiteral("wiring_tcp_json") },
        { QStringLiteral("listenPort"), 12001 },
        { QStringLiteral("listenAddress"), QStringLiteral("127.0.0.1") }
    };

    CimdrawIDataSource* source = factory.createConfiguredSource(QStringLiteral("wiring_tcp_json"),
                                                           validConfig,
                                                           []() -> CimdrawScene* { return nullptr; },
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

void TestCimdrawTopology::data_source_builder_filters_and_builds_sources_from_root()
{
    CimdrawDataSourceBuilder builder;
    QObject parent;

    const QJsonObject invalidRoot{
        { QStringLiteral("sources"), QStringLiteral("not-array") }
    };
    const CimdrawDataSourceBuilder::BuildResult invalid = builder.buildSources(invalidRoot, {}, &parent);
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

    const CimdrawDataSourceBuilder::BuildResult valid =
        builder.buildSources(validRoot, []() -> CimdrawScene* { return nullptr; }, &parent);
    QVERIFY(valid.error.isEmpty());
    QCOMPARE(valid.sources.size(), 1);
    QCOMPARE(valid.sources.first()->typeId(), QStringLiteral("wiring_tcp_json"));
    qDeleteAll(valid.sources);
}

void TestCimdrawTopology::data_source_runtime_controller_starts_stops_and_clears_sources()
{
    CimdrawDataSourceRuntimeController controller;
    QList<CimdrawIDataSource*> sources;

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

void TestCimdrawTopology::frame_document_workbench_controller_creates_scene_from_factory()
{
    FrameDocumentWorkbenchController controller;
    CimdrawCenterWidget centerWidget;
    bool factoryCalled = false;
    bool renamed = false;

    CimdrawView* view = controller.createPowerScene([&]()
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

void TestCimdrawTopology::frame_document_workbench_controller_applies_save_callbacks()
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

void TestCimdrawTopology::frame_document_workbench_controller_guards_missing_open_inputs()
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

void TestCimdrawTopology::frame_document_slot_workbench_controller_routes_center_widget_document_actions()
{
    FrameDocumentSlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;

    QCOMPARE(controller.currentView(nullptr), nullptr);
    QVERIFY(!controller.saveCurrentView(nullptr, {}, {}));
    QVERIFY(!controller.saveCurrentViewAs(nullptr, nullptr, {}, {}));

    bool factoryCalled = false;
    CimdrawView* createdView = controller.createPowerScene([&]()
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

void TestCimdrawTopology::frame_document_slot_workbench_controller_syncs_ui_for_missing_view_save_actions()
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

void TestCimdrawTopology::frame_editor_workbench_controller_resolves_scene_and_guards_missing_view()
{
    FrameEditorWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;
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

void TestCimdrawTopology::frame_editor_workbench_controller_routes_delete_and_shortcut_actions()
{
    FrameEditorWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;
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

void TestCimdrawTopology::frame_editor_slot_workbench_controller_routes_center_widget_actions()
{
    FrameEditorSlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;

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

    CimdrawScene* scene = new CimdrawScene();
    CimdrawView* view = new CimdrawView();
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

void TestCimdrawTopology::frame_window_state_slot_workbench_controller_routes_center_widget_state_actions()
{
    FrameWindowStateSlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;
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

    CimdrawScene* scene = new CimdrawScene();
    CimdrawView* view = new CimdrawView();
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

void TestCimdrawTopology::frame_topology_slot_workbench_controller_routes_center_widget_topology_actions()
{
    FrameTopologySlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;

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
                                              [&factoryCalled](CimdrawScene*)
                                              {
                                                  factoryCalled = true;
                                                  return FrameTopologyController::GenerateContext{};
                                              },
                                              FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);
    QVERIFY(!controller.generateGraph(nullptr,
                                      nullptr,
                                      [&factoryCalled](CimdrawScene*)
                                      {
                                          factoryCalled = true;
                                          return FrameTopologyController::GenerateContext{};
                                      },
                                      FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);

    CimdrawScene* scene = new CimdrawScene();
    CimdrawView* view = new CimdrawView();
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

void TestCimdrawTopology::frame_tool_selection_slot_workbench_controller_resolves_scene_and_guards_inputs()
{
    FrameToolSelectionSlotWorkbenchController controller;
    CimdrawCenterWidget centerWidget;

    QCOMPARE(controller.currentScene(nullptr), nullptr);
    QVERIFY(!controller.activateDrawTool(nullptr, nullptr));

    CimdrawScene* scene = new CimdrawScene();
    CimdrawView* view = new CimdrawView();
    view->setPowerScene(scene);
    scene->setView(view);
    centerWidget.newTab(view);

    QCOMPARE(controller.currentScene(&centerWidget), scene);
    QVERIFY(!controller.activateDrawTool(nullptr, &centerWidget));
}

void TestCimdrawTopology::scene_edit_workbench_controller_resolves_stack_from_scene()
{
    CimdrawSceneEditWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;

    QCOMPARE(controller.stackFromScene(nullptr), nullptr);
    QCOMPARE(controller.stackFromScene(&scene), nullptr);

    view.setPowerScene(&scene);
    scene.setView(&view);
    QCOMPARE(controller.stackFromScene(&scene), view.getStack());
}

void TestCimdrawTopology::scene_edit_workbench_controller_routes_shortcut_and_delete()
{
    CimdrawSceneEditWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;
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

void TestCimdrawTopology::scene_edit_slot_workbench_controller_routes_scene_actions()
{
    CimdrawSceneEditSlotWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;

    QVERIFY(!controller.copy(nullptr));
    QVERIFY(!controller.shortcutRight(nullptr, QPointF(1, 0)));
    QVERIFY(!controller.dispatch(nullptr, CimdrawSceneEditSlotWorkbenchController::Action::Copy));
    QVERIFY(!controller.dispatchShortcut(nullptr,
                                         CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Right,
                                         QPointF(1, 0)));

    view.setPowerScene(&scene);
    scene.setView(&view);

    auto* item = scene.addRect(QRectF(0, 0, 20, 20));
    item->setSelected(true);
    scene.setSelections({item});

    QVERIFY(controller.dispatchShortcut(&scene,
                                        CimdrawSceneEditSlotWorkbenchController::ShortcutDirection::Right,
                                        QPointF(7, 0)));
    QVERIFY(controller.shortcutRight(&scene, QPointF(7, 0)));
    QCOMPARE(item->pos(), QPointF(14, 0));

    scene.setSelections({item});
    QVERIFY(controller.dispatch(&scene, CimdrawSceneEditSlotWorkbenchController::Action::Copy));
    QVERIFY(controller.dispatch(&scene, CimdrawSceneEditSlotWorkbenchController::Action::Remove));
    QVERIFY(controller.remove(&scene));
    QVERIFY(item->scene() == nullptr);
    delete item;
}

void TestCimdrawTopology::scene_selection_workbench_controller_replaces_and_clears_selection()
{
    CimdrawSceneSelectionWorkbenchController controller;
    CimdrawScene scene;

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

void TestCimdrawTopology::scene_selection_workbench_controller_routes_lifecycle_operations()
{
    CimdrawSceneSelectionWorkbenchController controller;
    CimdrawScene scene;
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

void TestCimdrawTopology::scene_selection_slot_workbench_controller_routes_scene_selection_actions()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    CimdrawScene scene;

    QVERIFY(controller.firstSelection(nullptr) == nullptr);
    QVERIFY(controller.selections(nullptr).isEmpty());
    QVERIFY(controller.dispatch(nullptr,
                                CimdrawSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection)
            == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                CimdrawSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections)
                .isEmpty());
    QVERIFY(!controller.replaceSelection(nullptr, {}));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneSelectionSlotWorkbenchController::SelectionAction::Clear));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::AddSelection,
                                 nullptr));
    QCOMPARE(controller.dispatch(nullptr,
                                 CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                                 {}),
             0);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    auto* detached = new QGraphicsRectItem(QRectF(60, 0, 20, 20));

    QCOMPARE(controller.dispatch(&scene,
                                 CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::ReplaceSelection,
                                 {item1, item2}),
             1);
    QVERIFY(controller.replaceSelection(&scene, {item1, item2}));
    QCOMPARE(controller.firstSelection(&scene), static_cast<QGraphicsItem*>(item1));
    QCOMPARE(controller.selections(&scene).size(), 2);

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSelection,
                                item1));
    QVERIFY(controller.removeSelection(&scene, item1));
    QCOMPARE(controller.selections(&scene).size(), 1);

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::InsertSelection,
                                detached));
    QVERIFY(controller.insertSelection(&scene, detached));
    QVERIFY(controller.selections(&scene).contains(detached));
    QVERIFY(detached->scene() == &scene);

    QCOMPARE(controller.dispatch(&scene,
                                 CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::RemoveSelections,
                                 {item2, detached}),
             2);
    QCOMPARE(controller.removeSelections(&scene, {item2, detached}), 2);
    QVERIFY(controller.selections(&scene).isEmpty());

    QCOMPARE(controller.dispatch(&scene,
                                 CimdrawSceneSelectionSlotWorkbenchController::ItemsSelectionAction::AddSelections,
                                 {item1, item2}),
             2);
    QVERIFY(controller.addSelections(&scene, {item1, item2}));
    QCOMPARE(controller.selections(&scene).size(), 2);

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::RemoveSceneItem,
                                item2));
    QVERIFY(controller.removeSceneItem(&scene, item2));
    QVERIFY(item2->scene() == nullptr);
    QVERIFY(!controller.selections(&scene).contains(item2));
    delete item2;

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneSelectionSlotWorkbenchController::ItemSelectionAction::DeleteSelection,
                                detached));
    QVERIFY(controller.deleteSelection(&scene, detached));
    QVERIFY(detached->scene() == nullptr);
    delete detached;

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneSelectionSlotWorkbenchController::SelectionAction::Clear));
    QVERIFY(controller.selections(&scene).isEmpty());
}

void TestCimdrawTopology::scene_selection_slot_workbench_controller_routes_scene_selection_queries()
{
    CimdrawSceneSelectionSlotWorkbenchController controller;
    CimdrawScene scene;

    QVERIFY(controller.dispatch(nullptr,
                                CimdrawSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection)
            == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                CimdrawSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections)
                .isEmpty());

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    scene.setSelections({item1, item2});

    QCOMPARE(controller.dispatch(&scene,
                                 CimdrawSceneSelectionSlotWorkbenchController::QueryItemAction::FirstSelection),
             static_cast<QGraphicsItem*>(item1));
    const QList<QGraphicsItem*> selections = controller.dispatch(
        &scene,
        CimdrawSceneSelectionSlotWorkbenchController::QueryItemsAction::Selections);
    QCOMPARE(selections.size(), 2);
    QVERIFY(selections.contains(item1));
    QVERIFY(selections.contains(item2));
}

void TestCimdrawTopology::scene_state_slot_workbench_controller_routes_scene_state_actions()
{
    CimdrawSceneStateSlotWorkbenchController controller;

    QCOMPARE(controller.view(static_cast<CimdrawScene*>(nullptr)), nullptr);
    QCOMPARE(controller.view(static_cast<const CimdrawScene*>(nullptr)), nullptr);
    QCOMPARE(controller.dispatch(static_cast<CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QCOMPARE(controller.dispatch(static_cast<const CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QVERIFY(!controller.setView(nullptr, nullptr));
    QVERIFY(!controller.flagState(nullptr, CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                                 true));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::CancelConnectLinePathRecompute,
                                 nullptr));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
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
        static_cast<const CimdrawScene*>(nullptr),
        CimdrawSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(!controller.requestDeferredItemPropertyPanel(nullptr));
    QVERIFY(!controller.flushDeferredItemPropertyPanel(nullptr));
    QVERIFY(!controller.scheduleConnectLinesPathRecompute(nullptr, {}));
    QVERIFY(!controller.scheduleConnectorPostprocess(nullptr, {}));
    QVERIFY(!controller.setWiringData(nullptr, QStringLiteral("k"), 1));
    QVERIFY(!controller.wiringData(nullptr, QStringLiteral("k")).isValid());
    QVERIFY(!controller.dispatch(static_cast<const CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("k"))
                 .isValid());

    CimdrawScene scene;
    CimdrawView view;
    QVERIFY(controller.setView(&scene, &view));
    QCOMPARE(controller.view(&scene), &view);
    QCOMPARE(controller.dispatch(&scene, CimdrawSceneStateSlotWorkbenchController::ViewAction::Get), &view);

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState,
                                true));
    QVERIFY(controller.flagState(&scene, CimdrawSceneStateSlotWorkbenchController::FlagStateAction::PaintState));
    QVERIFY(controller.setPaintState(&scene, true));
    QVERIFY(controller.paintState(&scene));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive,
                                true));
    QVERIFY(controller.flagState(&scene,
                                 CimdrawSceneStateSlotWorkbenchController::FlagStateAction::InteractiveTransformActive));
    QVERIFY(controller.setInteractiveTransformActive(&scene, true));
    QVERIFY(controller.interactiveTransformActive(&scene));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::InvalidateTopologySnapshots));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::ScheduleTopologyRebuild));
    QVERIFY(controller.invalidateTopologySnapshots(&scene));
    QVERIFY(controller.scheduleTopologyRebuild(&scene));
    QVERIFY(!controller.isConnectLinePathUpdatePending(&scene));
    QVERIFY(!controller.dispatch(
        static_cast<const CimdrawScene*>(&scene),
        CimdrawSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::RequestDeferredItemPropertyPanel));
    QVERIFY(controller.requestDeferredItemPropertyPanel(&scene));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredSceneAction::FlushDeferredItemPropertyPanel));
    QVERIFY(controller.flushDeferredItemPropertyPanel(&scene));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectLinesPathRecompute,
                                {}));
    QVERIFY(controller.scheduleConnectLinesPathRecompute(&scene, {}));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneStateSlotWorkbenchController::DeferredLineAction::ScheduleConnectorPostprocess,
                                {}));
    QVERIFY(controller.scheduleConnectorPostprocess(&scene, {}));
    QVERIFY(controller.dispatchWiringData(&scene, QStringLiteral("dispatch-slot-k"), 84));
    QCOMPARE(controller.wiringData(&scene, QStringLiteral("dispatch-slot-k")).toInt(), 84);
    QCOMPARE(controller.dispatch(static_cast<const CimdrawScene*>(&scene),
                                 CimdrawSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("dispatch-slot-k"))
                 .toInt(),
             84);
    QVERIFY(controller.setWiringData(&scene, QStringLiteral("slot-k"), 42));
    QCOMPARE(controller.wiringData(&scene, QStringLiteral("slot-k")).toInt(), 42);
}

void TestCimdrawTopology::scene_state_slot_workbench_controller_routes_scene_state_queries()
{
    CimdrawSceneStateSlotWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;

    QCOMPARE(controller.dispatch(static_cast<CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QCOMPARE(controller.dispatch(static_cast<const CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::ViewAction::Get),
             nullptr);
    QVERIFY(!controller.dispatch(
        static_cast<const CimdrawScene*>(nullptr),
        CimdrawSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));
    QVERIFY(!controller.dispatch(static_cast<const CimdrawScene*>(nullptr),
                                 CimdrawSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("missing"))
                 .isValid());

    QVERIFY(controller.setView(&scene, &view));
    QCOMPARE(controller.dispatch(&scene, CimdrawSceneStateSlotWorkbenchController::ViewAction::Get), &view);
    QCOMPARE(controller.dispatch(static_cast<const CimdrawScene*>(&scene),
                                 CimdrawSceneStateSlotWorkbenchController::ViewAction::Get),
             &view);
    QVERIFY(!controller.dispatch(
        static_cast<const CimdrawScene*>(&scene),
        CimdrawSceneStateSlotWorkbenchController::DeferredQueryAction::IsConnectLinePathUpdatePending));

    QVERIFY(controller.dispatchWiringData(&scene, QStringLiteral("query-slot-k"), 108));
    QCOMPARE(controller.dispatch(static_cast<const CimdrawScene*>(&scene),
                                 CimdrawSceneStateSlotWorkbenchController::WiringDataAction::Get,
                                 QStringLiteral("query-slot-k"))
                 .toInt(),
             108);
}

void TestCimdrawTopology::scene_topology_slot_workbench_controller_routes_scene_topology_actions()
{
    CimdrawSceneTopologySlotWorkbenchController controller;

    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneTopologySlotWorkbenchController::SceneAction::ClearTopologyHighlights));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Node,
                                 QStringLiteral("n"),
                                 true,
                                 QColor(Qt::red)));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                                 QStringList{QStringLiteral("e")},
                                 true,
                                 QColor(Qt::green)));
    QCOMPARE(controller.dispatchSnapshot(
                 nullptr,
                 CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot)
                 .devices.size(),
             0);
    QCOMPARE(controller.dispatchDocumentExport(
                 nullptr,
                 CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport)
                 .devices.size(),
             0);
    QVERIFY(!controller.clearTopologyHighlights(nullptr));
    QVERIFY(!controller.highlightTopologyNode(nullptr, QStringLiteral("n"), true, QColor(Qt::red)));
    QVERIFY(controller.powerDeviceNodeIds(nullptr, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(nullptr, QStringLiteral("missing")).isEmpty());

    CimdrawScene scene;
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex));
    controller.rebuildTopologyIndex(&scene);

    const PowerTopologyAnalysisSnapshot snapshot = controller.buildPowerTopologySnapshot(&scene);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);
    const PowerTopologyAnalysisSnapshot dispatchedSnapshot =
        controller.dispatchSnapshot(
            &scene,
            CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot);
    QCOMPARE(dispatchedSnapshot.devices.size(), 0);
    QCOMPARE(dispatchedSnapshot.conductors.size(), 0);
    QCOMPARE(dispatchedSnapshot.nodes.size(), 0);
    const PowerTopologyDocumentExport dispatchedDocument =
        controller.dispatchDocumentExport(
            &scene,
            CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport);
    QCOMPARE(dispatchedDocument.devices.size(), 0);
    QCOMPARE(dispatchedDocument.conductors.size(), 0);
    QCOMPARE(dispatchedDocument.nodes.size(), 0);
    QCOMPARE(dispatchedDocument.viewScale, 1.0);

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Node,
                                QStringLiteral("n"),
                                true,
                                QColor(Qt::red)));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Edge,
                                QStringLiteral("e"),
                                true,
                                QColor(Qt::yellow)));
    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneTopologySlotWorkbenchController::HighlightAction::Edges,
                                QStringList{},
                                true,
                                QColor(Qt::blue)));
    QVERIFY(controller.dispatchDeviceNodeIds(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceNodeIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchIslandAnalysis(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands)
                .islands.isEmpty());
    QVERIFY(controller.dispatchBranchAnalysis(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerBranches,
                QStringLiteral("missing"))
                .branches.isEmpty());
    QVERIFY(controller.dispatchLoopAnalysis(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerLoops)
                .loops.isEmpty());
    QVERIFY(!controller.dispatchPath(
                 nullptr,
                 CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchGroundPath(
                 nullptr,
                 CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestPowerGroundPath,
                 QStringLiteral("a"))
                 .found);
    QVERIFY(controller.dispatchSwitchChangePreview(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
                QStringLiteral("sw"),
                1)
                .deviceId.isEmpty());
    QVERIFY(controller.dispatchProtectionRangePreview(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
                QStringLiteral("sw"),
                1,
                QStringLiteral("src"))
                .protectiveDeviceId.isEmpty());
    QVERIFY(controller.dispatchOperationPreview(
                nullptr,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
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
                CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceNodeIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::PowerDeviceConductorIds,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchDeviceQuery(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::DeviceQueryAction::ConnectedPowerDevices,
                QStringLiteral("missing"))
                .isEmpty());
    QVERIFY(controller.dispatchIslandAnalysis(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerIslands)
                .islands.isEmpty());
    QVERIFY(controller.dispatchBranchAnalysis(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerBranches,
                QStringLiteral("missing"))
                .branches.isEmpty());
    QVERIFY(controller.dispatchLoopAnalysis(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::AnalysisAction::AnalyzePowerLoops)
                .loops.isEmpty());
    QVERIFY(!controller.dispatchPath(
                 &scene,
                 CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchPath(
                 &scene,
                 CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestDirectedPowerSupplyPath,
                 QStringLiteral("a"),
                 QStringLiteral("b"))
                 .found);
    QVERIFY(!controller.dispatchGroundPath(
                 &scene,
                 CimdrawSceneTopologySlotWorkbenchController::PathAction::ShortestPowerGroundPath,
                 QStringLiteral("a"))
                 .found);
    QVERIFY(controller.dispatchSwitchChangePreview(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
                QStringLiteral("sw"),
                1)
                .deviceId.isEmpty());
    QVERIFY(controller.dispatchProtectionRangePreview(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
                QStringLiteral("sw"),
                1,
                QStringLiteral("src"))
                .protectiveDeviceId.isEmpty());
    QVERIFY(controller.dispatchOperationPreview(
                &scene,
                CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
                QStringLiteral("sw"),
                1,
                QStringLiteral("s1"),
                QStringLiteral("s2"),
                QStringLiteral("g1"))
                .switchChange.deviceId.isEmpty());
}

void TestCimdrawTopology::scene_topology_slot_workbench_controller_routes_scene_topology_index_and_snapshot_dispatch()
{
    CimdrawSceneTopologySlotWorkbenchController controller;
    CimdrawScene scene;

    QVERIFY(controller.dispatch(&scene,
                                CimdrawSceneTopologySlotWorkbenchController::SceneAction::RebuildTopologyIndex));

    const PowerTopologyAnalysisSnapshot snapshot =
        controller.dispatchSnapshot(
            &scene,
            CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologySnapshot);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);

    const PowerTopologyDocumentExport document =
        controller.dispatchDocumentExport(
            &scene,
            CimdrawSceneTopologySlotWorkbenchController::SnapshotAction::BuildPowerTopologyDocumentExport);
    QCOMPARE(document.devices.size(), 0);
    QCOMPARE(document.conductors.size(), 0);
    QCOMPARE(document.nodes.size(), 0);
    QCOMPARE(document.viewScale, 1.0);
}

void TestCimdrawTopology::scene_slot_workbench_controllers_support_scene_view_and_dom_entrypoints()
{
    CimdrawScene scene;
    CimdrawView view;

    scene.setView(&view);
    QCOMPARE(scene.getView(), &view);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene-slot.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><CimdrawFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(scene.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("CimdrawFile"));
}

void TestCimdrawTopology::scene_document_slot_workbench_controller_routes_scene_document_actions()
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    QString error;
    QDomDocument dom;

    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::Load,
                                 QStringLiteral("missing.xml"),
                                 &error));
    QVERIFY(!controller.dispatch(nullptr,
                                 CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::Save,
                                 QStringLiteral("missing.xml")));
    QVERIFY(!controller.dispatch(CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
                                 QStringLiteral("Z:/definitely-missing-file.xml"),
                                 &dom,
                                 &error));
    QVERIFY(!controller.load(nullptr, QStringLiteral("missing.xml"), &error));
    QVERIFY(!controller.save(nullptr, QStringLiteral("missing.xml")));
    QVERIFY(!controller.toDomDocument(QStringLiteral("Z:/definitely-missing-file.xml"), &dom, &error));
    QVERIFY(controller.createGroup(nullptr, {}) == nullptr);
    QVERIFY(controller.dispatch(nullptr,
                                CimdrawSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
                                {})
            == nullptr);
    QCOMPARE(controller.sceneType(nullptr), FILE_TYPE::CIMDRAW_DRAW);
    QCOMPARE(controller.dispatchSceneType(nullptr,
                                         CimdrawSceneDocumentSlotWorkbenchController::SceneTypeAction::Get),
             FILE_TYPE::CIMDRAW_DRAW);
    QVERIFY(!controller.setSceneType(nullptr, FILE_TYPE::CIMDRAW_DYNAMIC));

    CimdrawScene scene;
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::CIMDRAW_DRAW);
    QCOMPARE(controller.dispatchSceneType(&scene,
                                          CimdrawSceneDocumentSlotWorkbenchController::SceneTypeAction::Get),
             FILE_TYPE::CIMDRAW_DRAW);
    QVERIFY(controller.setSceneType(&scene, FILE_TYPE::CIMDRAW_DYNAMIC));
    QCOMPARE(controller.dispatchSceneType(&scene,
                                          CimdrawSceneDocumentSlotWorkbenchController::SceneTypeAction::Set,
                                          FILE_TYPE::CIMDRAW_DYNAMIC),
             FILE_TYPE::CIMDRAW_DYNAMIC);
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::CIMDRAW_DYNAMIC);
    QVERIFY(controller.setSceneType(&scene, FILE_TYPE::CIMDRAW_DYNAMIC));
    QCOMPARE(controller.sceneType(&scene), FILE_TYPE::CIMDRAW_DYNAMIC);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.dispatch(
        &scene,
        CimdrawSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestCimdrawTopology::scene_document_slot_workbench_controller_routes_document_and_group_dispatch()
{
    CimdrawSceneDocumentSlotWorkbenchController controller;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene-document-slot.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><CimdrawFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(controller.dispatch(CimdrawSceneDocumentSlotWorkbenchController::DocumentAction::ToDomDocument,
                                validPath,
                                &dom,
                                &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("CimdrawFile"));

    CimdrawScene scene;
    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.dispatch(
        &scene,
        CimdrawSceneDocumentSlotWorkbenchController::GroupAction::CreateGroup,
        {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestCimdrawTopology::scene_topology_workbench_controller_rebuilds_index_and_reads_snapshot()
{
    CimdrawSceneTopologyWorkbenchController controller;
    CimdrawSceneTopologyStateController stateController;
    CimdrawScene scene;

    controller.rebuildTopologyIndex(&scene, stateController);
    QVERIFY(controller.relationEdgeMetas(&scene, stateController).isEmpty());

    const PowerTopologyAnalysisSnapshot snapshot =
        controller.buildPowerTopologySnapshot(&scene, stateController);
    QCOMPARE(snapshot.devices.size(), 0);
    QCOMPARE(snapshot.conductors.size(), 0);
    QCOMPARE(snapshot.nodes.size(), 0);
}

void TestCimdrawTopology::scene_topology_workbench_controller_guards_empty_queries_and_wiring_data()
{
    CimdrawSceneTopologyWorkbenchController controller;
    CimdrawScene scene;
    QHash<QString, QVariant> wiringData;

    QVERIFY(controller.powerDeviceNodeIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.analyzePowerIslands(nullptr).islands.isEmpty());
    QVERIFY(!controller.setWiringData(wiringData, QString(), 1));
    QVERIFY(controller.setWiringData(wiringData, QStringLiteral("k"), 2));
    QCOMPARE(controller.wiringData(wiringData, QStringLiteral("k")).toInt(), 2);
}

void TestCimdrawTopology::frame_topology_workbench_controller_resolves_scene_and_guards_missing_view()
{
    FrameTopologyWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;
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
                                      [&factoryCalled](CimdrawScene*)
                                      {
                                          factoryCalled = true;
                                          return FrameTopologyController::GenerateContext{};
                                      },
                                      FrameTopologyController::GeneratedMode::Combined));
    QVERIFY(!factoryCalled);
}

void TestCimdrawTopology::frame_topology_workbench_controller_clears_highlights_for_active_scene()
{
    FrameTopologyWorkbenchController controller;
    CimdrawScene scene;
    CimdrawView view;
    view.setPowerScene(&scene);
    scene.setView(&view);

    QVERIFY(controller.clearHighlights(&view));
}

void TestCimdrawTopology::frame_topology_controller_generates_relation_edges_from_relation_snapshot()
{
    FrameTopologyController controller;
    CimdrawScene sourceScene;
    CimdrawScene generatedScene;
    CimdrawView generatedView;
    generatedView.setPowerScene(&generatedScene);
    generatedScene.setView(&generatedView);

    auto* busbar = new CimdrawPowerBusbarSectionItem(QRectF(0, 0, 140, 18));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    sourceScene.addItem(busbar);
    sourceScene.addItem(load);

    busbar->setPos(0, 0);
    load->setPos(220, 0);
    busbar->setTopologyNodeStableId(QStringLiteral("busbar-gen-001"));
    load->setTopologyNodeStableId(QStringLiteral("load-gen-001"));
    busbar->setDisplayName(QStringLiteral("生成母线"));
    load->setDisplayName(QStringLiteral("生成负荷"));

    CimdrawConnectPoint* loadRight = findConnectPort(load, RIGHT_DIRECTION);
    CimdrawConnectPoint* busbarLeft = findConnectPort(busbar, LEFT_DIRECTION);
    QVERIFY(loadRight != nullptr);
    QVERIFY(busbarLeft != nullptr);
    connectPowerItems(&sourceScene, load, loadRight, busbar, busbarLeft);

    CimdrawConnectLine* sourceLine = nullptr;
    for (QGraphicsItem* item : sourceScene.items())
    {
        sourceLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (sourceLine)
            break;
    }
    QVERIFY(sourceLine != nullptr);
    sourceLine->setArrowHead(CimdrawConnectLine::ArrowEnd);

    const CimGeneratedTopologyGraphSummary expectedGraphSummary =
        sourceScene.generatedTopologyGraphSummary(CimdrawTopologyDomain::None);
    QCOMPARE(expectedGraphSummary.nodeSummaries.size(), 2);

    bool generatedCalled = false;
    controller.generateGraph(
        FrameTopologyController::GenerateContext{
            nullptr,
            &sourceScene,
            [&generatedView]() -> CimdrawView*
            {
                return &generatedView;
            },
            {},
            [&generatedCalled]()
            {
                generatedCalled = true;
            }},
        FrameTopologyController::GeneratedMode::Combined);

    QVERIFY(generatedCalled);

    int generatedNodeCount = 0;
    int generatedLineCount = 0;
    CimdrawConnectLine* generatedLine = nullptr;
    for (QGraphicsItem* item : generatedScene.items())
    {
        if (qgraphicsitem_cast<CimdrawTopologyNodeItem*>(item))
            ++generatedNodeCount;
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            ++generatedLineCount;
            generatedLine = line;
        }
    }

    QCOMPARE(generatedNodeCount, 2);
    QCOMPARE(generatedLineCount, 1);
    QVERIFY(generatedLine != nullptr);
    QCOMPARE(generatedLine->arrowHead(), CimdrawConnectLine::ArrowStart);
    auto* generatedStartNode = qgraphicsitem_cast<CimdrawTopologyNodeItem*>(generatedLine->getStartItem());
    auto* generatedEndNode = qgraphicsitem_cast<CimdrawTopologyNodeItem*>(generatedLine->getEndItem());
    QVERIFY(generatedStartNode != nullptr);
    QVERIFY(generatedEndNode != nullptr);
    QCOMPARE(generatedStartNode->cimdrawObjectId(), QStringLiteral("busbar-gen-001"));
    QCOMPARE(generatedEndNode->cimdrawObjectId(), QStringLiteral("load-gen-001"));
    QCOMPARE(generatedStartNode->pos(), expectedGraphSummary.nodeScenePos(0));
    QCOMPARE(generatedEndNode->pos(), expectedGraphSummary.nodeScenePos(1));
    QVERIFY(generatedLine->startConnectPort() != nullptr);
    QVERIFY(generatedLine->endConnectPort() != nullptr);
    QCOMPARE(generatedLine->startConnectPort()->getDirection(), LEFT_DIRECTION);
    QCOMPARE(generatedLine->endConnectPort()->getDirection(), RIGHT_DIRECTION);

    QGraphicsSimpleTextItem* busbarLabel = nullptr;
    QGraphicsSimpleTextItem* loadLabel = nullptr;
    QGraphicsSimpleTextItem* relationLabel = nullptr;
    for (QGraphicsItem* item : generatedScene.items())
    {
        auto* textItem = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (!textItem)
            continue;
        if (textItem->text() == QStringLiteral("生成母线"))
            busbarLabel = textItem;
        else if (textItem->text() == QStringLiteral("生成负荷"))
            loadLabel = textItem;
        else if (textItem->text() == QStringLiteral("物理"))
            relationLabel = textItem;
    }

    QVERIFY(busbarLabel != nullptr);
    QVERIFY(loadLabel != nullptr);
    QVERIFY(relationLabel != nullptr);
    QCOMPARE(busbarLabel->pos(),
             cimGeneratedTopologyNodeLabelPosition(generatedStartNode->pos(),
                                                   CimGeneratedTopologySourceSummary{}));
    QCOMPARE(loadLabel->pos(),
             cimGeneratedTopologyNodeLabelPosition(generatedEndNode->pos(),
                                                   CimGeneratedTopologySourceSummary{}));
    QCOMPARE(relationLabel->pos(),
             cimGeneratedTopologyRelationLabelPosition(
                 generatedLine->startConnectPort()->connectionCenterInScene(),
                 generatedLine->endConnectPort()->connectionCenterInScene(),
                 CimGeneratedTopologyRelationSummary{}));
}

void TestCimdrawTopology::cim_model_browser_dock_status_column_uses_render_state_summary()
{
    CimdrawScene scene;
    CimIdIndex idIndex;
    CimQueryFacade facade;
    CimModel model;

    CimObject breakerObject;
    breakerObject.mrid = QStringLiteral("breaker-render-001");
    breakerObject.className = QStringLiteral("Breaker");
    breakerObject.name = QStringLiteral("Render Breaker");
    QVERIFY(model.addObject(breakerObject));

    CimObject gridObject;
    gridObject.mrid = QStringLiteral("grid-render-001");
    gridObject.className = QStringLiteral("ExternalNetworkInjection");
    gridObject.name = QStringLiteral("Render Grid");
    QVERIFY(model.addObject(gridObject));

    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(breaker);
    breaker->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("breaker-render-001"));
    breaker->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:breaker-render-001"));
    breaker->setWiringRunState(CimdrawWiringRunState::Energized);
    breaker->setSwitchPosition(0);

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    scene.addItem(grid);
    grid->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("grid-render-001"));
    grid->setData(CimSceneBuilder::CimGraphicItemIdDataKey, QStringLiteral("gfx:grid-render-001"));
    grid->setFlowSign(-1);

    QHash<QString, QGraphicsItem*> shapeByMrid;
    shapeByMrid.insert(QStringLiteral("breaker-render-001"), breaker);
    shapeByMrid.insert(QStringLiteral("grid-render-001"), grid);
    idIndex.bindGraphicItem(QStringLiteral("breaker-render-001"),
                            QStringLiteral("gfx:breaker-render-001"));
    idIndex.bindGraphicItem(QStringLiteral("grid-render-001"),
                            QStringLiteral("gfx:grid-render-001"));
    const CimGraphicQueryState queryState{{&shapeByMrid, &idIndex}, 0};
    const CimGraphicVisualSummary visualSummary = facade.buildGraphicVisualSummary(model, queryState);

    CimModelBrowserDock dock;
    dock.setImportResult(model, {}, {}, {}, {}, visualSummary);

    auto* treeWidget = dock.findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    const QList<QTreeWidgetItem*> breakerItems =
        treeWidget->findItems(QStringLiteral("breaker-render-001"),
                              Qt::MatchExactly | Qt::MatchRecursive,
                              2);
    QCOMPARE(breakerItems.size(), 1);
    const QString breakerState = breakerItems.first()->text(3);
    QVERIFY(breakerState.contains(QStringLiteral("已可视化")));
    QVERIFY(breakerState.contains(QStringLiteral("显示:带电")));
    QVERIFY(breakerState.contains(QStringLiteral("开关:分")));
    QVERIFY(breakerState.contains(QStringLiteral("行为可用")));
    QVERIFY(breakerState.contains(QStringLiteral("可操作")));

    const QList<QTreeWidgetItem*> gridItems =
        treeWidget->findItems(QStringLiteral("grid-render-001"),
                              Qt::MatchExactly | Qt::MatchRecursive,
                              2);
    QCOMPARE(gridItems.size(), 1);
    const QString gridState = gridItems.first()->text(3);
    QVERIFY(gridState.contains(QStringLiteral("已可视化")));
    QVERIFY(gridState.contains(QStringLiteral("显示:正常")));
    QVERIFY(gridState.contains(QStringLiteral("流向:反向")));
    QVERIFY(!gridState.contains(QStringLiteral("开关:")));
}

void TestCimdrawTopology::frame_workbench_ui_controller_syncs_tab_and_selection_state()
{
    FrameWorkbenchUiController controller;

    QTabWidget tabs;
    QPushButton button;
    CimdrawView view;
    CimdrawScene scene;
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
    CimQueryFacade facade;
    dock.setImportResult(model,
                         {},
                         {},
                         {},
                         {},
                         facade.buildGraphicVisualSummary(model, QSet<QString>{}));

    auto* shape = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    scene.addItem(shape);
    shape->setData(CimSceneBuilder::CimMridDataKey, QStringLiteral("breaker-001"));
    shape->setSelected(true);
    scene.setSelections({shape});

    FrameCimWorkbenchController::ImportState cimImportState;
    cimImportState.browserDock = &dock;
    controller.handleCurrentTabChanged(&tabs, &button, &cimImportState);
    QCOMPARE(button.palette().color(QPalette::Button), QColor(Qt::yellow));

    auto* treeWidget = dock.findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QVERIFY(treeWidget->currentItem() != nullptr);
    QCOMPARE(treeWidget->currentItem()->data(0, Qt::UserRole).toString(), QStringLiteral("breaker-001"));

    CimdrawItemController propertyController;
    controller.handleCurrentObjectChanged(&propertyController, {shape}, &cimImportState);
    QCOMPARE(propertyController.getObject(), static_cast<QObject*>(shape));
    QCOMPARE(propertyController.getView(), &view);
    QVERIFY(treeWidget->currentItem() != nullptr);
    QCOMPARE(treeWidget->currentItem()->data(0, Qt::UserRole).toString(), QStringLiteral("breaker-001"));
}

void TestCimdrawTopology::frame_workbench_ui_controller_handles_empty_and_multi_selection()
{
    FrameWorkbenchUiController controller;
    CimdrawItemController propertyController;
    CimModelBrowserDock dock;

    QObject seedObject;
    propertyController.setObject(&seedObject);

    QCOMPARE(controller.handleCurrentObjectChanged(&propertyController, {}, nullptr), nullptr);
    QCOMPARE(propertyController.getObject(), nullptr);

    auto* item1 = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* item2 = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    propertyController.setObject(item1);

    QCOMPARE(controller.handleCurrentObjectChanged(&propertyController, {item1, item2}, nullptr),
             static_cast<QObject*>(item1));
    QCOMPARE(propertyController.getObject(), static_cast<QObject*>(item1));

    delete item1;
    delete item2;
}

void TestCimdrawTopology::center_widget_emits_scene_and_tab_close_signals()
{
    CimdrawCenterWidget widget;
    QSignalSpy newSceneSpy(&widget, &CimdrawCenterWidget::requestNewScene);
    QSignalSpy tabClosedSpy(&widget, &CimdrawCenterWidget::currentTabClosed);
    QSignalSpy closeAllSpy(&widget, &CimdrawCenterWidget::closeAll);

    widget.newTab();
    QCOMPARE(newSceneSpy.count(), 1);

    auto* view = new CimdrawView();
    widget.newTab(view);
    QCOMPARE(widget.getCurrentView(), view);

    widget.closeTab();
    QCOMPARE(tabClosedSpy.count(), 1);
    QCOMPARE(closeAllSpy.count(), 1);
    QCOMPARE(widget.getCurrentView(), nullptr);
}

void TestCimdrawTopology::center_widget_ignores_invalid_close_index()
{
    CimdrawCenterWidget widget;
    QSignalSpy tabClosedSpy(&widget, &CimdrawCenterWidget::currentTabClosed);
    QSignalSpy closeAllSpy(&widget, &CimdrawCenterWidget::closeAll);

    auto* view = new CimdrawView();
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

void TestCimdrawTopology::cimdraw_tool_box_manager_exposes_default_sidebar_pages()
{
    CimdrawToolBoxManager toolBox(nullptr);

    QVERIFY(toolBox.getWidget() != nullptr);
    QCOMPARE(toolBox.pageIds(),
             QStringList({QStringLiteral("power"),
                          QStringLiteral("base"),
                          QStringLiteral("image")}));
    QVERIFY(toolBox.activityBar() != nullptr);
    QVERIFY(toolBox.toolList(QStringLiteral("power")) != nullptr);
}

void TestCimdrawTopology::cimdraw_item_controller_tracks_view_and_object()
{
    CimdrawItemController controller;
    CimdrawView view;
    QObject object;

    QCOMPARE(controller.getView(), nullptr);
    QCOMPARE(controller.getObject(), nullptr);

    controller.setView(&view);
    controller.setObject(&object);

    QCOMPARE(controller.getView(), &view);
    QCOMPARE(controller.getObject(), static_cast<QObject*>(&object));
}

void TestCimdrawTopology::cimdraw_image_manager_exposes_widget()
{
    CimdrawImageManager manager;

    QVERIFY(manager.getWidget() != nullptr);
    QCOMPARE(manager.getWidget()->viewMode(), QListView::IconMode);
}

void TestCimdrawTopology::viewport_controller_computes_contents_rect_and_dispatches_mouse_event()
{
    CimdrawSceneViewportController controller;
    CimdrawScene scene;
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

void TestCimdrawTopology::topology_state_controller_rebuilds_index_for_empty_scene()
{
    CimdrawSceneTopologyStateController controller;
    CimdrawScene scene;

    controller.rebuildTopologyIndex(&scene);
    QVERIFY(controller.relationEdgeMetas(&scene).isEmpty());
    const TopologyNodeRelationInfo relationInfo =
        controller.relationNodeInfo(&scene, QStringLiteral("missing-node"));
    QCOMPARE(relationInfo.nodeStableId, QStringLiteral("missing-node"));
    QVERIFY(relationInfo.incidentEdgeIds.isEmpty());
    QVERIFY(relationInfo.adjacentNodeIds.isEmpty());
}

void TestCimdrawTopology::topology_state_controller_invalidates_cached_snapshot()
{
    CimdrawSceneTopologyStateController controller;
    CimdrawScene scene;

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

void TestCimdrawTopology::topology_state_controller_exports_default_view_state_without_view()
{
    CimdrawSceneTopologyStateController controller;
    CimdrawScene scene;
    scene.setSceneRect(QRectF(0, 0, 120, 80));

    const PowerTopologyDocumentExport document = controller.buildPowerTopologyDocumentExport(&scene);

    QCOMPARE(document.devices.size(), 0);
    QCOMPARE(document.conductors.size(), 0);
    QCOMPARE(document.nodes.size(), 0);
    QCOMPARE(document.viewScale, 1.0);
    QCOMPARE(document.viewCenter, scene.getContentsRect().center());
}

void TestCimdrawTopology::power_topology_analysis_controller_returns_empty_results_without_scene()
{
    CimdrawScenePowerTopologyAnalysisController controller;

    QVERIFY(controller.analyzePowerIslands(nullptr).islands.isEmpty());
    QVERIFY(controller.analyzePowerLoops(nullptr).loops.isEmpty());
    QVERIFY(!controller.shortestPowerSupplyPath(nullptr, QStringLiteral("a"), QStringLiteral("b")).found);
    QVERIFY(!controller.shortestDirectedPowerSupplyPath(nullptr, QStringLiteral("a"), QStringLiteral("b")).found);
    QVERIFY(!controller.shortestPowerGroundPath(nullptr, QStringLiteral("a")).found);
}

void TestCimdrawTopology::power_topology_analysis_controller_preview_queries_share_facade_path()
{
    CimQueryFacade facade;
    CimdrawScenePowerTopologyAnalysisController controller;
    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologySlotWorkbenchController slotController;
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* breaker = new CimdrawPowerCircuitBreakerItem(QRectF(0, 0, 64, 92));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(grid);
    scene.addItem(breaker);
    scene.addItem(load);

    grid->setTopologyNodeStableId(QStringLiteral("preview-grid-001"));
    breaker->setTopologyNodeStableId(QStringLiteral("preview-breaker-001"));
    load->setTopologyNodeStableId(QStringLiteral("preview-load-001"));
    breaker->setSwitchPosition(1);

    CimdrawConnectPoint* gridRight = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* breakerTop = findConnectPort(breaker, TOP_DIRECTION);
    CimdrawConnectPoint* breakerBottom = findConnectPort(breaker, BOTTOM_DIRECTION);
    CimdrawConnectPoint* loadLeft = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(gridRight != nullptr);
    QVERIFY(breakerTop != nullptr);
    QVERIFY(breakerBottom != nullptr);
    QVERIFY(loadLeft != nullptr);
    connectPowerItems(&scene, grid, gridRight, breaker, breakerTop);
    connectPowerItems(&scene, breaker, breakerBottom, load, loadLeft);

    const QString breakerId = QStringLiteral("preview-breaker-001");
    const PowerTopologySwitchChangePreview facadeSwitchPreview =
        facade.previewPowerSwitchTopologyChange(&scene, breakerId, 0);
    const PowerTopologyProtectionRangePreview facadeProtectionPreview =
        facade.previewPowerProtectionRange(&scene, breakerId, 0, QStringLiteral("preview-grid-001"));
    const PowerTopologyOperationPreview facadeOperationPreview =
        facade.previewPowerSwitchOperation(&scene,
                                           breakerId,
                                           0,
                                           QStringLiteral("preview-grid-001"),
                                           QStringLiteral("preview-load-001"),
                                           QStringLiteral("preview-grid-001"));

    const PowerTopologySwitchChangePreview controllerSwitchPreview =
        controller.previewPowerSwitchTopologyChange(&scene, breakerId, 0);
    const PowerTopologyProtectionRangePreview controllerProtectionPreview =
        controller.previewPowerProtectionRange(&scene, breakerId, 0, QStringLiteral("preview-grid-001"));
    const PowerTopologyOperationPreview controllerOperationPreview =
        controller.previewPowerSwitchOperation(&scene,
                                               breakerId,
                                               0,
                                               QStringLiteral("preview-grid-001"),
                                               QStringLiteral("preview-load-001"),
                                               QStringLiteral("preview-grid-001"));
    const PowerTopologySwitchChangePreview sceneSwitchPreview =
        scene.previewPowerSwitchTopologyChange(breakerId, 0);
    const PowerTopologyProtectionRangePreview sceneProtectionPreview =
        scene.previewPowerProtectionRange(breakerId, 0, QStringLiteral("preview-grid-001"));
    const PowerTopologyOperationPreview sceneOperationPreview =
        scene.previewPowerSwitchOperation(breakerId,
                                          0,
                                          QStringLiteral("preview-grid-001"),
                                          QStringLiteral("preview-load-001"),
                                          QStringLiteral("preview-grid-001"));
    const PowerTopologySwitchChangePreview workbenchSwitchPreview =
        workbenchController.previewPowerSwitchTopologyChange(&scene, breakerId, 0);
    const PowerTopologyProtectionRangePreview workbenchProtectionPreview =
        workbenchController.previewPowerProtectionRange(&scene, breakerId, 0, QStringLiteral("preview-grid-001"));
    const PowerTopologyOperationPreview workbenchOperationPreview =
        workbenchController.previewPowerSwitchOperation(&scene,
                                                        breakerId,
                                                        0,
                                                        QStringLiteral("preview-grid-001"),
                                                        QStringLiteral("preview-load-001"),
                                                        QStringLiteral("preview-grid-001"));
    const PowerTopologySwitchChangePreview slotSwitchPreview = slotController.dispatchSwitchChangePreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
        breakerId,
        0);
    const PowerTopologyProtectionRangePreview slotProtectionPreview = slotController.dispatchProtectionRangePreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
        breakerId,
        0,
        QStringLiteral("preview-grid-001"));
    const PowerTopologyOperationPreview slotOperationPreview = slotController.dispatchOperationPreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
        breakerId,
        0,
        QStringLiteral("preview-grid-001"),
        QStringLiteral("preview-load-001"),
        QStringLiteral("preview-grid-001"));

    QVERIFY(facadeSwitchPreview.topologyChanged);
    QCOMPARE(controllerSwitchPreview.deviceId, facadeSwitchPreview.deviceId);
    QCOMPARE(controllerSwitchPreview.fromSwitchPosition, facadeSwitchPreview.fromSwitchPosition);
    QCOMPARE(controllerSwitchPreview.toSwitchPosition, facadeSwitchPreview.toSwitchPosition);
    QCOMPARE(controllerSwitchPreview.beforeNodeIds, facadeSwitchPreview.beforeNodeIds);
    QCOMPARE(controllerSwitchPreview.afterNodeIds, facadeSwitchPreview.afterNodeIds);
    QCOMPARE(sceneSwitchPreview.afterConnectedDevices, facadeSwitchPreview.afterConnectedDevices);
    QCOMPARE(workbenchSwitchPreview.removedDevices, facadeSwitchPreview.removedDevices);
    QCOMPARE(slotSwitchPreview.addedDevices, facadeSwitchPreview.addedDevices);

    QCOMPARE(controllerProtectionPreview.protectiveDeviceId, facadeProtectionPreview.protectiveDeviceId);
    QCOMPARE(sceneProtectionPreview.beforeDeviceIds, facadeProtectionPreview.beforeDeviceIds);
    QCOMPARE(workbenchProtectionPreview.afterDeviceIds, facadeProtectionPreview.afterDeviceIds);
    QCOMPARE(slotProtectionPreview.topologyChanged, facadeProtectionPreview.topologyChanged);

    QCOMPARE(controllerOperationPreview.switchChange.deviceId, facadeOperationPreview.switchChange.deviceId);
    QCOMPARE(sceneOperationPreview.switchChange.afterConnectedDevices,
             facadeOperationPreview.switchChange.afterConnectedDevices);
    QCOMPARE(workbenchOperationPreview.protectionRange.afterDeviceIds,
             facadeOperationPreview.protectionRange.afterDeviceIds);
    QCOMPARE(slotOperationPreview.afterSupplyPath.deviceIds,
             facadeOperationPreview.afterSupplyPath.deviceIds);
}

void TestCimdrawTopology::power_topology_analysis_controller_rejects_switch_preview_for_non_switch_device()
{
    CimdrawScenePowerTopologyAnalysisController controller;
    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologySlotWorkbenchController slotController;
    CimdrawScene scene;
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(load);

    const QString deviceId = QStringLiteral("load-preview-device");
    load->setTopologyNodeStableId(deviceId);

    const PowerTopologySwitchChangePreview switchPreview =
        controller.previewPowerSwitchTopologyChange(&scene, deviceId, 1);
    const PowerTopologyProtectionRangePreview protectionPreview =
        controller.previewPowerProtectionRange(&scene, deviceId, 1, QStringLiteral("source"));
    const PowerTopologyOperationPreview operationPreview =
        controller.previewPowerSwitchOperation(&scene, deviceId, 1, QStringLiteral("s1"), QStringLiteral("s2"), QStringLiteral("g1"));
    const PowerTopologySwitchChangePreview sceneSwitchPreview =
        scene.previewPowerSwitchTopologyChange(deviceId, 1);
    const PowerTopologyProtectionRangePreview sceneProtectionPreview =
        scene.previewPowerProtectionRange(deviceId, 1, QStringLiteral("source"));
    const PowerTopologyOperationPreview sceneOperationPreview =
        scene.previewPowerSwitchOperation(deviceId, 1, QStringLiteral("s1"), QStringLiteral("s2"), QStringLiteral("g1"));
    const PowerTopologySwitchChangePreview workbenchSwitchPreview =
        workbenchController.previewPowerSwitchTopologyChange(&scene, deviceId, 1);
    const PowerTopologyProtectionRangePreview workbenchProtectionPreview =
        workbenchController.previewPowerProtectionRange(&scene, deviceId, 1, QStringLiteral("source"));
    const PowerTopologyOperationPreview workbenchOperationPreview =
        workbenchController.previewPowerSwitchOperation(&scene,
                                                        deviceId,
                                                        1,
                                                        QStringLiteral("s1"),
                                                        QStringLiteral("s2"),
                                                        QStringLiteral("g1"));
    const PowerTopologySwitchChangePreview slotSwitchPreview = slotController.dispatchSwitchChangePreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchTopologyChange,
        deviceId,
        1);
    const PowerTopologyProtectionRangePreview slotProtectionPreview = slotController.dispatchProtectionRangePreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerProtectionRange,
        deviceId,
        1,
        QStringLiteral("source"));
    const PowerTopologyOperationPreview slotOperationPreview = slotController.dispatchOperationPreview(
        &scene,
        CimdrawSceneTopologySlotWorkbenchController::PathAction::PreviewPowerSwitchOperation,
        deviceId,
        1,
        QStringLiteral("s1"),
        QStringLiteral("s2"),
        QStringLiteral("g1"));

    QVERIFY(switchPreview.deviceId.isEmpty());
    QVERIFY(protectionPreview.protectiveDeviceId.isEmpty());
    QVERIFY(operationPreview.switchChange.deviceId.isEmpty());
    QVERIFY(sceneSwitchPreview.deviceId.isEmpty());
    QVERIFY(sceneProtectionPreview.protectiveDeviceId.isEmpty());
    QVERIFY(sceneOperationPreview.switchChange.deviceId.isEmpty());
    QVERIFY(workbenchSwitchPreview.deviceId.isEmpty());
    QVERIFY(workbenchProtectionPreview.protectiveDeviceId.isEmpty());
    QVERIFY(workbenchOperationPreview.switchChange.deviceId.isEmpty());
    QVERIFY(slotSwitchPreview.deviceId.isEmpty());
    QVERIFY(slotProtectionPreview.protectiveDeviceId.isEmpty());
    QVERIFY(slotOperationPreview.switchChange.deviceId.isEmpty());
}

void TestCimdrawTopology::power_topology_analysis_controller_returns_empty_device_queries_for_empty_scene()
{
    CimdrawScenePowerTopologyAnalysisController controller;
    CimdrawScene scene;

    QVERIFY(controller.powerDeviceNodeIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.powerDeviceConductorIds(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.busbarAttachedPowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.reachablePowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.directedReachablePowerDevices(&scene, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.analyzePowerBranches(&scene, QStringLiteral("missing")).branches.isEmpty());
    QVERIFY(controller.powerDeviceNodeIds(nullptr, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.connectedPowerDevices(nullptr, QStringLiteral("missing")).isEmpty());
    QVERIFY(controller.analyzePowerIslands(nullptr).islands.isEmpty());
    QVERIFY(!controller.shortestPowerSupplyPath(nullptr,
                                                QStringLiteral("source"),
                                                QStringLiteral("target")).found);
    QVERIFY(controller.previewPowerSwitchTopologyChange(nullptr, QStringLiteral("missing"), 1).deviceId.isEmpty());
}

void TestCimdrawTopology::context_menu_controller_creates_and_trims_menu_actions()
{
    CimdrawSceneContextMenuController controller;
    CimdrawScene scene;

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

void TestCimdrawTopology::context_menu_controller_resets_existing_menu()
{
    CimdrawSceneContextMenuController controller;
    CimdrawScene scene;

    QMenu* menu = controller.createMenu(&scene);
    menu->addAction(QStringLiteral("临时动作"));
    QVERIFY(menu->actions().size() > 6);

    QMenu* reset = controller.resetMenu(menu, &scene);
    QVERIFY(reset != nullptr);
    QCOMPARE(reset->actions().size(), 6);
    QCOMPARE(reset->actions().at(0)->text(), QStringLiteral("剪切"));

    delete reset;
}

void TestCimdrawTopology::context_menu_controller_ensures_menu_instance()
{
    CimdrawSceneContextMenuController controller;
    CimdrawScene scene;

    QMenu* created = controller.ensureMenu(nullptr, &scene);
    QVERIFY(created != nullptr);
    QCOMPARE(created->actions().size(), 6);

    QMenu* reused = controller.ensureMenu(created, &scene);
    QCOMPARE(reused, created);

    delete created;
}

void TestCimdrawTopology::context_menu_controller_prepares_menu_for_display()
{
    CimdrawSceneContextMenuController controller;
    CimdrawScene scene;

    QMenu* menu = controller.createMenu(&scene);
    menu->addAction(QStringLiteral("临时动作"));
    QVERIFY(menu->actions().size() > 6);

    QMenu* prepared = controller.prepareMenuForDisplay(menu, 6);
    QCOMPARE(prepared, menu);
    QCOMPARE(menu->actions().size(), 6);
    QCOMPARE(menu->actions().at(0)->text(), QStringLiteral("剪切"));

    delete menu;
}

void TestCimdrawTopology::scene_constructs_context_menu_for_exec_path()
{
    CimdrawScene scene;

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

void TestCimdrawTopology::scene_exec_context_menu_remains_repeatable()
{
    CimdrawScene scene;

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

void TestCimdrawTopology::interaction_controller_accepts_only_text_drop_mime()
{
    CimdrawSceneInteractionController controller;
    QMimeData emptyMime;
    QMimeData textMime;
    textMime.setText(QStringLiteral("rect"));

    QVERIFY(!controller.acceptsTextDrop(nullptr));
    QVERIFY(!controller.acceptsTextDrop(&emptyMime));
    QVERIFY(controller.acceptsTextDrop(&textMime));
}

void TestCimdrawTopology::scene_document_workbench_controller_routes_document_and_group_entrypoints()
{
    CimdrawSceneDocumentWorkbenchController controller;
    CimdrawScene scene;

    QVERIFY(!controller.load(nullptr, QStringLiteral("missing.xml"), nullptr));
    QVERIFY(!controller.save(nullptr, QStringLiteral("x.xml")));

    FILE_TYPE type = FILE_TYPE::CIMDRAW_DRAW;
    QCOMPARE(controller.sceneType(type), FILE_TYPE::CIMDRAW_DRAW);
    controller.setSceneType(&type, FILE_TYPE::CIMDRAW_DYNAMIC);
    QCOMPARE(type, FILE_TYPE::CIMDRAW_DYNAMIC);

    auto* item1 = scene.addRect(QRectF(0, 0, 20, 20));
    auto* item2 = scene.addRect(QRectF(30, 0, 20, 20));
    QGraphicsItemGroup* group = controller.createGroup(&scene, {item1, item2});
    QVERIFY(group != nullptr);
    QCOMPARE(item1->group(), group);
    QCOMPARE(item2->group(), group);
}

void TestCimdrawTopology::document_controller_rejects_missing_or_invalid_dom_sources()
{
    CimdrawSceneDocumentController controller;
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

void TestCimdrawTopology::document_controller_parses_valid_dom_and_manages_scene_type()
{
    CimdrawSceneDocumentController controller;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("valid.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><CimdrawFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(controller.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("CimdrawFile"));

    FILE_TYPE type = FILE_TYPE::CIMDRAW_DRAW;
    QCOMPARE(controller.sceneType(type), FILE_TYPE::CIMDRAW_DRAW);
    controller.setSceneType(&type, FILE_TYPE::CIMDRAW_DYNAMIC);
    QCOMPARE(type, FILE_TYPE::CIMDRAW_DYNAMIC);
}

void TestCimdrawTopology::document_controller_rejects_save_without_scene_or_path()
{
    CimdrawSceneDocumentController controller;
    CimdrawScene scene;

    QVERIFY(!controller.save(nullptr, QStringLiteral("x.xml")));
    QVERIFY(!controller.save(&scene, QString()));
    QVERIFY(!controller.load(nullptr, QStringLiteral("x.xml"), nullptr));
}

void TestCimdrawTopology::scene_to_dom_document_routes_through_document_controller()
{
    CimdrawScene scene;
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString validPath = dir.filePath(QStringLiteral("scene.xml"));
    QFile validFile(validPath);
    QVERIFY(validFile.open(QIODevice::WriteOnly | QIODevice::Text));
    validFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?><CimdrawFile/>");
    validFile.close();

    QDomDocument dom;
    QString error;
    QVERIFY(scene.toDomDocument(validPath, &dom, &error));
    QCOMPARE(dom.documentElement().tagName(), QStringLiteral("CimdrawFile"));
}

void TestCimdrawTopology::cimdraw_file_load_by_xml_rebinds_connect_lines_via_topology_binding_index()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString filePath = dir.filePath(QStringLiteral("topology-rebind.xml"));

    CimdrawScene sourceScene;
    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    sourceScene.addItem(grid);
    sourceScene.addItem(load);
    grid->setTopologyNodeStableId(QStringLiteral("file-grid-001"));
    load->setTopologyNodeStableId(QStringLiteral("file-load-001"));

    CimdrawConnectPoint* gridPort = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadPort = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(gridPort != nullptr);
    QVERIFY(loadPort != nullptr);
    connectPowerItems(&sourceScene, grid, gridPort, load, loadPort);

    CimdrawFile sourceFile(filePath);
    QVERIFY(sourceFile.saveAsXml(&sourceScene));

    QFile xmlFile(filePath);
    QVERIFY(xmlFile.open(QIODevice::ReadOnly | QIODevice::Text));
    QDomDocument dom;
    QString parseError;
    int errorLine = 0;
    int errorColumn = 0;
    QVERIFY(dom.setContent(xmlFile.readAll(), &parseError, &errorLine, &errorColumn));
    xmlFile.close();

    CimdrawScene loadedScene;
    CimdrawFile loadedFile(filePath);
    QVERIFY(loadedFile.loadByXml(&dom, &loadedScene));

    CimdrawConnectLine* loadedLine = nullptr;
    for (QGraphicsItem* item : loadedScene.items())
    {
        loadedLine = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (loadedLine)
            break;
    }
    QVERIFY(loadedLine != nullptr);
    QVERIFY(loadedLine->getStartItem() != nullptr);
    QVERIFY(loadedLine->getEndItem() != nullptr);
    QVERIFY(loadedLine->startConnectPort() != nullptr);
    QVERIFY(loadedLine->endConnectPort() != nullptr);

    const TopologyEdgeMeta binding = loadedLine->resolvedRelationEdgeMeta();
    auto* startItem = dynamic_cast<CimdrawItem*>(loadedLine->getStartItem());
    auto* endItem = dynamic_cast<CimdrawItem*>(loadedLine->getEndItem());
    QVERIFY(startItem != nullptr);
    QVERIFY(endItem != nullptr);
    QCOMPARE(startItem->topologyNodeStableId(), binding.startNodeStableId);
    QCOMPARE(endItem->topologyNodeStableId(), binding.endNodeStableId);
}

void TestCimdrawTopology::cimdraw_mime_data_instantiate_paste_batch_rebinds_connect_lines_via_topology_binding_index()
{
    CimdrawScene scene;

    auto* grid = new CimdrawPowerGridItem(QRectF(0, 0, 168, 36));
    auto* load = new CimdrawPowerLoadItem(QRectF(0, 0, 96, 36));
    scene.addItem(grid);
    scene.addItem(load);
    grid->setPos(0, 0);
    load->setPos(240, 0);
    grid->setTopologyNodeStableId(QStringLiteral("mime-grid-001"));
    load->setTopologyNodeStableId(QStringLiteral("mime-load-001"));

    CimdrawConnectPoint* gridPort = findConnectPort(grid, RIGHT_DIRECTION);
    CimdrawConnectPoint* loadPort = findConnectPort(load, LEFT_DIRECTION);
    QVERIFY(gridPort != nullptr);
    QVERIFY(loadPort != nullptr);
    connectPowerItems(&scene, grid, gridPort, load, loadPort);

    QList<QGraphicsItem*> clipboardItems;
    clipboardItems << grid << load;
    for (QGraphicsItem* item : scene.items())
    {
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
            clipboardItems << line;
    }

    CimdrawMimeData mimeData(clipboardItems);
    const QList<QGraphicsItem*> freshItems = mimeData.instantiatePasteBatch(QPointF(160.0, 80.0));
    QVERIFY(!freshItems.isEmpty());

    CimdrawConnectLine* freshLine = nullptr;
    QList<CimdrawItem*> freshShapes;
    for (QGraphicsItem* item : freshItems)
    {
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
            freshLine = line;
        else if (auto* shape = dynamic_cast<CimdrawItem*>(item))
            freshShapes.append(shape);
    }

    QVERIFY(freshLine != nullptr);
    QCOMPARE(freshShapes.size(), 2);
    QVERIFY(freshLine->getStartItem() != nullptr);
    QVERIFY(freshLine->getEndItem() != nullptr);
    QVERIFY(freshLine->startConnectPort() != nullptr);
    QVERIFY(freshLine->endConnectPort() != nullptr);

    const TopologyEdgeMeta binding = freshLine->resolvedRelationEdgeMeta();
    auto* startItem = dynamic_cast<CimdrawItem*>(freshLine->getStartItem());
    auto* endItem = dynamic_cast<CimdrawItem*>(freshLine->getEndItem());
    QVERIFY(startItem != nullptr);
    QVERIFY(endItem != nullptr);
    QCOMPARE(startItem->topologyNodeStableId(), binding.startNodeStableId);
    QCOMPARE(endItem->topologyNodeStableId(), binding.endNodeStableId);

    qDeleteAll(freshItems);
}

void TestCimdrawTopology::group_controller_creates_group_and_reparents_items()
{
    CimdrawSceneGroupController controller;
    CimdrawScene scene;

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

void TestCimdrawTopology::item_lifecycle_controller_handles_insert_delete_and_batch_selection()
{
    CimdrawSceneItemLifecycleController controller;
    CimdrawScene scene;
    QList<QGraphicsItem*> selection;

    auto* rect1 = new QGraphicsRectItem(QRectF(0, 0, 20, 20));
    auto* rect2 = new QGraphicsRectItem(QRectF(30, 0, 20, 20));
    auto* line = new CimdrawConnectLine();

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

void TestCimdrawTopology::item_lifecycle_controller_ignores_null_scene_or_items()
{
    CimdrawSceneItemLifecycleController controller;
    CimdrawScene scene;
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


