#ifndef LZWIRINGTYPES_H
#define LZWIRINGTYPES_H

/**
 * 接线图元的公共类型定义：上下游电气连接请使用
 *   LZ_CONNECT_LINE（折线/连接线工具）
 * 将线端点吸附到各图元四向 LzConnectPoint 上。
 * 「潮流」箭头图元（PowerFeeder）仅表示能量流向示意。
 */

enum class LzWiringItemKind : int
{
    BusbarSection = 0,
    CircuitBreaker,
    Disconnector,
    Transformer2W,
    PowerFeeder,
    Generator,
    Motor,
    Load,
    Ground,
    Capacitor,
    Reactor,
    Arrester,
    ArcCoil,
    Cable,
    LoadBreak,
    EarthSwitch,
    Grid,
    SvgComp,
    PvInfeed,
    Ess,
    WindInfeed,
    StationXfmr,
    Transformer3W,
    EarthingXfmr,
    AutoXfmr,
    SplitReactor,
    CurrentTransformer,
    VoltageTransformer,
    Fuse,
    Junction,
};

/** 运行着色/告警等，可由 LzScene::setWiringData 按 wiringDataKey 驱动 */
enum class LzWiringRunState : int
{
    Normal = 0,
    Energized = 1,
    Deenergized = 2,
    Alarm = 3,
};

/**
 * 接线图元所依据的图形体系（与 IEC 60617、IEEE 315、GB 图用符号等对照实现）。
 * 当前版本为接口预留：画布与工具箱仍共用一套几何，后续按枚举分岔或接入符号包插件。
 */
enum class LzWiringSymbolStandard : int
{
    IEC = 0,  ///< IEC / 欧洲及多国原理图习惯
    ANSI = 1, ///< 美标 ANSI / IEEE 单线图习惯
    GB = 2,   ///< 国标 GB 图用图形符号习惯
};

#endif // LZWIRINGTYPES_H
