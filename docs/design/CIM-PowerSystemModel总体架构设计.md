# CIM PowerSystemModel 总体架构设计

最后整理日期：2026-06-30

## 1. 文档目标

本文用于给 `lz-power` 的 CIM 重构提供新的统一总纲。

核心思想只有一句：

`Semantic`、`Topology`、`Behavior`、`Graphic` 不是四套彼此独立的模型，而是同一个 `PowerSystemModel` 的不同 Projection。

后续所有模块拆分、接口设计、缓存策略、图元重构，都建议围绕这个统一抽象展开。

## 2. 总体结构

推荐后续按下面的结构理解系统：

```text
                  +----------------------+
                  |   PowerSystemModel   |
                  +----------------------+
                             |
      -------------------------------------------------
      |                    |               |          |
      v                    v               v          v
+----------------+ +----------------+ +----------------+ +----------------+
| Semantic       | | Topology       | | Behavior       | | Graphic        |
| Projection     | | Projection     | | Projection     | | Projection     |
+----------------+ +----------------+ +----------------+ +----------------+
```

其中：

1. `PowerSystemModel` 是统一对象承载与统一语义来源。
2. `Semantic Projection` 负责标准语义解释。
3. `Topology Projection` 负责运行期连接关系表达。
4. `Behavior Projection` 负责行为求值结果表达。
5. `Graphic Projection` 负责图形展示与交互表达。

## 3. PowerSystemModel 定位

`PowerSystemModel` 不是新的重实体树，而是对当前 `CimObject/CimModel` 体系的统一命名与统一抽象。

建议定位如下：

1. `CimModel` 拥有全部 `CimObject`。
2. `CimObject` 继续作为唯一对象承载模型。
3. 导入完成后，`CimObject` 原则上视作只读。
4. 所有 Projection 都从同一个 `PowerSystemModel` 出发，而不是各自维护一份复制数据。

## 4. 四类 Projection

### 4.1 Semantic Projection

职责：

1. 把 `CimObject` 解释成标准 CIM 语义。
2. 提供 `IdentifiedObject`、`Equipment`、`Switch`、`Terminal` 等语义读取入口。
3. 为 `Device Traits`、`Topology Projection`、`Behavior Projection` 提供稳定输入。

当前推荐实现形态：

1. `Semantic View`
2. `Semantic Facade`

### 4.2 Topology Projection

职责：

1. 把对象间连接关系表达成统一运行期拓扑。
2. 成为导通、孤岛、连通域、分析与显示的共同基础。
3. 作为运行期网络连接关系唯一真源。

当前推荐实现形态：

1. `Topology Extractor`
2. `Topology Builder`
3. `TopologyGraph`
4. `TopologyIndex`

### 4.3 Behavior Projection

职责：

1. 基于语义、拓扑、上下文、状态计算设备行为。
2. 输出统一行为快照。
3. 为图形、分析、脚本提供稳定行为结果。

当前推荐实现形态：

1. `Behavior Service / Engine`
2. `Policy`
3. `BehaviorResult`

### 4.4 Graphic Projection

职责：

1. 把对象、拓扑、行为结果映射为图元表现。
2. 管理图元侧映射关系与索引。
3. 承接图元绘制、交互与场景接入。

当前推荐实现形态：

1. `GraphicMapper`
2. `GraphicRegistry`
3. `GraphicAdapter / Binder`
4. `GraphicItem`
5. `Scene`

## 5. 辅助基础设施

以下能力不建议再作为单独业务层理解，而应作为横向基础设施存在：

1. `Application / Workflow`
2. `Event / Update Dispatcher`
3. `Cache`
4. `Query API / Facade`
5. `ContextHub`
6. `DeviceTraitsRegistry`

职责约定：

1. `Application / Workflow` 负责编排主流程。
2. `Dispatcher` 负责统一失效传播与事件通知。
3. `Cache` 负责统一缓存、失效和按需重算。
4. `Query API / Facade` 负责对外暴露稳定查询边界。
5. `ContextHub` 负责统一上下文访问入口。
6. `DeviceTraitsRegistry` 负责静态只读工程元数据查询。

## 6. 关键约束

### 6.1 单一模型

1. 系统内部只维护一个 `PowerSystemModel`。
2. Projection 是解释、映射、求值和展示，不是复制对象。

### 6.2 可变与不可变边界

1. `CimObject` 导入后优先只读。
2. `Semantic Projection` 天然只读。
3. `Device Traits` 为静态只读元数据。
4. `BehaviorResult` 为不可变值对象。
5. `TopologyGraph` 是受控可变边界。
6. `GraphicItem/Scene` 是表现层可变对象。

### 6.3 依赖方向

1. `Semantic Projection -> PowerSystemModel`
2. `Topology Projection -> Semantic Projection`
3. `Behavior Projection -> Semantic Projection + Topology Projection + ContextHub`
4. `Graphic Projection -> Query API / Facade`
5. 上层模块优先依赖 Query API，而不是直接依赖底层图结构或语义对象。

## 7. 生命周期

### 7.1 首次构建

推荐主链：

`Source -> Parser -> Importer -> PowerSystemModel -> Semantic Projection -> Topology Projection -> Behavior Projection -> Graphic Projection`

### 7.2 增量更新

推荐主链：

`SSH/State Update -> Dispatcher -> Cache Invalidate -> Behavior Recompute -> Graphic Refresh`

原则：

1. 优先失效传播。
2. 优先按需重算。
3. 避免整网全量重建。

## 8. 建议的文档拆分

为了后续工程推进，建议围绕本总纲维护以下文档：

1. `CIM-PowerSystemModel总体架构设计.md`
2. `CIM-SemanticProjection框架设计.md`
3. `CIM-TopologyProjection框架设计.md`
4. `CIM-BehaviorProjection框架设计.md`
5. `CIM-GraphicProjection框架设计.md`

## 9. 结论

这套架构的核心不是“继续加层”，而是统一语言：

1. `PowerSystemModel` 是唯一模型。
2. `Semantic/Topology/Behavior/Graphic` 是四类 Projection。
3. 基础设施负责编排、缓存、查询、上下文和事件。
4. 代码重构应优先围绕 Projection 边界落地。
