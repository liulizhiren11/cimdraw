#ifndef CIMTOPOLOGYMAPPER_H
#define CIMTOPOLOGYMAPPER_H

#include "topology/PowerTopologyTypes.h"

class CimModel;

class CimTopologyMapper
{
public:
    PowerTopologyAnalysisSnapshot build(const CimModel& model) const;
};

#endif // CIMTOPOLOGYMAPPER_H
