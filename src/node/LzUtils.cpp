#include "LzUtils.h"

inline PortIndex getNodeId(PORT_TYPE portType, ConnectionId connectionId)
{
    NodeId id = InvalidNodeId;

    if (portType == PORT_TYPE::TYPE_OUT) {
        id = connectionId.outNodeId;
    } else if (portType == PORT_TYPE::TYPE_IN) {
        id = connectionId.inNodeId;
    }

    return id;
}

inline PortIndex getPortIndex(PORT_TYPE portType, ConnectionId connectionId)
{
    NodeId index = InvalidPortIndex;

    if (portType == PORT_TYPE::TYPE_OUT) {
        index = connectionId.outPortIndex;
    } else if (portType == PORT_TYPE::TYPE_IN) {
        index = connectionId.inPortIndex;
    }

    return index;
}

inline PORT_TYPE oppositePort(PORT_TYPE port)
{
    PORT_TYPE result = PORT_TYPE::TYPE_IN;

    switch (port) {
    case PORT_TYPE::TYPE_IN:
        result = PORT_TYPE::TYPE_OUT;
        break;

    case PORT_TYPE::TYPE_OUT:
        result = PORT_TYPE::TYPE_IN;
        break;

    case PORT_TYPE::TYPE_NONE:
        result = PORT_TYPE::TYPE_NONE;
        break;

    default:
        break;
    }
    return result;
}

inline bool isPortIndexValid(PortIndex index)
{
    return index!=InvalidPortIndex;
}

inline ConnectionId makeIncompleteConnectionId(NodeId const connectedNodeId, PORT_TYPE const connectedPort, PortIndex const connectedPortIndex)
{
    return (connectedPort == PORT_TYPE::TYPE_IN)
    ? ConnectionId{InvalidNodeId, InvalidPortIndex, connectedNodeId, connectedPortIndex}
    : ConnectionId{connectedNodeId, connectedPortIndex, InvalidNodeId, InvalidPortIndex};
}

inline ConnectionId makeIncompleteConnectionId(ConnectionId connectionId, PORT_TYPE const portToDisconnect)
{
    if (portToDisconnect == PORT_TYPE::TYPE_IN) {
        connectionId.outNodeId = InvalidNodeId;
        connectionId.outPortIndex = InvalidPortIndex;
    } else {
        connectionId.inNodeId = InvalidNodeId;
        connectionId.inPortIndex = InvalidPortIndex;
    }

    return connectionId;
}

inline ConnectionId makeCompleteConnectionId(ConnectionId incompleteConnectionId, NodeId const nodeId, PortIndex const portIndex)
{
    if (incompleteConnectionId.outNodeId == InvalidNodeId) {
        incompleteConnectionId.outNodeId = nodeId;
        incompleteConnectionId.outPortIndex = portIndex;
    } else {
        incompleteConnectionId.inNodeId = nodeId;
        incompleteConnectionId.inPortIndex = portIndex;
    }

    return incompleteConnectionId;
}