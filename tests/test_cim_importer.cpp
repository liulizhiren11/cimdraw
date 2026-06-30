#include "test_cim_importer.h"

#include "cim/importer/CgmesPackageImporter.h"
#include "cim/mapping/CimGraphicMapper.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimObject.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

namespace {

QString minimalSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/minimal/EQ_SSH_sample");
}

QString invalidPublicNonCimPath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/public-non-cim-input.json");
}

QString invalidPlainXmlPath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/plain_xml_not_cim/1.xml");
}

QString partialInvalidSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/partial_invalid");
}

QString duplicateMridSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/duplicate_mrid_same_profile");
}

QString classConflictSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/invalid/class_conflict_between_profiles");
}

QString mediumSwitchesSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/Sample_Grid_Switches_NodeBreaker");
}

QString mediumSwitchesSvFilePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/Sample_Grid_Switches_NodeBreaker/20191030T0924Z_XX_YYY_SV_.xml");
}

QString mediumCigreSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/CIGRE_MV");
}

QString mediumCigreReferencePath()
{
    return QFINDTESTDATA("tests/data/cim/medium/CIGREMV_reference");
}

QString transformer3wLayoutSamplePath()
{
    return QFINDTESTDATA("tests/data/cim/minimal/Transformer3w_layout_sample");
}

QString importDiagnostics(const CimImportResult& result)
{
    QStringList lines;
    lines << QStringLiteral("success=%1").arg(result.success);
    lines << QStringLiteral("profiles=%1").arg(result.loadedProfiles.join(QStringLiteral(",")));
    lines << QStringLiteral("errors=%1").arg(result.errors.join(QStringLiteral(" | ")));
    lines << QStringLiteral("warnings=%1").arg(result.warnings.join(QStringLiteral(" | ")));
    lines << QStringLiteral("logs=%1").arg(result.logs.join(QStringLiteral(" | ")));
    lines << QStringLiteral("count=%1").arg(result.model.objectCount());
    return lines.join(QStringLiteral("\n"));
}

bool writeUtf8File(const QString& path, const QString& content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    return file.write(content.toUtf8()) >= 0;
}

}

void TestCimImporter::importer_rejects_missing_path()
{
    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(QStringLiteral("tests/data/cim/not-found"));

    QVERIFY(!result.success);
    QVERIFY(!result.errors.isEmpty());
}

void TestCimImporter::importer_rejects_public_non_cim_file()
{
    CgmesPackageImporter importer;
    const QString path = invalidPublicNonCimPath();
    QVERIFY2(!path.isEmpty(), "未找到公开非 CIM 对照文件");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY(!result.success);
    QVERIFY(result.model.objectCount() == 0);
    QVERIFY(!result.errors.isEmpty());
    QVERIFY(result.errors.join(QStringLiteral(" | "))
                .contains(QStringLiteral("不是受支持的 CIM/CGMES RDF 文件")));
}

void TestCimImporter::importer_rejects_plain_xml_that_is_not_cim_rdf()
{
    CgmesPackageImporter importer;
    const QString path = invalidPlainXmlPath();
    QVERIFY2(!path.isEmpty(), "未找到普通 XML 非 CIM 样例");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY(!result.success);
    QCOMPARE(result.model.objectCount(), 0);
    QVERIFY(!result.errors.isEmpty());
    const QString joinedErrors = result.errors.join(QStringLiteral(" | "));
    QVERIFY(joinedErrors.contains(QStringLiteral("不是受支持的 CIM/CGMES RDF 文件")));
    QVERIFY(!joinedErrors.contains(QStringLiteral("Header")));
    QVERIFY(!joinedErrors.contains(QStringLiteral("item")));
}

void TestCimImporter::importer_reports_partial_success_with_invalid_secondary_file()
{
    CgmesPackageImporter importer;
    const QString path = partialInvalidSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 partial_invalid 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY(!result.success);
    QVERIFY(result.model.objectCount() > 0);
    QVERIFY(result.model.objectByMrid(QStringLiteral("breaker-partial-001")) != nullptr);
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(!result.errors.isEmpty());
}

void TestCimImporter::importer_reports_duplicate_mrid_in_same_profile()
{
    CgmesPackageImporter importer;
    const QString path = duplicateMridSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 duplicate_mrid_same_profile 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY(result.success);
    QVERIFY(result.model.objectCount() > 0);
    QVERIFY(result.errors.isEmpty());
    QVERIFY(!result.warnings.isEmpty());
    QVERIFY(result.model.objectByMrid(QStringLiteral("dup-001")) != nullptr);
    QVERIFY(result.warnings.join(QStringLiteral(" | ")).contains(QStringLiteral("重复定义")));
}

void TestCimImporter::importer_reports_class_conflict_between_profiles()
{
    CgmesPackageImporter importer;
    const QString path = classConflictSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 class_conflict_between_profiles 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY(!result.success);
    QVERIFY(result.model.objectCount() > 0);
    QVERIFY(!result.errors.isEmpty());
    QVERIFY(result.model.objectByMrid(QStringLiteral("conflict-001")) != nullptr);
    QVERIFY(result.errors.join(QStringLiteral(" | ")).contains(QStringLiteral("类名不一致")));
}

void TestCimImporter::importer_creates_minimal_model_for_directory()
{
    CgmesPackageImporter importer;
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QCOMPARE(result.model.objectCount(), 6);
    QCOMPARE(result.loadedProfiles.size(), 2);
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(result.model.objectByMrid(QStringLiteral("breaker-001")) != nullptr);
    QVERIFY(result.model.objectByMrid(QStringLiteral("busbar-001")) != nullptr);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("Breaker")).size(), 1);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("Disconnector")).size(), 1);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("PowerTransformer")).size(), 1);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("EnergyConsumer")).size(), 1);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("SynchronousMachine")).size(), 1);
}

void TestCimImporter::importer_merges_eq_and_ssh_attributes()
{
    CgmesPackageImporter importer;
    const QString path = minimalSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到最小 CIM 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));

    const CimObject* breaker = result.model.objectByMrid(QStringLiteral("breaker-001"));
    QVERIFY(breaker != nullptr);
    QCOMPARE(breaker->className, QStringLiteral("Breaker"));
    QCOMPARE(breaker->name, QStringLiteral("QF1"));
    QCOMPARE(breaker->attribute(QStringLiteral("Switch.open")).toString(), QStringLiteral("false"));
    QVERIFY(!breaker->references.isEmpty());
    QCOMPARE(breaker->references.first().relationName, QStringLiteral("Equipment.EquipmentContainer"));
    QCOMPARE(breaker->references.first().targetMrid, QStringLiteral("bay-001"));

    const CimObject* disconnector = result.model.objectByMrid(QStringLiteral("disconnector-001"));
    QVERIFY(disconnector != nullptr);
    QCOMPARE(disconnector->attribute(QStringLiteral("Switch.open")).toString(), QStringLiteral("true"));
}

void TestCimImporter::importer_reads_profiles_from_fullmodel_metadata()
{
    CgmesPackageImporter importer;
    const QString path = mediumCigreReferencePath();
    QVERIFY2(!path.isEmpty(), "未找到 CIGREMV_reference 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("TP")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SV")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("DL")));
}

void TestCimImporter::importer_accepts_metadata_only_cgmes_file()
{
    CgmesPackageImporter importer;
    const QString path = mediumSwitchesSvFilePath();
    QVERIFY2(!path.isEmpty(), "未找到 Sample_Grid_Switches_NodeBreaker 的 SV 元数据文件");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SV")));
    QCOMPARE(result.model.objectCount(), 0);
    QVERIFY(result.errors.isEmpty());
    QVERIFY(result.logs.join(QStringLiteral(" | ")).contains(QStringLiteral("已跳过对象创建")));
}

void TestCimImporter::importer_detects_multiple_profiles_from_public_sample()
{
    CgmesPackageImporter importer;
    const QString path = mediumSwitchesSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到公开 Sample_Grid_Switches 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.model.objectCount() > 0);
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("TP")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SV")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("DL")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("GL")));
    QVERIFY(result.model.objectsByClassName(QStringLiteral("Breaker")).size() > 0);
    QVERIFY(result.model.objectsByClassName(QStringLiteral("BusbarSection")).size() > 0);
}

void TestCimImporter::importer_loads_medium_public_cigre_sample()
{
    CgmesPackageImporter importer;
    const QString path = mediumCigreSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到公开 CIGRE_MV 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.model.objectCount() > 50);
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("TP")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SV")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("DL")));
    QVERIFY(result.model.objectsByClassName(QStringLiteral("ACLineSegment")).size() > 0
            || result.model.objectsByClassName(QStringLiteral("Breaker")).size() > 0
            || result.model.objectsByClassName(QStringLiteral("EnergyConsumer")).size() > 0);
}

void TestCimImporter::importer_loads_cigre_reference_profiles_and_specialized_classes()
{
    CgmesPackageImporter importer;
    const QString path = mediumCigreReferencePath();
    QVERIFY2(!path.isEmpty(), "未找到公开 CIGREMV_reference 样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.model.objectCount() > 50);
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("TP")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SV")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("DI")));
    QVERIFY(result.model.objectsByClassName(QStringLiteral("SvVoltage")).size() > 0
            || result.model.objectsByClassName(QStringLiteral("SvPowerFlow")).size() > 0
            || result.model.objectsByClassName(QStringLiteral("SvStatus")).size() > 0);
    QVERIFY(result.model.objectsByClassName(QStringLiteral("DiagramObject")).size() > 0
            || result.model.objectsByClassName(QStringLiteral("Diagram")).size() > 0);
}

void TestCimImporter::importer_merges_specialized_gap_classes_with_generic_conducting_equipment()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:gap-merge-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:ConductingEquipment rdf:ID="shunt-merge">
    <cim:IdentifiedObject.name>Generic Shunt</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="series-merge">
    <cim:IdentifiedObject.name>Generic Series</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="async-merge">
    <cim:IdentifiedObject.name>Generic Async</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="external-merge">
    <cim:IdentifiedObject.name>Generic External</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="equivalent-merge">
    <cim:IdentifiedObject.name>Generic Equivalent</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="ground-merge">
    <cim:IdentifiedObject.name>Generic Ground</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ShuntCompensator rdf:ID="linear-merge">
    <cim:IdentifiedObject.name>Generic Linear</cim:IdentifiedObject.name>
  </cim:ShuntCompensator>
</rdf:RDF>)");

    const QString sshXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:gap-merge-ssh">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/SteadyStateHypothesis-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:ShuntCompensator rdf:about="#shunt-merge"/>
  <cim:SeriesCompensator rdf:about="#series-merge"/>
  <cim:AsynchronousMachine rdf:about="#async-merge"/>
  <cim:ExternalNetworkInjection rdf:about="#external-merge"/>
  <cim:EquivalentInjection rdf:about="#equivalent-merge"/>
  <cim:Ground rdf:about="#ground-merge"/>
  <cim:LinearShuntCompensator rdf:about="#linear-merge"/>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("gap_merge_EQ.xml")), eqXml),
             "failed to write EQ test sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("gap_merge_SSH.xml")), sshXml),
             "failed to write SSH test sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QCOMPARE(result.model.objectCount(), 7);
    QVERIFY(!result.warnings.isEmpty());

    const CimObject* shunt = result.model.objectByMrid(QStringLiteral("shunt-merge"));
    const CimObject* series = result.model.objectByMrid(QStringLiteral("series-merge"));
    const CimObject* asyncMachine = result.model.objectByMrid(QStringLiteral("async-merge"));
    const CimObject* externalInjection = result.model.objectByMrid(QStringLiteral("external-merge"));
    const CimObject* equivalentInjection = result.model.objectByMrid(QStringLiteral("equivalent-merge"));
    const CimObject* ground = result.model.objectByMrid(QStringLiteral("ground-merge"));
    const CimObject* linear = result.model.objectByMrid(QStringLiteral("linear-merge"));

    QVERIFY(shunt != nullptr);
    QVERIFY(series != nullptr);
    QVERIFY(asyncMachine != nullptr);
    QVERIFY(externalInjection != nullptr);
    QVERIFY(equivalentInjection != nullptr);
    QVERIFY(ground != nullptr);
    QVERIFY(linear != nullptr);

    QCOMPARE(shunt->className, QStringLiteral("ShuntCompensator"));
    QCOMPARE(series->className, QStringLiteral("SeriesCompensator"));
    QCOMPARE(asyncMachine->className, QStringLiteral("AsynchronousMachine"));
    QCOMPARE(externalInjection->className, QStringLiteral("ExternalNetworkInjection"));
    QCOMPARE(equivalentInjection->className, QStringLiteral("EquivalentInjection"));
    QCOMPARE(ground->className, QStringLiteral("Ground"));
    QCOMPARE(linear->className, QStringLiteral("LinearShuntCompensator"));
}

void TestCimImporter::importer_merges_terminal_with_generic_acdcterminal_and_identifiedobject()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:terminal-lineage-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:Terminal rdf:ID="terminal-lineage-001">
    <cim:IdentifiedObject.name>T-A</cim:IdentifiedObject.name>
  </cim:Terminal>
  <cim:ACDCTerminal rdf:about="#terminal-lineage-001">
    <cim:IdentifiedObject.description>acdcterminal overlay</cim:IdentifiedObject.description>
  </cim:ACDCTerminal>
  <cim:IdentifiedObject rdf:about="#terminal-lineage-001">
    <cim:IdentifiedObject.aliasName>terminal alias</cim:IdentifiedObject.aliasName>
  </cim:IdentifiedObject>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("terminal_lineage_EQ.xml")), eqXml),
             "failed to write terminal lineage EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    const CimObject* terminal = result.model.objectByMrid(QStringLiteral("terminal-lineage-001"));
    QVERIFY(terminal != nullptr);
    QCOMPARE(terminal->className, QStringLiteral("Terminal"));
    QCOMPARE(terminal->name, QStringLiteral("T-A"));
    QCOMPARE(terminal->attribute(QStringLiteral("IdentifiedObject.description")).toString(),
             QStringLiteral("acdcterminal overlay"));
    QCOMPARE(terminal->attribute(QStringLiteral("IdentifiedObject.aliasName")).toString(),
             QStringLiteral("terminal alias"));
}

void TestCimImporter::importer_merges_voltagelevel_with_deep_container_lineage()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:voltagelevel-lineage-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:VoltageLevel rdf:ID="voltagelevel-lineage-001">
    <cim:IdentifiedObject.name>VL-A</cim:IdentifiedObject.name>
  </cim:VoltageLevel>
  <cim:EquipmentContainer rdf:about="#voltagelevel-lineage-001">
    <cim:IdentifiedObject.description>equipment container overlay</cim:IdentifiedObject.description>
  </cim:EquipmentContainer>
  <cim:ConnectivityNodeContainer rdf:about="#voltagelevel-lineage-001">
    <cim:IdentifiedObject.aliasName>connectivity node container overlay</cim:IdentifiedObject.aliasName>
  </cim:ConnectivityNodeContainer>
  <cim:PowerSystemResource rdf:about="#voltagelevel-lineage-001">
    <cim:IdentifiedObject.mRID>voltagelevel-lineage-001</cim:IdentifiedObject.mRID>
  </cim:PowerSystemResource>
  <cim:IdentifiedObject rdf:about="#voltagelevel-lineage-001">
    <cim:IdentifiedObject.shortName>VL-SHORT</cim:IdentifiedObject.shortName>
  </cim:IdentifiedObject>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("voltagelevel_lineage_EQ.xml")), eqXml),
             "failed to write voltagelevel lineage EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    const CimObject* voltageLevel = result.model.objectByMrid(QStringLiteral("voltagelevel-lineage-001"));
    QVERIFY(voltageLevel != nullptr);
    QCOMPARE(voltageLevel->className, QStringLiteral("VoltageLevel"));
    QCOMPARE(voltageLevel->name, QStringLiteral("VL-A"));
    QCOMPARE(voltageLevel->attribute(QStringLiteral("IdentifiedObject.description")).toString(),
             QStringLiteral("equipment container overlay"));
    QCOMPARE(voltageLevel->attribute(QStringLiteral("IdentifiedObject.aliasName")).toString(),
             QStringLiteral("connectivity node container overlay"));
    QCOMPARE(voltageLevel->attribute(QStringLiteral("IdentifiedObject.shortName")).toString(),
             QStringLiteral("VL-SHORT"));
}

void TestCimImporter::importer_does_not_merge_measurement_and_fuse_classes_with_generic_conducting_equipment()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:measure-gap-merge-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:ConductingEquipment rdf:ID="ct-merge">
    <cim:IdentifiedObject.name>Generic CT</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="pt-merge">
    <cim:IdentifiedObject.name>Generic PT</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="vt-merge">
    <cim:IdentifiedObject.name>Generic VT</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="fuse-merge">
    <cim:IdentifiedObject.name>Generic Fuse</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
  <cim:ConductingEquipment rdf:ID="arrester-merge">
    <cim:IdentifiedObject.name>Generic Arrester</cim:IdentifiedObject.name>
  </cim:ConductingEquipment>
</rdf:RDF>)");

    const QString sshXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:measure-gap-merge-ssh">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/SteadyStateHypothesis-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:CurrentTransformer rdf:about="#ct-merge"/>
  <cim:PotentialTransformer rdf:about="#pt-merge"/>
  <cim:VoltageTransformer rdf:about="#vt-merge"/>
  <cim:Fuse rdf:about="#fuse-merge"/>
  <cim:SurgeArrester rdf:about="#arrester-merge"/>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("measure_merge_EQ.xml")), eqXml),
             "failed to write EQ measurement/fuse sample");
    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("measure_merge_SSH.xml")), sshXml),
             "failed to write SSH measurement/fuse sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());

    QVERIFY(!result.success);
    QCOMPARE(result.model.objectCount(), 5);
    QVERIFY(!result.errors.isEmpty());
    QVERIFY(result.errors.join(QStringLiteral(" | ")).contains(QStringLiteral("类名不一致")));

    const CimObject* ct = result.model.objectByMrid(QStringLiteral("ct-merge"));
    const CimObject* pt = result.model.objectByMrid(QStringLiteral("pt-merge"));
    const CimObject* vt = result.model.objectByMrid(QStringLiteral("vt-merge"));
    const CimObject* fuse = result.model.objectByMrid(QStringLiteral("fuse-merge"));
    const CimObject* arrester = result.model.objectByMrid(QStringLiteral("arrester-merge"));

    QVERIFY(ct != nullptr);
    QVERIFY(pt != nullptr);
    QVERIFY(vt != nullptr);
    QVERIFY(fuse != nullptr);
    QVERIFY(arrester != nullptr);

    QCOMPARE(ct->className, QStringLiteral("ConductingEquipment"));
    QCOMPARE(pt->className, QStringLiteral("ConductingEquipment"));
    QCOMPARE(vt->className, QStringLiteral("ConductingEquipment"));
    QCOMPARE(fuse->className, QStringLiteral("ConductingEquipment"));
    QCOMPARE(arrester->className, QStringLiteral("ConductingEquipment"));
}

void TestCimImporter::importer_synthesizes_inverse_transformer_end_references()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString eqXml = QString::fromLatin1(R"(<?xml version="1.0" encoding="UTF-8"?>
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns:cim="http://iec.ch/TC57/2013/CIM-schema-cim16#"
         xmlns:md="http://iec.ch/TC57/61970-552/ModelDescription/1#">
  <md:FullModel rdf:about="urn:uuid:xfmr-3w-eq">
    <md:Model.profile>http://iec.ch/TC57/ns/CIM/Equipment-EU/3.0</md:Model.profile>
  </md:FullModel>
  <cim:PowerTransformer rdf:ID="xfmr-3w">
    <cim:IdentifiedObject.name>T3W</cim:IdentifiedObject.name>
  </cim:PowerTransformer>
  <cim:PowerTransformerEnd rdf:ID="xfmr-end-a">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-end-b">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w"/>
  </cim:PowerTransformerEnd>
  <cim:PowerTransformerEnd rdf:ID="xfmr-end-c">
    <cim:PowerTransformerEnd.PowerTransformer rdf:resource="#xfmr-3w"/>
  </cim:PowerTransformerEnd>
</rdf:RDF>)");

    QVERIFY2(writeUtf8File(dir.filePath(QStringLiteral("xfmr_3w_EQ.xml")), eqXml),
             "failed to write 3w transformer EQ sample");

    CgmesPackageImporter importer;
    const CimImportResult result = importer.importFromPath(dir.path());

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    const CimObject* transformer = result.model.objectByMrid(QStringLiteral("xfmr-3w"));
    QVERIFY(transformer != nullptr);

    int endRefCount = 0;
    for (const CimReference& reference : transformer->references)
    {
        if (reference.relationName == QStringLiteral("PowerTransformer.PowerTransformerEnd"))
            ++endRefCount;
    }
    QCOMPARE(endRefCount, 3);
}

void TestCimImporter::importer_loads_transformer_3w_layout_sample_with_diagram_objects()
{
    CgmesPackageImporter importer;
    const QString path = transformer3wLayoutSamplePath();
    QVERIFY2(!path.isEmpty(), "未找到 Transformer3w_layout_sample 最小样例目录");
    const CimImportResult result = importer.importFromPath(path);

    QVERIFY2(result.success, qPrintable(importDiagnostics(result)));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("EQ")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("SSH")));
    QVERIFY(result.loadedProfiles.contains(QStringLiteral("DL")));

    const CimObject* transformer = result.model.objectByMrid(QStringLiteral("xfmr-3w-layout-001"));
    const CimObject* diagramObject = result.model.objectByMrid(QStringLiteral("diagram-xfmr-3w-layout-001"));
    const CimObject* point1 = result.model.objectByMrid(QStringLiteral("diagram-xfmr-3w-layout-001-p1"));
    const CimObject* point2 = result.model.objectByMrid(QStringLiteral("diagram-xfmr-3w-layout-001-p2"));

    QVERIFY(transformer != nullptr);
    QVERIFY(diagramObject != nullptr);
    QVERIFY(point1 != nullptr);
    QVERIFY(point2 != nullptr);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("DiagramObject")).size(), 1);
    QCOMPARE(result.model.objectsByClassName(QStringLiteral("DiagramObjectPoint")).size(), 2);
    QCOMPARE(diagramObject->attribute(QStringLiteral("DiagramObject.rotation")).toString(), QStringLiteral("270"));
    QCOMPARE(point1->attribute(QStringLiteral("DiagramObjectPoint.xPosition")).toString(), QStringLiteral("100"));
    QCOMPARE(point2->attribute(QStringLiteral("DiagramObjectPoint.xPosition")).toString(), QStringLiteral("260"));
}

void TestCimImporter::graphic_mapper_maps_core_classes()
{
    CimGraphicMapper mapper;

    CimObject breaker;
    breaker.mrid = QStringLiteral("mrid-breaker");
    breaker.className = QStringLiteral("Breaker");

    CimObject disconnector;
    disconnector.mrid = QStringLiteral("mrid-disconnector");
    disconnector.className = QStringLiteral("Disconnector");

    CimObject busbar;
    busbar.mrid = QStringLiteral("mrid-busbar");
    busbar.className = QStringLiteral("BusbarSection");

    CimObject transformer;
    transformer.mrid = QStringLiteral("mrid-transformer");
    transformer.className = QStringLiteral("PowerTransformer");
    transformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-1"));
    transformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-2"));

    CimObject autoTransformer;
    autoTransformer.mrid = QStringLiteral("mrid-transformer-auto");
    autoTransformer.className = QStringLiteral("PowerTransformer");
    autoTransformer.name = QStringLiteral("1#自耦变");
    autoTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-a"));
    autoTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-b"));

    CimObject stationTransformer;
    stationTransformer.mrid = QStringLiteral("mrid-transformer-station");
    stationTransformer.className = QStringLiteral("PowerTransformer");
    stationTransformer.name = QStringLiteral("T2");
    stationTransformer.setAttribute(QStringLiteral("IdentifiedObject.aliasName"),
                                    QStringLiteral("Auxiliary Transformer"));
    stationTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-c"));
    stationTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-d"));

    CimObject load;
    load.mrid = QStringLiteral("mrid-load");
    load.className = QStringLiteral("EnergyConsumer");

    CimObject generator;
    generator.mrid = QStringLiteral("mrid-generator");
    generator.className = QStringLiteral("SynchronousMachine");

    CimObject unsupported;
    unsupported.mrid = QStringLiteral("mrid-unsupported");
    unsupported.className = QStringLiteral("Terminal");

    QCOMPARE(mapper.drawTypeForObject(breaker), LZ_SLD_CIRCUIT_BREAKER);
    QCOMPARE(mapper.drawTypeForObject(disconnector), LZ_SLD_DISCONNECTOR);
    QCOMPARE(mapper.drawTypeForObject(busbar), LZ_SLD_BUSBAR_SECTION);
    QCOMPARE(mapper.drawTypeForObject(transformer), LZ_SLD_TRANSFORMER_2W);
    QCOMPARE(mapper.drawTypeForObject(autoTransformer), LZ_WSYM_AUTO_XFMR);
    QCOMPARE(mapper.drawTypeForObject(stationTransformer), LZ_WSYM_STATION_XFMR);
    QCOMPARE(mapper.drawTypeForObject(load), LZ_SLD_LOAD);
    QCOMPARE(mapper.drawTypeForObject(generator), LZ_SLD_GENERATOR);
    QVERIFY(mapper.canMap(breaker));
    QVERIFY(mapper.canMap(disconnector));
    QVERIFY(mapper.canMap(busbar));
    QVERIFY(mapper.canMap(transformer));
    QVERIFY(mapper.canMap(autoTransformer));
    QVERIFY(mapper.canMap(stationTransformer));
    QVERIFY(mapper.canMap(load));
    QVERIFY(mapper.canMap(generator));
    QVERIFY(!mapper.canMap(unsupported));
}

void TestCimImporter::graphic_mapper_maps_p0_gap_classes()
{
    CimGraphicMapper mapper;

    CimObject line;
    line.mrid = QStringLiteral("mrid-line");
    line.className = QStringLiteral("ACLineSegment");

    CimObject loadBreak;
    loadBreak.mrid = QStringLiteral("mrid-load-break");
    loadBreak.className = QStringLiteral("LoadBreakSwitch");

    CimObject groundDisconnector;
    groundDisconnector.mrid = QStringLiteral("mrid-ground-disconnector");
    groundDisconnector.className = QStringLiteral("GroundDisconnector");

    CimObject shunt;
    shunt.mrid = QStringLiteral("mrid-shunt");
    shunt.className = QStringLiteral("LinearShuntCompensator");

    QCOMPARE(mapper.drawTypeForObject(line), LZ_WSYM_CABLE);
    QCOMPARE(mapper.drawTypeForObject(loadBreak), LZ_WSYM_LOAD_BREAK);
    QCOMPARE(mapper.drawTypeForObject(groundDisconnector), LZ_WSYM_EARTH_SWITCH);
    QCOMPARE(mapper.drawTypeForObject(shunt), LZ_WSYM_CAPACITOR);
    QVERIFY(mapper.canMap(line));
    QVERIFY(mapper.canMap(loadBreak));
    QVERIFY(mapper.canMap(groundDisconnector));
    QVERIFY(mapper.canMap(shunt));
}

void TestCimImporter::graphic_mapper_maps_p1_reuse_classes()
{
    CimGraphicMapper mapper;

    CimObject shunt;
    shunt.mrid = QStringLiteral("mrid-shunt-p1");
    shunt.className = QStringLiteral("ShuntCompensator");

    CimObject series;
    series.mrid = QStringLiteral("mrid-series");
    series.className = QStringLiteral("SeriesCompensator");

    CimObject asyncMachine;
    asyncMachine.mrid = QStringLiteral("mrid-async");
    asyncMachine.className = QStringLiteral("AsynchronousMachine");

    CimObject externalInjection;
    externalInjection.mrid = QStringLiteral("mrid-external");
    externalInjection.className = QStringLiteral("ExternalNetworkInjection");

    CimObject equivalentInjection;
    equivalentInjection.mrid = QStringLiteral("mrid-equivalent");
    equivalentInjection.className = QStringLiteral("EquivalentInjection");

    CimObject ground;
    ground.mrid = QStringLiteral("mrid-ground");
    ground.className = QStringLiteral("Ground");

    CimObject arrester;
    arrester.mrid = QStringLiteral("mrid-arrester");
    arrester.className = QStringLiteral("SurgeArrester");

    QCOMPARE(mapper.drawTypeForObject(shunt), LZ_WSYM_CAPACITOR);
    QCOMPARE(mapper.drawTypeForObject(series), LZ_WSYM_REACTOR);
    QCOMPARE(mapper.drawTypeForObject(asyncMachine), LZ_SLD_MOTOR);
    QCOMPARE(mapper.drawTypeForObject(externalInjection), LZ_WSYM_GRID);
    QCOMPARE(mapper.drawTypeForObject(equivalentInjection), LZ_WSYM_GRID);
    QCOMPARE(mapper.drawTypeForObject(arrester), LZ_WSYM_ARRESTER);
    QCOMPARE(mapper.drawTypeForObject(ground), LZ_SLD_GROUND);

    QVERIFY(mapper.canMap(shunt));
    QVERIFY(mapper.canMap(series));
    QVERIFY(mapper.canMap(asyncMachine));
    QVERIFY(mapper.canMap(externalInjection));
    QVERIFY(mapper.canMap(equivalentInjection));
    QVERIFY(mapper.canMap(arrester));
    QVERIFY(mapper.canMap(ground));
}

void TestCimImporter::graphic_mapper_maps_p2_missing_graphics()
{
    CimGraphicMapper mapper;

    CimObject currentTransformer;
    currentTransformer.mrid = QStringLiteral("mrid-ct");
    currentTransformer.className = QStringLiteral("CurrentTransformer");

    CimObject potentialTransformer;
    potentialTransformer.mrid = QStringLiteral("mrid-pt");
    potentialTransformer.className = QStringLiteral("PotentialTransformer");

    CimObject voltageTransformer;
    voltageTransformer.mrid = QStringLiteral("mrid-vt");
    voltageTransformer.className = QStringLiteral("VoltageTransformer");

    CimObject fuse;
    fuse.mrid = QStringLiteral("mrid-fuse");
    fuse.className = QStringLiteral("Fuse");

    QCOMPARE(mapper.drawTypeForObject(currentTransformer), LZ_WSYM_CURRENT_TRANSFORMER);
    QCOMPARE(mapper.drawTypeForObject(potentialTransformer), LZ_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(mapper.drawTypeForObject(voltageTransformer), LZ_WSYM_VOLTAGE_TRANSFORMER);
    QCOMPARE(mapper.drawTypeForObject(fuse), LZ_WSYM_FUSE);

    QVERIFY(mapper.canMap(currentTransformer));
    QVERIFY(mapper.canMap(potentialTransformer));
    QVERIFY(mapper.canMap(voltageTransformer));
    QVERIFY(mapper.canMap(fuse));
}

void TestCimImporter::graphic_mapper_maps_projection_infeed_classes()
{
    CimGraphicMapper mapper;

    CimObject solar;
    solar.mrid = QStringLiteral("mrid-solar");
    solar.className = QStringLiteral("SolarGeneratingUnit");

    CimObject wind;
    wind.mrid = QStringLiteral("mrid-wind");
    wind.className = QStringLiteral("WindGeneratingUnit");

    CimObject battery;
    battery.mrid = QStringLiteral("mrid-battery");
    battery.className = QStringLiteral("BatteryUnit");

    QCOMPARE(mapper.drawTypeForObject(solar), LZ_WSYM_PV_INFEED);
    QCOMPARE(mapper.drawTypeForObject(wind), LZ_WSYM_WIND_INFEED);
    QCOMPARE(mapper.drawTypeForObject(battery), LZ_WSYM_ESS);

    QVERIFY(mapper.canMap(solar));
    QVERIFY(mapper.canMap(wind));
    QVERIFY(mapper.canMap(battery));
}

void TestCimImporter::graphic_mapper_maps_transformer_3w_when_three_ends_exist()
{
    CimGraphicMapper mapper;

    CimObject transformer2w;
    transformer2w.mrid = QStringLiteral("xfmr-2w");
    transformer2w.className = QStringLiteral("PowerTransformer");
    transformer2w.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-1"));
    transformer2w.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-2"));

    CimObject transformer3w;
    transformer3w.mrid = QStringLiteral("xfmr-3w");
    transformer3w.className = QStringLiteral("PowerTransformer");
    transformer3w.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-a"));
    transformer3w.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-b"));
    transformer3w.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-c"));

    QCOMPARE(mapper.drawTypeForObject(transformer2w), LZ_SLD_TRANSFORMER_2W);
    QCOMPARE(mapper.drawTypeForObject(transformer3w), LZ_WSYM_TRANSFORMER_3W);
    QVERIFY(mapper.canMap(transformer3w));
}

void TestCimImporter::graphic_mapper_refines_power_transformer_specializations_from_name_tokens()
{
    CimGraphicMapper mapper;

    CimObject autoTransformer;
    autoTransformer.mrid = QStringLiteral("xfmr-auto");
    autoTransformer.className = QStringLiteral("PowerTransformer");
    autoTransformer.name = QStringLiteral("1#自耦变");
    autoTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-a"));
    autoTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-b"));

    CimObject earthingTransformer;
    earthingTransformer.mrid = QStringLiteral("xfmr-earthing");
    earthingTransformer.className = QStringLiteral("PowerTransformer");
    earthingTransformer.name = QStringLiteral("Earthing Transformer");
    earthingTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-g"));
    earthingTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-h"));

    CimObject stationTransformer;
    stationTransformer.mrid = QStringLiteral("xfmr-station");
    stationTransformer.className = QStringLiteral("PowerTransformer");
    stationTransformer.name = QStringLiteral("Station Service Transformer");
    stationTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-c"));
    stationTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-d"));

    CimObject plainTransformer;
    plainTransformer.mrid = QStringLiteral("xfmr-plain");
    plainTransformer.className = QStringLiteral("PowerTransformer");
    plainTransformer.name = QStringLiteral("Main Transformer");
    plainTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-e"));
    plainTransformer.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"), QStringLiteral("end-f"));

    CimObject autoTransformerByAttribute;
    autoTransformerByAttribute.mrid = QStringLiteral("xfmr-auto-attr");
    autoTransformerByAttribute.className = QStringLiteral("PowerTransformer");
    autoTransformerByAttribute.name = QStringLiteral("T1");
    autoTransformerByAttribute.setAttribute(QStringLiteral("IdentifiedObject.description"),
                                            QStringLiteral("Auto Transformer for feeder tie"));
    autoTransformerByAttribute.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                            QStringLiteral("end-i"));
    autoTransformerByAttribute.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                            QStringLiteral("end-j"));

    CimObject stationTransformerByAttribute;
    stationTransformerByAttribute.mrid = QStringLiteral("xfmr-station-attr");
    stationTransformerByAttribute.className = QStringLiteral("PowerTransformer");
    stationTransformerByAttribute.name = QStringLiteral("T2");
    stationTransformerByAttribute.setAttribute(QStringLiteral("IdentifiedObject.aliasName"),
                                               QStringLiteral("Auxiliary Transformer"));
    stationTransformerByAttribute.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                               QStringLiteral("end-k"));
    stationTransformerByAttribute.addReference(QStringLiteral("PowerTransformer.PowerTransformerEnd"),
                                               QStringLiteral("end-l"));

    QCOMPARE(mapper.drawTypeForObject(autoTransformer), LZ_WSYM_AUTO_XFMR);
    QCOMPARE(mapper.drawTypeForObject(earthingTransformer), LZ_WSYM_EARTHING_XFMR);
    QCOMPARE(mapper.drawTypeForObject(stationTransformer), LZ_WSYM_STATION_XFMR);
    QCOMPARE(mapper.drawTypeForObject(plainTransformer), LZ_SLD_TRANSFORMER_2W);
    QCOMPARE(mapper.drawTypeForObject(autoTransformerByAttribute), LZ_WSYM_AUTO_XFMR);
    QCOMPARE(mapper.drawTypeForObject(stationTransformerByAttribute), LZ_WSYM_STATION_XFMR);
}

void TestCimImporter::graphic_mapper_refines_shunt_compensator_family_from_parameters_and_classes()
{
    CimGraphicMapper mapper;

    CimObject capacitor;
    capacitor.mrid = QStringLiteral("cap-bank");
    capacitor.className = QStringLiteral("ShuntCompensator");
    capacitor.setAttribute(QStringLiteral("ShuntCompensator.bPerSection"), QStringLiteral("0.0042"));

    CimObject reactor;
    reactor.mrid = QStringLiteral("reactor-bank");
    reactor.className = QStringLiteral("LinearShuntCompensator");
    reactor.setAttribute(QStringLiteral("ShuntCompensator.bPerSection"), QStringLiteral("-0.0035"));

    CimObject splitReactor;
    splitReactor.mrid = QStringLiteral("split-reactor");
    splitReactor.className = QStringLiteral("LinearShuntCompensator");
    splitReactor.name = QStringLiteral("Split Reactor 1");
    splitReactor.setAttribute(QStringLiteral("ShuntCompensator.bPerSection"), QStringLiteral("-0.0030"));
    splitReactor.setAttribute(QStringLiteral("ShuntCompensator.maximumSections"), QStringLiteral("2"));

    CimObject nonlinearReactor;
    nonlinearReactor.mrid = QStringLiteral("nonlinear-reactor");
    nonlinearReactor.className = QStringLiteral("NonlinearShuntCompensator");
    nonlinearReactor.name = QStringLiteral("Nonlinear Reactor 1");
    nonlinearReactor.setAttribute(QStringLiteral("ShuntCompensator.bPerSection"), QStringLiteral("-0.0025"));

    CimObject svg;
    svg.mrid = QStringLiteral("svg-comp");
    svg.className = QStringLiteral("StaticVarCompensator");
    svg.name = QStringLiteral("SVG1");

    CimObject arcCoil;
    arcCoil.mrid = QStringLiteral("arc-coil");
    arcCoil.className = QStringLiteral("PetersenCoil");
    arcCoil.name = QStringLiteral("Petersen 1");

    QCOMPARE(mapper.drawTypeForObject(capacitor), LZ_WSYM_CAPACITOR);
    QCOMPARE(mapper.drawTypeForObject(reactor), LZ_WSYM_REACTOR);
    QCOMPARE(mapper.drawTypeForObject(splitReactor), LZ_WSYM_SPLIT_REACTOR);
    QCOMPARE(mapper.drawTypeForObject(nonlinearReactor), LZ_WSYM_REACTOR);
    QCOMPARE(mapper.drawTypeForObject(svg), LZ_WSYM_SVG_COMP);
    QCOMPARE(mapper.drawTypeForObject(arcCoil), LZ_WSYM_ARC_COIL);
}

void TestCimImporter::graphic_mapper_refines_series_compensator_family_from_name_and_reactance()
{
    CimGraphicMapper mapper;

    CimObject seriesCapacitor;
    seriesCapacitor.mrid = QStringLiteral("series-cap");
    seriesCapacitor.className = QStringLiteral("SeriesCompensator");
    seriesCapacitor.name = QStringLiteral("Series Capacitor Bank");

    CimObject seriesReactor;
    seriesReactor.mrid = QStringLiteral("series-reactor");
    seriesReactor.className = QStringLiteral("SeriesCompensator");
    seriesReactor.name = QStringLiteral("Current Limiting Reactor");

    CimObject reactanceCapacitor;
    reactanceCapacitor.mrid = QStringLiteral("series-cap-x");
    reactanceCapacitor.className = QStringLiteral("SeriesCompensator");
    reactanceCapacitor.setAttribute(QStringLiteral("SeriesCompensator.x"), QStringLiteral("-0.0045"));

    CimObject reactanceReactor;
    reactanceReactor.mrid = QStringLiteral("series-reactor-x");
    reactanceReactor.className = QStringLiteral("SeriesCompensator");
    reactanceReactor.setAttribute(QStringLiteral("SeriesCompensator.x"), QStringLiteral("0.0072"));

    QCOMPARE(mapper.drawTypeForObject(seriesCapacitor), LZ_WSYM_CAPACITOR);
    QCOMPARE(mapper.drawTypeForObject(seriesReactor), LZ_WSYM_REACTOR);
    QCOMPARE(mapper.drawTypeForObject(reactanceCapacitor), LZ_WSYM_CAPACITOR);
    QCOMPARE(mapper.drawTypeForObject(reactanceReactor), LZ_WSYM_REACTOR);
}

void TestCimImporter::id_index_binds_bidirectionally()
{
    CimIdIndex index;
    index.bindGraphicItem(QStringLiteral("mrid-001"), QStringLiteral("gfx-001"));

    QVERIFY(index.hasBinding(QStringLiteral("mrid-001")));
    QCOMPARE(index.bindingCount(), 1);
    QCOMPARE(index.graphicItemIdForMrid(QStringLiteral("mrid-001")), QStringLiteral("gfx-001"));
    QCOMPARE(index.mridForGraphicItemId(QStringLiteral("gfx-001")), QStringLiteral("mrid-001"));
}
