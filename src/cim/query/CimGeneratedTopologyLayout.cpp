#include "cim/query/CimGeneratedTopologyLayout.h"

QPointF cimGeneratedTopologyNodeLabelPosition(const QPointF& nodePos,
                                              const CimGeneratedTopologySourceSummary& summary)
{
    return nodePos + summary.labelOffset;
}

QPointF cimGeneratedTopologyRelationLabelPosition(const QPointF& startScenePos,
                                                  const QPointF& endScenePos,
                                                  const CimGeneratedTopologyRelationSummary& summary)
{
    return (startScenePos + endScenePos) * 0.5 + summary.labelOffset;
}
