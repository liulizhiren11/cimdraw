#ifndef LZITEMCONFIG_H
#define LZITEMCONFIG_H

#include <type_traits>

using LzDrawTypeId = int;

// 基础编辑图元
enum BASIC_DRAW_TYPE : int
{
    LZ_SELECTION=0,
    LZ_RECT,
    LZ_ARC,
    LZ_IMAGE,
    LZ_HYPER_LINK,
    LZ_ROUNDRECT,
    LZ_ELLIPSE,
    LZ_GROUP,
    LZ_LINE,
    LZ_POLYLINE,
    LZ_POLYGON,
    LZ_BEZIER,
    LZ_BRACE_CALLOUT,
    LZ_TEXT,
    LZ_CONNECT_LINE,
    LZ_TOPOLOGY_NODE,
};

// 一次接线图 / 单线图设备图元
enum SLD_DRAW_TYPE : int
{
    LZ_SLD_BUSBAR_SECTION = LZ_TOPOLOGY_NODE + 1,
    LZ_SLD_CIRCUIT_BREAKER,
    LZ_SLD_DISCONNECTOR,
    LZ_SLD_TRANSFORMER_2W,
    LZ_SLD_POWER_FEEDER,
    LZ_SLD_GENERATOR,
    LZ_SLD_LOAD,
    LZ_SLD_GROUND,
    LZ_SLD_MOTOR,
};

enum POWER_WIRING_SYMBOL_DRAW_TYPE : int
{
    // 显式保留历史编号，避免既有场景文件中的 shape 数值失配
    LZ_WSYM_CAPACITOR = 21,
    LZ_WSYM_REACTOR = 28,
    LZ_WSYM_ARRESTER = 126,
    LZ_WSYM_ARC_COIL = 128,
    LZ_WSYM_CABLE = 129,
    LZ_WSYM_LOAD_BREAK = 131,
    LZ_WSYM_EARTH_SWITCH = 133,
    LZ_WSYM_GRID = 134,
    LZ_WSYM_SVG_COMP = 135,
    LZ_WSYM_PV_INFEED = 136,
    LZ_WSYM_ESS = 137,
    LZ_WSYM_WIND_INFEED = 138,
    LZ_WSYM_STATION_XFMR = 139,
    LZ_WSYM_TRANSFORMER_3W = 140,
    LZ_WSYM_EARTHING_XFMR = 141,
    LZ_WSYM_AUTO_XFMR = 142,
    LZ_WSYM_SPLIT_REACTOR = 143,
    LZ_WSYM_CURRENT_TRANSFORMER = 144,
    LZ_WSYM_VOLTAGE_TRANSFORMER = 145,
    LZ_WSYM_FUSE = 146,
    LZ_WSYM_JUNCTION = 147,
};

//控制点状态
enum HANDLE_STATE : int
{
    HANDLE_OFF = 0,
    HANDLE_INACTIVE,
    HANDLE_ACTIVE,
    HANDLE_HOVER,
    HANDLE_DRAG,
};

//控制点
enum HANDLE_POINT : int
{
    HANDLE_NONE = 0 ,
    LEFT_TOP ,
    TOP,
    RIGHT_TOP,
    RIGHT,
    RIGHT_BOTTOM,
    BOTTOM,
    LEFT_BOTTOM,
    LEFT,
};

//选择模式
enum SELECT_MODE : int
{
    MODE_NONE = 0,
    MODE_SELECT,            //单选
    MODE_NET_SELECT,        //框选
    MODE_CTRL_SELECT,       //ctrl多选
    MODE_MOVE,              //移动
    MODE_SIZE,              //缩放
    MODE_ROTATE,            //旋转
    MODE_EDITOR,            //编辑
    MODE_SEGMENT_DRAG,      // draw.io 风格：拖拽线段添加路径点
};

//链接图元
enum HYPER_LINK_TYPE : int
{
    HYPER_LINK_NONE = -1,
    HYPER_LINK_XML,       //超链接场景
    HYPER_LINK_EXE,       //超链接exe
};

//图元的构建方式
enum CREATE_MODE : int
{
    CREATE_MODE_NONE = 0,
    CREATE_MODE_DRAG = 1,   //拖拽,适用于确定形状的
    CREATE_MODE_PRESS = 2   //点击,适用于不确定形状的
};

// 枚举类型转换模板
template<typename Enum>
constexpr auto toInt(Enum e) -> typename std::underlying_type<Enum>::type
{
    static_assert(std::is_enum<Enum>::value, "toInt only accepts enum types");
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}

inline constexpr bool lzIsSldDrawType(LzDrawTypeId t)
{
    return t >= LZ_SLD_BUSBAR_SECTION && t <= LZ_SLD_MOTOR;
}

inline constexpr bool lzIsPowerWiringSymbolDrawType(LzDrawTypeId t)
{
    switch (t)
    {
    case LZ_WSYM_CAPACITOR:
    case LZ_WSYM_REACTOR:
    case LZ_WSYM_ARRESTER:
    case LZ_WSYM_ARC_COIL:
    case LZ_WSYM_CABLE:
    case LZ_WSYM_LOAD_BREAK:
    case LZ_WSYM_EARTH_SWITCH:
    case LZ_WSYM_GRID:
    case LZ_WSYM_SVG_COMP:
    case LZ_WSYM_PV_INFEED:
    case LZ_WSYM_ESS:
    case LZ_WSYM_WIND_INFEED:
    case LZ_WSYM_STATION_XFMR:
    case LZ_WSYM_TRANSFORMER_3W:
    case LZ_WSYM_EARTHING_XFMR:
    case LZ_WSYM_AUTO_XFMR:
    case LZ_WSYM_SPLIT_REACTOR:
    case LZ_WSYM_CURRENT_TRANSFORMER:
    case LZ_WSYM_VOLTAGE_TRANSFORMER:
    case LZ_WSYM_FUSE:
    case LZ_WSYM_JUNCTION:
        return true;
    default:
        return false;
    }
}

inline constexpr bool lzIsWiringSymbolDrawType(LzDrawTypeId t)
{
    return lzIsPowerWiringSymbolDrawType(t);
}

#define LZ_PI  3.14159265358979323846

#ifdef WIN32
# ifdef LZ_DLL_EXPORT
#  define LZ_DLL_EXPORT Q_DECL_EXPORT
# else
#  define LZ_DLL_EXPORT Q_DECL_IMPORT
# endif
#else
# define LZ_DLL_EXPORT
#endif

#endif
