# CIM 开源元数据复用设计

最后整理日期：2026-06-29

## 1. 背景

当前项目在导入合并、浏览器分类、图元映射判断时，都依赖一份 CIM 类继承关系。

此前仓库里维护的是一张“项目常用对象的简化继承表”，它能覆盖当前主链，但不是完整的 CIM 元模型。例如：

1. 有 `ConductingEquipment -> Equipment`，但缺少 `Equipment -> PowerSystemResource -> IdentifiedObject`
2. 有 `Terminal -> ACDCTerminal`，但缺少 `ACDCTerminal -> IdentifiedObject`
3. 有 `VoltageLevel / Bay / Substation -> EquipmentContainer`，但没有继续展开到 `ConnectivityNodeContainer -> PowerSystemResource -> IdentifiedObject`

这类缺口说明，手工维护简化继承表会不断遇到“当前样例够用，但一换样例就不完整”的问题。

## 2. 目标

本轮调整的目标不是继续人工补齐继承表，而是直接复用现成开源项目中的 CIM/CGMES 元数据。

优先目标：

1. 继承关系来自上游开源项目，而不是手写维护
2. 能继续服务当前 `CimClassLineage` 运行时查询
3. 同步保留基础 Profile 元数据，方便后续扩展
4. 运行时不再依赖本地手写兼容继承补丁，直接消费完整上游元数据

## 3. 方案

当前采用新的统一同步脚本：

1. 主脚本：`tools/sync_cim_metadata.py`
2. 默认模式：`--source auto`，合并 `CIMpy + GridCal`

数据来源优先级：

1. `CIMpy`
   位置：`cimpy/cgmes_v2_4_15/*.py`
   用途：提取类继承、推荐 Profile、class-level Profile
2. `GridCal`
   位置：`src/VeraGridEngine/IO/cim/cgmes/cgmes_v2_4_15/devices/*.py`
   用途：补齐 `CIMpy` 缺失的类继承，并作为交叉校验来源

`PowSyBl Core` 当前仍主要定位为参考/校验来源：

1. 它本地资源更偏 `sparql` / `xsd` / 解析链
2. 适合后续拿来做 Profile、RDF/XML、模型规则层面的进一步验证
3. 现阶段还不是最快的类继承提取入口，所以没有放进第一批运行时同步适配器

## 4. 当前输出

同步后产出两份文件：

1. `src/cim/model/CimClassLineageGenerated.inc`
   供 `CimClassLineage.h` 运行时直接消费
2. `src/cim/model/cim_class_metadata_cgmes_v2_4_15.json`
   用于记录来源、继承快照和精简后的 Profile 元数据

其中 JSON 只保留当前最有价值的信息：

1. `parents`
2. `recommended_class_profile`
3. `class_profiles`

不再把每个成员级别的全部 Profile 映射都塞进仓库快照，避免噪音过大。

## 5. 运行时口径

`src/cim/model/CimClassLineage.h` 运行时只消费生成结果：

1. `src/cim/model/CimClassLineageGenerated.inc`
2. 不再额外注入本地假父类
3. 当项目后续遇到新的标准对象时，优先刷新上游快照，而不是继续往运行时里补手写继承

## 6. 现阶段结论

当前这份表仍然是“项目运行所需的继承摘录”，但它已经按完整上游链路生成，而不是旧的简化兼容表。

区别在于：

1. 继承摘录现在直接从开源上游生成，不再靠人工补链
2. 当项目后续遇到新的标准对象时，优先刷新上游快照，而不是先改本地手写表
