#ifndef TEST_CIM_MODEL_H
#define TEST_CIM_MODEL_H

#include <QObject>

class TestCimModel : public QObject
{
    Q_OBJECT

private slots:
    void object_attributes_roundtrip();
    void model_indexes_objects_by_mrid();
    void model_supports_sourcefile_queries_and_statistics();
    void model_iteration_order_is_stable_by_mrid();
    void semantic_views_expose_minimal_read_model();
    void model_validation_reports_missing_references_and_graphic_binding_gaps();
    void model_subset_exporter_writes_minimal_xml_and_report();
    void model_writeback_applies_controlled_key_object_patch();
    void model_diff_reports_added_removed_and_changed_objects();
    void profile_set_tracks_unique_profiles();
    void class_lineage_resolves_complete_upstream_hierarchy();
    void class_lineage_rejects_removed_local_compatibility_shortcuts();
};

#endif // TEST_CIM_MODEL_H
