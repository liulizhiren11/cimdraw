# CIM Invalid Test Data

本目录存放 P0/P1 阶段用于校验失败路径的异常样例。

当前样例：

- `public-non-cim-input.json`
  来源：`PowerGridModel/power-grid-model-io`
  用途：验证导入器面对公开的非 `XML/RDF` 文件时，会给出明确失败结果。
- `plain_xml_not_cim/`
  来源：仓库内构造样例
  用途：验证普通 `XML` 文件不会被误识别为 `CIM/CGMES RDF`，并给出明确错误提示。
- `partial_invalid/`
  来源：仓库内构造样例
  用途：验证目录中存在可解析 `EQ` 文件、但另一个 `SSH` 文件损坏时，导入器会保留已导对象，同时报告错误并返回失败。
- `duplicate_mrid_same_profile/`
  来源：仓库内构造样例
  用途：验证同一 Profile 文件中重复定义同一个 `mRID` 时，导入器会报告重复对象错误。
- `class_conflict_between_profiles/`
  来源：仓库内构造样例
  用途：验证同一个 `mRID` 在不同 Profile 中被声明成不同类时，导入器会报告类名冲突错误。

建议后续补充：

- 缺少依赖文件
- Profile 不一致
- 端子关系不完整
