# CIM 标准图元补齐 ToDo 与测试验收方案

最后整理日期：2026-06-29

## 1. 文档目标

本文件基于《CIM标准图元缺口与补齐设计.md》，按当前仓库真实代码状态重新整理图元补齐待办。

如果需要直接拉研发排期，建议同时查看《CIM标准图元补齐执行ToDoList.md》，那份文档会把“真实缺口 / 非图元对象 / 已补齐待细化项”拆得更细；原先更细的“本周任务清单 / 具体开发任务表”已经在整理中删除。

这份清单只回答三件事：

1. 现在到底还有哪些标准 CIM 图元是真的缺。
2. 哪些旧设计里写成“缺失”的项，其实已经补上了。
3. 下一轮研发和测试应该按什么顺序推进。

本文件的口径以当前仓库实现为准，重点参考：

1. `src/cim/mapping/CimGraphicMapper.cpp`
2. `src/cim/ui/CimSceneBuilder.cpp`
3. `src/cim/importer/CgmesPackageImporter.cpp`
4. `tests/test_cim_importer.cpp`
5. `tests/test_power_topology.cpp`

## 2. 当前基线

### 2.1 已经具备稳定映射和场景实例化的 CIM 类

下面这些类不应再继续放在“缺失图元”清单里：

| CIM 类 | 当前图元承接 | 当前状态 |
| --- | --- | --- |
| `BusbarSection` | `LzPowerBusbarSectionItem` | 已完成 |
| `Breaker` | `LzPowerCircuitBreakerItem` | 已完成 |
| `Disconnector` | `LzPowerDisconnectorItem` | 已完成 |
| `PowerTransformer` | `LzPowerTransformer2wItem / LzPowerTransformer3wItem / LzPowerAutoXfmrItem / LzPowerEarthingXfmrItem / LzPowerStationXfmrItem` | 已完成当前阶段细分映射 |
| `EnergyConsumer` | `LzPowerLoadItem` | 已完成 |
| `SynchronousMachine` | `LzPowerGeneratorItem` | 已完成 |
| `ACLineSegment` | `LzPowerCableItem` | 已完成 |
| `LoadBreakSwitch` | `LzPowerLoadBreakItem` | 已完成 |
| `GroundDisconnector` | `LzPowerEarthSwitchItem` | 已完成 |
| `LinearShuntCompensator` | `LzPowerCapacitorItem` | 已完成 |
| `ShuntCompensator` | `LzPowerCapacitorItem / LzPowerReactorItem / LzPowerSplitReactorItem / LzPowerSvgCompItem / LzPowerArcCoilItem` | 已完成基础分流 |
| `SeriesCompensator` | `LzPowerCapacitorItem / LzPowerReactorItem` | 已完成细分映射 |
| `AsynchronousMachine` | `LzPowerMotorItem` | 已完成 |
| `ExternalNetworkInjection` | `LzPowerGridItem` | 已完成 |
| `EquivalentInjection` | `LzPowerGridItem` | 已完成 |
| `SurgeArrester` | `LzPowerArresterItem` | 已完成 |
| `Ground` | `LzPowerGroundItem` | 已完成 |
| `CurrentTransformer` | `LzPowerCurrentTransformerItem` | 已完成 |
| `PotentialTransformer` / `VoltageTransformer` | `LzPowerVoltageTransformerItem` | 已完成 |
| `Fuse` | `LzPowerFuseItem` | 已完成 |
| `Junction` / `Connector` | `LzPowerJunctionItem` | 已完成 |

### 2.2 仍需保留的语义约束

下面这些规则当前已经成立，后续补图元时不能回退：

1. `Terminal` 不作为一次设备图元实例化。
2. `ConnectivityNode` 不作为一次设备图元实例化。
3. `TopologicalNode` 不作为一次设备图元实例化。
4. `AsynchronousMachine` 图形上是电动机，但不应被当成电源侧设备。
5. `ACLineSegment` 已有独立导体图元承接，不应再退化为普通矩形设备。

## 3. 按标准建模重算后的真实缺口

### 3.1 当前仍缺失的标准图元

截至 2026-06-22，当前仓库里此前明确列入 ToDo 的标准缺失图元，已经完成补齐。当前不再存在“已确认缺失但仍未落地”的标准图元项。

| CIM 类 | 当前可复用图元 | 当前缺口 | 优先级 |
| --- | --- | --- | --- |
| 无 | - | 当前缺口已清零，后续工作转入覆盖率提示与映射细化 | - |

### 3.2 本轮已补齐、但需要持续回归的标准图元

下面这些项此前确实是缺口，但本轮已经完成图元落地，不应再继续放在“仍缺失”清单里：

| CIM 类 | 当前图元承接 | 当前状态 |
| --- | --- | --- |
| `CurrentTransformer` | `LzPowerCurrentTransformerItem` | 已补齐，需持续回归 |
| `PotentialTransformer` / `VoltageTransformer` | `LzPowerVoltageTransformerItem` | 已补齐，需持续回归 |
| `Fuse` | `LzPowerFuseItem` | 已补齐，需持续回归 |
| `Junction` / `Connector` | `LzPowerJunctionItem` | 已补齐，`Connector` 按同策略映射 |

### 3.3 不属于“图元完全缺失”，但仍然需要补强的项

这些项已有基础承接，但标准建模仍未完全落稳，不应和“完全没图元”混在一起。

| 主题 | 当前状态 | 需要补的点 | 优先级 |
| --- | --- | --- | --- |
| `PowerTransformer` | 已按 `自耦变 / 接地变 / 厂站变 / 2W / 3W` 细分落图 | 后续可继续细化端子顺序、朝向与三绕组布局 | 已完成当前阶段细分 |
| `ShuntCompensator` 族 | 已按参数与子类细分到 `Capacitor / Reactor / SplitReactor / SvgComp / ArcCoil` | 后续继续细化真实样例回归、默认布局与命名回退策略 | 已完成基础分流 |
| `SeriesCompensator` | 已按命名与 `x` 电抗符号细分到 `Capacitor / Reactor` | 后续继续细化真实样例回归、默认朝向与布局规则 | 已完成基础分流 |
| `ACLineSegment` | 已映射 `Cable` 图元 | 需继续区分“标准导体对象”与“普通连接辅助线”在摘要和统计上的口径 | P1 |
| 导入摘要 | 已显示对象总数、Profile、日志/告警/错误、可视化覆盖率 | 后续只需按更多样例继续回归 | 已完成 |
| 对象浏览器 | 已可区分已可视化、应显示未实例化、语义/布局/容器/抽象/未映射 | 后续只需随新类扩充分类白名单 | 已完成 |

## 4. 具体 TodoList

## 4.1 基线校准

- √ 盘点当前 `CimGraphicMapper` 已接通的标准图元映射
- √ 盘点当前 `CimSceneBuilder` 已能实例化的标准图元
- √ 确认 `ACLineSegment / LoadBreakSwitch / GroundDisconnector / LinearShuntCompensator` 已不再属于缺口
- √ 确认 `AsynchronousMachine / ExternalNetworkInjection / EquivalentInjection / SurgeArrester / Ground` 已不再属于缺口
- √ 把其他设计文档中的旧 P0/P1 缺口表同步到当前真实状态

## 4.2 已完成的 P0 新增图元任务

### 4.2.1 `CurrentTransformer`

- √ 定义 CT 的图元类型枚举、XML 形状名和对象工厂注册
- √ 新增 `LzPowerCurrentTransformerItem`
- √ 新增 `LzPowerCurrentTransformerTool`
- √ 在 `CimGraphicMapper` 中接通 `CurrentTransformer`
- √ 在 `CimSceneBuilder` 中补默认尺寸、分组和连接点规则
- √ 增加最小样例和映射/实例化测试

### 4.2.2 `PotentialTransformer / VoltageTransformer`

- √ 定义 PT/VT 的图元类型枚举、XML 形状名和对象工厂注册
- √ 新增 `LzPowerVoltageTransformerItem`
- √ 新增 `LzPowerVoltageTransformerTool`
- √ 在 `CimGraphicMapper` 中接通 `PotentialTransformer / VoltageTransformer`
- √ 在 `CimSceneBuilder` 中补默认尺寸、分组和连接点规则
- √ 增加最小样例和映射/实例化测试

### 4.2.3 `Fuse`

- √ 定义熔断器图元类型枚举、XML 形状名和对象工厂注册
- √ 新增 `LzPowerFuseItem`
- √ 新增 `LzPowerFuseTool`
- √ 在 `CimGraphicMapper` 中接通 `Fuse`
- √ 在 `CimSceneBuilder` 中补默认尺寸、分组和连接点规则
- √ 增加最小样例和映射/实例化测试

## 4.3 P0 可视化覆盖率任务

- √ 在导入摘要中增加“已导入未可视化对象”统计
- √ 在导入摘要中增加“已可视化对象数 / 未可视化对象数 / 覆盖率”说明
- √ 在对象浏览器中增加状态列，区分已可视化、应显示未实例化、语义对象、布局对象、元数据容器对象、抽象层对象、未映射对象
- √ 为上述统计补充 UI 测试与覆盖率断言
- [ ] 随更多真实样例继续补充分类白名单与回归用例

## 4.4 P1 映射细化任务

### 4.4.1 `Junction / Connector`

- √ 明确标准口径下这类对象需要独立连接结点图元
- √ 定义 `Connector` 复用 `Junction` 图元策略
- √ 补齐图元类、Tool、映射和测试

### 4.4.2 `PowerTransformer`

- √ 依据绕组数或端子结构区分 `2W` 和 `3W`
- √ 依据命名与属性 token 细分 `自耦变 / 接地变 / 厂站变`
- √ 在 `CimGraphicMapper` 或场景构建阶段增加分流规则
- √ 为 `3W` 变压器增加最小样例和实例化断言
- √ 为 `自耦变 / 接地变 / 厂站变` 增加实例化、浏览器状态与属性 token 回归
- [ ] 继续细化三绕组变压器的端子顺序、默认朝向和布局表现

### 4.4.3 `ShuntCompensator` 族

- √ 明确 `LinearShuntCompensator` 与一般 `ShuntCompensator` 的参数分流规则
- √ 评估并落地到 `Capacitor / Reactor / SplitReactor / SvgComp / ArcCoil`
- √ 为参数分流规则补最小样例和断言
- [ ] 继续补更多真实样例下的命名回退、默认布局与场景回归

## 4.5 测试任务

- √ 新增 `CurrentTransformer` 最小样例
- √ 新增 `PotentialTransformer / VoltageTransformer` 最小样例
- √ 新增 `Fuse` 最小样例
- √ 新增 `Junction / Connector` 策略样例
- √ 补 `CimGraphicMapper` 新增映射断言
- √ 补 `CimSceneBuilder` 新增实例类型断言
- √ 补“`Terminal / ConnectivityNode / TopologicalNode` 不被实例化”回归断言
- √ 补“未可视化对象统计正确”断言
- √ 增加独立 `lz_cim_ui_tests` 入口，支持单独回归浏览器状态与覆盖率 UI 用例

## 5. 建议研发顺序

### 第一步

可视化覆盖率与提示能力已完成，下一步顺序调整为：

1. 更多中型/大型样例下的浏览器状态回归
2. 三绕组变压器的端子顺序与布局细化
3. `ShuntCompensator` 族在真实样例下的布局与命名回退回归

### 第二步

继续做已有映射细化：

1. `ShuntCompensator` 族参数分流后的真实样例补充
2. 三绕组变压器布局细化后的样例补充

### 第三步

最后补齐可视化覆盖率相关测试：

1. 大样例下的覆盖率断言
2. 新 CIM 类引入后的状态分类断言
3. 浏览器状态筛选与定位联动回归

## 6. 验收标准

### 6.1 图元补齐验收

1. 新增标准类能在 `CimGraphicMapper` 中得到稳定图元类型。
2. 导入样例后，场景中能实例化正确设备类型。
3. `Terminal / ConnectivityNode / TopologicalNode` 仍不会被误画成一次设备。

### 6.2 可视化覆盖率验收

1. 导入摘要能显示已可视化对象数、未可视化对象数和覆盖率。
2. 对象浏览器能区分“对象已导入”和“对象已可视化”。
3. 遇到标准对象暂未支持显示时，系统给出清晰提示而不是静默忽略。

### 6.3 回归验收

下面这些已完成映射必须持续通过回归：

1. `BusbarSection`
2. `Breaker`
3. `Disconnector`
4. `PowerTransformer`
5. `EnergyConsumer`
6. `SynchronousMachine`
7. `ACLineSegment`
8. `LoadBreakSwitch`
9. `GroundDisconnector`
10. `LinearShuntCompensator`
11. `ShuntCompensator`
12. `SeriesCompensator`
13. `AsynchronousMachine`
14. `ExternalNetworkInjection`
15. `EquivalentInjection`
16. `SurgeArrester`
17. `Ground`
18. `CurrentTransformer`
19. `PotentialTransformer` / `VoltageTransformer`
20. `Fuse`
21. `Junction` / `Connector`

## 7. 结论

按当前仓库真实状态看，旧设计文档里最早列出的那批 P0/P1 缺口已经完成了大半。此前明确缺失的 `CurrentTransformer`、`PotentialTransformer / VoltageTransformer`、`Fuse` 已经补齐并有样例与自动化测试兜底；`PowerTransformer` 也已从基础 `2W/3W` 分流推进到 `自耦变 / 接地变 / 厂站变 / 2W / 3W` 当前阶段细分落地。

当前缺失图元项已经补齐，后续待办主要收敛为：

1. `ShuntCompensator` 族真实样例回归与布局细化
2. 浏览器状态分类在更多真实样例上的回归扩充
3. 三绕组变压器布局细化

因此下一轮 ToDo 不应再继续围绕 `ACLineSegment`、`LoadBreakSwitch`、`GroundDisconnector` 或 `Junction` 反复排期，而应把研发重点切到“`ShuntCompensator` 族真实样例回归与布局细化 + 浏览器状态分类回归扩充 + 三绕组变压器布局细化”这几条主线。

