#ifndef CIMTOPOLOGYOBJECTSUMMARY_H
#define CIMTOPOLOGYOBJECTSUMMARY_H

#include "topology/TopologyTypes.h"

#include <QString>
#include <QStringList>
#include <QVector>

struct CimTopologyObjectSummary
{
    QString nodeId;
    TopologyNodeRelationInfo relationInfo;
    QVector<int> deviceNodeIds;
    QStringList conductorIds;
    QStringList connectedDevices;
    QStringList reachableDevices;

    bool isValid() const
    {
        return !nodeId.isEmpty();
    }
};

#endif
