#include "test_cim_graphics.h"

#include "CimdrawScene.h"
#include "Tool/CimdrawTool.h"
#include "cim/importer/CgmesPackageImporter.h"
#include "cim/mapping/CimGraphicMapper.h"
#include "cim/ui/CimSceneBuilder.h"
#include "wiring/power/CimdrawPowerCurrentTransformerItem.h"
#include "wiring/power/CimdrawPowerCapacitorItem.h"
#include "wiring/power/CimdrawPowerEssItem.h"
#include "wiring/power/CimdrawPowerFuseItem.h"
#include "wiring/power/CimdrawPowerGeneratorItem.h"
#include "wiring/power/CimdrawPowerGridItem.h"
#include "wiring/power/CimdrawPowerJunctionItem.h"
#include "wiring/power/CimdrawPowerArcCoilItem.h"
#include "wiring/power/CimdrawPowerAutoXfmrItem.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"
#include "wiring/power/CimdrawPowerDisconnectorItem.h"
#include "wiring/power/CimdrawPowerEarthingXfmrItem.h"
#include "wiring/power/CimdrawPowerLoadItem.h"
#include "wiring/power/CimdrawPowerPvInfeedItem.h"
#include "wiring/power/CimdrawPowerSplitReactorItem.h"
#include "wiring/power/CimdrawPowerSvgCompItem.h"
#include "wiring/power/CimdrawPowerReactorItem.h"
#include "wiring/power/CimdrawPowerStationXfmrItem.h"
#include "wiring/power/CimdrawPowerTransformer2wItem.h"
#include "wiring/power/CimdrawPowerTransformer3wItem.h"
#include "wiring/power/CimdrawPowerVoltageTransformerItem.h"
#include "wiring/power/CimdrawPowerWindInfeedItem.h"
#include "wiring/power/CimdrawPowerCircuitBreakerItem.h"
#include "item/CimdrawConnectLine.h"
#include "item/CimdrawConnectPoint.h"
#include "CimdrawConnectConfig.h"

#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

namespace {

bool writeUtf8File(const QString& path, const QString& content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    return file.write(content.toUtf8()) >= 0;
}

QString mediumCigrePublicPath()
{
    return QFINDTESTDATA("tests/data/cim/medium/CIGRE_MV");
}

QString mediumSwitchesSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/Sample_Grid_Switches_NodeBreaker");
}

int normalizedDegrees(qreal rotation)
{
    const int rounded = qRound(rotation);
    const int normalized = rounded % 360;
    return normalized < 0 ? normalized + 360 : normalized;
}

CimdrawConnectLine* findGeneratedHelperLineBetween(CimdrawScene& scene, QGraphicsItem* first, QGraphicsItem* second)
{
    for (QGraphicsItem* item : scene.items())
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (!line || !line->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            continue;
        if ((line->getStartItem() == first && line->getEndItem() == second)
            || (line->getStartItem() == second && line->getEndItem() == first))
        {
            return line;
        }
    }
    return nullptr;
}

CimdrawConnectPoint* linePortOnItem(CimdrawConnectLine* line, QGraphicsItem* item)
{
    if (!line || !item)
        return nullptr;
    if (line->getStartItem() == item)
        return line->startConnectPort();
    if (line->getEndItem() == item)
        return line->endConnectPort();
    return nullptr;
}

QPointF normalizedSceneDirection(const QPointF& vector)
{
    const qreal length = QLineF(QPointF(), vector).length();
    if (length < 1e-6)
        return {};
    return QPointF(vector.x() / length, vector.y() / length);
}

QPointF baseUnitForDirection(CONNECT_DIRECTION direction)
{
    switch (direction)
    {
    case TOP_DIRECTION:
        return QPointF(0.0, -1.0);
    case RIGHT_DIRECTION:
        return QPointF(1.0, 0.0);
    case BOTTOM_DIRECTION:
        return QPointF(0.0, 1.0);
    case LEFT_DIRECTION:
        return QPointF(-1.0, 0.0);
    default:
        return {};
    }
}

QPointF rotatedSceneDirectionForPort(CimdrawConnectPoint* port)
{
    if (!port)
        return {};
    const QPointF declaredUnit = baseUnitForDirection(port->getDirection());
    if (declaredUnit.isNull())
        return {};
    const QPointF localAnchor = port->connectionCenterLocal();
    const QPointF sceneAnchor = port->mapToScene(localAnchor);
    const QPointF sceneNeighbor = port->mapToScene(localAnchor + declaredUnit);
    return normalizedSceneDirection(sceneNeighbor - sceneAnchor);
}

QPointF firstDistinctStepDirectionFromItem(CimdrawConnectLine* line, QGraphicsItem* item)
{
    if (!line || !item)
        return {};

    QVector<QPointF> path = line->pathInSceneCoords();
    if (path.size() < 2)
        return {};

    if (line->getEndItem() == item)
    {
        QVector<QPointF> reversed;
        reversed.reserve(path.size());
        for (int index = path.size() - 1; index >= 0; --index)
            reversed.append(path.at(index));
        path = reversed;
    }
    else if (line->getStartItem() != item)
    {
        return {};
    }

    const QPointF anchor = path.first();
    for (int index = 1; index < path.size(); ++index)
    {
        const QPointF delta = path.at(index) - anchor;
        const QPointF unit = normalizedSceneDirection(delta);
        if (!unit.isNull())
            return unit;
    }
    return {};
}

QPointF sceneCenterForPort(CimdrawConnectPoint* port)
{
    if (!port)
        return {};
    return port->connectionCenterInScene();
}

void assertPortDirectionOnItem(CimdrawConnectLine* line, QGraphicsItem* item, CONNECT_DIRECTION expectedDirection)
{
    QVERIFY(line != nullptr);
    CimdrawConnectPoint* port = linePortOnItem(line, item);
    QVERIFY(port != nullptr);
    QCOMPARE(port->getDirection(), expectedDirection);
}

void assertLineExitsItemAlongRotatedPortNormal(CimdrawConnectLine* line,
                                               QGraphicsItem* item,
                                               CONNECT_DIRECTION expectedDirection)
{
    assertPortDirectionOnItem(line, item, expectedDirection);

    CimdrawConnectPoint* port = linePortOnItem(line, item);
    QVERIFY(port != nullptr);

    const QPointF expectedUnit = rotatedSceneDirectionForPort(port);
    const QPointF actualUnit = firstDistinctStepDirectionFromItem(line, item);
    QVERIFY(!expectedUnit.isNull());
    QVERIFY(!actualUnit.isNull());

    const qreal dot = expectedUnit.x() * actualUnit.x() + expectedUnit.y() * actualUnit.y();
    QVERIFY2(dot > 0.9,
             qPrintable(QStringLiteral("expected first step (%1, %2), actual (%3, %4)")
                            .arg(expectedUnit.x(), 0, 'f', 3)
                            .arg(expectedUnit.y(), 0, 'f', 3)
                            .arg(actualUnit.x(), 0, 'f', 3)
                            .arg(actualUnit.y(), 0, 'f', 3)));
}

}

void TestCimGraphics::tool_manager_registers_missing_graphics()
{
    auto* toolMgr = CimdrawToolManager::getInstance();
    CimGraphicMapper mapper;

    CimdrawPowerCurrentTransformerItem currentTransformer(QRectF(0, 0, 72, 48));
    CimdrawPowerVoltageTransformerItem voltageTransformer(QRectF(0, 0, 78, 52));
    CimdrawPowerFuseItem fuse(QRectF(0, 0, 84, 32));
    CimdrawPowerJunctionItem junction(QRectF(0, 0, 28, 28));

    CimObject connector;
    connector.mrid = QStringLiteral("connector-001");
    connector.className = QStringLiteral("Connector");

    QVERIFY(toolMgr->changeTool(currentTransformer.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(voltageTransformer.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(fuse.drawTypeForXml()) != nullptr);
    QVERIFY(toolMgr->changeTool(junction.drawTypeForXml()) != nullptr);
    QCOMPARE(mapper.drawTypeForObject(connector), CIMDRAW_WSYM_JUNCTION);
    QVERIFY(mapper.canMap(connector));
}

void TestCimGraphics::importer_and_scene_builder_materialize_missing_graphics()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-graphics-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:CurrentTransformer rdf:ID="ct-001">
    <cim:IdentifiedObject.name>CT1</cim:IdentifiedObject.name>
  </cim:CurrentTransformer>
  <cim:PotentialTransformer rdf:ID="pt-001">
    <cim:IdentifiedObject.name>PT1</cim:IdentifiedObject.name>
  </cim:PotentialTransformer>
  <cim:VoltageTransformer rdf:ID="vt-001">
    <cim:IdentifiedObject.name>VT1</cim:IdentifiedObject.name>
  </cim:VoltageTransformer>
  <cim:Fuse rdf:ID="fuse-001">
    <cim:IdentifiedObject.name>FU1</cim:IdentifiedObject.name>
  </cim:Fuse>
  <cim:Terminal rdf:ID="terminal-ct-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#ct-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-measure"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-pt-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#pt-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-measure"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-vt-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#vt-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-measure"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-fuse-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#fuse-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-fuse"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString sshXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-graphics-ssh">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/SteadyStateHypothesis-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:CurrentTransformer rdf:about="#ct-001"/>
  <cim:PotentialTransformer rdf:about="#pt-001"/>
  <cim:VoltageTransformer rdf:about="#vt-001"/>
  <cim:Fuse rdf:about="#fuse-001"/>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("graphics_EQ.xml")), eqXml),
             "failed to write graphics EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("graphics_SSH.xml")), sshXml),
             "failed to write graphics SSH sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    QVERIFY(dynamic_cast<CimdrawPowerCurrentTransformerItem*>(buildResult.shapeByMrid.value(QStringLiteral("ct-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerVoltageTransformerItem*>(buildResult.shapeByMrid.value(QStringLiteral("pt-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerVoltageTransformerItem*>(buildResult.shapeByMrid.value(QStringLiteral("vt-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerFuseItem*>(buildResult.shapeByMrid.value(QStringLiteral("fuse-001"))) != nullptr);
}

void TestCimGraphics::importer_and_scene_builder_materialize_projection_infeed_graphics()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-infeed-eq">
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

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("infeed_EQ.xml")), eqXml),
             "failed to write infeed EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    QVERIFY(dynamic_cast<CimdrawPowerPvInfeedItem*>(buildResult.shapeByMrid.value(QStringLiteral("solar-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerWindInfeedItem*>(buildResult.shapeByMrid.value(QStringLiteral("wind-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerEssItem*>(buildResult.shapeByMrid.value(QStringLiteral("battery-001"))) != nullptr);
}

void TestCimGraphics::importer_and_scene_builder_preserve_projection_infeed_layout_from_diagram_objects()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-infeed-layout-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SolarGeneratingUnit rdf:ID="solar-layout-001">
    <cim:IdentifiedObject.name>PV-L1</cim:IdentifiedObject.name>
  </cim:SolarGeneratingUnit>
  <cim:WindGeneratingUnit rdf:ID="wind-layout-001">
    <cim:IdentifiedObject.name>WIND-L1</cim:IdentifiedObject.name>
  </cim:WindGeneratingUnit>
  <cim:BatteryUnit rdf:ID="battery-layout-001">
    <cim:IdentifiedObject.name>ESS-L1</cim:IdentifiedObject.name>
  </cim:BatteryUnit>
  <cim:Terminal rdf:ID="terminal-solar-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#solar-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-infeed-layout-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-wind-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#wind-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-infeed-layout-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-battery-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#battery-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-infeed-layout-c"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-infeed-layout-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-solar-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#solar-layout-001"/>
    <cim:DiagramObject.rotation>180</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-solar-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-solar-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>100</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-wind-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#wind-layout-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-wind-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-wind-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>300</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>220</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-battery-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#battery-layout-001"/>
    <cim:DiagramObject.rotation>90</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-battery-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-battery-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>500</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>300</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("infeed_layout_EQ.xml")), eqXml),
             "failed to write infeed layout EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("infeed_layout_DL.xml")), dlXml),
             "failed to write infeed layout DL sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* solar =
        dynamic_cast<CimdrawPowerPvInfeedItem*>(buildResult.shapeByMrid.value(QStringLiteral("solar-layout-001")));
    auto* wind =
        dynamic_cast<CimdrawPowerWindInfeedItem*>(buildResult.shapeByMrid.value(QStringLiteral("wind-layout-001")));
    auto* battery =
        dynamic_cast<CimdrawPowerEssItem*>(buildResult.shapeByMrid.value(QStringLiteral("battery-layout-001")));

    QVERIFY(solar != nullptr);
    QVERIFY(wind != nullptr);
    QVERIFY(battery != nullptr);

    QCOMPARE(qRound(solar->scenePos().x()), 80);
    QCOMPARE(qRound(solar->scenePos().y()), 80);
    QCOMPARE(qRound(solar->rotation()), 180);

    QCOMPARE(qRound(wind->scenePos().x()), 440);
    QCOMPARE(qRound(wind->scenePos().y()), 296);
    QCOMPARE(qRound(wind->rotation()), 270);

    QCOMPARE(qRound(battery->scenePos().x()), 800);
    QCOMPARE(qRound(battery->scenePos().y()), 440);
    QCOMPARE(qRound(battery->rotation()), 90);

    QVERIFY(solar->scenePos().x() < wind->scenePos().x());
    QVERIFY(wind->scenePos().x() < battery->scenePos().x());
}

void TestCimGraphics::importer_and_scene_builder_materialize_source_and_equivalent_grid_graphics()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-source-grid-eq">
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
             "failed to write source/grid EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    QVERIFY(dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.shapeByMrid.value(QStringLiteral("generator-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("external-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("equivalent-001"))) != nullptr);

    source_grid_load_default_ports_follow_node_anchor();
}

void TestCimGraphics::importer_and_scene_builder_preserve_source_and_equivalent_grid_layout_from_diagram_objects()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-source-grid-layout-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SynchronousMachine rdf:ID="generator-layout-001">
    <cim:IdentifiedObject.name>G-L1</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:ExternalNetworkInjection rdf:ID="external-layout-001">
    <cim:IdentifiedObject.name>Grid-L1</cim:IdentifiedObject.name>
  </cim:ExternalNetworkInjection>
  <cim:EquivalentInjection rdf:ID="equivalent-layout-001">
    <cim:IdentifiedObject.name>Eq-L1</cim:IdentifiedObject.name>
  </cim:EquivalentInjection>
  <cim:Terminal rdf:ID="terminal-generator-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#generator-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-generator-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-external-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#external-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-external-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-equivalent-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#equivalent-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-equivalent-layout"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-source-grid-layout-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-generator-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#generator-layout-001"/>
    <cim:DiagramObject.rotation>180</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-generator-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-generator-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>100</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-external-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#external-layout-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-external-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-external-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>300</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>180</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-equivalent-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#equivalent-layout-001"/>
    <cim:DiagramObject.rotation>90</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-equivalent-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-equivalent-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>500</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>300</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("source_grid_layout_EQ.xml")), eqXml),
             "failed to write source/grid layout EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("source_grid_layout_DL.xml")), dlXml),
             "failed to write source/grid layout DL sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* generator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.shapeByMrid.value(QStringLiteral("generator-layout-001")));
    auto* external =
        dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("external-layout-001")));
    auto* equivalent =
        dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("equivalent-layout-001")));

    QVERIFY(generator != nullptr);
    QVERIFY(external != nullptr);
    QVERIFY(equivalent != nullptr);

    QCOMPARE(qRound(generator->scenePos().x()), 80);
    QCOMPARE(qRound(generator->scenePos().y()), 80);
    QCOMPARE(qRound(generator->rotation()), 180);

    QCOMPARE(qRound(external->scenePos().x()), 440);
    QCOMPARE(qRound(external->scenePos().y()), 224);
    QCOMPARE(qRound(external->rotation()), 270);

    QCOMPARE(qRound(equivalent->scenePos().x()), 800);
    QCOMPARE(qRound(equivalent->scenePos().y()), 440);
    QCOMPARE(qRound(equivalent->rotation()), 90);

    QVERIFY(generator->scenePos().x() < external->scenePos().x());
    QVERIFY(external->scenePos().x() < equivalent->scenePos().x());
}

void TestCimGraphics::source_grid_load_default_ports_follow_node_anchor()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-source-grid-load-default-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SynchronousMachine rdf:ID="generator-default-001">
    <cim:IdentifiedObject.name>G-Default</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:ExternalNetworkInjection rdf:ID="grid-default-001">
    <cim:IdentifiedObject.name>Grid-Default</cim:IdentifiedObject.name>
  </cim:ExternalNetworkInjection>
  <cim:EnergyConsumer rdf:ID="load-default-001">
    <cim:IdentifiedObject.name>Load-Default</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:Terminal rdf:ID="terminal-generator-default-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#generator-default-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-default-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-grid-default-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#grid-default-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-default-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-default-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-default-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-default-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-grid-default-002">
    <cim:Terminal.ConductingEquipment rdf:resource="#grid-default-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-default-b"/>
  </cim:Terminal>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("source_grid_load_default_EQ.xml")), eqXml),
             "failed to write source/grid/load default EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* generator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.shapeByMrid.value(QStringLiteral("generator-default-001")));
    auto* grid =
        dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("grid-default-001")));
    auto* load =
        dynamic_cast<CimdrawPowerLoadItem*>(buildResult.shapeByMrid.value(QStringLiteral("load-default-001")));

    QVERIFY(generator != nullptr);
    QVERIFY(grid != nullptr);
    QVERIFY(load != nullptr);

    QVERIFY(generator->scenePos().y() < load->scenePos().y());
    QVERIFY(grid->scenePos().y() < load->scenePos().y());

    QList<CimdrawConnectLine*> helperLines;
    for (QGraphicsItem* item : scene.items())
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (line && line->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            helperLines.push_back(line);
    }

    QCOMPARE(helperLines.size(), 2);

    CimdrawConnectLine* generatorToGrid = nullptr;
    CimdrawConnectLine* gridToLoad = nullptr;
    for (CimdrawConnectLine* line : helperLines)
    {
        QGraphicsItem* startItem = line->getStartItem();
        QGraphicsItem* endItem = line->getEndItem();
        const bool isGeneratorGrid =
            (startItem == generator && endItem == grid) || (startItem == grid && endItem == generator);
        const bool isGridLoad =
            (startItem == grid && endItem == load) || (startItem == load && endItem == grid);
        if (isGeneratorGrid)
            generatorToGrid = line;
        else if (isGridLoad)
            gridToLoad = line;
    }

    QVERIFY(generatorToGrid != nullptr);
    QVERIFY(gridToLoad != nullptr);
    QVERIFY(generatorToGrid->startConnectPort() != nullptr);
    QVERIFY(generatorToGrid->endConnectPort() != nullptr);
    QVERIFY(gridToLoad->startConnectPort() != nullptr);
    QVERIFY(gridToLoad->endConnectPort() != nullptr);

    CimdrawConnectPoint* generatorPort = linePortOnItem(generatorToGrid, generator);
    CimdrawConnectPoint* gridPortToGenerator = linePortOnItem(generatorToGrid, grid);
    CimdrawConnectPoint* gridPortToLoad = linePortOnItem(gridToLoad, grid);
    CimdrawConnectPoint* loadPort = linePortOnItem(gridToLoad, load);

    QVERIFY(generatorPort != nullptr);
    QVERIFY(gridPortToGenerator != nullptr);
    QVERIFY(gridPortToLoad != nullptr);
    QVERIFY(loadPort != nullptr);

    QCOMPARE(generatorPort->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(gridPortToGenerator->getDirection(), LEFT_DIRECTION);
    QCOMPARE(gridPortToLoad->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(loadPort->getDirection(), RIGHT_DIRECTION);
    QVERIFY(sceneCenterForPort(gridPortToGenerator).x() < sceneCenterForPort(gridPortToLoad).x());
}

void TestCimGraphics::importer_and_scene_builder_materialize_p0_core_device_graphics()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/minimal/EQ_SSH_sample");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 EQ_SSH_sample 最小样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY2(importResult.success, qPrintable(importResult.summary().diagnosticText()));

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult =
        builder.populateSceneFromCimModel(&scene, importResult.model);

    const CimSceneBuilder::BuildResult::Summary buildSummary = buildResult.summary();
    QCOMPARE(buildSummary.sceneItemCount, 6);
    QCOMPARE(buildSummary.bindingCount, 6);
    QCOMPARE(buildSummary.generatedHelperLineCount, buildResult.generatedHelperLineCount);
    QVERIFY(!buildSummary.isEmpty());
    QCOMPARE(buildResult.sceneItemCount(), 6);
    QCOMPARE(buildResult.bindingCount(), 6);

    bool allCoreBindingsValid = true;
    QString bindingError;
    auto validateCoreBinding = [&](const QString& mrid) {
        QGraphicsItem* item = buildResult.sceneItemForMrid(mrid);
        if (!item)
        {
            allCoreBindingsValid = false;
            bindingError = QStringLiteral("核心设备未生成图元: %1").arg(mrid);
            return;
        }

        const QString itemMrid = item->data(CimSceneBuilder::CimMridDataKey).toString();
        if (itemMrid != mrid)
        {
            allCoreBindingsValid = false;
            bindingError = QStringLiteral("核心设备 mRID data 不一致: %1 -> %2").arg(mrid, itemMrid);
            return;
        }

        const QString graphicItemId = item->data(CimSceneBuilder::CimGraphicItemIdDataKey).toString();
        if (graphicItemId.isEmpty())
        {
            allCoreBindingsValid = false;
            bindingError = QStringLiteral("核心设备未写入 graphic item id: %1").arg(mrid);
            return;
        }

        if (!buildResult.hasBinding(mrid))
        {
            allCoreBindingsValid = false;
            bindingError = QStringLiteral("核心设备未写入 idIndex: %1").arg(mrid);
            return;
        }

        if (buildResult.graphicItemIdForMrid(mrid) != graphicItemId
            || buildResult.mridForGraphicItemId(graphicItemId) != mrid)
        {
            allCoreBindingsValid = false;
            bindingError = QStringLiteral("核心设备双向索引不一致: %1").arg(mrid);
        }
    };

    const QStringList coreMrids{
        QStringLiteral("busbar-001"),
        QStringLiteral("breaker-001"),
        QStringLiteral("disconnector-001"),
        QStringLiteral("transformer-001"),
        QStringLiteral("load-001"),
        QStringLiteral("generator-001")
    };
    for (const QString& mrid : coreMrids)
        validateCoreBinding(mrid);
    QVERIFY2(allCoreBindingsValid, qPrintable(bindingError));
    QCOMPARE(buildResult.mappedMrids().size(), coreMrids.size());
    QCOMPARE(buildResult.graphicQueryState().generatedHelperLineCount, buildResult.generatedHelperLineCount);
    QCOMPARE(buildResult.queryContext().idIndex, &buildResult.idIndex);

    auto* busbar =
        dynamic_cast<CimdrawPowerBusbarSectionItem*>(buildResult.sceneItemForMrid(QStringLiteral("busbar-001")));
    auto* breaker =
        dynamic_cast<CimdrawPowerCircuitBreakerItem*>(buildResult.sceneItemForMrid(QStringLiteral("breaker-001")));
    auto* disconnector =
        dynamic_cast<CimdrawPowerDisconnectorItem*>(buildResult.sceneItemForMrid(QStringLiteral("disconnector-001")));
    auto* transformer =
        dynamic_cast<CimdrawPowerTransformer2wItem*>(buildResult.sceneItemForMrid(QStringLiteral("transformer-001")));
    auto* load =
        dynamic_cast<CimdrawPowerLoadItem*>(buildResult.sceneItemForMrid(QStringLiteral("load-001")));
    auto* generator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.sceneItemForMrid(QStringLiteral("generator-001")));

    QVERIFY(busbar != nullptr);
    QVERIFY(breaker != nullptr);
    QVERIFY(disconnector != nullptr);
    QVERIFY(transformer != nullptr);
    QVERIFY(load != nullptr);
    QVERIFY(generator != nullptr);
    QCOMPARE(busbar->drawTypeForXml(), CIMDRAW_SLD_BUSBAR_SECTION);
    QCOMPARE(breaker->drawTypeForXml(), CIMDRAW_SLD_CIRCUIT_BREAKER);
    QCOMPARE(disconnector->drawTypeForXml(), CIMDRAW_SLD_DISCONNECTOR);
    QCOMPARE(transformer->drawTypeForXml(), CIMDRAW_SLD_TRANSFORMER_2W);
    QCOMPARE(load->drawTypeForXml(), CIMDRAW_SLD_LOAD);
    QCOMPARE(generator->drawTypeForXml(), CIMDRAW_SLD_GENERATOR);
}

void TestCimGraphics::importer_and_scene_builder_materialize_reference_source_and_load_graphics()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/medium/CIGREMV_reference");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 CIGREMV_reference 中型样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    auto* grid = dynamic_cast<CimdrawPowerGridItem*>(buildResult.shapeByMrid.value(QStringLiteral("HV-Netz")));
    auto* load = dynamic_cast<CimdrawPowerLoadItem*>(buildResult.shapeByMrid.value(QStringLiteral("Load7-I")));

    QVERIFY(grid != nullptr);
    QVERIFY(load != nullptr);
    QCOMPARE(grid->drawTypeForXml(), CIMDRAW_WSYM_GRID);
    QCOMPARE(load->drawTypeForXml(), CIMDRAW_SLD_LOAD);
    QCOMPARE(normalizedDegrees(grid->rotation()), 270);
    QVERIFY(grid->scenePos().x() < load->scenePos().x());

    const QString publicSampleDir = mediumCigrePublicPath();
    QVERIFY2(!publicSampleDir.isEmpty(), "未找到公开 CIGRE_MV 中型样例目录");

    const CimImportResult publicImportResult = importer.importFromPath(publicSampleDir);
    QVERIFY(publicImportResult.success);
    QVERIFY(publicImportResult.model.objectsByClassName(QStringLiteral("EnergyConsumer")).size() >= 10);
    QVERIFY(publicImportResult.model.objectsByClassName(QStringLiteral("ExternalNetworkInjection")).size() >= 1);

    CimdrawScene publicScene;
    const CimSceneBuilder::BuildResult publicBuildResult =
        builder.populateSceneFromCimModel(&publicScene, publicImportResult.model);

    const QVector<CimObject> publicLoads =
        publicImportResult.model.objectsByClassName(QStringLiteral("EnergyConsumer"));
    for (const CimObject& object : publicLoads)
    {
        QVERIFY2(dynamic_cast<CimdrawPowerLoadItem*>(publicBuildResult.shapeByMrid.value(object.mrid)) != nullptr,
                 qPrintable(QStringLiteral("EnergyConsumer 未落成负荷图元: %1").arg(object.mrid)));
    }

    const QVector<CimObject> publicExternalNetworks =
        publicImportResult.model.objectsByClassName(QStringLiteral("ExternalNetworkInjection"));
    for (const CimObject& object : publicExternalNetworks)
    {
        QVERIFY2(dynamic_cast<CimdrawPowerGridItem*>(publicBuildResult.shapeByMrid.value(object.mrid)) != nullptr,
                 qPrintable(QStringLiteral("ExternalNetworkInjection 未落成电网图元: %1").arg(object.mrid)));
    }

    auto* publicGrid =
        dynamic_cast<CimdrawPowerGridItem*>(publicBuildResult.shapeByMrid.value(QStringLiteral("HV-Netz")));
    auto* publicLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(publicBuildResult.shapeByMrid.value(QStringLiteral("Load7-I")));

    QVERIFY(publicGrid != nullptr);
    QVERIFY(publicLoad != nullptr);
    QCOMPARE(publicGrid->drawTypeForXml(), CIMDRAW_WSYM_GRID);
    QCOMPARE(publicLoad->drawTypeForXml(), CIMDRAW_SLD_LOAD);
    QVERIFY(publicGrid->scenePos().x() < publicLoad->scenePos().x());
}

void TestCimGraphics::importer_and_scene_builder_materialize_junction_graphics()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/minimal/Junction_sample");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 Junction_sample 最小样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    auto* junction = dynamic_cast<CimdrawPowerJunctionItem*>(buildResult.shapeByMrid.value(QStringLiteral("junction-001")));
    auto* breakerA = dynamic_cast<CimdrawPowerCircuitBreakerItem*>(buildResult.shapeByMrid.value(QStringLiteral("breaker-a")));
    auto* breakerB = dynamic_cast<CimdrawPowerCircuitBreakerItem*>(buildResult.shapeByMrid.value(QStringLiteral("breaker-b")));

    QVERIFY(junction != nullptr);
    QVERIFY(breakerA != nullptr);
    QVERIFY(breakerB != nullptr);
    QCOMPARE(junction->drawTypeForXml(), CIMDRAW_WSYM_JUNCTION);
    QCOMPARE(junction->referenceDesignator(), QStringLiteral("J1"));

    int generatedHelperShapeCount = 0;
    int generatedLineCount = 0;
    for (QGraphicsItem* item : scene.items())
    {
        if (!item->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            continue;
        if (auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item))
        {
            ++generatedLineCount;
            const bool touchesJunction = line->getStartItem() == junction || line->getEndItem() == junction;
            QVERIFY(touchesJunction);
            continue;
        }
        ++generatedHelperShapeCount;
    }

    QCOMPARE(generatedHelperShapeCount, 0);
    QCOMPARE(generatedLineCount, 2);
}

void TestCimGraphics::importer_and_scene_builder_materialize_medium_sample_junction_graphics()
{
    const QString sampleDir = mediumSwitchesSamplePath();
    QVERIFY2(!sampleDir.isEmpty(), "未找到 Sample_Grid_Switches_NodeBreaker 中型样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    const QStringList junctionMrids = {
        QStringLiteral("_4de2118a-8f14-0e13-88e0-60766b3fec7f"),
        QStringLiteral("_f8fbb745-f059-69f6-d613-37808243853e"),
        QStringLiteral("_4a2e40de-5c65-c4cf-ebb4-f571e7b26ba8"),
        QStringLiteral("_4fa95e61-3023-9e82-f0a2-9401050d22bb"),
        QStringLiteral("_c0fe66f9-bb7a-0e75-49a6-357cfb8c0e51"),
        QStringLiteral("_cf4d5c42-bee3-d124-e94a-667243b90594")
    };

    int totalConnectedItems = 0;
    for (const QString& mrid : junctionMrids)
    {
        auto* junction = dynamic_cast<CimdrawPowerJunctionItem*>(buildResult.shapeByMrid.value(mrid));
        QVERIFY2(junction != nullptr, qPrintable(QStringLiteral("Junction 未实例化: %1").arg(mrid)));
        QCOMPARE(junction->drawTypeForXml(), CIMDRAW_WSYM_JUNCTION);
        QVERIFY(!junction->referenceDesignator().isEmpty());
        totalConnectedItems += junction->connectedItems().size();
    }

    QVERIFY(totalConnectedItems > junctionMrids.size());
}

void TestCimGraphics::importer_and_scene_builder_materialize_transformer_specialization_graphics()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-special-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:PowerTransformer rdf:ID="xfmr-auto-001">
    <cim:IdentifiedObject.name>T1</cim:IdentifiedObject.name>
    <cim:IdentifiedObject.description>Auto Transformer for feeder tie</cim:IdentifiedObject.description>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-auto-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-auto-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformer rdf:ID="xfmr-station-001">
    <cim:IdentifiedObject.name>T2</cim:IdentifiedObject.name>
    <cim:IdentifiedObject.aliasName>Auxiliary Transformer</cim:IdentifiedObject.aliasName>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-station-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-station-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformer rdf:ID="xfmr-earthing-001">
    <cim:IdentifiedObject.name>Earthing Transformer</cim:IdentifiedObject.name>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-earthing-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-earthing-end-b"/>
  </cim:PowerTransformer>
  <cim:PowerTransformer rdf:ID="xfmr-plain-001">
    <cim:IdentifiedObject.name>Main Transformer</cim:IdentifiedObject.name>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-plain-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-plain-end-b"/>
  </cim:PowerTransformer>
  <cim:SynchronousMachine rdf:ID="gen-auto-001">
    <cim:IdentifiedObject.name>G-AUTO</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:EnergyConsumer rdf:ID="load-auto-001">
    <cim:IdentifiedObject.name>L-AUTO</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:SynchronousMachine rdf:ID="gen-station-001">
    <cim:IdentifiedObject.name>G-STATION</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:EnergyConsumer rdf:ID="load-station-001">
    <cim:IdentifiedObject.name>L-STATION</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:SynchronousMachine rdf:ID="gen-earthing-001">
    <cim:IdentifiedObject.name>G-EARTHING</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:EnergyConsumer rdf:ID="load-earthing-001">
    <cim:IdentifiedObject.name>L-EARTHING</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:SynchronousMachine rdf:ID="gen-plain-001">
    <cim:IdentifiedObject.name>G-PLAIN</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:EnergyConsumer rdf:ID="load-plain-001">
    <cim:IdentifiedObject.name>L-PLAIN</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:PowerTransformerEnd rdf:ID="xfmr-auto-end-a">
    <cim:TransformerEnd.endNumber>1</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-auto-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-auto-end-b">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-auto-002"/>
    <cim:TransformerEnd.endNumber>2</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-auto-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-station-end-a">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-station-001"/>
    <cim:TransformerEnd.endNumber>1</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-station-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-station-end-b">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-station-002"/>
    <cim:TransformerEnd.endNumber>2</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-station-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-earthing-end-a">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-earthing-001"/>
    <cim:TransformerEnd.endNumber>1</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-earthing-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-earthing-end-b">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-earthing-002"/>
    <cim:TransformerEnd.endNumber>2</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-earthing-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-plain-end-a">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-plain-001"/>
    <cim:TransformerEnd.endNumber>1</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-plain-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-plain-end-b">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-plain-002"/>
    <cim:TransformerEnd.endNumber>2</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-plain-001"/>
  </cim:PowerTransformerEnd>
  <cim:Terminal rdf:ID="terminal-auto-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-auto-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-auto-a"/>
    <cim:Terminal.TransformerEnd rdf:resource="#xfmr-auto-end-a"/>
    <cim:ACDCTerminal.sequenceNumber>1</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-auto-002">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-auto-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-auto-b"/>
    <cim:ACDCTerminal.sequenceNumber>2</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-station-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-station-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-station-a"/>
    <cim:ACDCTerminal.sequenceNumber>1</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-station-002">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-station-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-station-b"/>
    <cim:ACDCTerminal.sequenceNumber>2</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-earthing-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-earthing-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-earthing-a"/>
    <cim:ACDCTerminal.sequenceNumber>1</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-earthing-002">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-earthing-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-earthing-b"/>
    <cim:ACDCTerminal.sequenceNumber>2</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-plain-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-plain-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-plain-a"/>
    <cim:ACDCTerminal.sequenceNumber>1</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-plain-002">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-plain-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-plain-b"/>
    <cim:ACDCTerminal.sequenceNumber>2</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-gen-auto-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#gen-auto-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-auto-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-auto-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-auto-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-auto-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-gen-station-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#gen-station-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-station-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-station-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-station-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-station-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-gen-earthing-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#gen-earthing-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-earthing-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-earthing-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-earthing-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-earthing-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-gen-plain-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#gen-plain-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-plain-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-plain-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-plain-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-plain-b"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-special-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>

  <cim:DiagramObject rdf:ID="diagram-xfmr-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-auto-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-auto-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-auto-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-xfmr-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-plain-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-plain-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-plain-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("transformer_special_EQ.xml")), eqXml),
             "failed to write transformer specialization EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("transformer_special_DL.xml")), dlXml),
             "failed to write transformer specialization DL sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* autoXfmr =
        dynamic_cast<CimdrawPowerAutoXfmrItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-auto-001")));
    auto* earthingXfmr =
        dynamic_cast<CimdrawPowerEarthingXfmrItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-earthing-001")));
    auto* autoGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.shapeByMrid.value(QStringLiteral("gen-auto-001")));
    auto* autoLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(buildResult.shapeByMrid.value(QStringLiteral("load-auto-001")));
    auto* plainGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(buildResult.shapeByMrid.value(QStringLiteral("gen-plain-001")));
    auto* plainLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(buildResult.shapeByMrid.value(QStringLiteral("load-plain-001")));
    auto* plainXfmr =
        dynamic_cast<CimdrawPowerTransformer2wItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-plain-001")));
    auto* stationXfmr =
        dynamic_cast<CimdrawPowerStationXfmrItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-station-001")));

    QVERIFY(autoXfmr != nullptr);
    QVERIFY(autoGenerator != nullptr);
    QVERIFY(autoLoad != nullptr);
    QVERIFY(earthingXfmr != nullptr);
    QVERIFY(plainGenerator != nullptr);
    QVERIFY(plainLoad != nullptr);
    QVERIFY(plainXfmr != nullptr);
    QVERIFY(stationXfmr != nullptr);

    QCOMPARE(autoXfmr->wiringConnectPorts().size(), 2);
    QVERIFY(autoXfmr->wiringConnectPorts().at(0) != nullptr);
    QVERIFY(autoXfmr->wiringConnectPorts().at(1) != nullptr);
    QCOMPARE(autoXfmr->wiringConnectPorts().at(0)->getDirection(), LEFT_DIRECTION);
    QCOMPARE(autoXfmr->wiringConnectPorts().at(1)->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(autoXfmr->connectedItems().size(), 2);

    QCOMPARE(earthingXfmr->wiringConnectPorts().size(), 2);
    QVERIFY(earthingXfmr->wiringConnectPorts().at(0) != nullptr);
    QVERIFY(earthingXfmr->wiringConnectPorts().at(1) != nullptr);
    QCOMPARE(earthingXfmr->wiringConnectPorts().at(0)->getDirection(), LEFT_DIRECTION);
    QCOMPARE(earthingXfmr->wiringConnectPorts().at(1)->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(earthingXfmr->connectedItems().size(), 2);

    QCOMPARE(plainXfmr->wiringConnectPorts().size(), 2);
    QVERIFY(plainXfmr->wiringConnectPorts().at(0) != nullptr);
    QVERIFY(plainXfmr->wiringConnectPorts().at(1) != nullptr);
    QCOMPARE(plainXfmr->wiringConnectPorts().at(0)->getDirection(), LEFT_DIRECTION);
    QCOMPARE(plainXfmr->wiringConnectPorts().at(1)->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(plainXfmr->connectedItems().size(), 2);

    QCOMPARE(stationXfmr->wiringConnectPorts().size(), 2);
    QVERIFY(stationXfmr->wiringConnectPorts().at(0) != nullptr);
    QVERIFY(stationXfmr->wiringConnectPorts().at(1) != nullptr);
    QCOMPARE(stationXfmr->wiringConnectPorts().at(0)->getDirection(), LEFT_DIRECTION);
    QCOMPARE(stationXfmr->wiringConnectPorts().at(1)->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(stationXfmr->connectedItems().size(), 2);

    auto collectUsedPorts = [](TmpShape* shape) {
        QSet<CimdrawConnectPoint*> usedPorts;
        for (QGraphicsItem* item : shape->connectedItems())
        {
            auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
            if (!line)
                continue;
            if (line->getStartItem() == shape)
                usedPorts.insert(line->startConnectPort());
            if (line->getEndItem() == shape)
                usedPorts.insert(line->endConnectPort());
        }
        return usedPorts;
    };

    QCOMPARE(collectUsedPorts(autoXfmr).size(), 2);
    QCOMPARE(collectUsedPorts(earthingXfmr).size(), 2);
    QCOMPARE(collectUsedPorts(plainXfmr).size(), 2);
    QCOMPARE(collectUsedPorts(stationXfmr).size(), 2);

    QVERIFY(autoGenerator->sceneBoundingRect().center().x() > autoXfmr->sceneBoundingRect().center().x());
    QVERIFY(autoLoad->sceneBoundingRect().center().x() < autoXfmr->sceneBoundingRect().center().x());
    QVERIFY(plainGenerator->sceneBoundingRect().center().x() > plainXfmr->sceneBoundingRect().center().x());
    QVERIFY(plainLoad->sceneBoundingRect().center().x() < plainXfmr->sceneBoundingRect().center().x());

    CimdrawConnectLine* autoGeneratorLine = findGeneratedHelperLineBetween(scene, autoXfmr, autoGenerator);
    CimdrawConnectLine* autoLoadLine = findGeneratedHelperLineBetween(scene, autoXfmr, autoLoad);
    CimdrawConnectLine* plainGeneratorLine = findGeneratedHelperLineBetween(scene, plainXfmr, plainGenerator);
    CimdrawConnectLine* plainLoadLine = findGeneratedHelperLineBetween(scene, plainXfmr, plainLoad);

    assertPortDirectionOnItem(autoGeneratorLine, autoXfmr, LEFT_DIRECTION);
    assertPortDirectionOnItem(autoLoadLine, autoXfmr, RIGHT_DIRECTION);
    assertPortDirectionOnItem(plainGeneratorLine, plainXfmr, LEFT_DIRECTION);
    assertPortDirectionOnItem(plainLoadLine, plainXfmr, RIGHT_DIRECTION);

    const QString rotatedDlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-special-rotated-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>

  <cim:DiagramObject rdf:ID="diagram-xfmr-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-auto-001"/>
    <cim:DiagramObject.rotation>90</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-auto-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-auto-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-auto-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-auto-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-auto-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>140</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-xfmr-station-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-station-001"/>
    <cim:DiagramObject.rotation>180</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-station-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-station-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-station-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-station-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-station-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-station-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-station-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-station-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-station-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-station-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>320</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-xfmr-earthing-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-earthing-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-earthing-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-earthing-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>500</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-earthing-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-earthing-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-earthing-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-earthing-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>500</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-earthing-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-earthing-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-earthing-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-earthing-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>500</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-xfmr-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-plain-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>680</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-gen-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-plain-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>680</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>

  <cim:DiagramObject rdf:ID="diagram-load-plain-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-plain-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-plain-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-plain-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>680</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QTemporaryDir rotatedDir;
    QVERIFY(rotatedDir.isValid());
    QVERIFY2(writeUtf8File(rotatedDir.filePath(QStringLiteral("transformer_special_EQ.xml")), eqXml),
             "failed to write rotated transformer specialization EQ sample");
    QVERIFY2(writeUtf8File(rotatedDir.filePath(QStringLiteral("transformer_special_rotated_DL.xml")), rotatedDlXml),
             "failed to write rotated transformer specialization DL sample");

    const CimImportResult rotatedResult = importer.importFromPath(rotatedDir.path());
    QVERIFY(rotatedResult.success);

    CimdrawScene rotatedScene;
    const CimSceneBuilder::BuildResult rotatedBuildResult =
        builder.populateSceneFromCimModel(&rotatedScene, rotatedResult.model);

    auto* rotatedAutoXfmr =
        dynamic_cast<CimdrawPowerAutoXfmrItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("xfmr-auto-001")));
    auto* rotatedStationXfmr =
        dynamic_cast<CimdrawPowerStationXfmrItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("xfmr-station-001")));
    auto* rotatedEarthingXfmr =
        dynamic_cast<CimdrawPowerEarthingXfmrItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("xfmr-earthing-001")));
    auto* rotatedPlainXfmr =
        dynamic_cast<CimdrawPowerTransformer2wItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("xfmr-plain-001")));
    auto* rotatedAutoGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("gen-auto-001")));
    auto* rotatedAutoLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("load-auto-001")));
    auto* rotatedStationGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("gen-station-001")));
    auto* rotatedStationLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("load-station-001")));
    auto* rotatedEarthingGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("gen-earthing-001")));
    auto* rotatedEarthingLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("load-earthing-001")));
    auto* rotatedPlainGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("gen-plain-001")));
    auto* rotatedPlainLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(rotatedBuildResult.shapeByMrid.value(QStringLiteral("load-plain-001")));

    QVERIFY(rotatedAutoXfmr != nullptr);
    QVERIFY(rotatedStationXfmr != nullptr);
    QVERIFY(rotatedEarthingXfmr != nullptr);
    QVERIFY(rotatedPlainXfmr != nullptr);
    QVERIFY(rotatedAutoGenerator != nullptr);
    QVERIFY(rotatedAutoLoad != nullptr);
    QVERIFY(rotatedStationGenerator != nullptr);
    QVERIFY(rotatedStationLoad != nullptr);
    QVERIFY(rotatedEarthingGenerator != nullptr);
    QVERIFY(rotatedEarthingLoad != nullptr);
    QVERIFY(rotatedPlainGenerator != nullptr);
    QVERIFY(rotatedPlainLoad != nullptr);
    QCOMPARE(normalizedDegrees(rotatedAutoXfmr->rotation()), 90);
    QCOMPARE(normalizedDegrees(rotatedStationXfmr->rotation()), 180);
    QCOMPARE(normalizedDegrees(rotatedEarthingXfmr->rotation()), 270);
    QCOMPARE(normalizedDegrees(rotatedPlainXfmr->rotation()), 270);

    CimdrawConnectLine* rotatedAutoGeneratorLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedAutoXfmr, rotatedAutoGenerator);
    CimdrawConnectLine* rotatedAutoLoadLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedAutoXfmr, rotatedAutoLoad);
    CimdrawConnectLine* rotatedStationGeneratorLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedStationXfmr, rotatedStationGenerator);
    CimdrawConnectLine* rotatedStationLoadLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedStationXfmr, rotatedStationLoad);
    CimdrawConnectLine* rotatedEarthingGeneratorLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedEarthingXfmr, rotatedEarthingGenerator);
    CimdrawConnectLine* rotatedEarthingLoadLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedEarthingXfmr, rotatedEarthingLoad);
    CimdrawConnectLine* rotatedPlainGeneratorLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedPlainXfmr, rotatedPlainGenerator);
    CimdrawConnectLine* rotatedPlainLoadLine =
        findGeneratedHelperLineBetween(rotatedScene, rotatedPlainXfmr, rotatedPlainLoad);

    assertLineExitsItemAlongRotatedPortNormal(rotatedAutoGeneratorLine, rotatedAutoXfmr, LEFT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedAutoLoadLine, rotatedAutoXfmr, RIGHT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedStationGeneratorLine, rotatedStationXfmr, LEFT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedStationLoadLine, rotatedStationXfmr, RIGHT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedEarthingGeneratorLine, rotatedEarthingXfmr, LEFT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedEarthingLoadLine, rotatedEarthingXfmr, RIGHT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedPlainGeneratorLine, rotatedPlainXfmr, LEFT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedPlainLoadLine, rotatedPlainXfmr, RIGHT_DIRECTION);
}

void TestCimGraphics::importer_and_scene_builder_materialize_transformer_3w_graphics()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/minimal/Transformer3w_sample");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 Transformer3w_sample 最小样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    const CimObject* transformer = importResult.model.objectByMrid(QStringLiteral("xfmr-3w-001"));
    QVERIFY(transformer != nullptr);

    int endRefCount = 0;
    for (const CimReference& reference : transformer->references)
    {
        if (reference.relationName == QStringLiteral("PowerTransformer.PowerTransformerEnd"))
            ++endRefCount;
    }
    QCOMPARE(endRefCount, 3);

    CimGraphicMapper mapper;
    QCOMPARE(mapper.drawTypeForObject(*transformer), CIMDRAW_WSYM_TRANSFORMER_3W);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    auto* transformer3w = dynamic_cast<CimdrawPowerTransformer3wItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-3w-001")));
    QVERIFY(transformer3w != nullptr);
    QCOMPARE(transformer3w->drawTypeForXml(), CIMDRAW_WSYM_TRANSFORMER_3W);
    QCOMPARE(transformer3w->referenceDesignator(), QStringLiteral("T3W1"));
    QCOMPARE(transformer3w->wiringConnectPorts().size(), 3);
    QVERIFY(transformer3w->wiringConnectPorts().at(0) != nullptr);
    QVERIFY(transformer3w->wiringConnectPorts().at(1) != nullptr);
    QVERIFY(transformer3w->wiringConnectPorts().at(2) != nullptr);
    QCOMPARE(transformer3w->wiringConnectPorts().at(0)->getDirection(), LEFT_DIRECTION);
    QCOMPARE(transformer3w->wiringConnectPorts().at(1)->getDirection(), RIGHT_DIRECTION);
    QCOMPARE(transformer3w->wiringConnectPorts().at(2)->getDirection(), BOTTOM_DIRECTION);
    QCOMPARE(transformer3w->connectedItems().size(), 3);
    QSet<CimdrawConnectPoint*> usedPorts;
    for (QGraphicsItem* item : transformer3w->connectedItems())
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        QVERIFY(line != nullptr);
        if (line->getStartItem() == transformer3w)
            usedPorts.insert(line->startConnectPort());
        if (line->getEndItem() == transformer3w)
            usedPorts.insert(line->endConnectPort());
    }
    QCOMPARE(usedPorts.size(), 3);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-3w-conflict-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:PowerTransformer rdf:ID="xfmr-3w-conflict-001">
    <cim:IdentifiedObject.name>T3W-C1</cim:IdentifiedObject.name>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-3w-conflict-end-a"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-3w-conflict-end-b"/>
    <cim:PowerTransformer.PowerTransformerEnd rdf:resource="#xfmr-3w-conflict-end-c"/>
  </cim:PowerTransformer>
  <cim:SynchronousMachine rdf:ID="gen-3w-conflict-001">
    <cim:IdentifiedObject.name>G-3W</cim:IdentifiedObject.name>
  </cim:SynchronousMachine>
  <cim:EnergyConsumer rdf:ID="load-3w-conflict-001">
    <cim:IdentifiedObject.name>L-3W</cim:IdentifiedObject.name>
  </cim:EnergyConsumer>
  <cim:ExternalNetworkInjection rdf:ID="grid-3w-conflict-001">
    <cim:IdentifiedObject.name>GRID-3W</cim:IdentifiedObject.name>
  </cim:ExternalNetworkInjection>
  <cim:PowerTransformerEnd rdf:ID="xfmr-3w-conflict-end-a">
    <cim:TransformerEnd.endNumber>1</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w-conflict-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-3w-conflict-end-b">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-xfmr-3w-conflict-b"/>
    <cim:TransformerEnd.endNumber>2</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w-conflict-001"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-3w-conflict-end-c">
    <cim:TransformerEnd.Terminal rdf:resource="#terminal-xfmr-3w-conflict-c"/>
    <cim:TransformerEnd.endNumber>3</cim:TransformerEnd.endNumber>
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w-conflict-001"/>
  </cim:PowerTransformerEnd>
  <cim:Terminal rdf:ID="terminal-xfmr-3w-conflict-a">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-a"/>
    <cim:Terminal.TransformerEnd rdf:resource="#xfmr-3w-conflict-end-a"/>
    <cim:ACDCTerminal.sequenceNumber>1</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-xfmr-3w-conflict-b">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-b"/>
    <cim:ACDCTerminal.sequenceNumber>2</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-xfmr-3w-conflict-c">
    <cim:Terminal.ConductingEquipment rdf:resource="#xfmr-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-c"/>
    <cim:ACDCTerminal.sequenceNumber>3</cim:ACDCTerminal.sequenceNumber>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-gen-3w-conflict-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#gen-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-a"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-load-3w-conflict-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#load-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-b"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-grid-3w-conflict-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#grid-3w-conflict-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-xfmr-3w-conflict-c"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-3w-conflict-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-xfmr-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-gen-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-load-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-grid-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#grid-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-grid-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-grid-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>60</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("transformer_3w_conflict_EQ.xml")), eqXml),
             "failed to write transformer 3w conflict EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("transformer_3w_conflict_DL.xml")), dlXml),
             "failed to write transformer 3w conflict DL sample");

    const CimImportResult conflictResult = importer.importFromPath(dir.path());
    QVERIFY(conflictResult.success);

    CimdrawScene conflictScene;
    const CimSceneBuilder::BuildResult conflictBuildResult =
        builder.populateSceneFromCimModel(&conflictScene, conflictResult.model);

    auto* conflictTransformer =
        dynamic_cast<CimdrawPowerTransformer3wItem*>(conflictBuildResult.shapeByMrid.value(QStringLiteral("xfmr-3w-conflict-001")));
    auto* conflictGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(conflictBuildResult.shapeByMrid.value(QStringLiteral("gen-3w-conflict-001")));
    auto* conflictLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(conflictBuildResult.shapeByMrid.value(QStringLiteral("load-3w-conflict-001")));
    auto* conflictGrid =
        dynamic_cast<CimdrawPowerGridItem*>(conflictBuildResult.shapeByMrid.value(QStringLiteral("grid-3w-conflict-001")));

    QVERIFY(conflictTransformer != nullptr);
    QVERIFY(conflictGenerator != nullptr);
    QVERIFY(conflictLoad != nullptr);
    QVERIFY(conflictGrid != nullptr);
    QCOMPARE(conflictTransformer->wiringConnectPorts().size(), 3);
    QCOMPARE(conflictTransformer->connectedItems().size(), 3);

    QVERIFY(conflictGenerator->sceneBoundingRect().center().x()
             > conflictTransformer->sceneBoundingRect().center().x());
    QVERIFY(conflictLoad->sceneBoundingRect().center().x()
             < conflictTransformer->sceneBoundingRect().center().x());
    QVERIFY(conflictGrid->sceneBoundingRect().center().y()
             < conflictTransformer->sceneBoundingRect().center().y());

    CimdrawConnectLine* generatorLine = findGeneratedHelperLineBetween(conflictScene, conflictTransformer, conflictGenerator);
    CimdrawConnectLine* loadLine = findGeneratedHelperLineBetween(conflictScene, conflictTransformer, conflictLoad);
    CimdrawConnectLine* gridLine = findGeneratedHelperLineBetween(conflictScene, conflictTransformer, conflictGrid);

    assertPortDirectionOnItem(generatorLine, conflictTransformer, LEFT_DIRECTION);
    assertPortDirectionOnItem(loadLine, conflictTransformer, RIGHT_DIRECTION);
    assertPortDirectionOnItem(gridLine, conflictTransformer, BOTTOM_DIRECTION);

    const QString rotatedConflictDlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-transformer-3w-conflict-rotated-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-xfmr-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#xfmr-3w-conflict-001"/>
    <cim:DiagramObject.rotation>90</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-xfmr-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-xfmr-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-gen-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#gen-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-gen-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-gen-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>420</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-load-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#load-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-load-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-load-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>60</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>240</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-grid-3w-conflict-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#grid-3w-conflict-001"/>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-grid-3w-conflict-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-grid-3w-conflict-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>240</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>60</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QTemporaryDir rotatedConflictDir;
    QVERIFY(rotatedConflictDir.isValid());
    QVERIFY2(writeUtf8File(rotatedConflictDir.filePath(QStringLiteral("transformer_3w_conflict_EQ.xml")), eqXml),
             "failed to write rotated transformer 3w conflict EQ sample");
    QVERIFY2(writeUtf8File(rotatedConflictDir.filePath(QStringLiteral("transformer_3w_conflict_rotated_DL.xml")), rotatedConflictDlXml),
             "failed to write rotated transformer 3w conflict DL sample");

    const CimImportResult rotatedConflictResult = importer.importFromPath(rotatedConflictDir.path());
    QVERIFY(rotatedConflictResult.success);

    CimdrawScene rotatedConflictScene;
    const CimSceneBuilder::BuildResult rotatedConflictBuildResult =
        builder.populateSceneFromCimModel(&rotatedConflictScene, rotatedConflictResult.model);

    auto* rotatedConflictTransformer =
        dynamic_cast<CimdrawPowerTransformer3wItem*>(rotatedConflictBuildResult.shapeByMrid.value(QStringLiteral("xfmr-3w-conflict-001")));
    auto* rotatedConflictGenerator =
        dynamic_cast<CimdrawPowerGeneratorItem*>(rotatedConflictBuildResult.shapeByMrid.value(QStringLiteral("gen-3w-conflict-001")));
    auto* rotatedConflictLoad =
        dynamic_cast<CimdrawPowerLoadItem*>(rotatedConflictBuildResult.shapeByMrid.value(QStringLiteral("load-3w-conflict-001")));
    auto* rotatedConflictGrid =
        dynamic_cast<CimdrawPowerGridItem*>(rotatedConflictBuildResult.shapeByMrid.value(QStringLiteral("grid-3w-conflict-001")));

    QVERIFY(rotatedConflictTransformer != nullptr);
    QVERIFY(rotatedConflictGenerator != nullptr);
    QVERIFY(rotatedConflictLoad != nullptr);
    QVERIFY(rotatedConflictGrid != nullptr);
    QCOMPARE(normalizedDegrees(rotatedConflictTransformer->rotation()), 90);

    CimdrawConnectLine* rotatedGeneratorLine =
        findGeneratedHelperLineBetween(rotatedConflictScene, rotatedConflictTransformer, rotatedConflictGenerator);
    CimdrawConnectLine* rotatedLoadLine =
        findGeneratedHelperLineBetween(rotatedConflictScene, rotatedConflictTransformer, rotatedConflictLoad);
    CimdrawConnectLine* rotatedGridLine =
        findGeneratedHelperLineBetween(rotatedConflictScene, rotatedConflictTransformer, rotatedConflictGrid);

    assertLineExitsItemAlongRotatedPortNormal(rotatedGeneratorLine, rotatedConflictTransformer, LEFT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedLoadLine, rotatedConflictTransformer, RIGHT_DIRECTION);
    assertLineExitsItemAlongRotatedPortNormal(rotatedGridLine, rotatedConflictTransformer, BOTTOM_DIRECTION);
}

void TestCimGraphics::importer_and_scene_builder_preserve_transformer_3w_layout_when_diagram_data_exists()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/minimal/Transformer3w_layout_sample");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 Transformer3w_layout_sample 最小样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    auto* transformer3w =
        dynamic_cast<CimdrawPowerTransformer3wItem*>(buildResult.shapeByMrid.value(QStringLiteral("xfmr-3w-layout-001")));
    QVERIFY(transformer3w != nullptr);
    QCOMPARE(transformer3w->drawTypeForXml(), CIMDRAW_WSYM_TRANSFORMER_3W);
    QCOMPARE(transformer3w->referenceDesignator(), QStringLiteral("T3W-L1"));
    QCOMPARE(qRound(transformer3w->sceneBoundingRect().center().x()), 272);
    QCOMPARE(qRound(transformer3w->sceneBoundingRect().center().y()), 80);
    QCOMPARE(qRound(transformer3w->rotation()), 270);
    QCOMPARE(transformer3w->wiringConnectPorts().size(), 3);
    QCOMPARE(transformer3w->connectedItems().size(), 3);
}

void TestCimGraphics::importer_and_scene_builder_materialize_shunt_compensator_family_graphics()
{
    const QString sampleDir = QFINDTESTDATA("tests/data/cim/minimal/ShuntCompensator_family_sample");
    QVERIFY2(!sampleDir.isEmpty(), "未找到 ShuntCompensator_family_sample 最小样例目录");

    CgmesPackageImporter importer;
    const CimImportResult importResult = importer.importFromPath(sampleDir);
    QVERIFY(importResult.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, importResult.model);

    QVERIFY(dynamic_cast<CimdrawPowerCapacitorItem*>(buildResult.shapeByMrid.value(QStringLiteral("cap-bank-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("reactor-bank-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerSplitReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("split-reactor-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("nonlinear-reactor-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerSvgCompItem*>(buildResult.shapeByMrid.value(QStringLiteral("svg-comp-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerArcCoilItem*>(buildResult.shapeByMrid.value(QStringLiteral("arc-coil-001"))) != nullptr);
}

void TestCimGraphics::importer_and_scene_builder_preserve_shunt_compensator_family_layout_from_diagram_objects()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-shunt-layout-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:ShuntCompensator rdf:ID="cap-layout-001">
    <cim:IdentifiedObject.name>Cap Layout 1</cim:IdentifiedObject.name>
    <cim:ShuntCompensator.bPerSection>0.0042</cim:ShuntCompensator.bPerSection>
  </cim:ShuntCompensator>
  <cim:LinearShuntCompensator rdf:ID="reactor-layout-001">
    <cim:IdentifiedObject.name>Reactor Layout 1</cim:IdentifiedObject.name>
    <cim:ShuntCompensator.bPerSection>-0.0035</cim:ShuntCompensator.bPerSection>
  </cim:LinearShuntCompensator>
  <cim:LinearShuntCompensator rdf:ID="split-layout-001">
    <cim:IdentifiedObject.name>Split Reactor Layout 1</cim:IdentifiedObject.name>
    <cim:ShuntCompensator.bPerSection>-0.0030</cim:ShuntCompensator.bPerSection>
    <cim:ShuntCompensator.maximumSections>2</cim:ShuntCompensator.maximumSections>
  </cim:LinearShuntCompensator>
  <cim:StaticVarCompensator rdf:ID="svg-layout-001">
    <cim:IdentifiedObject.name>SVG Layout 1</cim:IdentifiedObject.name>
  </cim:StaticVarCompensator>
  <cim:PetersenCoil rdf:ID="arc-layout-001">
    <cim:IdentifiedObject.name>Petersen Layout 1</cim:IdentifiedObject.name>
  </cim:PetersenCoil>
  <cim:Terminal rdf:ID="terminal-cap-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#cap-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-cap-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-reactor-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#reactor-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-reactor-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-split-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#split-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-split-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-svg-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#svg-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-svg-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-arc-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#arc-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-arc-layout"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-shunt-layout-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-cap-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#cap-layout-001"/>
    <cim:DiagramObject.rotation>0</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-cap-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-cap-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>100</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-reactor-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#reactor-layout-001"/>
    <cim:DiagramObject.rotation>90</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-reactor-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-reactor-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>250</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-split-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#split-layout-001"/>
    <cim:DiagramObject.rotation>180</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-split-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-split-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>400</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-svg-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#svg-layout-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-svg-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-svg-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>550</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-arc-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#arc-layout-001"/>
    <cim:DiagramObject.rotation>0</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-arc-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-arc-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>700</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("shunt_layout_EQ.xml")), eqXml),
             "failed to write shunt layout EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("shunt_layout_DL.xml")), dlXml),
             "failed to write shunt layout DL sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* capacitor =
        dynamic_cast<CimdrawPowerCapacitorItem*>(buildResult.shapeByMrid.value(QStringLiteral("cap-layout-001")));
    auto* reactor =
        dynamic_cast<CimdrawPowerReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("reactor-layout-001")));
    auto* splitReactor =
        dynamic_cast<CimdrawPowerSplitReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("split-layout-001")));
    auto* svg =
        dynamic_cast<CimdrawPowerSvgCompItem*>(buildResult.shapeByMrid.value(QStringLiteral("svg-layout-001")));
    auto* arcCoil =
        dynamic_cast<CimdrawPowerArcCoilItem*>(buildResult.shapeByMrid.value(QStringLiteral("arc-layout-001")));

    QVERIFY(capacitor != nullptr);
    QVERIFY(reactor != nullptr);
    QVERIFY(splitReactor != nullptr);
    QVERIFY(svg != nullptr);
    QVERIFY(arcCoil != nullptr);

    QCOMPARE(qRound(capacitor->scenePos().x()), 80);
    QCOMPARE(qRound(capacitor->scenePos().y()), 80);
    QCOMPARE(qRound(capacitor->rotation()), 0);

    QCOMPARE(qRound(reactor->scenePos().x()), 260);
    QCOMPARE(qRound(reactor->scenePos().y()), 80);
    QCOMPARE(qRound(reactor->rotation()), 90);

    QCOMPARE(qRound(splitReactor->scenePos().x()), 440);
    QCOMPARE(qRound(splitReactor->scenePos().y()), 80);
    QCOMPARE(qRound(splitReactor->rotation()), 180);

    QCOMPARE(qRound(svg->scenePos().x()), 620);
    QCOMPARE(qRound(svg->scenePos().y()), 80);
    QCOMPARE(qRound(svg->rotation()), 270);

    QCOMPARE(qRound(arcCoil->scenePos().x()), 800);
    QCOMPARE(qRound(arcCoil->scenePos().y()), 80);
    QCOMPARE(qRound(arcCoil->rotation()), 0);

    QVERIFY(capacitor->scenePos().x() < reactor->scenePos().x());
    QVERIFY(reactor->scenePos().x() < splitReactor->scenePos().x());
    QVERIFY(splitReactor->scenePos().x() < svg->scenePos().x());
    QVERIFY(svg->scenePos().x() < arcCoil->scenePos().x());
}

void TestCimGraphics::importer_and_scene_builder_materialize_series_compensator_family_graphics()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-series-family-eq">
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
             "failed to write series compensator EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    QVERIFY(dynamic_cast<CimdrawPowerCapacitorItem*>(buildResult.shapeByMrid.value(QStringLiteral("series-cap-001"))) != nullptr);
    QVERIFY(dynamic_cast<CimdrawPowerReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("series-reactor-001"))) != nullptr);
}

void TestCimGraphics::importer_and_scene_builder_preserve_series_compensator_family_layout_from_diagram_objects()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-series-layout-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:SeriesCompensator rdf:ID="series-cap-layout-001">
    <cim:IdentifiedObject.name>Series Capacitor Layout 1</cim:IdentifiedObject.name>
    <cim:SeriesCompensator.x>-0.0035</cim:SeriesCompensator.x>
  </cim:SeriesCompensator>
  <cim:SeriesCompensator rdf:ID="series-reactor-layout-001">
    <cim:IdentifiedObject.name>Current Limiting Reactor Layout 1</cim:IdentifiedObject.name>
    <cim:SeriesCompensator.x>0.0062</cim:SeriesCompensator.x>
  </cim:SeriesCompensator>
  <cim:Terminal rdf:ID="terminal-series-cap-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#series-cap-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-series-cap-layout"/>
  </cim:Terminal>
  <cim:Terminal rdf:ID="terminal-series-reactor-layout-001">
    <cim:Terminal.ConductingEquipment rdf:resource="#series-reactor-layout-001"/>
    <cim:Terminal.TopologicalNode rdf:resource="#node-series-reactor-layout"/>
  </cim:Terminal>
</rdf:RDF>)");

    const QString dlXml = QString::fromUtf8(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:cim-series-layout-dl">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/DiagramLayout-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:DiagramObject rdf:ID="diagram-series-cap-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#series-cap-layout-001"/>
    <cim:DiagramObject.rotation>180</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-series-cap-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-series-cap-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>100</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>100</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
  <cim:DiagramObject rdf:ID="diagram-series-reactor-layout-001">
    <cim:DiagramObject.IdentifiedObject rdf:resource="#series-reactor-layout-001"/>
    <cim:DiagramObject.rotation>270</cim:DiagramObject.rotation>
  </cim:DiagramObject>
  <cim:DiagramObjectPoint rdf:ID="diagram-series-reactor-layout-001-p1">
    <cim:DiagramObjectPoint.DiagramObject rdf:resource="#diagram-series-reactor-layout-001"/>
    <cim:DiagramObjectPoint.sequenceNumber>1</cim:DiagramObjectPoint.sequenceNumber>
    <cim:DiagramObjectPoint.xPosition>300</cim:DiagramObjectPoint.xPosition>
    <cim:DiagramObjectPoint.yPosition>200</cim:DiagramObjectPoint.yPosition>
  </cim:DiagramObjectPoint>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("series_layout_EQ.xml")), eqXml),
             "failed to write series compensator layout EQ sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("series_layout_DL.xml")), dlXml),
             "failed to write series compensator layout DL sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());
    QVERIFY(result.success);

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, result.model);

    auto* capacitor =
        dynamic_cast<CimdrawPowerCapacitorItem*>(buildResult.shapeByMrid.value(QStringLiteral("series-cap-layout-001")));
    auto* reactor =
        dynamic_cast<CimdrawPowerReactorItem*>(buildResult.shapeByMrid.value(QStringLiteral("series-reactor-layout-001")));

    QVERIFY(capacitor != nullptr);
    QVERIFY(reactor != nullptr);

    QCOMPARE(qRound(capacitor->scenePos().x()), 80);
    QCOMPARE(qRound(capacitor->scenePos().y()), 80);
    QCOMPARE(qRound(capacitor->rotation()), 180);

    QCOMPARE(qRound(reactor->scenePos().x()), 560);
    QCOMPARE(qRound(reactor->scenePos().y()), 320);
    QCOMPARE(qRound(reactor->rotation()), 270);

    QVERIFY(capacitor->scenePos().x() < reactor->scenePos().x());
}

void TestCimGraphics::terminal_connectivity_and_topological_nodes_remain_non_visual()
{
    CimModel model;

    CimObject breaker;
    breaker.mrid = QStringLiteral("breaker-001");
    breaker.className = QStringLiteral("Breaker");
    breaker.name = QStringLiteral("QF1");
    QVERIFY(model.addObject(breaker));

    CimObject terminal;
    terminal.mrid = QStringLiteral("terminal-001");
    terminal.className = QStringLiteral("Terminal");
    terminal.addReference(QStringLiteral("Terminal.ConductingEquipment"), QStringLiteral("breaker-001"));
    terminal.addReference(QStringLiteral("Terminal.ConnectivityNode"), QStringLiteral("connectivity-001"));
    terminal.addReference(QStringLiteral("Terminal.TopologicalNode"), QStringLiteral("topology-001"));
    QVERIFY(model.addObject(terminal));

    CimObject connectivityNode;
    connectivityNode.mrid = QStringLiteral("connectivity-001");
    connectivityNode.className = QStringLiteral("ConnectivityNode");
    connectivityNode.name = QStringLiteral("CN1");
    connectivityNode.addReference(QStringLiteral("ConnectivityNode.TopologicalNode"), QStringLiteral("topology-001"));
    QVERIFY(model.addObject(connectivityNode));

    CimObject topologicalNode;
    topologicalNode.mrid = QStringLiteral("topology-001");
    topologicalNode.className = QStringLiteral("TopologicalNode");
    topologicalNode.name = QStringLiteral("TN1");
    QVERIFY(model.addObject(topologicalNode));

    CimdrawScene scene;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(&scene, model);

    QVERIFY(dynamic_cast<CimdrawPowerCircuitBreakerItem*>(buildResult.shapeByMrid.value(QStringLiteral("breaker-001"))) != nullptr);
    QVERIFY(!buildResult.shapeByMrid.contains(QStringLiteral("terminal-001")));
    QVERIFY(!buildResult.shapeByMrid.contains(QStringLiteral("connectivity-001")));
    QVERIFY(!buildResult.shapeByMrid.contains(QStringLiteral("topology-001")));
}
