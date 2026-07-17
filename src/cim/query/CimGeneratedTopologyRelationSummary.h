#ifndef CIMGENERATEDTOPOLOGYRELATIONSUMMARY_H
#define CIMGENERATEDTOPOLOGYRELATIONSUMMARY_H

#include "topology/TopologyTypes.h"

#include <QPointF>
#include <QString>

struct CimGeneratedTopologyRelationSummary
{
    QString edgeStableId;
    QString startNodeId;
    QString endNodeId;
    QString startPortKey;
    QString endPortKey;
    QString relationLabel;
    QPointF labelOffset = QPointF(-16.0, -18.0);
    TopologyRelationType relationType = TopologyRelationType::Physical;
    int arrowHeadStyle = 1;

    bool isValid() const
    {
        return !edgeStableId.isEmpty() && !startNodeId.isEmpty() && !endNodeId.isEmpty();
    }
};

#endif
