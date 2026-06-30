# CIM 异步电机图元补充说明
最后整理日期：2026-06-21

## 1. 背景

`AsynchronousMachine` 在 CIM 语义上属于旋转电机设备，但在 `cimdraw` 早期补齐阶段，仓库内并没有专用的电动机图元。

为了先保证导入链路可用，早期实现采用了一个阶段性策略：

1. 不把 `AsynchronousMachine` 误映射成发电机图元
2. 在没有专用图元前，先按“电气语义更接近负荷端设备”的原则，临时落到现有 `Load` 图元
3. 等仓库具备专用电动机图元后，再替换掉这条临时映射

这个策略的重点不是把异步电机等同为普通负荷，而是在“缺少专用图元”的前提下，优先避免源荷语义被画反。

## 2. 当前实现结论

当前仓库已经补充专用电动机图元：

1. 新增图元类型 `LZ_SLD_MOTOR`
2. 新增图元类 `LzPowerMotorItem`
3. 新增工具类 `LzPowerMotorTool`
4. `CimGraphicMapper` 中 `AsynchronousMachine` 默认映射到 `LZ_SLD_MOTOR`

因此，`AsynchronousMachine` 不再继续复用 `Load` 图元作为最终表达。

## 3. 图形语义与拓扑语义的区分

这里必须明确区分两层语义：

### 3.1 图形语义

在画布和导入结果上，`AsynchronousMachine` 应显示为电动机图元，而不是：

1. 发电机图元
2. 普通负荷锯齿图元

这样做的原因是，电动机在一次接线图中应当具备独立可识别的设备形态。

### 3.2 拓扑语义

即便已经补了电动机专用图元，`AsynchronousMachine` 在当前拓扑分析里仍应按负荷端设备处理：

1. 不应被归类为电源设备
2. 不应在供电路径分析中充当源节点
3. 不应替代 `SynchronousMachine`、`ExternalNetworkInjection`、`EquivalentInjection` 这类电源侧对象

也就是说：

- 图上它是 `Motor`
- 拓扑上它不是 `Source`

## 4. 当前落地规则

建议后续统一按以下口径执行：

1. `SynchronousMachine -> LzPowerGeneratorItem`
2. `AsynchronousMachine -> LzPowerMotorItem`
3. `EnergyConsumer -> LzPowerLoadItem`
4. `ExternalNetworkInjection / EquivalentInjection -> LzPowerGridItem`

其中：

1. `AsynchronousMachine` 的图形表达与 `EnergyConsumer` 分离
2. `AsynchronousMachine` 的拓扑角色仍与负荷端设备保持一致

## 5. 测试与验收补充

围绕 `AsynchronousMachine`，至少需要覆盖两类断言：

### 5.1 图元映射

1. `CimGraphicMapper` 返回 `LZ_SLD_MOTOR`
2. 场景构建后实例类型为 `LzPowerMotorItem`

### 5.2 语义约束

1. 电动机图元不能误映射为 `LzPowerGeneratorItem`
2. 电动机在当前拓扑语义中仍不作为电源设备参与源侧判定

## 6. 后续演进

如果后续需要继续细化电动机设备，可以在 `LzPowerMotorItem` 基础上扩展，而不再回退到 `Load` 占位策略。可演进方向包括：

1. 电动机专用符号细化
2. 电动机额定参数显示
3. 不同电机子类的图形差异
4. 电动机工况相关的状态着色
