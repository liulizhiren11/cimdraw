# 电力工程设计文档入口

本目录用于承接 `cimdraw` 当前使用中的 CIM 与电力图元专题设计文档。

## 当前主方案

当前目录已围绕统一主线重组为：

`PowerSystemModel -> Semantic Projection / Topology Projection / Behavior Projection / Graphic Projection`

建议阅读顺序如下。

### 总体架构

1. [CIM-PowerSystemModel总体架构设计.md](CIM-PowerSystemModel总体架构设计.md)

### 四个 Projection 框架

1. [CIM-SemanticProjection框架设计.md](CIM-SemanticProjection框架设计.md)
2. [CIM-TopologyProjection框架设计.md](CIM-TopologyProjection框架设计.md)
3. [CIM-BehaviorProjection框架设计.md](CIM-BehaviorProjection框架设计.md)
4. [CIM-GraphicProjection框架设计.md](CIM-GraphicProjection框架设计.md)

## 保留的参考与补充文档

以下文档继续保留，作为补充说明、范围梳理、执行与验收资料：

1. [CIM标准模型与工程图元对应表.md](CIM标准模型与工程图元对应表.md)
2. [CIM标准图元缺口与补齐设计.md](CIM标准图元缺口与补齐设计.md)
3. [CIM拓扑分组与正交布置设计.md](CIM拓扑分组与正交布置设计.md)
4. [CIM开源元数据复用设计.md](CIM开源元数据复用设计.md)
5. [CIM产品迭代与阶段目标设计.md](CIM产品迭代与阶段目标设计.md)
6. [CIM开发ToDo与测试方案.md](CIM开发ToDo与测试方案.md)
7. [CIM分阶段ToDo与验收清单.md](CIM分阶段ToDo与验收清单.md)
8. [CIM标准图元补齐ToDo与测试验收方案.md](CIM标准图元补齐ToDo与测试验收方案.md)
9. [CIM标准图元补齐执行ToDoList.md](CIM标准图元补齐执行ToDoList.md)
10. [CIM异步电机图元补充说明.md](CIM异步电机图元补充说明.md)
11. [CIM避雷器图元补充说明.md](CIM避雷器图元补充说明.md)

## 维护约定

1. 总架构与主边界优先写入 Projection 系列文档。
2. 补充说明、ToDo、验收、范围清单不应反向覆盖主架构文档。
3. 新增设计文档时，优先判断它属于“总体”“某个 Projection”还是“补充/执行资料”。

## 标准参考入口

1. UCAIug CIM 模型概览：
   https://cim-mg.ucaiug.io/section4-cim-overview/?utm_source=chatgpt.com
