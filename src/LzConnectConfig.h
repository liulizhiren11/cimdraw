#ifndef LZCONNECTCONFIG_H
#define LZCONNECTCONFIG_H

#include <limits>
#include <utility>

#include <Qt>

//连线模式
enum CONNECTION_STRATEGY : int
{
    //直连
    DIRECT_CONNECTION = 0,
    //简单折线
    BROKEN_CONNECTION,
    //吸附连线
    ADSORPTION_CONNECTION,
    //自动连线
    AUTOMATIC_CONNECTION,
    //规避连线
    OBSTACLE_AVOIDANCE_CONNECTION,
};

/** 产品默认：正交折线（与 draw.io Orthogonal 一致） */
inline CONNECTION_STRATEGY lzDefaultConnectLineStrategy()
{
    return BROKEN_CONNECTION;
}

/** 中间折点编辑是否约束为正交（仅 DIRECT 为自由斜向） */
inline bool lzConnectStrategyUsesOrthogonalEdit(CONNECTION_STRATEGY strategy)
{
    return strategy != DIRECT_CONNECTION;
}

/** 图元拖动结束后倾向全量避障重算（非 lite L 形） */
inline bool lzConnectStrategyPrefersFullRecompute(CONNECTION_STRATEGY strategy)
{
    return strategy == AUTOMATIC_CONNECTION || strategy == OBSTACLE_AVOIDANCE_CONNECTION;
}

//连接点方向
enum CONNECT_DIRECTION : int
{
    NONE_DIRECTION = -1,
    TOP_DIRECTION = 0,
    RIGHT_DIRECTION,
    BOTTOM_DIRECTION,
    LEFT_DIRECTION,
};

//连接线出入
enum PORT_TYPE : int
{
    TYPE_NONE = 0,
    TYPE_IN = 1,
    TYPE_OUT = 2,
};

using ConnectCount = unsigned int;

using PortCount = unsigned int;

using PortIndex = unsigned int;

static constexpr PortIndex InvalidPortIndex = std::numeric_limits<PortIndex>::max();

using NodeId = unsigned int;

static constexpr NodeId InvalidNodeId = std::numeric_limits<NodeId>::max();

/** 连接线默认 z：低于符号图元(≥0)，避免导线压住端口 */
static constexpr double LzConnectLineLayerZ = -10000.0;
/** 选中编辑时置于符号之上，便于拖点且不被选中框完全挡住 */
static constexpr double LzConnectLineHighlightLayerZ = 1.0;

/** draw.io 风格：场景坐标下吸附到连接点圆心的最大距离（像素） */
static constexpr double LzConnectPointSnapRadiusPx = 20.0;

/** 选择工具：按住 Ctrl 从图元本体/母线边找最近端口拉线；直接点中端口无需 Ctrl */
inline bool lzWiringGestureActive(Qt::KeyboardModifiers mods)
{
    return (mods & Qt::ControlModifier) != 0;
}

/** 母线：光标距上下沿导体的最大吸附距离（像素），便于松手/拖线接到母线 */
static constexpr double LzBusbarSnapRadiusPx = 48.0;

/** 正交路由时端口沿法向引出到图元外的距离，避免导线被符号本体遮挡 */
static constexpr double LzWireRoutingLeadOutPx = 18.0;

/** 连线在视觉上与图元端口保持的最小留白，避免导线紧贴符号边缘 */
static constexpr double LzWireEndpointVisualGapPx = 14.0;

/** 路径路由模式：自动规划 vs 用户拖拽后优先保留折点 */
enum class ConnectorPathRoutingMode : int
{
    Auto = 0,
    Manual = 1,
};

struct ConnectionId
{
    NodeId outNodeId;
    PortIndex outPortIndex;
    NodeId inNodeId;
    PortIndex inPortIndex;
};

inline bool operator==(ConnectionId const &a, ConnectionId const &b)
{
    return a.outNodeId == b.outNodeId && a.outPortIndex == b.outPortIndex
           && a.inNodeId == b.inNodeId && a.inPortIndex == b.inPortIndex;
}

inline bool operator!=(ConnectionId const &a, ConnectionId const &b)
{
    return !(a == b);
}

inline void invertConnection(ConnectionId &id)
{
    std::swap(id.outNodeId, id.inNodeId);
    std::swap(id.outPortIndex, id.inPortIndex);
}

#endif
