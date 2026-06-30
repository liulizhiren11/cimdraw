# CIM Behavior Projection 框架设计

最后整理日期：2026-06-30

## 1. 文档目标

本文定义 `Behavior Projection` 的职责、求值入口、上下文依赖与结果约束。

在统一术语下，`Behavior Projection` 是 `PowerSystemModel` 在运行期行为结果上的表达。

## 2. 目标

`Behavior Projection` 回答的问题是：

“在当前拓扑、状态、配置、上下文下，这个设备现在表现成什么行为状态？”

## 3. 推荐结构

推荐把行为相关能力拆成：

1. `Behavior Service / Engine`
2. `Policy`
3. `BehaviorResult`

理解方式：

1. `Service/Engine` 负责算
2. `Policy` 负责差异规则
3. `BehaviorResult` 负责表达结果

## 4. 为什么不是对象实体

不建议长期使用：

1. `SwitchBehavior`
2. `TransformerBehavior`

这种长生命周期对象实体方式。

原因：

1. 行为往往依赖拓扑
2. 行为往往依赖状态
3. 行为往往依赖上下文
4. 行为往往依赖配置

因此它更像计算服务，而不是设备自带方法。

## 5. 输入依赖

`Behavior Projection` 的输入建议统一收敛为：

1. `Semantic Projection`
2. `Device Traits`
3. `Topology Projection`
4. `ContextHub`
5. `State/SSH`
6. `Configuration`

## 6. Context 约束

建议统一使用：

1. `ContextHub`
2. `ContextSnapshot`

而不是让服务层到处传：

1. `AnalysisContext`
2. `SimulationContext`
3. `RuntimeContext`
4. `RenderContext`

这样可以避免上下文对象分散传递和参数爆炸。

## 7. BehaviorResult 约束

`BehaviorResult` 是 `Behavior Projection` 的主要承载形式。

它应满足：

1. 不可变
2. 值对象
3. 快照化
4. 不原地修改

推荐典型字段：

1. `conductivity`
2. `canOperate`
3. `available`
4. `energized`
5. `terminalConnectivity`

## 8. 生命周期

### 8.1 首次构建

在 `Semantic Projection` 和 `Topology Projection` 建好后，按需生成行为结果。

### 8.2 增量更新

推荐链路：

`State Update -> Dispatcher -> Cache Invalidate -> Behavior Recompute`

原则：

1. 结果失效而不是对象改写
2. 按需重算而不是全量刷新

## 9. 查询与暴露

不建议让图形层直接依赖行为服务。

更推荐：

1. `Behavior Projection` 输出 `BehaviorResult`
2. `Query API / Facade` 向图形、分析、脚本暴露查询接口
3. `Graphic Projection` 消费结果而不是消费求值服务

## 10. 与其他 Projection 的关系

1. `Behavior Projection` 依赖 `Semantic Projection`
2. `Behavior Projection` 依赖 `Topology Projection`
3. `Graphic Projection` 消费 `Behavior Projection`

## 11. 结论

`Behavior Projection` 的关键是：

1. 统一求值入口
2. 统一上下文入口
3. 结果快照化
4. 不把行为做成长期实体对象
