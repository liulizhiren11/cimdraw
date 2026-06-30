#ifndef TOPOLOGYTYPES_H
#define TOPOLOGYTYPES_H

#include <QString>

enum class LzTopologyDomain : int
{
    None = 0,
    PowerSystem = 1,
};

/// 边关系类型（与《拓扑设计方案》对齐）
enum class TopologyRelationType : int
{
    Physical = 0,
    Logical = 1,
    Flow = 2,
    Control = 3,
    Dependency = 4,
    Reference = 5
};

struct TopologyPortRef
{
    QString nodeStableId;
    QString portKey;
};

struct LzTopologyTerminalRef
{
    QString componentId;
    QString terminalId;
};

struct TopologyEdgeMeta
{
    QString edgeStableId;
    TopologyRelationType relationType = TopologyRelationType::Physical;
    QString startNodeStableId;
    QString endNodeStableId;
    QString startPortKey;
    QString endPortKey;
};

#endif // TOPOLOGYTYPES_H
