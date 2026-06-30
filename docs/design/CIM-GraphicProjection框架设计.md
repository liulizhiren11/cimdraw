# CIM Graphic Projection 框架设计

最后整理日期：2026-06-30

## 1. 文档目标

本文定义 `Graphic Projection` 的职责、组成、索引归属与表现层边界。

在统一术语下，`Graphic Projection` 是 `PowerSystemModel` 在图元表现与交互上的表达。

## 2. 目标

`Graphic Projection` 回答的问题是：

“这个模型在图上应该怎么显示、怎么连接、怎么交互？”

## 3. 推荐组成

推荐把图形相关能力理解为一整个 Projection，而不是几个零散类：

1. `GraphicMapper`
2. `GraphicRegistry`
3. `GraphicAdapter / Binder`
4. `GraphicItem`
5. `Scene`

## 4. 职责拆分

### 4.1 GraphicMapper

职责：

1. 对象到图元的映射
2. 拓扑到连接点的映射
3. 行为结果到显示状态的映射

它是算法组件，不是数据仓库。

### 4.2 GraphicRegistry

职责：

1. 双向索引
2. `ObjectId -> Item`
3. `TerminalId -> ConnectPoint`
4. `TopologyNode -> Items`

它是索引归属者，不建议把这类职责继续塞回 `GraphicMapper`。

### 4.3 GraphicAdapter / Binder

职责：

1. 把映射结果绑定到图元
2. 把行为状态喂给图元
3. 隔离图元实现与上层结构

### 4.4 GraphicItem / Scene

职责：

1. 绘制
2. 交互
3. 场景接入

`Scene` 是运行环境，不是业务层。

## 5. 关键边界

### 5.1 不承载标准语义

图元层不应直接承载：

1. `Terminal` 语义
2. `Equipment` 语义
3. 拓扑真源职责

推荐关系：

`Terminal -> GraphicMapper -> ConnectPoint`

而不是：

`GraphicItem` 自己知道 `Terminal`

### 5.2 不承载拓扑真源

1. `Graphic Projection` 不是连接关系真源
2. 它只是消费 `Topology Projection`

### 5.3 不直接驱动行为求值

1. 图元层不直接调 `Behavior Service`
2. 图元层优先消费 `BehaviorResult`

## 6. 依赖约束

推荐依赖：

1. `Graphic Projection -> Query API / Facade`
2. `Graphic Projection -> Behavior Projection`
3. `Graphic Projection -> Topology Projection`

不推荐依赖：

1. `GraphicItem -> Semantic Projection`
2. `GraphicItem -> Behavior Service`
3. `Scene -> TopologyGraph`

## 7. 生命周期

### 7.1 首次构建

推荐链路：

`Query API / Facade -> GraphicMapper -> GraphicRegistry -> GraphicAdapter -> GraphicItem`

### 7.2 增量更新

推荐链路：

`BehaviorResult Update -> Graphic State Refresh -> Scene Update`

## 8. 结论

`Graphic Projection` 的关键是：

1. 它是表现层
2. 它不是语义层
3. 它不是拓扑真源
4. 它应通过 Mapper/Registry/Adapter/Item 分清算法、索引和表现
