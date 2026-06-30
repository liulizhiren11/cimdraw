#include "test_cim_model.h"

#include "cim/model/CimClassLineage.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimProfileSet.h"

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
