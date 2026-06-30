# CIM避雷器图元补充说明

## 背景

`SurgeArrester` 在 CIM 语义里对应避雷器。`lz-power` 仓库里原本已经有可直接复用的避雷器图元 `LzPowerArresterItem`，也有对应工具和拓扑角色，但早期 CIM 补齐阶段没有把这条映射接通。

这类缺口和 `AsynchronousMachine` 不一样：

1. 不是仓库里完全没有图元
2. 也不需要先临时借用别的符号
3. 只是不应继续遗漏 `CIM -> 图元` 的映射链路

## 本次补齐

本次把 `SurgeArrester` 补齐到现有避雷器图元：

1. `CimGraphicMapper` 中增加 `SurgeArrester -> LZ_WSYM_ARRESTER`
2. 运行时继承关系改为直接消费完整上游元数据，不再额外手写 `SurgeArrester -> ConductingEquipment` 兼容父类
3. `CimSceneBuilder` 中增加避雷器分组与实例化尺寸
4. 回归测试增加 `SurgeArrester` 的映射与场景实例化断言

## 语义约束

`SurgeArrester` 应映射为避雷器图元，而不是：

1. 负荷
2. 电抗器
3. 接地
4. 任意泛化导电设备占位符

原因很直接：仓库里已经存在语义匹配的专用图元，再继续复用其它图元只会降低图形表达的准确性。

## 后续规则

后面继续补 CIM 缺口时，优先按下面顺序判断：

1. 仓库里是否已有语义准确的现成图元
2. 如果有，优先直接接通映射链路
3. 如果没有，再评估是否需要临时复用接近语义的图元
4. 临时复用必须写入设计说明，避免后续误当成最终方案
