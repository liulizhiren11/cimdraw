# CIM 标准图元补齐执行 ToDoList

最后整理日期：2026-06-29

## 1. 目的

这份清单只服务一件事：把“根据标准 CIM 建模整理出来的图元缺口与补强项”压缩成可以直接排期、开发和回归的执行列表。

补充说明：

1. 更细的阶段性拆分文档《CIM标准图元补齐本周任务清单.md》和《CIM标准图元补齐具体开发任务表.md》已在本轮整理中删除。
2. 相关完成状态与后续剩余事项统一以本文件和《CIM标准图元补齐ToDo与测试验收方案.md》为准。

使用时建议先按下面三类口径判断，不要再把它们混在一起：

1. `真实缺失图元`
   当前标准设备类没有稳定图元承接。
2. `非图元语义对象`
   在浏览器里能看到，但按标准口径本来就不应直接画成一次设备图元。
3. `已补齐但待细化`
   已经有图元和场景实例化，只是还要继续做真实样例回归、布局和命名细化。

## 2. 当前结论

### 2.1 当前“真实缺失图元”清单

按当前仓库代码和测试状态，截至 2026-06-23，本轮标准图元缺口已经清零。

| 类型 | 结论 |
| --- | --- |
| 真实缺失图元 | 当前无 |
| 当前工作重点 | 从“补缺图元”转向“已补齐图元的真实样例回归、布局细化、统计口径细化” |

本轮复核证据补充（2026-06-23）：

1. 扫描范围：
   `tests/data/cim/medium/CIGRE_MV`
   `tests/data/cim/medium/CIGREMV_reference`
   `tests/data/cim/medium/Sample_Grid_Switches_NodeBreaker`
   `tests/data/cim/minimal`
2. 去重后共得到 `49` 个唯一 CIM 类名。
3. 分类结果：
   `18` 个已映射设备类；
   `7` 个语义对象；
   `3` 个布局对象；
   `12` 个元数据/容器对象；
   `9` 个抽象层对象。
4. `uncovered = 0`，即当前样例范围内没有落在“既未映射、也未被浏览器分类收口”的标准类。

### 2.2 容易被误判成“缺图元”的对象

下面这些对象在模型浏览器里可见，但按标准建模口径不应直接画成一次设备图元：

| CIM 类 | 正确口径 | 当前状态 |
| --- | --- | --- |
| `Terminal` | 端子语义对象，投影到连接点，不单独实例化为设备 | 已稳定 |
| `ConnectivityNode` | 连通语义对象，不单独实例化为设备 | 已稳定 |
| `TopologicalNode` | 拓扑语义对象，不单独实例化为设备 | 已稳定 |
| `DiagramObject` / `DiagramObjectPoint` | 布局对象，不是一次设备 | 已稳定 |
| `Diagram` / `CoordinateSystem` | 图纸/坐标元数据，不是一次设备 | 已稳定 |
| `Substation` / `VoltageLevel` / `Bay` | 容器对象，用于层级组织，不是一次设备 | 已稳定 |
| `BaseVoltage` / `OperationalLimit*` | 参数/约束对象，不是一次设备 | 已稳定 |

### 2.3 已补齐、但仍需持续回归的标准设备

下面这些对象不要再按“缺失图元”排期，但需要保留回归：

| CIM 类 | 当前图元承接 | 回归重点 |
| --- | --- | --- |
| `PowerTransformer` | `2W / 3W / 自耦变 / 接地变 / 厂站变` 已分流 | 2W/3W 及细分类端子顺序、三绕组布局、朝向 |
| `SolarGeneratingUnit / WindGeneratingUnit / BatteryUnit` | 已分流到 `PvInfeed / WindInfeed / Ess` | 真实样例命名、朝向、端口语义与更多中型样例回归 |
| `SynchronousMachine / ExternalNetworkInjection / EquivalentInjection` | 已分流到 `Generator / Grid` | 真实样例命名、朝向、端口语义与更多中型样例回归 |
| `ShuntCompensator` 族 | 已分流到 `Capacitor / Reactor / SplitReactor / SvgComp / ArcCoil` | 真实样例命名回退、布局、场景回归 |
| `SeriesCompensator` | 已分流到 `Capacitor / Reactor` | 真实样例命名回退、默认朝向与更多公开样例回归 |
| `ACLineSegment` | `LzPowerCableItem` | 摘要和统计里区分“标准导体对象”与“普通辅助连线” |
| `CurrentTransformer` | `LzPowerCurrentTransformerItem` | 中大型样例持续回归 |
| `PotentialTransformer / VoltageTransformer` | `LzPowerVoltageTransformerItem` | 中大型样例持续回归 |
| `Fuse` | `LzPowerFuseItem` | 中大型样例持续回归 |
| `Junction / Connector` | `LzPowerJunctionItem` | 多端连接、浏览器状态回归 |

当前进展补充（2026-06-22）：

1. 已补 `PowerTransformer` 细分 `importer` 回归：
   `tests/test_cim_importer.cpp`
   `graphic_mapper_refines_power_transformer_specializations_from_name_tokens()`
   已同时验证 `name` 与属性 token 可触发 `自耦变 / 厂站变` 细分
2. 已补 `PowerTransformer` 细分 `graphics` 回归：
   `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_transformer_specialization_graphics()`
   已验证 `description / aliasName` 可在真实导入链路中驱动 `自耦变 / 厂站变` 正确落图
3. 已补 `PowerTransformer` 细分 `UI` 回归：
   `tests/test_cim_ui.cpp`
   `browser_marks_power_transformer_specializations_as_visualized()`
   已验证属性 token 驱动的 `自耦变 / 厂站变` 在浏览器中显示为 `已可视化`
4. 已新增 `CIGREMV_reference` 中型样例回归：
   `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_reference_source_and_load_graphics()`
5. 已验证 `ExternalNetworkInjection(HV-Netz)` 稳定落成 `LzPowerGridItem`
6. 已验证 `EnergyConsumer(Load7-I)` 稳定落成 `LzPowerLoadItem`
7. 已新增浏览器状态回归：
   `tests/test_cim_ui.cpp`
   `browser_marks_reference_source_and_load_objects_as_visualized()`
8. 已新增增强最小样例回归：
   `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_source_and_equivalent_grid_graphics()`
9. 已验证 `SynchronousMachine(generator-001)` 稳定落成 `LzPowerGeneratorItem`
10. 已验证 `ExternalNetworkInjection(external-001)` 与 `EquivalentInjection(equivalent-001)` 稳定落成 `LzPowerGridItem`
11. 已新增浏览器状态回归：
   `tests/test_cim_ui.cpp`
   `browser_marks_source_and_equivalent_grid_objects_as_visualized()`
12. 已新增 `Pv / Wind / Ess` 浏览器状态回归：
   `tests/test_cim_ui.cpp`
   `browser_marks_projection_infeed_objects_as_visualized()`
13. 已新增 `Pv / Wind / Ess` 带 `DiagramObject / DiagramObjectPoint` 的布局回归：
    `tests/test_cim_graphics.cpp`
    `importer_and_scene_builder_preserve_projection_infeed_layout_from_diagram_objects()`
14. 已扩充 `ShuntCompensator` 家族浏览器状态回归，覆盖 `Capacitor / Reactor / SplitReactor / NonlinearReactor / SvgComp / ArcCoil`
    `tests/test_cim_ui.cpp`
    `browser_marks_shunt_compensator_family_sample_as_visualized()`
15. 已新增 `ShuntCompensator` 家族带 `DiagramObject / DiagramObjectPoint` 的布局回归：
    `tests/test_cim_graphics.cpp`
    `importer_and_scene_builder_preserve_shunt_compensator_family_layout_from_diagram_objects()`
16. 已新增 `SynchronousMachine / ExternalNetworkInjection / EquivalentInjection` 的增强最小样例 `DL` 布局回归：
    `tests/test_cim_graphics.cpp`
    `importer_and_scene_builder_preserve_source_and_equivalent_grid_layout_from_diagram_objects()`
17. 已新增 `SeriesCompensator` 家族带 `DiagramObject / DiagramObjectPoint` 的布局回归：
    `tests/test_cim_graphics.cpp`
    `importer_and_scene_builder_preserve_series_compensator_family_layout_from_diagram_objects()`
18. 已把 `CIGRE_MV` 公开中型样例并入 `importer_and_scene_builder_materialize_reference_source_and_load_graphics()`：
    已验证样例中全部 `EnergyConsumer(*)` 稳定落成 `LzPowerLoadItem`
19. 已验证 `CIGRE_MV` 中 `ExternalNetworkInjection(HV-Netz)` 稳定落成 `LzPowerGridItem`
20. 已把 `HV-Netz` 真实样例旋转断言收口为等价角度判断，避免 `-90 / 270` 表达差异导致假失败

## 3. 下一轮执行 ToDo

这一轮的总原则已经从“找新缺图元”切到“把已补齐图元做稳、把浏览器解释做准”。
除非后续新增样例再次扫出 `uncovered > 0` 的标准设备类，否则不再单独为“补标准图元”立新项。

## 3.1 P1-A `ShuntCompensator` 族真实样例回归与布局细化

### 目标

把当前已经打通的参数/子类分流规则，推进到真实样例稳定可用。

### 开发任务

- [ ] 收集至少 1 组中型或真实样例，覆盖 `ShuntCompensator / LinearShuntCompensator / StaticVarCompensator / PetersenCoil`
- [ ] 当前公开中型样例暂无该类对象，需要单独补样例或引入新的公开数据源
- √ 补充 `NonlinearShuntCompensator` 样例，确认当前家族样例稳定落到 `Reactor` 分支
- [ ] 校准命名关键词回退策略，避免真实样例里的中英文别名误分流
- [ ] 细化 `Capacitor / Reactor / SplitReactor / SvgComp / ArcCoil` 的默认尺寸、默认朝向和默认排布
- [ ] 检查图元显示名、参考代号和属性回填是否与导入对象一致

### 测试任务

- [ ] 为真实样例新增 `CimGraphicMapper` 映射断言
- [ ] 为真实样例新增 `CimSceneBuilder` 实例类型断言
- √ 为最小家族样例补浏览器状态断言，确认这些对象不再落入“未映射”
- [ ] 为至少 1 组中型或真实样例补浏览器状态断言，确认这些对象不再落入“未映射”

### 验收标准

1. 真实样例中的补偿设备不会再全部塌缩到同一类图元。
2. `ShuntCompensator` 族对象在浏览器中应显示为已可视化，而不是“应显示未实例化”。
3. 同一批样例在重复导入时，图元类型和数量稳定。

当前验收结果（2026-06-22）：

- 标准 2 已在最小家族样例上通过：`browser_marks_shunt_compensator_family_sample_as_visualized()` 已覆盖 `Capacitor / Reactor / SplitReactor / NonlinearReactor / SvgComp / ArcCoil`。
- 标准 3 已在最小家族样例上通过：`importer_and_scene_builder_materialize_shunt_compensator_family_graphics()` 与 `importer_and_scene_builder_preserve_shunt_compensator_family_layout_from_diagram_objects()` 已固定类型、顺序与旋转。
- 标准 1 仍需至少 1 组中型或真实样例继续补证。

## 3.2 P1-B 三绕组变压器布局细化

### 目标

把当前 `2W / 3W` 基础分流，推进到更稳定的三绕组图面表达。

当前进展（2026-06-22）：

- 已新增 `tests/data/cim/minimal/Transformer3w_layout_sample/`，补齐 `EQ / SSH / DL` 三文件样例。
- 已新增导入回归 `importer_loads_transformer_3w_layout_sample_with_diagram_objects()`。
- 已新增图形回归 `importer_and_scene_builder_preserve_transformer_3w_layout_when_diagram_data_exists()`。
- 已修复 `src/cim/ui/CimSceneBuilder.cpp` 中布局点边界计算偏差，带布局样例的三绕组定位与旋转已稳定通过回归。

### 开发任务

- √ 明确 `PowerTransformerEnd` 的端子顺序使用规则
- [ ] 细化三绕组变压器默认朝向与引线分布
- √ 评估是否需要按端子侧别调整连接点选择策略
- √ 检查 `DiagramObject / DiagramObjectPoint` 导入时对三绕组布局的保留效果

### 测试任务

- √ 增加包含 `DL` 或更完整端子信息的三绕组样例
- √ 补充三绕组布局后的图元位置、旋转或连接关系断言

### 验收标准

1. 三绕组变压器不会再退化成双绕组图元。
2. 多次导入同一带布局样例时，三绕组图面方向稳定。
3. 三绕组与端子连接关系在场景中可解释。

当前验收结果（2026-06-22）：

- 标准 1 已通过：`tests/test_cim_graphics.cpp` 已断言三绕组图元类型仍为 `LzPowerTransformer3wItem`。
- 标准 2 已通过：同一样例重复导入时，当前回归稳定断言 `center=(272, 80)`、`rotation=270`。
- 标准 3 已进一步推进：`CimSceneBuilder` 已消费 `Terminal.sequenceNumber / ACDCTerminal.sequenceNumber / TransformerEnd.endNumber`，并把顺序端口选择扩展到 `2W / 3W / 自耦变 / 接地变 / 厂站变`。
- 图形回归也已补到同一口径：`tests/test_cim_graphics.cpp` 现已断言普通 `2W` 与三类细分 `2W` 变压器都具备双端口、左右方向以及双侧连线实际占用。
- 图形回归已进一步加强：当图形布局故意制造“几何最近点会误导选口”的场景时，`CimSceneBuilder` 仍优先按 `TransformerEnd.endNumber / Terminal.sequenceNumber` 选择变压器端口。
- 该优先级回归已覆盖到 `3W`：`tests/test_cim_graphics.cpp` 现已对三绕组构造“右侧/左侧/上方”几何冲突场景，并分别断言 `LEFT / RIGHT / BOTTOM` 三个端口仍按端子顺序命中。
- 端子关联兼容路径也已纳入图形回归：当前样例同时覆盖 `TransformerEnd.Terminal` 与 `Terminal.TransformerEnd` 两种标准关联写法，避免顺序选口只依赖单边关系。

## 3.3 P1-C `ACLineSegment` 统计口径细化

### 目标

把“标准导体对象”和“为连通关系自动生成的辅助线”在摘要、浏览器和测试口径上拆开。

当前进展（2026-06-22）：

- 已在浏览器摘要中新增 `ACLineSegment` 与自动辅助连线的分项统计。
- 已把 `CimSceneBuilder` 生成的自动辅助连线数量沿 `FrameCimWorkbenchController -> CimBrowserBridge -> CimModelBrowserDock` 传递到 UI 摘要。
- 已新增 UI 回归 `browser_summary_explains_aclinesegment_vs_helper_lines()`，固定“画布连线数不等同于 ACLineSegment 对象数”的说明口径。

### 开发任务

- [ ] 梳理当前场景中哪些线是真实 `ACLineSegment` 图元，哪些是辅助连接线
- √ 在导入摘要或测试统计中增加对应区分口径
- √ 评估是否需要在浏览器说明或摘要中增加“导体对象已图元化、辅助线仅为连通表达”的提示

### 测试任务

- √ 增加包含 `ACLineSegment` 的样例统计断言
- [ ] 补充“对象数 != 画布线数”但口径可解释的回归用例

### 验收标准

1. `ACLineSegment` 不再和普通自动连线混算。
2. 浏览器对象数、设备图元数、辅助线数量三者差异可解释。

当前验收结果（2026-06-22）：

- 标准 1 已通过：摘要已单列 `标准导体对象数（ACLineSegment）` 与 `自动辅助连线数`。
- 标准 2 已通过：UI 回归已验证摘要说明明确指出“画布中的连线数量会包含自动辅助连线，因此不等同于 ACLineSegment 对象数”。

## 3.4 P1-D 浏览器状态分类回归扩充

### 目标

让模型浏览器在更多真实样例下稳定解释“为什么还有对象没画出来”。

当前进展（2026-06-22）：

- 已按 `tests/data/cim/medium/CIGREMV_reference/` 样例扩充浏览器状态分类口径。
- 已把 `CIGRE_MV` 公开样例中的 `Name / NameType` 收口到“元数据/容器对象”，避免继续误判成“未映射对象”。
- 已把 `SvPowerFlow`、`SvVoltage` 归入“语义对象”，把 `ConnectivityNodeContainer`、`TransformerEnd` 归入“抽象层对象”。
- 已新增 UI 回归 `browser_classifies_reference_sample_standard_non_device_objects()`，验证参考样例中的标准非设备对象不再误落入“未映射对象”。
- 已把 `CIGRE_MV` 中型样例的覆盖率摘要断言并入 `browser_classifies_cigre_public_name_objects_as_metadata()`，固定 `Name / NameType` 收口后摘要不再出现“其他未映射对象数”。

### 开发任务

- √ 按真实样例补充语义对象、布局对象、容器对象、未映射对象白名单
- √ 检查 `CIGREMV_reference` 样例中标准非设备对象是否误落到“未映射对象”
- [ ] 评估是否需要把浏览器统计项进一步拆成“设备类未映射”和“非设备类对象”

### 测试任务

- √ 为至少 1 组中型样例补浏览器状态分类回归
- √ 为至少 1 组中型样例补覆盖率摘要断言

### 验收标准

1. 用户在浏览器里看到“还有对象没显示”时，系统能解释原因。
2. 标准一次设备不应长期停留在“未映射对象”分类里。

当前验收结果（2026-06-22）：

- 标准 1 已进一步补证：`CIGREMV_reference` 样例下 `SvVoltage`、`SvPowerFlow`、`PowerTransformerEnd`，以及 `CIGRE_MV` 样例下 `Name / NameType` 已能在浏览器中按正确口径解释。
- 标准 2 已进一步补证：`CIGREMV_reference` 与 `CIGRE_MV` 中型样例摘要回归均已固定不再出现 `其他未映射对象数`。

## 4. 暂不再排期的项目

下面这些项已经完成基础补齐，当前不应再作为“图元缺失”继续反复立项：

- √ `ACLineSegment`
- √ `LoadBreakSwitch`
- √ `GroundDisconnector`
- √ `LinearShuntCompensator`
- √ `AsynchronousMachine`
- √ `ExternalNetworkInjection`
- √ `EquivalentInjection`
- √ `SurgeArrester`
- √ `CurrentTransformer`
- √ `PotentialTransformer / VoltageTransformer`
- √ `Fuse`
- √ `Junction / Connector`

## 5. 排期建议

如果下一轮只做 3 条主线，建议顺序如下：

1. `ShuntCompensator` 族真实样例回归与布局细化
2. 三绕组变压器布局细化与全家族端子顺序细节继续打磨
3. 浏览器状态分类与 `ACLineSegment` 统计口径细化

这样做的好处是：

1. 先把已经补齐的标准图元做稳，而不是继续虚报缺口。
2. 让浏览器和导入摘要能更准确解释“还剩什么问题”。
3. 后续如果再扩 CIM 范围，团队能更容易区分“真缺图元”和“非图元语义对象”。
## 6. Rotation Semantic Follow-up (2026-06-23)

- Added transformer rotation regression coverage in `tests/test_cim_graphics.cpp`.
- Coverage now checks both:
  `TransformerEnd.endNumber` / `Terminal.sequenceNumber` 仍驱动端口选择；
  `LzConnectLine::pathInSceneCoords()` 的首段出线仍沿旋转后图元的视觉外法向离开端口。
- Covered transformer cases:
  `AutoXfmr` / plain 2W transformer / `StationXfmr` / `EarthingXfmr` / `Transformer3w`
- Root cause fixed in `src/item/LzConnectLine.cpp`:
  共线点压缩不再删除“同一直线但发生反向折返”的中间点，避免旋转后 helper line 从端口首段朝反方向出线。
- Verification status updated:
  transformer-focused subset is green；
  current workspace `lz_cim_graphics_tests` full suite is green。

