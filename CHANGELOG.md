# 更新日志

本文件记录 `cimdraw` 每次提交中的功能、构建、文档与约束变化。

格式遵循“未发布 + 日期条目”的轻量约定。每次提交前至少补充一条与本次变更相关的记录；如果只是临时本地验证或未提交实验，不需要写入。

## 未发布

### 功能

- 继续以 `PowerSystemModel -> Projection` 为 CIM 功能主线，保留 CIM 相关源码、测试、设计文档与构建入口。

### 构建

- 统一 `BUILD_CIMDRAW_TESTS` 下的 QTest 目标声明，测试目标共用 `add_cimdraw_qtest()` 配置入口，减少 Qt 链接、输出目录和 Windows 宏定义重复。

### 仓库治理

- 明确远端 Git 跟踪范围以源码、CMake、README、必要测试与核心文档为主；本地构建产物、临时导出、旧资源和非必要本地数据保留在工作区但不再进入远端跟踪。

## 2026-07-17

### 功能

- CIM 图元保留策略收口：保留 CIM 相关电力图元，移除不再属于 CIM 范围的馈线图元枚举、图元与工具链路。

### 验证

- 已验证 `cimdraw_cim_graphics_tests`、`cimdraw_cim_importer_tests`、`cimdraw_cimdraw_topology_tests` 构建通过。
