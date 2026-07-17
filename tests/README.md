# cimdraw tests

`tests/` 只保留当前 `cimdraw` 工程自己的回归测试。

## Files

- `test_cimdraw_topology_main.cpp`: QTest 入口
- `test_cimdraw_topology.h`: 测试声明
- `test_cimdraw_topology.cpp`: 工程默认配置、拓扑角色映射和工具注册覆盖
- `test_cim_model.h/.cpp`: `CimModel`、`CimObject`、`CimProfileSet` 基础覆盖
- `test_cim_importer.h/.cpp`: `CgmesPackageImporter`、`CimGraphicMapper`、`CimIdIndex` 基础覆盖

## Data

- `data/cim/minimal/`: 最小可用 `EQ + SSH` 样例目录
- `data/cim/medium/`: 公开中等规模 `CGMES` 样例目录，覆盖 `EQ / SSH / TP / SV / DL / GL`
- `data/cim/invalid/`: 异常与失败路径样例目录
