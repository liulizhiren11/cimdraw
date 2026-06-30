#ifndef TEST_CIM_MODEL_H
#define TEST_CIM_MODEL_H

#include <QObject>

class TestCimModel : public QObject
{
    Q_OBJECT

private slots:
    void object_attributes_roundtrip();
    void model_indexes_objects_by_mrid();
    void model_iteration_order_is_stable_by_mrid();
    void profile_set_tracks_unique_profiles();
    void class_lineage_resolves_complete_upstream_hierarchy();
    void class_lineage_rejects_removed_local_compatibility_shortcuts();
};

#endif // TEST_CIM_MODEL_H
