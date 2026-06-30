#ifndef LZUTILS_H
#define LZUTILS_H

#include "LzConnectConfig.h"

inline PortIndex getNodeId(PORT_TYPE portType, ConnectionId connectionId);

inline PortIndex getPortIndex(PORT_TYPE portType, ConnectionId connectionId);

inline PORT_TYPE oppositePort(PORT_TYPE port);

inline bool isPortIndexValid(PortIndex index);

inline ConnectionId makeIncompleteConnectionId(NodeId const connectedNodeId, PORT_TYPE const connectedPort, PortIndex const connectedPortIndex);

inline ConnectionId makeIncompleteConnectionId(ConnectionId connectionId, PORT_TYPE const portToDisconnect);

inline ConnectionId makeCompleteConnectionId(ConnectionId incompleteConnectionId, NodeId const nodeId, PortIndex const portIndex);
#endif
