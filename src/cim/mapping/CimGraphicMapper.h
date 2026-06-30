#ifndef CIMGRAPHICMAPPER_H
#define CIMGRAPHICMAPPER_H

#include "cim/model/CimObject.h"

#include "LzItemConfig.h"

class CimGraphicMapper
{
public:
    LzDrawTypeId drawTypeForObject(const CimObject& object) const;
    bool canMap(const CimObject& object) const;
};

#endif // CIMGRAPHICMAPPER_H
