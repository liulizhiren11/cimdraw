# CIM Semantic Projection 框架设计

最后整理日期：2026-06-30

## 1. 文档目标

本文定义 `Semantic Projection` 的职责、边界、接口风格与实现约束。

在统一术语下，`Semantic Projection` 是 `PowerSystemModel` 的标准语义解释层。

## 2. 目标

`Semantic Projection` 回答的问题只有一个：

“这个 `CimObject` 在 CIM 标准语义上应该如何理解？”

它不负责：

1. 运行期拓扑真源
2. 行为求值
3. 图形展示
4. 工程元数据管理

## 3. 推荐实现形态

当前推荐使用轻量只读对象：

1. `IdentifiedObjectView`
2. `PowerSystemResourceView`
3. `EquipmentView`
4. `ConductingEquipmentView`
5. `SwitchView`
6. `TerminalView`
7. `ConnectivityNodeView`

这些 `View/Facade` 是 `Semantic Projection` 的当前代码落地形态，不建议再理解成标准类继承树。

## 4. 核心职责

1. 基于 `className + attribute + reference + profile` 解释对象。
2. 提供标准语义读取接口。
3. 向 `Device Traits` 提供语义输入。
4. 向 `Topology Projection` 提供连接关系解释输入。
5. 向 `Behavior Projection` 提供设备语义输入。

## 5. 接口风格

推荐调用方式：

```cpp
SwitchView sw(cimObject);
bool open = sw.isOpen();
auto terms = sw.terminals();
```

接口风格建议：

1. 小而直白
2. 只读
3. 不缓存运行态
4. 不混入图元逻辑
5. 不混入拓扑求值

## 6. 约束

### 6.1 无状态

1. `Semantic Projection` 应保持无状态。
2. 不拥有对象。
3. 不保存计算结果。
4. 不保存行为缓存。

### 6.2 非实体化

1. 不把它做成庞大的 `C++` 实体继承树。
2. 不要求每个 CIM 类都对应一个重对象。
3. 优先使用解释式视图，而不是复制式实体。

### 6.3 只读边界

1. `Semantic Projection` 只读取 `PowerSystemModel`。
2. 不反向修改 `CimObject`。

## 7. 与 Device Traits 的关系

`Device Traits` 不属于 `Semantic Projection`。

关系应理解为：

1. `Semantic Projection` 回答“标准上它是什么”
2. `Device Traits` 回答“工程上如何分类/如何画/如何选策略”

因此：

1. `Device Traits` 是工程元数据，不是标准语义。
2. 不同 CIM 类可以共享同一份 `Device Traits`。

## 8. 与其他 Projection 的关系

1. `Topology Projection` 消费 `Semantic Projection`
2. `Behavior Projection` 消费 `Semantic Projection`
3. `Graphic Projection` 不应直接依赖 `Semantic Projection`，而应优先通过 `Query API / Facade`

## 9. 建议的代码边界

建议聚焦在：

1. `src/cim/model`
2. `src/cim/semantic`

可逐步拆为：

1. 语义视图基类/基础工具
2. 常见 CIM 语义视图
3. 语义查询辅助函数
4. 语义缓存辅助能力

## 10. 结论

`Semantic Projection` 的关键不是“类很多”，而是“解释稳定”：

1. 保持只读
2. 保持轻量
3. 不做继承树复制
4. 为其他 Projection 提供统一标准语义入口
