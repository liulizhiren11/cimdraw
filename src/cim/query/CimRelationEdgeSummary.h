#ifndef CIMRELATIONEDGESUMMARY_H
#define CIMRELATIONEDGESUMMARY_H

#include "topology/TopologyTypes.h"

struct CimRelationEdgeSummary
{
    TopologyEdgeMeta edgeMeta;
    bool relationLink = false;

    bool isValid() const
    {
        return relationLink || !edgeMeta.edgeStableId.isEmpty();
    }
};

#endif
