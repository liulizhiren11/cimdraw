# CIM Minimal Test Data

本目录存放 P0 阶段最小可用的 CIM/CGMES 测试样例。

当前样例：

- `EQ_SSH_sample/`: 真实最小 `EQ + SSH` 样例，用于验证 XML/RDF 解析、EQ/SSH 合并、对象树展示和图元定位主链路。
- `CT_PT_Fuse_sample/`: 覆盖 `CurrentTransformer`、`PotentialTransformer`、`VoltageTransformer`、`Fuse` 的最小样例，用于验证新补齐图元的导入、映射与场景实例化。
- `Junction_sample/`: 覆盖 `Junction` 连接结点图元的最小样例，用于验证连接结点的导入、映射、场景实例化，以及同节点下优先使用可见结点而非隐藏辅助枢纽。
- `Transformer3w_sample/`: 覆盖 `PowerTransformer` 三绕组判定样例，用于验证 `PowerTransformerEnd -> PowerTransformer` 逆向归并、`2W/3W` 图元分流与场景实例化。
- `Transformer3w_layout_sample/`: 覆盖 `PowerTransformer` 三绕组带 `DL` 布局信息样例，用于验证 `DiagramObject / DiagramObjectPoint / rotation` 导入后，三绕组图元的位置与旋转在场景中稳定保留。
- `ShuntCompensator_family_sample/`: 覆盖 `ShuntCompensator / LinearShuntCompensator / NonlinearShuntCompensator / StaticVarCompensator / PetersenCoil` 的补偿设备分流样例，用于验证 `Capacitor / Reactor / SplitReactor / SvgComp / ArcCoil` 图元映射与场景实例化。
