#include "test_cim_ui.h"

#include "CimdrawFrame.h"
#include "CimdrawScene.h"
#include "Item/CimdrawConnectLine.h"
#include "Item/TmpBase.h"
#include "cim/importer/CimImportResult.h"
#include "cim/ui/CimFrameCoordinator.h"
#include "cim/ui/CimModelBrowserDock.h"

#include <QAction>
#include <QFile>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QTemporaryDir>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QtTest>

namespace {

QString minimalSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/minimal/EQ_SSH_sample");
}

QString minimalEqFilePath()
{
    return QFINDTESTDATA("tests/data/cim/minimal/EQ_SSH_sample/EQ.xml");
}

QString mediumSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/Sample_Grid_Switches_NodeBreaker");
}

QString mediumReferencePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/CIGREMV_reference");
}

QString mediumCigrePublicPath()
{
    return QFINDTESTDATA("tests/data/cim/medium/CIGRE_MV");
}

QString shuntFamilySamplePath()
{
    return QFINDTESTDATA("tests/data/cim/minimal/ShuntCompensator_family_sample");
}

QString duplicateMridSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/duplicate_mrid_same_profile");
}

bool writeUtf8File(const QString& path, const QString& content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    return file.write(content.toUtf8()) >= 0;
}

QTreeWidgetItem* findItemByMrid(QTreeWidget* treeWidget, const QString& mrid)
{
    if (!treeWidget)
        return nullptr;
    QTreeWidgetItemIterator it(treeWidget);
    while (*it)
    {
        if ((*it)->data(0, Qt::UserRole).toString() == mrid)
            return *it;
        ++it;
    }
    return nullptr;
}

QTreeWidgetItem* findTopLevelItemByLabel(QTreeWidget* treeWidget, const QString& label)
{
    if (!treeWidget)
        return nullptr;
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        if (item && item->text(0) == label)
            return item;
    }
    return nullptr;
}

}

void TestCimUi::file_menu_exposes_import_cim_action()
{
    CimdrawFrame frame;

    bool found = false;
    const QList<QAction*> topLevelActions = frame.menuBar()->actions();
    for (QAction* action : topLevelActions)
    {
        QMenu* menu = action ? action->menu() : nullptr;
        if (!menu)
            continue;
        const QList<QAction*> actions = menu->actions();
        for (QAction* childAction : actions)
        {
            if (childAction && childAction->text() == QStringLiteral("导入 CIM/CGMES"))
            {
                found = true;
                break;
            }
        }
        if (found)
            break;
    }

    QVERIFY(found);
}

void TestCimUi::import_creates_browser_dock_and_scene_items()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    QVERIFY(dock->isVisible());

    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QCOMPARE(treeWidget->topLevelItemCount(), 6);

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);
    QVERIFY(!scene->items().isEmpty());
}

void TestCimUi::medium_sample_import_keeps_browser_visible_and_filterable()
{
    const QString path = mediumSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到中等规模 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    QVERIFY(dock->isVisible());

    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QVERIFY(treeWidget->topLevelItemCount() > 0);

    auto* filterEdit = dock->findChild<QLineEdit*>();
    QVERIFY(filterEdit != nullptr);
    filterEdit->setText(QStringLiteral("Breaker"));
    QCoreApplication::processEvents();

    bool hasVisibleItem = false;
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* top = treeWidget->topLevelItem(i);
        if (top && !top->isHidden())
        {
            hasVisibleItem = true;
            break;
        }
    }
    QVERIFY(hasVisibleItem);
}

void TestCimUi::cigre_reference_import_shows_specialized_groups_in_browser()
{
    const QString path = mediumReferencePath();
    QVERIFY2(!path.isEmpty(), "未找到 CIGREMV_reference 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    QVERIFY(dock->isVisible());

    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    QVERIFY(treeWidget->topLevelItemCount() > 0);

    bool foundSpecializedGroup = false;
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* top = treeWidget->topLevelItem(i);
        if (!top)
            continue;
        const QString groupName = top->text(0);
        if (groupName.contains(QStringLiteral("Diagram"))
            || groupName.contains(QStringLiteral("Sv")))
        {
            foundSpecializedGroup = true;
            break;
        }
    }

    QVERIFY(foundSpecializedGroup);
}

void TestCimUi::browser_classifies_reference_sample_standard_non_device_objects()
{
    const QString path = mediumReferencePath();
    QVERIFY2(!path.isEmpty(), "未找到 CIGREMV_reference 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    QTreeWidgetItem* svVoltageGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("SvVoltage"));
    QVERIFY(svVoltageGroup != nullptr);
    QVERIFY(svVoltageGroup->childCount() > 0);
    QCOMPARE(svVoltageGroup->child(0)->text(3), QStringLiteral("语义对象"));

    QTreeWidgetItem* svPowerFlowGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("SvPowerFlow"));
    QVERIFY(svPowerFlowGroup != nullptr);
    QVERIFY(svPowerFlowGroup->childCount() > 0);
    QCOMPARE(svPowerFlowGroup->child(0)->text(3), QStringLiteral("语义对象"));

    QTreeWidgetItem* powerTransformerEndGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("PowerTransformerEnd"));
    QVERIFY(powerTransformerEndGroup != nullptr);
    QVERIFY(powerTransformerEndGroup->childCount() > 0);
    QCOMPARE(powerTransformerEndGroup->child(0)->text(3), QStringLiteral("语义对象"));

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(!summary.contains(QStringLiteral("其他未映射对象数")));
}

void TestCimUi::browser_classifies_cigre_public_name_objects_as_metadata()
{
    const QString path = mediumCigrePublicPath();
    QVERIFY2(!path.isEmpty(), "未找到公开 CIGRE_MV 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    QTreeWidgetItem* nameGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("Name"));
    QVERIFY(nameGroup != nullptr);
    QVERIFY(nameGroup->childCount() > 0);
    QCOMPARE(nameGroup->child(0)->text(3), QStringLiteral("元数据/容器对象"));

    QTreeWidgetItem* nameTypeGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("NameType"));
    QVERIFY(nameTypeGroup != nullptr);
    QVERIFY(nameTypeGroup->childCount() > 0);
    QCOMPARE(nameTypeGroup->child(0)->text(3), QStringLiteral("元数据/容器对象"));

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("应可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("已可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("未可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("可视化覆盖率")));
    QVERIFY(summary.contains(QStringLiteral("非图元对象数")));
    QVERIFY(summary.contains(QStringLiteral("标准导体对象数（ACLineSegment）")));
    QVERIFY(!summary.contains(QStringLiteral("其他未映射对象数")));
}

void TestCimUi::browser_marks_reference_source_and_load_objects_as_visualized()
{
    const QString path = mediumReferencePath();
    QVERIFY2(!path.isEmpty(), "未找到 CIGREMV_reference 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* gridItem = findItemByMrid(treeWidget, QStringLiteral("HV-Netz"));
    QVERIFY(gridItem != nullptr);
    QCOMPARE(gridItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* loadItem = findItemByMrid(treeWidget, QStringLiteral("Load7-I"));
    QVERIFY(loadItem != nullptr);
    QCOMPARE(loadItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::browser_marks_projection_infeed_objects_as_visualized()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-infeed-ui-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SolarGeneratingUnit rdf:ID="solar-001">
    <cim:IdentifiedObject.name>PV1</cim:IdentifiedObject.name>
  </cim:SolarGeneratingUnit>
  <cim:WindGeneratingUnit rdf:ID="wind-001">
    <cim:IdentifiedObject.name>WIND1</cim:IdentifiedObject.name>
  </cim:WindGeneratingUnit>
  <cim:BatteryUnit rdf:ID="battery-001">
    <cim:IdentifiedObject.name>ESS1</cim:IdentifiedObject.name>
  </cim:BatteryUnit>
  <cim:Terminal rdf:ID="terminal-solar-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#solar-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-infeed"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-wind-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#wind-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-infeed"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-battery-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#battery-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-storage"/>
  </cim:Terminal>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("infeed_UI_EQ.xml")), eqXml),
             "failed to write projection infeed UI EQ sample");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(dir.path()));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* solarItem = findItemByMrid(treeWidget, QStringLiteral("solar-001"));
    QVERIFY(solarItem != nullptr);
    QCOMPARE(solarItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* windItem = findItemByMrid(treeWidget, QStringLiteral("wind-001"));
    QVERIFY(windItem != nullptr);
    QCOMPARE(windItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* batteryItem = findItemByMrid(treeWidget, QStringLiteral("battery-001"));
    QVERIFY(batteryItem != nullptr);
    QCOMPARE(batteryItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::browser_marks_source_and_equivalent_grid_objects_as_visualized()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-source-grid-ui-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SynchronousMachine rdf:ID="generator-001">
    <cim:IdentifiedObject.name>G1</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:ExternalNetworkInjection rdf:ID="external-001">
    <cim:IdentifiedObject.name>Grid1</cim:IdentifiedObject.name>
  </cim:ExternalNetworkInjection>
  <cim:EquivalentInjection rdf:ID="equivalent-001">
    <cim:IdentifiedObject.name>Eq1</cim:IdentifiedObject.name>
  </cim:EquivalentInjection>
  <cim:Terminal rdf:ID="terminal-generator-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#generator-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-source"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-external-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#external-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-grid"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-equivalent-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#equivalent-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-grid"/>
  </cim:Terminal>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("source_grid_EQ.xml")), eqXml),
             "failed to write source/grid UI EQ sample");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(dir.path()));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* generatorItem = findItemByMrid(treeWidget, QStringLiteral("generator-001"));
    QVERIFY(generatorItem != nullptr);
    QCOMPARE(generatorItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* externalItem = findItemByMrid(treeWidget, QStringLiteral("external-001"));
    QVERIFY(externalItem != nullptr);
    QCOMPARE(externalItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* equivalentItem = findItemByMrid(treeWidget, QStringLiteral("equivalent-001"));
    QVERIFY(equivalentItem != nullptr);
    QCOMPARE(equivalentItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::import_entry_accepts_directory_path_directly()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    QFileInfo info(path);
    QVERIFY(info.exists());
    QVERIFY(info.isDir());

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(info.absoluteFilePath()));
}

void TestCimUi::import_entry_accepts_xml_file_path_by_resolving_parent_directory()
{
    const QString path = minimalEqFilePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 的 EQ.xml");

    QFileInfo info(path);
    QVERIFY(info.exists());
    QVERIFY(info.isFile());

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(info.absoluteFilePath()));
}

void TestCimUi::import_with_recoverable_warnings_keeps_summary_in_dock()
{
    const QString path = duplicateMridSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 duplicate_mrid_same_profile 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    QVERIFY(dock->isVisible());

    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("导入告警")));
    QVERIFY(summary.contains(QStringLiteral("重复定义")));

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);
    QVERIFY(!scene->items().isEmpty());
}

void TestCimUi::import_failure_presentation_reports_status_and_log_details()
{
    CimImportResult result;
    result.success = false;
    result.errors << QStringLiteral("文件 bad.xml 不是受支持的 CIM/CGMES RDF 文件。");
    result.warnings << QStringLiteral("失败前保留的兼容告警");
    result.logs << QStringLiteral("已检查输入文件: bad.xml");

    const CimFrameCoordinator::ImportPresentation presentation =
        CimFrameCoordinator::buildImportPresentation(result);
    QCOMPARE(presentation.statusMessage, QStringLiteral("CIM 导入失败：1 个错误"));
    QVERIFY(presentation.hasDetails());
    QVERIFY(presentation.detailText.contains(QStringLiteral("导入错误")));
    QVERIFY(presentation.detailText.contains(QStringLiteral("bad.xml")));
    QVERIFY(presentation.detailText.contains(QStringLiteral("导入告警")));
    QVERIFY(presentation.detailText.contains(QStringLiteral("导入日志")));

    CimModelBrowserDock dock;
    CimGraphicVisualSummary visualSummary;
    dock.setImportResult(result.model,
                         result.loadedProfiles,
                         result.logs,
                         result.warnings,
                         result.errors,
                         visualSummary,
                         result.topologyProjection);

    auto* summaryEdit = dock.findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);
    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("导入错误")));
    QVERIFY(summary.contains(QStringLiteral("不是受支持的 CIM/CGMES RDF 文件")));
    QVERIFY(summary.contains(QStringLiteral("导入告警")));
    QVERIFY(summary.contains(QStringLiteral("导入日志")));
    QVERIFY(summary.contains(QStringLiteral("已检查输入文件")));
}

void TestCimUi::import_layout_spreads_primary_equipment_positions()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);

    TmpShape* busbarShape = nullptr;
    TmpShape* breakerShape = nullptr;
    TmpShape* loadShape = nullptr;
    const QList<QGraphicsItem*> sceneItems = scene->items();
    for (QGraphicsItem* item : sceneItems)
    {
        auto* shape = dynamic_cast<TmpShape*>(item);
        if (!shape)
            continue;

        if (shape->cimdrawObjectId() == QStringLiteral("busbar-001"))
            busbarShape = shape;
        else if (shape->cimdrawObjectId() == QStringLiteral("breaker-001"))
            breakerShape = shape;
        else if (shape->cimdrawObjectId() == QStringLiteral("load-001"))
            loadShape = shape;
    }

    QVERIFY(busbarShape != nullptr);
    QVERIFY(breakerShape != nullptr);
    QVERIFY(loadShape != nullptr);

    QVERIFY(busbarShape->scenePos() != breakerShape->scenePos());
    QVERIFY(breakerShape->scenePos() != loadShape->scenePos());
    QVERIFY(busbarShape->scenePos().y() < breakerShape->scenePos().y());
    QVERIFY(breakerShape->scenePos().y() < loadShape->scenePos().y());
}

void TestCimUi::import_generates_readable_connection_lines()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);

    int lineCount = 0;
    TmpShape* busbarShape = nullptr;
    const QList<QGraphicsItem*> sceneItems = scene->items();
    for (QGraphicsItem* item : sceneItems)
    {
        if (qgraphicsitem_cast<CimdrawConnectLine*>(item))
            ++lineCount;

        auto* shape = dynamic_cast<TmpShape*>(item);
        if (shape && shape->cimdrawObjectId() == QStringLiteral("busbar-001"))
            busbarShape = shape;
    }

    QVERIFY(lineCount > 0);
    QVERIFY(busbarShape != nullptr);
    QVERIFY(busbarShape->connectedItems().size() > 0);
}

void TestCimUi::browser_marks_visualization_statuses_for_medium_sample()
{
    const QString path = mediumSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到中等规模 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* junctionItem = findItemByMrid(treeWidget, QStringLiteral("_4de2118a-8f14-0e13-88e0-60766b3fec7f"));
    QVERIFY(junctionItem != nullptr);
    QCOMPARE(junctionItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* terminalItem = findItemByMrid(treeWidget, QStringLiteral("_2a7df911-e5a0-2c0e-5e75-280dca39d3cc"));
    QVERIFY(terminalItem != nullptr);
    QCOMPARE(terminalItem->text(3), QStringLiteral("语义对象"));

    QTreeWidgetItem* diagramGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("DiagramObjectPoint"));
    QVERIFY(diagramGroup != nullptr);
    QVERIFY(diagramGroup->childCount() > 0);
    QCOMPARE(diagramGroup->child(0)->text(3), QStringLiteral("布局对象"));

    QTreeWidgetItem* regionGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("SubGeographicalRegion"));
    QVERIFY(regionGroup != nullptr);
    QVERIFY(regionGroup->childCount() > 0);
    QCOMPARE(regionGroup->child(0)->text(3), QStringLiteral("元数据/容器对象"));
}

void TestCimUi::browser_summary_reports_visualization_coverage()
{
    const QString path = mediumSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到中等规模 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("应可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("已可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("未可视化设备数")));
    QVERIFY(summary.contains(QStringLiteral("可视化覆盖率")));
    QVERIFY(summary.contains(QStringLiteral("非图元对象数")));
}

void TestCimUi::browser_summary_reports_model_topology_snapshot()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "minimal CIM sample not found");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("\u6A21\u578B\u62D3\u6251\u5FEB\u7167")));
    QVERIFY(summary.contains(QStringLiteral("\u8BBE\u5907")));
    QVERIFY(summary.contains(QStringLiteral("\u8282\u70B9")));
    QVERIFY(summary.contains(QStringLiteral("\u62D3\u6251\u6295\u5F71\u6765\u6E90")));
    QVERIFY(summary.contains(QStringLiteral("\u5BFC\u5165\u6A21\u578B\u8BED\u4E49\u5FEB\u7167")));
    QVERIFY(summary.contains(QStringLiteral("\u771F\u6E90\u8FB9\u754C")));
}

void TestCimUi::browser_summary_explains_aclinesegment_vs_helper_lines()
{
    const QString path = mediumSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到中等规模 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    QTreeWidgetItem* lineGroup = findTopLevelItemByLabel(treeWidget, QStringLiteral("ACLineSegment"));
    QVERIFY(lineGroup != nullptr);

    int visualizedAcLineSegments = 0;
    for (int i = 0; i < lineGroup->childCount(); ++i)
    {
        if (lineGroup->child(i)->text(3) == QStringLiteral("已可视化"))
            ++visualizedAcLineSegments;
    }

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);
    int helperLineCount = 0;
    for (QGraphicsItem* item : scene->items())
    {
        if (qgraphicsitem_cast<CimdrawConnectLine*>(item))
            ++helperLineCount;
    }

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(
        QStringLiteral("标准导体对象数（ACLineSegment）：%1，其中已图元化 %2")
            .arg(lineGroup->childCount())
            .arg(visualizedAcLineSegments)));
    QVERIFY(summary.contains(QStringLiteral("自动辅助连线数：%1").arg(helperLineCount)));
    QVERIFY(summary.contains(QStringLiteral("不等同于 ACLineSegment 对象数")));
}

void TestCimUi::browser_summary_classifies_grounding_impedance_as_unmapped_device()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-browser-summary-split-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:GroundingImpedance rdf:ID="grounding-impedance-001">
    <cim:IdentifiedObject.name>GI1</cim:IdentifiedObject.name>
  </cim:GroundingImpedance>
  <cim:MutualCoupling rdf:ID="mutual-coupling-001">
    <cim:IdentifiedObject.name>MC1</cim:IdentifiedObject.name>
  </cim:MutualCoupling>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("browser_summary_split_EQ.xml")), eqXml),
             "failed to write browser summary split EQ sample");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(dir.path()));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);
    auto* summaryEdit = dock->findChild<QTextEdit*>();
    QVERIFY(summaryEdit != nullptr);

    QTreeWidgetItem* groundingItem = findItemByMrid(treeWidget, QStringLiteral("grounding-impedance-001"));
    QVERIFY(groundingItem != nullptr);
    QCOMPARE(groundingItem->text(3), QStringLiteral("设备未映射"));

    const QString summary = summaryEdit->toPlainText();
    QVERIFY(summary.contains(QStringLiteral("未可视化设备数：0")));
    QVERIFY(summary.contains(QStringLiteral("其他未映射设备数：1")));
    QVERIFY(summary.contains(QStringLiteral("GroundingImpedance")));
    QVERIFY(summary.contains(QStringLiteral("其他未归类对象数：1")));
    QVERIFY(summary.contains(QStringLiteral("MutualCoupling")));
}

void TestCimUi::browser_marks_shunt_compensator_family_sample_as_visualized()
{
    const QString path = shuntFamilySamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 ShuntCompensator_family_sample 最小样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* capacitorItem = findItemByMrid(treeWidget, QStringLiteral("cap-bank-001"));
    QVERIFY(capacitorItem != nullptr);
    QCOMPARE(capacitorItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* reactorItem = findItemByMrid(treeWidget, QStringLiteral("reactor-bank-001"));
    QVERIFY(reactorItem != nullptr);
    QCOMPARE(reactorItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* splitReactorItem = findItemByMrid(treeWidget, QStringLiteral("split-reactor-001"));
    QVERIFY(splitReactorItem != nullptr);
    QCOMPARE(splitReactorItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* nonlinearItem = findItemByMrid(treeWidget, QStringLiteral("nonlinear-reactor-001"));
    QVERIFY(nonlinearItem != nullptr);
    QCOMPARE(nonlinearItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* svgItem = findItemByMrid(treeWidget, QStringLiteral("svg-comp-001"));
    QVERIFY(svgItem != nullptr);
    QCOMPARE(svgItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* arcCoilItem = findItemByMrid(treeWidget, QStringLiteral("arc-coil-001"));
    QVERIFY(arcCoilItem != nullptr);
    QCOMPARE(arcCoilItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::browser_marks_series_compensator_family_as_visualized()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-series-family-ui-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SeriesCompensator rdf:ID="series-cap-001">
    <cim:IdentifiedObject.name>Series Capacitor Bank</cim:IdentifiedObject.name>
    <cim:SeriesCompensator.x>-0.0035</cim:SeriesCompensator.x>
  </cim:SeriesCompensator>
  <cim:SeriesCompensator rdf:ID="series-reactor-001">
    <cim:IdentifiedObject.name>Current Limiting Reactor</cim:IdentifiedObject.name>
    <cim:SeriesCompensator.x>0.0062</cim:SeriesCompensator.x>
  </cim:SeriesCompensator>
  <cim:Terminal rdf:ID="terminal-series-cap-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#series-cap-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-series-cap"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-series-reactor-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#series-reactor-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-series-reactor"/>
  </cim:Terminal>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("series_family_EQ.xml")), eqXml),
             "failed to write series compensator ui EQ sample");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(dir.path()));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* capacitorItem = findItemByMrid(treeWidget, QStringLiteral("series-cap-001"));
    QVERIFY(capacitorItem != nullptr);
    QCOMPARE(capacitorItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* reactorItem = findItemByMrid(treeWidget, QStringLiteral("series-reactor-001"));
    QVERIFY(reactorItem != nullptr);
    QCOMPARE(reactorItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::browser_marks_power_transformer_specializations_as_visualized()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-ui-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:PowerTransformer rdf:ID="xfmr-auto-001">
    <cim:IdentifiedObject.name>T1</cim:IdentifiedObject.name>
    <cim:IdentifiedObject.description>Auto Transformer for feeder tie</cim:IdentifiedObject.description>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-auto-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-auto-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformer rdf:ID="xfmr-earthing-001">
    <cim:IdentifiedObject.name>Earthing Transformer</cim:IdentifiedObject.name>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-earthing-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-earthing-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformer rdf:ID="xfmr-station-001">
    <cim:IdentifiedObject.name>T2</cim:IdentifiedObject.name>
    <cim:IdentifiedObject.aliasName>Auxiliary Transformer</cim:IdentifiedObject.aliasName>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-station-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-station-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformerEnd rdf:ID="xfmr-auto-end-a">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-auto-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-auto-end-b">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-auto-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-earthing-end-a">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-earthing-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-earthing-end-b">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-earthing-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-station-end-a">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-station-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-station-end-b">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-station-001"/>
  </cim:PowerTransformerEnd>
  <cim:Terminal rdf:ID="terminal-auto-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-auto-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-auto"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-earthing-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-earthing-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-earthing"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-station-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-station-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-station"/>
  </cim:Terminal>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("transformer_ui_EQ.xml")), eqXml),
             "failed to write transformer ui EQ sample");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(dir.path()));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* autoItem = findItemByMrid(treeWidget, QStringLiteral("xfmr-auto-001"));
    QVERIFY(autoItem != nullptr);
    QCOMPARE(autoItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* earthingItem = findItemByMrid(treeWidget, QStringLiteral("xfmr-earthing-001"));
    QVERIFY(earthingItem != nullptr);
    QCOMPARE(earthingItem->text(3), QStringLiteral("已可视化"));

    QTreeWidgetItem* stationItem = findItemByMrid(treeWidget, QStringLiteral("xfmr-station-001"));
    QVERIFY(stationItem != nullptr);
    QCOMPARE(stationItem->text(3), QStringLiteral("已可视化"));
}

void TestCimUi::browser_selection_locates_scene_item()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    QTreeWidgetItem* breakerItem = findItemByMrid(treeWidget, QStringLiteral("breaker-001"));
    QVERIFY(breakerItem != nullptr);
    treeWidget->setCurrentItem(breakerItem);
    QCoreApplication::processEvents();

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);
    QCOMPARE(scene->getSelections().size(), 1);

    auto* selectedShape = dynamic_cast<TmpShape*>(scene->getSelections().first());
    QVERIFY(selectedShape != nullptr);
    QCOMPARE(selectedShape->cimdrawObjectId(), QStringLiteral("breaker-001"));
}

void TestCimUi::scene_selection_syncs_browser_item()
{
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");

    CimdrawFrame frame;
    QVERIFY(frame.importCimFromPath(path));

    auto* dock = frame.findChild<CimModelBrowserDock*>(QStringLiteral("cimModelBrowserDock"));
    QVERIFY(dock != nullptr);
    auto* treeWidget = dock->findChild<QTreeWidget*>();
    QVERIFY(treeWidget != nullptr);

    CimdrawScene* scene = frame.activeScene();
    QVERIFY(scene != nullptr);

    QTreeWidgetItem* breakerItem = findItemByMrid(treeWidget, QStringLiteral("breaker-001"));
    QTreeWidgetItem* loadItem = findItemByMrid(treeWidget, QStringLiteral("load-001"));
    QVERIFY(breakerItem != nullptr);
    QVERIFY(loadItem != nullptr);

    treeWidget->setCurrentItem(breakerItem);
    QCoreApplication::processEvents();
    QCOMPARE(scene->getSelections().size(), 1);
    auto* breakerShape = dynamic_cast<TmpShape*>(scene->getSelections().first());
    QVERIFY(breakerShape != nullptr);

    treeWidget->setCurrentItem(loadItem);
    QCoreApplication::processEvents();
    QCOMPARE(scene->getSelections().size(), 1);
    auto* loadShape = dynamic_cast<TmpShape*>(scene->getSelections().first());
    QVERIFY(loadShape != nullptr);
    QVERIFY(loadShape != breakerShape);

    scene->cleanSelection();
    scene->addSelection(breakerShape);
    frame.onCurrentObjectChanged({breakerShape});
    QCoreApplication::processEvents();

    QTreeWidgetItem* currentItem = treeWidget->currentItem();
    QVERIFY(currentItem != nullptr);
    QCOMPARE(currentItem->data(0, Qt::UserRole).toString(), QStringLiteral("breaker-001"));
}
