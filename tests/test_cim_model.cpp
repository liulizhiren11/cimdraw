#include "test_cim_model.h"

#include "cim/semantic/CimConnectivityNodeView.h"
#include "cim/semantic/CimDiagramObjectPointView.h"
#include "cim/semantic/CimDiagramObjectView.h"
#include "cim/semantic/CimEquipmentView.h"
#include "cim/semantic/CimIdentifiedObjectView.h"
#include "cim/semantic/CimPowerTransformerView.h"
#include "cim/semantic/CimSeriesCompensatorView.h"
#include "cim/semantic/CimShuntCompensatorView.h"
#include "cim/semantic/CimSwitchView.h"
#include "cim/semantic/CimTerminalView.h"
#include "cim/semantic/CimTransformerEndView.h"
#include "cim/diff/CimModelDiff.h"
#include "cim/model/CimClassLineage.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimProfileSet.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/exporter/CimModelSubsetExporter.h"
#include "cim/validation/CimModelValidation.h"
#include "cim/writeback/CimModelWriteback.h"

#include <QtTest>

void TestCimModel::object_attributes_roundtrip()
{
    CimObject object;
    object.mrid = QStringLiteral("mrid-001");
    object.className = QStringLiteral("Breaker");
    object.name = QStringLiteral("QF1");
    object.setAttribute(QStringLiteral("normalOpen"), false);
    object.addReference(QStringLiteral("EquipmentContainer"), QStringLiteral("bay-001"));

    QVERIFY(object.isValid());
    QCOMPARE(object.attribute(QStringLiteral("normalOpen")).toBool(), false);
    QCOMPARE(object.references.size(), 1);
    QCOMPARE(object.references.first().relationName, QStringLiteral("EquipmentContainer"));
    QCOMPARE(object.references.first().targetMrid, QStringLiteral("bay-001"));
}

void TestCimModel::model_indexes_objects_by_mrid()
{
    CimModel model;

    CimObject breaker;
    breaker.mrid = QStringLiteral("mrid-breaker");
    breaker.className = QStringLiteral("Breaker");
    breaker.name = QStringLiteral("QF1");

    CimObject load;
    load.mrid = QStringLiteral("mrid-load");
    load.className = QStringLiteral("EnergyConsumer");
    load.name = QStringLiteral("Load1");

    QVERIFY(model.addObject(breaker));
    QVERIFY(model.addObject(load));
    QVERIFY(!model.addObject(load));
    QCOMPARE(model.objectCount(), 2);
    QVERIFY(model.contains(QStringLiteral("mrid-breaker")));
    QVERIFY(model.objectByMrid(QStringLiteral("mrid-load")) != nullptr);
    QCOMPARE(model.objectsByClassName(QStringLiteral("Breaker")).size(), 1);
    QCOMPARE(model.objectsByClassName(QStringLiteral("EnergyConsumer")).size(), 1);
}

void TestCimModel::model_supports_sourcefile_queries_and_statistics()
{
    CimModel model;

    CimObject breakerA;
    breakerA.mrid = QStringLiteral("mrid-breaker-a");
    breakerA.className = QStringLiteral("Breaker");
    breakerA.name = QStringLiteral("QF-A");
    breakerA.sourceFile = QStringLiteral("eq-a.xml");
    breakerA.addReference(QStringLiteral("Terminal"), QStringLiteral("terminal-a-1"));
    breakerA.addReference(QStringLiteral("Terminal"), QStringLiteral("terminal-a-2"));

    CimObject breakerB;
    breakerB.mrid = QStringLiteral("mrid-breaker-b");
    breakerB.className = QStringLiteral("Breaker");
    breakerB.name = QStringLiteral("QF-B");
    breakerB.sourceFile = QStringLiteral("eq-a.xml");
    breakerB.addReference(QStringLiteral("Terminal"), QStringLiteral("terminal-b-1"));

    CimObject load;
    load.mrid = QStringLiteral("mrid-load-a");
    load.className = QStringLiteral("EnergyConsumer");
    load.name = QStringLiteral("Load-A");
    load.sourceFile = QStringLiteral("ssh-a.xml");

    QVERIFY(model.addObject(breakerA));
    QVERIFY(model.addObject(breakerB));
    QVERIFY(model.addObject(load));

    const QVector<CimObject> eqObjects = model.objectsBySourceFile(QStringLiteral("eq-a.xml"));
    QCOMPARE(eqObjects.size(), 2);
    QCOMPARE(eqObjects.at(0).mrid, QStringLiteral("mrid-breaker-a"));
    QCOMPARE(eqObjects.at(1).mrid, QStringLiteral("mrid-breaker-b"));
    QVERIFY(model.objectsBySourceFile(QStringLiteral("missing.xml")).isEmpty());

    const QHash<QString, int> classDistribution = model.classDistribution();
    QCOMPARE(classDistribution.value(QStringLiteral("Breaker")), 2);
    QCOMPARE(classDistribution.value(QStringLiteral("EnergyConsumer")), 1);

    const QHash<QString, int> sourceFileDistribution = model.sourceFileDistribution();
    QCOMPARE(sourceFileDistribution.value(QStringLiteral("eq-a.xml")), 2);
    QCOMPARE(sourceFileDistribution.value(QStringLiteral("ssh-a.xml")), 1);

    QCOMPARE(model.referenceCount(), 3);
}

void TestCimModel::model_iteration_order_is_stable_by_mrid()
{
    CimModel model;

    CimObject zBreaker;
    zBreaker.mrid = QStringLiteral("mrid-z");
    zBreaker.className = QStringLiteral("Breaker");
    zBreaker.name = QStringLiteral("QF-Z");

    CimObject aBreaker;
    aBreaker.mrid = QStringLiteral("mrid-a");
    aBreaker.className = QStringLiteral("Breaker");
    aBreaker.name = QStringLiteral("QF-A");

    CimObject mLoad;
    mLoad.mrid = QStringLiteral("mrid-m");
    mLoad.className = QStringLiteral("EnergyConsumer");
    mLoad.name = QStringLiteral("Load-M");

    QVERIFY(model.addObject(zBreaker));
    QVERIFY(model.addObject(aBreaker));
    QVERIFY(model.addObject(mLoad));

    const QVector<CimObject> allObjects = model.allObjects();
    QCOMPARE(allObjects.size(), 3);
    QCOMPARE(allObjects.at(0).mrid, QStringLiteral("mrid-a"));
    QCOMPARE(allObjects.at(1).mrid, QStringLiteral("mrid-m"));
    QCOMPARE(allObjects.at(2).mrid, QStringLiteral("mrid-z"));

    const QVector<CimObject> breakers = model.objectsByClassName(QStringLiteral("Breaker"));
    QCOMPARE(breakers.size(), 2);
    QCOMPARE(breakers.at(0).mrid, QStringLiteral("mrid-a"));
    QCOMPARE(breakers.at(1).mrid, QStringLiteral("mrid-z"));
}

void TestCimModel::semantic_views_expose_minimal_read_model()
{
    CimObject breaker;
    breaker.mrid = QStringLiteral("breaker-001");
    breaker.className = QStringLiteral("Breaker");
    breaker.name = QStringLiteral("QF1");
    breaker.sourceFile = QStringLiteral("eq.xml");
    breaker.setAttribute(QStringLiteral("IdentifiedObject.description"), QStringLiteral("主断路器"));
    breaker.setAttribute(QStringLiteral("IdentifiedObject.aliasName"), QStringLiteral("BRK-1"));
    breaker.setAttribute(QStringLiteral("Switch.open"), QStringLiteral("true"));
    breaker.setAttribute(QStringLiteral("Switch.normalOpen"), false);
    breaker.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-001"));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-001");
    terminal.className = QStringLiteral("Terminal");
    terminal.name = QStringLiteral("T1");
    terminal.setAttribute(QStringLiteral("ACDCTerminal.sequenceNumber"), 2);
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("breaker-001"));
    terminal.addReference(QStringLiteral("Terminal.TopologicalNode"), QStringLiteral("tn-001"));
    terminal.addReference(QStringLiteral("Terminal.ConnectivityNode"), QStringLiteral("node-001"));
    terminal.addReference(QStringLiteral("Terminal.TransformerEnd"), QStringLiteral("xf-end-001"));

    CimObject connectivityNode;
    connectivityNode.mrid = QStringLiteral("node-001");
    connectivityNode.className = QStringLiteral("ConnectivityNode");
    connectivityNode.addReference(QStringLiteral("ConnectivityNode.TopologicalNode"), QStringLiteral("tn-001"));

    CimObject transformerEnd;
    transformerEnd.mrid = QStringLiteral("xf-end-001");
    transformerEnd.className = QStringLiteral("PowerTransformerEnd");
    transformerEnd.setAttribute(QStringLiteral("TransformerEnd.endNumber"), 3);
    transformerEnd.addReference(QStringLiteral("PowerTransformerEnd.PowerTransformer"), QStringLiteral("xf-001"));
    transformerEnd.addReference(QStringLiteral("TransformerEnd.Terminal"), QStringLiteral("terminal-001"));

    CimObject diagramObject;
    diagramObject.mrid = QStringLiteral("diagram-object-001");
    diagramObject.className = QStringLiteral("DiagramObject");
    diagramObject.setAttribute(QStringLiteral("DiagramObject.rotation"), 90.0);
    diagramObject.addReference(QStringLiteral("DiagramObject.IdentifiedObject"), QStringLiteral("terminal-001"));

    CimObject diagramPoint;
    diagramPoint.mrid = QStringLiteral("diagram-point-001");
    diagramPoint.className = QStringLiteral("DiagramObjectPoint");
    diagramPoint.setAttribute(QStringLiteral("DiagramObjectPoint.xPosition"), 12.5);
    diagramPoint.setAttribute(QStringLiteral("DiagramObjectPoint.yPosition"), 34.5);
    diagramPoint.setAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), 4);
    diagramPoint.addReference(QStringLiteral("DiagramObjectPoint.DiagramObject"), QStringLiteral("diagram-object-001"));

    CimObject powerTransformer;
    powerTransformer.mrid = QStringLiteral("power-transformer-001");
    powerTransformer.className = QStringLiteral("PowerTransformer");
    powerTransformer.name = QStringLiteral("T1");
    powerTransformer.setAttribute(QStringLiteral("IdentifiedObject.description"),
                                  QStringLiteral("Auto Transformer for bay tie"));
    powerTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                  QStringLiteral("xf-end-a"));
    powerTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                  QStringLiteral("xf-end-b"));

    CimObject shuntCompensator;
    shuntCompensator.mrid = QStringLiteral("shunt-001");
    shuntCompensator.className = QStringLiteral("LinearShuntCompensator");
    shuntCompensator.name = QStringLiteral("Split Reactor");
    shuntCompensator.setAttribute(QStringLiteral("ShuntCompensator.bPerSection"), -0.0035);
    shuntCompensator.setAttribute(QStringLiteral("ShuntCompensator.maximumSections"), 2);

    CimObject seriesCompensator;
    seriesCompensator.mrid = QStringLiteral("series-001");
    seriesCompensator.className = QStringLiteral("SeriesCompensator");
    seriesCompensator.setAttribute(QStringLiteral("SeriesCompensator.x"), -0.0045);

    CimIdentifiedObjectView identifiedView(&breaker);
    QCOMPARE(identifiedView.mrid(), QStringLiteral("breaker-001"));
    QCOMPARE(identifiedView.name(), QStringLiteral("QF1"));
    QCOMPARE(identifiedView.description(), QStringLiteral("主断路器"));
    QCOMPARE(identifiedView.aliasName(), QStringLiteral("BRK-1"));
    QCOMPARE(identifiedView.sourceFile(), QStringLiteral("eq.xml"));

    CimEquipmentView equipmentView(&breaker);
    QVERIFY(equipmentView.isEquipment());
    QCOMPARE(equipmentView.equipmentContainerMrid(), QStringLiteral("bay-001"));

    CimSwitchView switchView(&breaker);
    QVERIFY(switchView.isSwitch());
    QVERIFY(switchView.isOpen());
    QCOMPARE(switchView.normalOpen(), false);

    CimTerminalView terminalView(&terminal);
    QVERIFY(terminalView.isTerminal());
    QCOMPARE(terminalView.conductingEquipmentMrid(), QStringLiteral("breaker-001"));
    QCOMPARE(terminalView.topologicalNodeMrid(), QStringLiteral("tn-001"));
    QCOMPARE(terminalView.connectivityNodeMrid(), QStringLiteral("node-001"));
    QCOMPARE(terminalView.transformerEndMrid(), QStringLiteral("xf-end-001"));
    QCOMPARE(terminalView.sequenceNumber(), 2);

    CimConnectivityNodeView connectivityNodeView(&connectivityNode);
    QVERIFY(connectivityNodeView.isConnectivityNode());
    QCOMPARE(connectivityNodeView.topologicalNodeMrid(), QStringLiteral("tn-001"));

    CimTransformerEndView transformerEndView(&transformerEnd);
    QVERIFY(transformerEndView.isTransformerEnd());
    QCOMPARE(transformerEndView.powerTransformerMrid(), QStringLiteral("xf-001"));
    QCOMPARE(transformerEndView.terminalMrid(), QStringLiteral("terminal-001"));
    QCOMPARE(transformerEndView.endNumber(), 3);

    CimDiagramObjectView diagramObjectView(&diagramObject);
    QVERIFY(diagramObjectView.isDiagramObject());
    QCOMPARE(diagramObjectView.identifiedObjectMrid(), QStringLiteral("terminal-001"));
    QVERIFY(diagramObjectView.hasRotation());
    QCOMPARE(diagramObjectView.rotation(), 90.0);

    CimDiagramObjectPointView diagramPointView(&diagramPoint);
    QVERIFY(diagramPointView.isDiagramObjectPoint());
    QCOMPARE(diagramPointView.diagramObjectMrid(), QStringLiteral("diagram-object-001"));
    QVERIFY(diagramPointView.hasPosition());
    QCOMPARE(diagramPointView.position(), QPointF(12.5, 34.5));
    QCOMPARE(diagramPointView.sequenceNumber(), 4);

    CimPowerTransformerView powerTransformerView(&powerTransformer);
    QVERIFY(powerTransformerView.isPowerTransformer());
    QCOMPARE(powerTransformerView.transformerEndCount(), 2);
    QVERIFY(powerTransformerView.looksLikeAutoTransformer());

    CimShuntCompensatorView shuntCompensatorView(&shuntCompensator);
    QVERIFY(shuntCompensatorView.isShuntCompensatorFamily());
    QVERIFY(shuntCompensatorView.looksLikeReactor());
    QVERIFY(shuntCompensatorView.looksLikeSplitReactor());

    CimSeriesCompensatorView seriesCompensatorView(&seriesCompensator);
    QVERIFY(seriesCompensatorView.isSeriesCompensator());
    QVERIFY(seriesCompensatorView.looksLikeCapacitor());
}

void TestCimModel::model_validation_reports_missing_references_and_graphic_binding_gaps()
{
    CimModel emptyModel;
    const CimModelValidationReport emptyReport = validateCimModelStructure(emptyModel);
    QVERIFY(emptyReport.ok());
    QCOMPARE(emptyReport.summary.warningCount, 1);
    QCOMPARE(static_cast<int>(emptyReport.issues.first().code),
             static_cast<int>(CimValidationIssueCode::EmptyModel));

    CimModel model;

    CimObject breaker;
    breaker.mrid = QStringLiteral("breaker-001");
    breaker.className = QStringLiteral("Breaker");
    breaker.addReference(QStringLiteral("Terminal"), QStringLiteral("terminal-001"));
    breaker.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-missing"));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-001");
    terminal.className = QStringLiteral("Terminal");
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("breaker-001"));

    CimObject load;
    load.mrid = QStringLiteral("load-001");
    load.className = QStringLiteral("EnergyConsumer");

    QVERIFY(model.addObject(breaker));
    QVERIFY(model.addObject(terminal));
    QVERIFY(model.addObject(load));

    CimIdIndex graphicIndex;
    graphicIndex.bindGraphicItem(QStringLiteral("breaker-001"), QStringLiteral("graphic-breaker-001"));
    graphicIndex.bindGraphicItem(QStringLiteral("ghost-001"), QStringLiteral("graphic-ghost-001"));

    CimModelValidationOptions options;
    options.requireGraphicBindingsForMappableObjects = true;
    const CimModelValidationReport report = validateCimModelStructure(model, &graphicIndex, options);

    QVERIFY(!report.ok());
    QCOMPARE(report.summary.objectCount, 3);
    QCOMPARE(report.summary.referenceCount, 3);
    QCOMPARE(report.summary.mappableObjectCount, 2);
    QCOMPARE(report.summary.graphicBindingCount, 2);
    QCOMPARE(report.summary.missingGraphicBindingCount, 1);
    QCOMPARE(report.summary.danglingGraphicBindingCount, 1);
    QCOMPARE(report.summary.errorCount, 2);
    QCOMPARE(report.summary.warningCount, 1);

    bool sawMissingReference = false;
    bool sawDanglingBinding = false;
    bool sawMissingGraphicBinding = false;
    for (const CimValidationIssue& issue : report.issues)
    {
        if (issue.code == CimValidationIssueCode::MissingReference)
        {
            sawMissingReference = true;
            QCOMPARE(issue.objectMrid, QStringLiteral("breaker-001"));
            QCOMPARE(issue.targetMrid, QStringLiteral("bay-missing"));
        }
        if (issue.code == CimValidationIssueCode::DanglingGraphicBinding)
        {
            sawDanglingBinding = true;
            QCOMPARE(issue.objectMrid, QStringLiteral("ghost-001"));
            QCOMPARE(issue.graphicItemId, QStringLiteral("graphic-ghost-001"));
        }
        if (issue.code == CimValidationIssueCode::MissingGraphicBinding)
        {
            sawMissingGraphicBinding = true;
            QCOMPARE(issue.objectMrid, QStringLiteral("load-001"));
        }
    }

    QVERIFY(sawMissingReference);
    QVERIFY(sawDanglingBinding);
    QVERIFY(sawMissingGraphicBinding);
    QVERIFY(report.messages().join(QLatin1Char('\n')).contains(QStringLiteral("引用目标不存在")));
}

void TestCimModel::model_subset_exporter_writes_minimal_xml_and_report()
{
    CimModel model;

    CimObject breaker;
    breaker.mrid = QStringLiteral("breaker-export-001");
    breaker.className = QStringLiteral("Breaker");
    breaker.name = QStringLiteral("QF Export");
    breaker.sourceFile = QStringLiteral("EQ.xml");
    breaker.profileTags = {QStringLiteral("EQ"), QStringLiteral("SSH")};
    breaker.setAttribute(QStringLiteral("Switch.open"), false);
    breaker.addReference(QStringLiteral("Terminal"), QStringLiteral("terminal-export-001"));
    breaker.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-missing"));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-export-001");
    terminal.className = QStringLiteral("Terminal");
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("breaker-export-001"));

    QVERIFY(model.addObject(breaker));
    QVERIFY(model.addObject(terminal));

    CimIdIndex graphicIndex;
    graphicIndex.bindGraphicItem(QStringLiteral("breaker-export-001"),
                                 QStringLiteral("graphic-breaker-export-001"));

    CimModelSubsetExportOptions options;
    options.mrids = {QStringLiteral("breaker-export-001"), QStringLiteral("missing-export-001")};
    options.includeReferencedObjects = true;
    const CimModelSubsetExportResult result = exportCimModelSubsetToXml(model, options, &graphicIndex);

    QVERIFY(!result.success);
    QCOMPARE(result.summary.requestedCount, 2);
    QCOMPARE(result.summary.exportedObjectCount, 2);
    QCOMPARE(result.summary.missingObjectCount, 1);
    QCOMPARE(result.summary.validationErrorCount, 1);
    QCOMPARE(result.missingMrids, QStringList({QStringLiteral("missing-export-001")}));

    const QString xml = result.xmlText();
    QVERIFY(xml.contains(QStringLiteral("<cimSubsetExport")));
    QVERIFY(xml.contains(QStringLiteral("projection=\"PowerSystemModel\"")));
    QVERIFY(xml.contains(QStringLiteral("mrid=\"breaker-export-001\"")));
    QVERIFY(xml.contains(QStringLiteral("graphicItemId=\"graphic-breaker-export-001\"")));
    QVERIFY(xml.contains(QStringLiteral("name=\"Switch.open\"")));
    QVERIFY(xml.contains(QStringLiteral("targetMrid=\"terminal-export-001\"")));
    QVERIFY(xml.contains(QStringLiteral("targetMrid=\"bay-missing\"")));
    QVERIFY(xml.contains(QStringLiteral("missing=\"true\"")));
    QVERIFY(xml.contains(QStringLiteral("mrid=\"missing-export-001\"")));
    QVERIFY(xml.contains(QStringLiteral("missingReference")));

    const QString reportText = result.reportLines().join(QLatin1Char('\n'));
    QVERIFY(reportText.contains(QStringLiteral("导出成功：否")));
    QVERIFY(reportText.contains(QStringLiteral("缺失对象：missing-export-001")));
    QVERIFY(reportText.contains(QStringLiteral("引用目标不存在")));
}

void TestCimModel::model_writeback_applies_controlled_key_object_patch()
{
    CimModel model;

    CimObject breaker;
    breaker.mrid = QStringLiteral("breaker-writeback-001");
    breaker.className = QStringLiteral("Breaker");
    breaker.name = QStringLiteral("Old Breaker");
    breaker.setAttribute(QStringLiteral("Switch.open"), true);
    breaker.setAttribute(QStringLiteral("IdentifiedObject.aliasName"), QStringLiteral("OLD"));
    breaker.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-old"));

    CimObject bay;
    bay.mrid = QStringLiteral("bay-new");
    bay.className = QStringLiteral("Bay");

    QVERIFY(model.addObject(breaker));
    QVERIFY(model.addObject(bay));

    CimModelObjectPatch patch;
    patch.mrid = QStringLiteral("breaker-writeback-001");
    patch.updateName = true;
    patch.name = QStringLiteral("New Breaker");
    patch.attributesToSet.insert(QStringLiteral("Switch.open"), false);
    patch.attributesToSet.insert(QStringLiteral("IdentifiedObject.description"), QStringLiteral("回写后的断路器"));
    patch.attributesToRemove << QStringLiteral("IdentifiedObject.aliasName");
    patch.referencesToRemove.push_back({QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-old")});
    patch.referencesToAdd.push_back({QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-new")});

    CimModelObjectPatch missingPatch;
    missingPatch.mrid = QStringLiteral("missing-writeback-001");
    missingPatch.updateName = true;
    missingPatch.name = QStringLiteral("Should Not Create");

    const CimModelWritebackResult result = applyCimModelObjectPatches(
        model,
        {patch, missingPatch});

    QVERIFY(!result.ok());
    QCOMPARE(result.appliedCount, 1);
    QCOMPARE(result.rejectedCount, 1);
    QCOMPARE(static_cast<int>(result.items.first().status),
             static_cast<int>(CimModelWritebackStatus::Applied));
    QCOMPARE(result.items.first().changedFieldCount, 6);
    QCOMPARE(static_cast<int>(result.items.last().status),
             static_cast<int>(CimModelWritebackStatus::MissingObject));
    QVERIFY(result.messages().join(QLatin1Char('\n')).contains(QStringLiteral("只能回写已存在的关键对象")));

    const CimObject* updatedBreaker = model.objectByMrid(QStringLiteral("breaker-writeback-001"));
    QVERIFY(updatedBreaker != nullptr);
    QCOMPARE(updatedBreaker->className, QStringLiteral("Breaker"));
    QCOMPARE(updatedBreaker->name, QStringLiteral("New Breaker"));
    QCOMPARE(updatedBreaker->attribute(QStringLiteral("Switch.open")).toBool(), false);
    QCOMPARE(updatedBreaker->attribute(QStringLiteral("IdentifiedObject.description")).toString(),
             QStringLiteral("回写后的断路器"));
    QVERIFY(!updatedBreaker->attributeMap.contains(QStringLiteral("IdentifiedObject.aliasName")));
    QCOMPARE(updatedBreaker->references.size(), 1);
    QCOMPARE(updatedBreaker->references.first().relationName,
             QStringLiteral("Equipment.EquipmentContainer"));
    QCOMPARE(updatedBreaker->references.first().targetMrid, QStringLiteral("bay-new"));
    QVERIFY(model.objectByMrid(QStringLiteral("missing-writeback-001")) == nullptr);
}

void TestCimModel::model_diff_reports_added_removed_and_changed_objects()
{
    CimModel beforeModel;
    CimObject breakerBefore;
    breakerBefore.mrid = QStringLiteral("breaker-diff-001");
    breakerBefore.className = QStringLiteral("Breaker");
    breakerBefore.name = QStringLiteral("Old Breaker");
    breakerBefore.setAttribute(QStringLiteral("Switch.open"), true);
    breakerBefore.setAttribute(QStringLiteral("IdentifiedObject.aliasName"), QStringLiteral("OLD"));
    breakerBefore.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-old"));

    CimObject removedLoad;
    removedLoad.mrid = QStringLiteral("load-removed-001");
    removedLoad.className = QStringLiteral("EnergyConsumer");

    QVERIFY(beforeModel.addObject(breakerBefore));
    QVERIFY(beforeModel.addObject(removedLoad));

    CimModel afterModel;
    CimObject breakerAfter = breakerBefore;
    breakerAfter.name = QStringLiteral("New Breaker");
    breakerAfter.setAttribute(QStringLiteral("Switch.open"), false);
    breakerAfter.attributeMap.remove(QStringLiteral("IdentifiedObject.aliasName"));
    breakerAfter.setAttribute(QStringLiteral("IdentifiedObject.description"),
                              QStringLiteral("diff description"));
    breakerAfter.references.clear();
    breakerAfter.addReference(QStringLiteral("Equipment.EquipmentContainer"), QStringLiteral("bay-new"));

    CimObject addedGenerator;
    addedGenerator.mrid = QStringLiteral("generator-added-001");
    addedGenerator.className = QStringLiteral("SynchronousMachine");

    QVERIFY(afterModel.addObject(breakerAfter));
    QVERIFY(afterModel.addObject(addedGenerator));

    const CimModelDiffReport report = diffCimModels(beforeModel, afterModel);

    QVERIFY(!report.empty());
    QCOMPARE(report.summary.addedObjectCount, 1);
    QCOMPARE(report.summary.removedObjectCount, 1);
    QCOMPARE(report.summary.changedObjectCount, 3);
    QCOMPARE(report.summary.changeCount, 8);
    QCOMPARE(report.changedMrids(),
             QStringList({QStringLiteral("breaker-diff-001"),
                          QStringLiteral("generator-added-001"),
                          QStringLiteral("load-removed-001")}));

    bool sawNameChange = false;
    bool sawAttributeAdded = false;
    bool sawAttributeRemoved = false;
    bool sawAttributeChanged = false;
    bool sawReferenceAdded = false;
    bool sawReferenceRemoved = false;
    for (const CimModelDiffChange& change : report.changes)
    {
        if (change.kind == CimModelDiffChangeKind::NameChanged)
            sawNameChange = true;
        if (change.kind == CimModelDiffChangeKind::AttributeAdded
            && change.fieldName == QStringLiteral("IdentifiedObject.description"))
            sawAttributeAdded = true;
        if (change.kind == CimModelDiffChangeKind::AttributeRemoved
            && change.fieldName == QStringLiteral("IdentifiedObject.aliasName"))
            sawAttributeRemoved = true;
        if (change.kind == CimModelDiffChangeKind::AttributeChanged
            && change.fieldName == QStringLiteral("Switch.open"))
            sawAttributeChanged = true;
        if (change.kind == CimModelDiffChangeKind::ReferenceAdded
            && change.newValue.contains(QStringLiteral("bay-new")))
            sawReferenceAdded = true;
        if (change.kind == CimModelDiffChangeKind::ReferenceRemoved
            && change.oldValue.contains(QStringLiteral("bay-old")))
            sawReferenceRemoved = true;
    }

    QVERIFY(sawNameChange);
    QVERIFY(sawAttributeAdded);
    QVERIFY(sawAttributeRemoved);
    QVERIFY(sawAttributeChanged);
    QVERIFY(sawReferenceAdded);
    QVERIFY(sawReferenceRemoved);
    const QString reportText = report.reportLines().join(QLatin1Char('\n'));
    QVERIFY(reportText.contains(QStringLiteral("新增对象数：1")));
    QVERIFY(reportText.contains(QStringLiteral("属性变化[breaker-diff-001] Switch.open")));
}

void TestCimModel::profile_set_tracks_unique_profiles()
{
    CimProfileSet profiles;
    profiles.addProfile(QStringLiteral("EQ"));
    profiles.addProfile(QStringLiteral("SSH"));
    profiles.addProfile(QStringLiteral("EQ"));

    QVERIFY(profiles.contains(QStringLiteral("EQ")));
    QVERIFY(profiles.contains(QStringLiteral("SSH")));
    QCOMPARE(profiles.allProfiles().size(), 2);
}

void TestCimModel::class_lineage_resolves_complete_upstream_hierarchy()
{
    QVERIFY(CimClassLineage::isA(QStringLiteral("Terminal"), QStringLiteral("ACDCTerminal")));
    QVERIFY(CimClassLineage::isA(QStringLiteral("Terminal"), QStringLiteral("IdentifiedObject")));
    QVERIFY(CimClassLineage::isA(QStringLiteral("VoltageLevel"), QStringLiteral("EquipmentContainer")));
    QVERIFY(CimClassLineage::isA(QStringLiteral("VoltageLevel"), QStringLiteral("ConnectivityNodeContainer")));
    QVERIFY(CimClassLineage::isA(QStringLiteral("VoltageLevel"), QStringLiteral("PowerSystemResource")));
    QVERIFY(CimClassLineage::isA(QStringLiteral("VoltageLevel"), QStringLiteral("IdentifiedObject")));
}

void TestCimModel::class_lineage_rejects_removed_local_compatibility_shortcuts()
{
    QVERIFY(!CimClassLineage::isA(QStringLiteral("CurrentTransformer"), QStringLiteral("ConductingEquipment")));
    QVERIFY(!CimClassLineage::isA(QStringLiteral("PotentialTransformer"), QStringLiteral("ConductingEquipment")));
    QVERIFY(!CimClassLineage::isA(QStringLiteral("VoltageTransformer"), QStringLiteral("ConductingEquipment")));
    QVERIFY(!CimClassLineage::isA(QStringLiteral("Fuse"), QStringLiteral("ConductingEquipment")));
    QVERIFY(!CimClassLineage::isA(QStringLiteral("SurgeArrester"), QStringLiteral("ConductingEquipment")));
}
