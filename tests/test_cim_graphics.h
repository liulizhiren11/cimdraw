#ifndef TEST_CIM_GRAPHICS_H
#define TEST_CIM_GRAPHICS_H

#include <QObject>

class TestCimGraphics : public QObject
{
    Q_OBJECT

private slots:
    void tool_manager_registers_missing_graphics();
    void importer_and_scene_builder_materialize_missing_graphics();
    void importer_and_scene_builder_materialize_projection_infeed_graphics();
    void importer_and_scene_builder_preserve_projection_infeed_layout_from_diagram_objects();
    void importer_and_scene_builder_materialize_source_and_equivalent_grid_graphics();
    void importer_and_scene_builder_preserve_source_and_equivalent_grid_layout_from_diagram_objects();
    void importer_and_scene_builder_materialize_p0_core_device_graphics();
    void importer_and_scene_builder_materialize_reference_source_and_load_graphics();
    void importer_and_scene_builder_materialize_junction_graphics();
    void importer_and_scene_builder_materialize_medium_sample_junction_graphics();
    void importer_and_scene_builder_materialize_transformer_specialization_graphics();
    void importer_and_scene_builder_materialize_transformer_3w_graphics();
    void importer_and_scene_builder_preserve_transformer_3w_layout_when_diagram_data_exists();
    void importer_and_scene_builder_materialize_shunt_compensator_family_graphics();
    void importer_and_scene_builder_preserve_shunt_compensator_family_layout_from_diagram_objects();
    void importer_and_scene_builder_materialize_series_compensator_family_graphics();
    void importer_and_scene_builder_preserve_series_compensator_family_layout_from_diagram_objects();
    void terminal_connectivity_and_topological_nodes_remain_non_visual();

private:
    void source_grid_load_default_ports_follow_node_anchor();
};

#endif // TEST_CIM_GRAPHICS_H
