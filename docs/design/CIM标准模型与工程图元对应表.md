# CIM 标准模型与工程图元对应表

最后整理日期：2026-06-29

## 1. 目的

这份文档只回答一件事：当前 `lz-power` 工程里，标准 `CIM/CGMES` 对象进入系统后，分别落到什么图元、什么处理口径，哪些已经接上了导入主链，哪些只是工程里“有现成图元但还没纳入当前 CIM 映射”。

它不是完整的 CIM 元模型，也不是官方 UML 的全量继承树。
文中出现的继承关系，只是当前项目导入、兼容类合并、浏览器分类会实际用到的“继承摘录”。

它和《CIM对象模型与代码映射设计.md》的区别是：

1. 那份文档偏设计目标。
2. 这份文档偏当前代码真实状态。

标准参考入口：

1. UCAIug CIM 模型概览：
   https://cim-mg.ucaiug.io/section4-cim-overview/?utm_source=chatgpt.com

当前代码里已经收口到共享 lineage helper 的关键链路示例：

1. `ConductingEquipment -> Equipment -> PowerSystemResource -> IdentifiedObject`
2. `Terminal -> ACDCTerminal -> IdentifiedObject`
3. `VoltageLevel / Bay / Substation -> EquipmentContainer -> ConnectivityNodeContainer -> PowerSystemResource -> IdentifiedObject`

当前继承摘录来源说明：

1. 运行时由 `src/cim/model/CimClassLineageGenerated.inc` 提供
2. 对应快照元数据在 `src/cim/model/cim_class_metadata_cgmes_v2_4_15.json`
3. 当前默认来源是开源 `CIMpy + GridCal` 合并快照：`CIMpy` 提供主链与 Profile 元数据，`GridCal` 用于补齐缺失的类继承
4. 后续建议统一通过 `tools/sync_cim_metadata.py --source auto` 刷新
5. `src/cim/model/CimClassLineage.h` 不再注入本地兼容父类，运行时只消费生成结果

## 2. 当前总口径

按当前代码，CIM 对象分三层处理：

1. `直接图元化对象`
   `CimGraphicMapper` 能直接给出 `drawType`，`CimSceneBuilder` 会在场景中实例化图元。
2. `语义/布局/容器对象`
   会导入到 `CimModel` 并出现在浏览器里，但不会直接生成一次设备图元。
3. `工程里已有图元但未接入当前 CIM 导入映射`
   图元类已经存在，但 `CimGraphicMapper` 还没有把相应 CIM 类接进当前主链。

## 3. 已接入当前 CIM 导入主链的对象

下面这些类当前已经能从 `CimGraphicMapper -> CimSceneBuilder` 主链落图。

| CIM 类 | 当前映射 drawType | 当前图元类 | 浏览器状态口径 | 说明 |
| --- | --- | --- | --- | --- |
| `Breaker` | `LZ_SLD_CIRCUIT_BREAKER` | `LzPowerCircuitBreakerItem` | 已可视化 / 应显示未实例化 | 直接映射 |
| `Disconnector` | `LZ_SLD_DISCONNECTOR` | `LzPowerDisconnectorItem` | 已可视化 / 应显示未实例化 | 直接映射 |
| `LoadBreakSwitch` | `LZ_WSYM_LOAD_BREAK` | `LzPowerLoadBreakItem` | 已可视化 / 应显示未实例化 | 直接映射 |
| `GroundDisconnector` | `LZ_WSYM_EARTH_SWITCH` | `LzPowerEarthSwitchItem` | 已可视化 / 应显示未实例化 | 接地刀闸 |
| `BusbarSection` | `LZ_SLD_BUSBAR_SECTION` | `LzPowerBusbarSectionItem` | 已可视化 / 应显示未实例化 | 母线，支持沿边接入 |
| `PowerTransformer` 2绕组 | `LZ_SLD_TRANSFORMER_2W` | `LzPowerTransformer2wItem` | 已可视化 / 应显示未实例化 | 端数 < 3 |
| `PowerTransformer` 3绕组 | `LZ_WSYM_TRANSFORMER_3W` | `LzPowerTransformer3wItem` | 已可视化 / 应显示未实例化 | 端数 >= 3 |
| `PowerTransformer` 自耦变细分 | `LZ_WSYM_AUTO_XFMR` | `LzPowerAutoXfmrItem` | 已可视化 / 应显示未实例化 | 依据命名与属性 token 细分 |
| `PowerTransformer` 接地变细分 | `LZ_WSYM_EARTHING_XFMR` | `LzPowerEarthingXfmrItem` | 已可视化 / 应显示未实例化 | 依据命名与属性 token 细分 |
| `PowerTransformer` 厂站变细分 | `LZ_WSYM_STATION_XFMR` | `LzPowerStationXfmrItem` | 已可视化 / 应显示未实例化 | 依据命名与属性 token 细分；`2W / Auto / Station / Earthing / 3W` 旋转出线语义已补回归覆盖 |
| `EnergyConsumer` | `LZ_SLD_LOAD` | `LzPowerLoadItem` | 已可视化 / 应显示未实例化 | 负荷，已补 `CIGREMV_reference` 与 `CIGRE_MV` 中型样例落图回归 |
| `SynchronousMachine` | `LZ_SLD_GENERATOR` | `LzPowerGeneratorItem` | 已可视化 / 应显示未实例化 | 发电机，已补增强最小样例 `DL` 布局回归 |
| `AsynchronousMachine` | `LZ_SLD_MOTOR` | `LzPowerMotorItem` | 已可视化 / 应显示未实例化 | 电动机 |
| `SolarGeneratingUnit` | `LZ_WSYM_PV_INFEED` | `LzPowerPvInfeedItem` | 已可视化 / 应显示未实例化 | 光伏注入图元已接入主链，已补 UI 与 `DL` 布局回归 |
| `WindGeneratingUnit` | `LZ_WSYM_WIND_INFEED` | `LzPowerWindInfeedItem` | 已可视化 / 应显示未实例化 | 风电注入图元已接入主链，已补 UI 与 `DL` 布局回归 |
| `BatteryUnit` | `LZ_WSYM_ESS` | `LzPowerEssItem` | 已可视化 / 应显示未实例化 | 储能图元已接入主链，已补 UI 与 `DL` 布局回归 |
| `ACLineSegment` | `LZ_WSYM_CABLE` | `LzPowerCableItem` | 已可视化 / 应显示未实例化 | 标准导体对象，和辅助连线分开统计 |
| `LinearShuntCompensator` | 参数分流 | `LzPowerCapacitorItem` / `LzPowerReactorItem` / `LzPowerSplitReactorItem` / `LzPowerSvgCompItem` / `LzPowerArcCoilItem` | 已可视化 / 应显示未实例化 | 按属性和命名推断，已补家族样例 UI 与 `DL` 布局回归 |
| `ShuntCompensator` | 参数分流 | 同上 | 已可视化 / 应显示未实例化 | 基类按参数分流，已补家族样例 UI 与 `DL` 布局回归 |
| `NonlinearShuntCompensator` | 参数分流 | 同上 | 已可视化 / 应显示未实例化 | 当前已接入主链，已补家族样例 UI 回归 |
| `StaticVarCompensator` | `LZ_WSYM_SVG_COMP` | `LzPowerSvgCompItem` | 已可视化 / 应显示未实例化 | 静止无功补偿，已补家族样例 UI 与 `DL` 布局回归 |
| `PetersenCoil` | `LZ_WSYM_ARC_COIL` | `LzPowerArcCoilItem` | 已可视化 / 应显示未实例化 | 消弧线圈，已补家族样例 UI 与 `DL` 布局回归 |
| `SeriesCompensator` 串补电容 | `LZ_WSYM_CAPACITOR` | `LzPowerCapacitorItem` | 已可视化 / 应显示未实例化 | 优先按命名与 `x < 0` 细分，已补最小样例 `DL` 布局回归 |
| `SeriesCompensator` 串联电抗 | `LZ_WSYM_REACTOR` | `LzPowerReactorItem` | 已可视化 / 应显示未实例化 | 优先按命名与 `x > 0` 细分，已补最小样例 `DL` 布局回归 |
| `ExternalNetworkInjection` | `LZ_WSYM_GRID` | `LzPowerGridItem` | 已可视化 / 应显示未实例化 | 外部电源，已补增强最小样例 `DL` 布局回归，并已补 `CIGRE_MV` 公开样例落图回归 |
| `EquivalentInjection` | `LZ_WSYM_GRID` | `LzPowerGridItem` | 已可视化 / 应显示未实例化 | 等值注入，已补增强最小样例 `DL` 布局回归 |
| `SurgeArrester` | `LZ_WSYM_ARRESTER` | `LzPowerArresterItem` | 已可视化 / 应显示未实例化 | 避雷器 |
| `Ground` | `LZ_SLD_GROUND` | `LzPowerGroundItem` | 已可视化 / 应显示未实例化 | 接地设备 |
| `CurrentTransformer` | `LZ_WSYM_CURRENT_TRANSFORMER` | `LzPowerCurrentTransformerItem` | 已可视化 / 应显示未实例化 | CT |
| `PotentialTransformer` | `LZ_WSYM_VOLTAGE_TRANSFORMER` | `LzPowerVoltageTransformerItem` | 已可视化 / 应显示未实例化 | PT |
| `VoltageTransformer` | `LZ_WSYM_VOLTAGE_TRANSFORMER` | `LzPowerVoltageTransformerItem` | 已可视化 / 应显示未实例化 | VT |
| `Fuse` | `LZ_WSYM_FUSE` | `LzPowerFuseItem` | 已可视化 / 应显示未实例化 | 熔断器 |
| `Junction` | `LZ_WSYM_JUNCTION` | `LzPowerJunctionItem` | 已可视化 / 应显示未实例化 | 连接结点 |
| `Connector` | `LZ_WSYM_JUNCTION` | `LzPowerJunctionItem` | 已可视化 / 应显示未实例化 | 当前复用 `Junction` 图元 |

## 4. 已导入但不直接生成一次图元的标准对象

这组对象当前是“模型层保留、浏览器可见、图面不单独画设备”。

| CIM 类 | 当前浏览器状态 | 当前处理口径 | 说明 |
| --- | --- | --- | --- |
| `Terminal` | 语义对象 | 参与设备到结点连接，不单独画设备 | 可作为 `DiagramObject` 中转对象 |
| `ACDCTerminal` | 语义对象 | 语义层保留 | 当前跟 `Terminal` 同口径 |
| `PowerTransformerEnd` | 语义对象 | 用于判断 2W / 3W 和端子关系 | 不单独实例化图元 |
| `SvPowerFlow` | 语义对象 | 状态量对象 | 当前已从“未映射对象”收口到语义对象 |
| `SvVoltage` | 语义对象 | 状态量对象 | 当前已从“未映射对象”收口到语义对象 |
| `ConnectivityNode` | 语义对象 | 连通结点 | 不单独画设备 |
| `TopologicalNode` | 语义对象 | 拓扑结点 | 不单独画设备 |
| `Diagram` | 布局对象 | 图纸元信息 | 不单独画设备 |
| `DiagramObject` | 布局对象 | 记录设备布局锚点/旋转 | 驱动设备落图 |
| `DiagramObjectPoint` | 布局对象 | 记录点坐标和折点 | 驱动设备定位、长度、朝向 |
| `CoordinateSystem` | 布局对象 | 坐标系统元信息 | 当前只保留，不做 CRS 换算 |
| `BaseVoltage` | 元数据/容器对象 | 参数对象 | 不单独画设备 |
| `VoltageLevel` | 元数据/容器对象 | 层级容器 | 不单独画设备 |
| `Bay` | 元数据/容器对象 | 层级容器 | 不单独画设备 |
| `Substation` | 元数据/容器对象 | 层级容器 | 不单独画设备 |
| `SubGeographicalRegion` | 元数据/容器对象 | 区域容器 | 不单独画设备 |
| `GeographicalRegion` | 元数据/容器对象 | 区域容器 | 不单独画设备 |
| `OperationalLimit` | 元数据/容器对象 | 约束对象 | 不单独画设备 |
| `OperationalLimitSet` | 元数据/容器对象 | 约束对象 | 不单独画设备 |
| `OperationalLimitType` | 元数据/容器对象 | 约束对象 | 不单独画设备 |
| `VoltageLimit` | 元数据/容器对象 | 限值对象 | 不单独画设备 |
| `CurrentLimit` | 元数据/容器对象 | 限值对象 | 不单独画设备 |
| `Name` | 元数据/容器对象 | 别名/命名对象 | 不单独画设备 |
| `NameType` | 元数据/容器对象 | 命名类型对象 | 不单独画设备 |
| `IdentifiedObject` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `Equipment` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `PowerSystemResource` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `ConductingEquipment` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `Conductor` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `ConnectivityNodeContainer` | 抽象层对象 | 抽象容器 | 当前已从“未映射对象”收口 |
| `Switch` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `ProtectedSwitch` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `RotatingMachine` | 抽象层对象 | 抽象基类 | 浏览器解释用 |
| `TransformerEnd` | 抽象层对象 | 抽象基类 | 当前已从“未映射对象”收口 |
| `EquipmentContainer` | 抽象层对象 | 抽象容器 | 浏览器解释用 |

## 5. 布局对象怎样影响当前画布

`DiagramObject / DiagramObjectPoint` 虽然不直接画成设备，但会影响最终图面：

1. `DiagramObject.IdentifiedObject` 先定位到对应对象。
2. 如果对应对象是 `Terminal`，当前会进一步转到其 `ConductingEquipment`。
3. `DiagramObjectPoint` 按 `sequenceNumber` 排序后组成几何信息。
4. 单点布局时，用该点作为设备中心。
5. 多点布局时，用首尾点中点作为设备中心。
6. 对 `BusbarSection / ACLineSegment`，还会用首尾点距离推导长度，并在未给 `rotation` 时推导朝向。
7. 所有 `DL` 坐标会先做一次全局缩放和平移，再放进当前画布坐标系。

因此，布局对象当前的角色是“驱动设备怎么画”，不是“自己也画出来”。

## 6. 工程里已有图元但当前未接入 CIM 映射的对象

截至 2026-06-23，这一组已明显收缩。此前位于这里的：

1. `LzPowerAutoXfmrItem`
2. `LzPowerEarthingXfmrItem`
3. `LzPowerStationXfmrItem`
4. `LzPowerPvInfeedItem`
5. `LzPowerWindInfeedItem`
6. `LzPowerEssItem`

都已经接入当前 `CimGraphicMapper -> CimSceneBuilder` 主链。

基于本轮对公开/最小样例的复核，当前还可以进一步收口一个结论：

1. 扫描范围：
   `tests/data/cim/medium/CIGRE_MV`
   `tests/data/cim/medium/CIGREMV_reference`
   `tests/data/cim/minimal`
2. 去重后共得到 `49` 个唯一 CIM 类名。
3. 其中：
   `18` 个已落入 `CimGraphicMapper` 的设备图元映射；
   `7` 个已收口为语义对象；
   `3` 个已收口为布局对象；
   `12` 个已收口为元数据/容器对象；
   `9` 个已收口为抽象层对象。
4. 剩余 `0` 个类落在“既未映射、也未被浏览器分类收口”的空白区。

这意味着：在当前公开样例覆盖范围内，已经没有证据表明还存在新的“标准设备类缺图元”。

当前更值得继续排查的是两类情况：

1. 工程里仍有现成图元，但还没有稳定标准 CIM 口径去承接，例如 `LzPowerFeederItem` 这类更偏工程语义的图元。
2. 标准类已经能落图，但还需要继续做更细分表达、布局规则和浏览器状态回归。

## 7. 当前最重要的几个一对多映射

有几类标准对象当前不是简单 1:1：

### 7.1 `PowerTransformer`

1. 先按命名/属性 token 识别更细分类：
   `Auto Transformer / 自耦` -> `LzPowerAutoXfmrItem`
2. `Earthing Transformer / 接地变` -> `LzPowerEarthingXfmrItem`
3. `Station Service Transformer / 站用变 / 厂用变 / 所用变` -> `LzPowerStationXfmrItem`
4. 上述细分都未命中时，再按 `PowerTransformer.PowerTransformerEnd` 数量分流：
   端数 < 3 -> `LzPowerTransformer2wItem`
5. 端数 >= 3 -> `LzPowerTransformer3wItem`
6. 场景建线阶段优先消费 `TransformerEnd.endNumber`，其次消费 `Terminal.sequenceNumber / ACDCTerminal.sequenceNumber`，从而稳定选择变压器端口。

### 7.2 `ShuntCompensator` 族

1. `StaticVarCompensator` -> `LzPowerSvgCompItem`
2. `PetersenCoil` -> `LzPowerArcCoilItem`
3. 分裂电抗器特征 -> `LzPowerSplitReactorItem`
4. 电抗器特征 -> `LzPowerReactorItem`
5. 其余默认 -> `LzPowerCapacitorItem`

当前分流依据包括：

1. `className`
2. `name`
3. 关键属性值与别名字段，例如 `IdentifiedObject.description / aliasName`
4. 分段数，例如 `maximumSections / normalSections / sections`

### 7.3 `SeriesCompensator`

1. 串补电容命名特征，例如 `Series Capacitor / Series Cap / 串补 / 串联电容`
   -> `LzPowerCapacitorItem`
2. 串联电抗命名特征，例如 `Series Reactor / Current Limiting Reactor / 串联电抗 / 限流电抗`
   -> `LzPowerReactorItem`
3. 命名未命中时，再按 `SeriesCompensator.x` 符号分流：
   `x < 0` -> `LzPowerCapacitorItem`
4. `x > 0` -> `LzPowerReactorItem`
5. 仍无法判断时，默认回退到 `LzPowerReactorItem`

### 7.4 `Connector`

1. 当前不单独新增一个 `Connector` 图元类
2. 直接复用 `LzPowerJunctionItem`

## 8. 当前文档使用建议

如果要回答“这个 CIM 类为什么没有画出来”，建议按下面顺序判断：

1. 先看它是不是第 4 节里的语义/布局/容器对象
2. 再看它是不是第 3 节里已接入主链的对象，但当前样例没有成功实例化
3. 最后再看它是不是第 6 节里“工程有图元但 CIM 还没接上”的对象

这样可以避免把三类问题混成同一个“缺图元”问题。

## 9. 关键代码入口索引

如果后面要继续扩这份表，建议优先看下面几个代码入口：

### 9.1 导入与对象归并

1. `src/cim/importer/CgmesPackageImporter.cpp`
   负责 `EQ / SSH / TP / SV / DL / GL` 文件读取、对象入模、兼容类合并和 `PowerTransformerEnd` 反向关系补全。

### 9.2 标准类到图元类型映射

1. `src/cim/mapping/CimGraphicMapper.cpp`
   当前所有“CIM 类 -> drawType”主映射都在这里。
2. `drawTypeForShuntCompensator(...)`
   `ShuntCompensator` 族的一对多分流入口。

### 9.3 图元实例化与布局落图

1. `src/cim/ui/CimSceneBuilder.cpp`
   负责 `drawType -> 真实图元实例`、默认尺寸、布局坐标应用、端子连线推导。
2. `buildDiagramGeometryByEquipmentMrid(...)`
   负责消费 `DiagramObject / DiagramObjectPoint`。
3. `buildDiagramLayoutTransform(...)`
   负责把原始 `DL` 坐标变换到当前画布坐标系。

### 9.4 浏览器状态解释

1. `src/cim/ui/CimModelBrowserDock.cpp`
   负责把对象解释成“已可视化 / 语义对象 / 布局对象 / 元数据对象 / 抽象对象 / 未映射对象”。

## 10. 当前优先建议继续细化的标准方向

截至 2026-06-23，当前公开样例范围内暂无新的“尚未打通的标准设备类”需要立项补图元。
这一节转而记录更值得继续细化的方向。

| 建议优先级 | 标准类方向 | 现成工程图元 | 建议接入方式 | 当前备注 |
| --- | --- | --- | --- | --- |
| P1 | 源荷类方向细化 | 现有 `Generator / Load / Grid / PvInfeed / WindInfeed / Ess` | 明确命名、朝向、端口语义与浏览器状态回归 | `Pv/Wind/Ess` 与 `Generator/Grid/EquivalentGrid` 已补 UI 和增强最小样例 `DL` 布局证据，其余源荷类细节仍待补强 |

## 11. 可直接落任务的增量 Todo

如果下一轮要继续做“标准模型 -> 工程图元”细化，建议直接从下面几项开工：

1. 为 `EnergyConsumer / SynchronousMachine / ExternalNetworkInjection / EquivalentInjection` 继续细化进出线方向与默认布局语义。
2. 继续从中型样例中抽取带真实命名与布局的 transformer / source / shunt 局部样例做回归。
3. 为 `SeriesCompensator` 补真实样例命名回退、默认朝向与布局回归。
4. 评估 `LzPowerFeederItem` 这类工程语义图元是否存在稳定的标准 CIM 承接口径。
5. 持续用公开样例复核“未映射对象”为零的结论，避免后续新增导入链路时把标准非设备对象重新误报成缺图元。

## 12. 本轮已完成补齐

截至 2026-06-22，本轮已完成下面这批“工程已有图元但 CIM 主链未接入”的补齐：

1. `SolarGeneratingUnit -> LzPowerPvInfeedItem`
2. `WindGeneratingUnit -> LzPowerWindInfeedItem`
3. `BatteryUnit -> LzPowerEssItem`
4. `PowerTransformer(自耦变) -> LzPowerAutoXfmrItem`
5. `PowerTransformer(接地变) -> LzPowerEarthingXfmrItem`
6. `PowerTransformer(厂站变) -> LzPowerStationXfmrItem`
7. `SeriesCompensator(串补电容) -> LzPowerCapacitorItem`
8. `SeriesCompensator(串联电抗) -> LzPowerReactorItem`
9. `CIGREMV_reference` 中 `ExternalNetworkInjection(HV-Netz) -> LzPowerGridItem`
10. `CIGREMV_reference` 中 `EnergyConsumer(Load7-I) -> LzPowerLoadItem`
11. `CIGRE_MV` 中全部 `EnergyConsumer(*) -> LzPowerLoadItem`
12. `CIGRE_MV` 中 `ExternalNetworkInjection(HV-Netz) -> LzPowerGridItem`
13. `SynchronousMachine(generator-001) -> LzPowerGeneratorItem`
14. `ExternalNetworkInjection(external-001) -> LzPowerGridItem`
15. `EquivalentInjection(equivalent-001) -> LzPowerGridItem`

对应补证包含：

1. `tests/test_cim_importer.cpp`
   `graphic_mapper_refines_power_transformer_specializations_from_name_tokens()`
   已同时覆盖 `name` 与属性 token 对 `自耦变 / 厂站变` 的命中
2. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_transformer_specialization_graphics()`
   已验证导入后即使 `name` 中性，`description / aliasName` 仍可驱动 `自耦变 / 厂站变` 正确落图
3. `tests/test_cim_ui.cpp`
   `browser_marks_power_transformer_specializations_as_visualized()`
   已验证属性 token 驱动的 `自耦变 / 厂站变` 在浏览器中仍显示为 `已可视化`

对应验证：

1. `tests/test_cim_importer.cpp`
   `graphic_mapper_maps_projection_infeed_classes()`
2. `tests/test_cim_importer.cpp`
   `graphic_mapper_refines_power_transformer_specializations_from_name_tokens()`
3. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_projection_infeed_graphics()`
4. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_preserve_projection_infeed_layout_from_diagram_objects()`
5. `tests/test_cim_ui.cpp`
   `browser_marks_projection_infeed_objects_as_visualized()`
6. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_transformer_specialization_graphics()`
7. `tests/test_cim_ui.cpp`
   `browser_marks_power_transformer_specializations_as_visualized()`
8. `tests/test_cim_importer.cpp`
   `graphic_mapper_refines_series_compensator_family_from_name_and_reactance()`
9. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_series_compensator_family_graphics()`
10. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_preserve_series_compensator_family_layout_from_diagram_objects()`
11. `tests/test_cim_ui.cpp`
   `browser_marks_series_compensator_family_as_visualized()`
12. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_reference_source_and_load_graphics()`
13. `tests/test_cim_ui.cpp`
   `browser_marks_reference_source_and_load_objects_as_visualized()`
14. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_materialize_source_and_equivalent_grid_graphics()`
15. `tests/test_cim_ui.cpp`
   `browser_marks_source_and_equivalent_grid_objects_as_visualized()`
16. `tests/test_cim_graphics.cpp`
   `importer_and_scene_builder_preserve_source_and_equivalent_grid_layout_from_diagram_objects()`
17. 输出文件：
   `out/cim_importer_projection_infeed.txt`
   `out/cim_graphics_projection_infeed.txt`
   `out/cim_graphics_projection_infeed_layout.txt`
   `out/cim_ui_projection_infeed.txt`
   `out/cim_graphics_shunt_compensator_family.txt`
   `out/cim_graphics_shunt_compensator_layout.txt`
   `out/cim_ui_shunt_compensator_family.txt`
   `out/cim_importer_transformer_specialization.txt`
   `out/cim_graphics_transformer_specialization.txt`
   `out/cim_ui_transformer_specialization.txt`
   `out/cim_importer_series_compensator.txt`
   `out/cim_graphics_series_compensator.txt`
   `out/cim_graphics_series_compensator_layout.txt`
   `out/cim_graphics_reference_source_load.txt`
   `out/cim_ui_reference_source_load.txt`
   `out/cim_graphics_source_and_equivalent_grid.txt`
   `out/cim_ui_source_and_equivalent_grid.txt`
   `out/cim_graphics_source_and_equivalent_grid_layout.txt`
