#ifndef TOPOLOGYTYPES_H
#define TOPOLOGYTYPES_H

#include <QString>
#include <QStringList>

enum class CimdrawTopologyDomain : int
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

struct CimdrawTopologyTerminalRef
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

struct TopologyEdgeLookupHint
{
    QString edgeStableId;
    QString startNodeStableId;
    QString endNodeStableId;
    QString startPortKey;
    QString endPortKey;
    TopologyRelationType relationType = TopologyRelationType::Physical;
    bool relationTypeSpecified = false;

    bool hasLookupFields() const
    {
        return !edgeStableId.isEmpty()
            || !startNodeStableId.isEmpty()
            || !endNodeStableId.isEmpty()
            || !startPortKey.isEmpty()
            || !endPortKey.isEmpty()
            || relationTypeSpecified;
    }
};

struct TopologyNodeRelationInfo
{
    QString nodeStableId;
    QStringList incidentEdgeIds;
    QStringList adjacentNodeIds;

    int incidentEdgeCount() const
    {
        return incidentEdgeIds.size();
    }

    int adjacentNodeCount() const
    {
        return adjacentNodeIds.size();
    }
};

#endif // TOPOLOGYTYPES_H
