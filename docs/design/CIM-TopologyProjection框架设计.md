# CIM Topology Projection 框架设计

最后整理日期：2026-06-30

## 1. 文档目标

本文定义 `Topology Projection` 的职责、构建方式、可变边界与查询约束。

在统一术语下，`Topology Projection` 是 `PowerSystemModel` 在运行期网络连接关系上的表达。

## 2. 目标

`Topology Projection` 回答的问题是：

“当前模型在网络连接上是怎样连起来的？”

它应成为以下能力的共同基础：

1. 导通判断
2. 连通域判断
3. 孤岛判断
4. 潮流/短路等分析准备
5. 图元连线与展示

## 3. 核心构成

推荐稳定边界：

1. `Topology Extractor`
2. `Topology Builder`
3. `TopologyGraph`
4. `TopologyIndex`

实现中间物：

1. `TopologyDescription`

其中 `TopologyDescription` 更适合作为内部收敛对象，而不是长期公共模型。

## 4. 构建链路

推荐主链：

`PowerSystemModel -> Semantic Projection -> Topology Extractor -> Topology Builder -> TopologyGraph -> TopologyIndex`

解释：

1. `Extractor` 负责提取连接关系。
2. `Builder` 负责生成统一图结构。
3. `Graph` 负责承载运行期拓扑。
4. `Index` 负责查询加速。

## 5. 真源定位

必须明确：

1. `TopologyGraph` 是运行期网络连接关系的唯一真源。
2. `Graphic Projection` 不是拓扑真源。
3. `Behavior Projection` 不是拓扑真源。
4. `CimObject` 是导入数据真源，但不是运行期连接关系真源。

## 6. 可变边界

`Topology Projection` 是整个系统中最重要的受控可变边界之一。

建议约束：

1. 对普通消费者只提供只读接口。
2. 图结构修改只能由 `TopologyBuilder` 或受控更新流程执行。
3. 不允许任意业务模块直接改图。

## 7. 查询职责

`TopologyIndex` 负责：

1. 设备定位
2. 端子定位
3. 连接节点定位
4. 拓扑节点定位
5. 高效查询支撑

上层读取时建议：

1. 分析模块可在受控情况下读取 `Topology Projection`
2. 图形模块优先通过 `Query API / Facade`

## 8. 生命周期

### 8.1 首次构建

导入后尽早建立 `Topology Projection`，不要等到图元层之后再补建。

### 8.2 增量更新

如果状态更新不改变连接关系：

1. 不重建 `TopologyGraph`
2. 只传播行为/图形层失效

如果连接关系本身变化：

1. 触发受控拓扑重建
2. 再传播行为与图形层失效

## 9. 与其他 Projection 的关系

1. `Topology Projection` 依赖 `Semantic Projection`
2. `Behavior Projection` 依赖 `Topology Projection`
3. `Graphic Projection` 消费 `Topology Projection`

## 10. 结论

`Topology Projection` 的关键是：

1. 尽早构建
2. 只认一个真源
3. 对外只读
4. 修改受控
