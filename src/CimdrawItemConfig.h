#ifndef CIMDRAWITEMCONFIG_H
#define CIMDRAWITEMCONFIG_H

#include <type_traits>

using CimdrawDrawTypeId = int;

// 基础编辑图元
enum BASIC_DRAW_TYPE : int
{
    CIMDRAW_SELECTION=0,
    CIMDRAW_RECT,
    CIMDRAW_ARC,
    CIMDRAW_IMAGE,
    CIMDRAW_HYPER_LINK,
    CIMDRAW_ROUNDRECT,
    CIMDRAW_ELLIPSE,
    CIMDRAW_GROUP,
    CIMDRAW_LINE,
    CIMDRAW_POLYLINE,
    CIMDRAW_POLYGON,
    CIMDRAW_BEZIER,
    CIMDRAW_BRACE_CALLOUT,
    CIMDRAW_TEXT,
    CIMDRAW_CONNECT_LINE,
    CIMDRAW_TOPOLOGY_NODE,
};

// CIM 一次接线图设备图元
enum SLD_DRAW_TYPE : int
{
    CIMDRAW_SLD_BUSBAR_SECTION = CIMDRAW_TOPOLOGY_NODE + 1,
    CIMDRAW_SLD_CIRCUIT_BREAKER,
    CIMDRAW_SLD_DISCONNECTOR,
    CIMDRAW_SLD_TRANSFORMER_2W,
    CIMDRAW_SLD_GENERATOR,
    CIMDRAW_SLD_LOAD,
    CIMDRAW_SLD_GROUND,
    CIMDRAW_SLD_MOTOR,
};

// CIM 电力设备补充图元
enum POWER_WIRING_SYMBOL_DRAW_TYPE : int
{
    CIMDRAW_WSYM_CAPACITOR = CIMDRAW_SLD_MOTOR + 1,
    CIMDRAW_WSYM_REACTOR,
    CIMDRAW_WSYM_ARRESTER,
    CIMDRAW_WSYM_ARC_COIL,
    CIMDRAW_WSYM_CABLE,
    CIMDRAW_WSYM_LOAD_BREAK,
    CIMDRAW_WSYM_EARTH_SWITCH,
    CIMDRAW_WSYM_GRID,
    CIMDRAW_WSYM_SVG_COMP,
    CIMDRAW_WSYM_PV_INFEED,
    CIMDRAW_WSYM_ESS,
    CIMDRAW_WSYM_WIND_INFEED,
    CIMDRAW_WSYM_STATION_XFMR,
    CIMDRAW_WSYM_TRANSFORMER_3W,
    CIMDRAW_WSYM_EARTHING_XFMR,
    CIMDRAW_WSYM_AUTO_XFMR,
    CIMDRAW_WSYM_SPLIT_REACTOR,
    CIMDRAW_WSYM_CURRENT_TRANSFORMER,
    CIMDRAW_WSYM_VOLTAGE_TRANSFORMER,
    CIMDRAW_WSYM_FUSE,
    CIMDRAW_WSYM_JUNCTION,
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

inline constexpr bool cimdrawIsSldDrawType(CimdrawDrawTypeId t)
{
    return t >= CIMDRAW_SLD_BUSBAR_SECTION && t <= CIMDRAW_SLD_MOTOR;
}

inline constexpr bool cimdrawIsPowerWiringSymbolDrawType(CimdrawDrawTypeId t)
{
    return t >= CIMDRAW_WSYM_CAPACITOR && t <= CIMDRAW_WSYM_JUNCTION;
}

inline constexpr bool cimdrawIsWiringSymbolDrawType(CimdrawDrawTypeId t)
{
    return cimdrawIsPowerWiringSymbolDrawType(t);
}

#define CIMDRAW_PI  3.14159265358979323846

#ifdef WIN32
# ifdef CIMDRAW_DLL_EXPORT
#  define CIMDRAW_DLL_EXPORT Q_DECL_EXPORT
# else
#  define CIMDRAW_DLL_EXPORT Q_DECL_IMPORT
# endif
#else
# define CIMDRAW_DLL_EXPORT
#endif

#endif
