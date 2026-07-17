#ifndef CIMGRAPHICMAPPER_H
#define CIMGRAPHICMAPPER_H

#include "cim/model/CimObject.h"

#include "CimdrawItemConfig.h"

#include <QString>

enum class CimGraphicMappingRole
{
    Unsupported = 0,
    Busbar,
    SwitchingDevice,
    Transformer,
    Source,
    Load,
    Motor,
    Measurement,
    Fuse,
    Junction,
    Conductor,
    ShuntCompensator,
    SeriesCompensator,
    SurgeArrester,
    Ground,
    Other,
};

struct CimGraphicMappingResult
{
    QString mrid;
    QString className;
    CimdrawDrawTypeId drawType = -1;
    CimGraphicMappingRole role = CimGraphicMappingRole::Unsupported;
    bool requiresDedicatedCapacitorItemFactory = false;

    bool isMappable() const
    {
        return drawType >= 0 && role != CimGraphicMappingRole::Unsupported;
    }
};

class CimGraphicMapper
{
public:
    CimGraphicMappingResult mapObject(const CimObject& object) const;
    CimdrawDrawTypeId drawTypeForObject(const CimObject& object) const;
    bool canMap(const CimObject& object) const;
};

#endif // CIMGRAPHICMAPPER_H
