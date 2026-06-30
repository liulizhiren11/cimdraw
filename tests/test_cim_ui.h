#ifndef TEST_CIM_UI_H
#define TEST_CIM_UI_H

#include <QObject>

class TestCimUi : public QObject
{
    Q_OBJECT

private slots:
    void file_menu_exposes_import_cim_action();
    void import_creates_browser_dock_and_scene_items();
    void medium_sample_import_keeps_browser_visible_and_filterable();
    void cigre_reference_import_shows_specialized_groups_in_browser();
    void browser_classifies_reference_sample_standard_non_device_objects();
    void browser_classifies_cigre_public_name_objects_as_metadata();
    void browser_marks_reference_source_and_load_objects_as_visualized();
    void browser_marks_projection_infeed_objects_as_visualized();
    void browser_marks_source_and_equivalent_grid_objects_as_visualized();
    void import_entry_accepts_directory_path_directly();
    void import_entry_accepts_xml_file_path_by_resolving_parent_directory();
    void import_with_recoverable_warnings_keeps_summary_in_dock();
    void import_layout_spreads_primary_equipment_positions();
    void import_generates_readable_connection_lines();
    void browser_marks_visualization_statuses_for_medium_sample();
    void browser_summary_reports_visualization_coverage();
    void browser_summary_explains_aclinesegment_vs_helper_lines();
    void browser_summary_classifies_grounding_impedance_as_unmapped_device();
    void browser_marks_shunt_compensator_family_sample_as_visualized();
    void browser_marks_series_compensator_family_as_visualized();
    void browser_marks_power_transformer_specializations_as_visualized();
    void browser_selection_locates_scene_item();
    void scene_selection_syncs_browser_item();
};

#endif // TEST_CIM_UI_H
