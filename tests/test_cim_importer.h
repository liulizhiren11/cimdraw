#ifndef TEST_CIM_IMPORTER_H
#define TEST_CIM_IMPORTER_H

#include <QObject>

class TestCimImporter : public QObject
{
    Q_OBJECT

private slots:
    void importer_rejects_missing_path();
    void importer_rejects_public_non_cim_file();
    void importer_rejects_plain_xml_that_is_not_cim_rdf();
    void importer_reports_partial_success_with_invalid_secondary_file();
    void importer_reports_duplicate_mrid_in_same_profile();
    void importer_reports_class_conflict_between_profiles();
    void importer_creates_minimal_model_for_directory();
    void importer_merges_eq_and_ssh_attributes();
    void importer_reads_profiles_from_fullmodel_metadata();
    void importer_accepts_metadata_only_cgmes_file();
    void importer_detects_multiple_profiles_from_public_sample();
    void importer_loads_medium_public_cigre_sample();
    void importer_loads_cigre_reference_profiles_and_specialized_classes();
    void importer_merges_specialized_gap_classes_with_generic_conducting_equipment();
    void importer_merges_terminal_with_generic_acdcterminal_and_identifiedobject();
    void importer_merges_voltagelevel_with_deep_container_lineage();
    void importer_synthesizes_inverse_transformer_end_references();
    void importer_loads_transformer_3w_layout_sample_with_diagram_objects();
    void graphic_mapper_maps_core_classes();
    void graphic_mapper_maps_p0_gap_classes();
    void graphic_mapper_maps_p1_reuse_classes();
    void graphic_mapper_maps_p2_missing_graphics();
    void graphic_mapper_maps_projection_infeed_classes();
    void graphic_mapper_maps_transformer_3w_when_three_ends_exist();
    void graphic_mapper_refines_power_transformer_specializations_from_name_tokens();
    void graphic_mapper_refines_shunt_compensator_family_from_parameters_and_classes();
    void graphic_mapper_refines_series_compensator_family_from_name_and_reactance();
    void importer_does_not_merge_measurement_and_fuse_classes_with_generic_conducting_equipment();
    void id_index_binds_bidirectionally();
};

#endif // TEST_CIM_IMPORTER_H
