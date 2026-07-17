#ifndef CIMGENERATEDTOPOLOGYSOURCESUMMARY_H
#define CIMGENERATEDTOPOLOGYSOURCESUMMARY_H

#include "topology/PowerTopologyTypes.h"
#include "topology/TopologyTypes.h"

#include <QPointF>
#include <QString>

struct CimGeneratedTopologySourceSummary
{
    QString nodeId;
    QString displayName;
    QString displayLabel;
    QPointF labelOffset = QPointF(-42.0, 44.0);
    CimdrawTopologyDomain topologyDomain = CimdrawTopologyDomain::None;
    bool participatesInTopology = false;
    bool isTopologyGraphNode = false;
    PowerTopologyRole powerTopologyRole = PowerTopologyRole::GenericEquipment;
    bool supportsDirectedFlow = false;
    int flowSign = 1;

    bool isValid() const
    {
        return participatesInTopology || isTopologyGraphNode;
    }
};

#endif
