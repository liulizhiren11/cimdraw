#include "CimGraphicMapper.h"

#include "cim/semantic/CimPowerTransformerView.h"
#include "cim/semantic/CimSeriesCompensatorView.h"
#include "cim/semantic/CimShuntCompensatorView.h"

#include <QtGlobal>

namespace {

CimdrawDrawTypeId drawTypeForSeriesCompensator(const CimSeriesCompensatorView& view)
{
    if (view.looksLikeCapacitor())
        return CIMDRAW_WSYM_CAPACITOR;
    if (view.looksLikeReactor())
        return CIMDRAW_WSYM_REACTOR;
    return CIMDRAW_WSYM_REACTOR;
}

CimdrawDrawTypeId drawTypeForShuntCompensator(const CimShuntCompensatorView& view)
{
    if (view.looksLikeSvg())
        return CIMDRAW_WSYM_SVG_COMP;
    if (view.looksLikeArcCoil())
        return CIMDRAW_WSYM_ARC_COIL;
    if (view.looksLikeSplitReactor())
        return CIMDRAW_WSYM_SPLIT_REACTOR;
    if (view.looksLikeReactor())
        return CIMDRAW_WSYM_REACTOR;
    return CIMDRAW_WSYM_CAPACITOR;
}

CimdrawDrawTypeId mappedDrawTypeForObject(const CimObject& object)
{
    if (object.className == QStringLiteral("Breaker"))
        return CIMDRAW_SLD_CIRCUIT_BREAKER;
    if (object.className == QStringLiteral("Disconnector"))
        return CIMDRAW_SLD_DISCONNECTOR;
    if (object.className == QStringLiteral("BusbarSection"))
        return CIMDRAW_SLD_BUSBAR_SECTION;
    if (object.className == QStringLiteral("PowerTransformer"))
    {
        const CimPowerTransformerView transformerView(&object);
        if (transformerView.looksLikeAutoTransformer())
            return CIMDRAW_WSYM_AUTO_XFMR;
        if (transformerView.looksLikeEarthingTransformer())
            return CIMDRAW_WSYM_EARTHING_XFMR;
        if (transformerView.looksLikeStationTransformer())
            return CIMDRAW_WSYM_STATION_XFMR;
        return transformerView.transformerEndCount() >= 3 ? CIMDRAW_WSYM_TRANSFORMER_3W
                                                          : CIMDRAW_SLD_TRANSFORMER_2W;
    }
    if (object.className == QStringLiteral("EnergyConsumer"))
        return CIMDRAW_SLD_LOAD;
    if (object.className == QStringLiteral("SynchronousMachine"))
        return CIMDRAW_SLD_GENERATOR;
    if (object.className == QStringLiteral("SolarGeneratingUnit"))
        return CIMDRAW_WSYM_PV_INFEED;
    if (object.className == QStringLiteral("WindGeneratingUnit"))
        return CIMDRAW_WSYM_WIND_INFEED;
    if (object.className == QStringLiteral("BatteryUnit"))
        return CIMDRAW_WSYM_ESS;
    if (object.className == QStringLiteral("ACLineSegment"))
        return CIMDRAW_WSYM_CABLE;
    if (object.className == QStringLiteral("LoadBreakSwitch"))
        return CIMDRAW_WSYM_LOAD_BREAK;
    if (object.className == QStringLiteral("GroundDisconnector"))
        return CIMDRAW_WSYM_EARTH_SWITCH;
    if (object.className == QStringLiteral("LinearShuntCompensator")
        || object.className == QStringLiteral("ShuntCompensator")
        || object.className == QStringLiteral("NonlinearShuntCompensator")
        || object.className == QStringLiteral("StaticVarCompensator")
        || object.className == QStringLiteral("PetersenCoil"))
        return drawTypeForShuntCompensator(CimShuntCompensatorView(&object));
    if (object.className == QStringLiteral("SeriesCompensator"))
        return drawTypeForSeriesCompensator(CimSeriesCompensatorView(&object));
    if (object.className == QStringLiteral("AsynchronousMachine"))
        return CIMDRAW_SLD_MOTOR;
    if (object.className == QStringLiteral("ExternalNetworkInjection"))
        return CIMDRAW_WSYM_GRID;
    if (object.className == QStringLiteral("EquivalentInjection"))
        return CIMDRAW_WSYM_GRID;
    if (object.className == QStringLiteral("SurgeArrester"))
        return CIMDRAW_WSYM_ARRESTER;
    if (object.className == QStringLiteral("Ground"))
        return CIMDRAW_SLD_GROUND;
    if (object.className == QStringLiteral("CurrentTransformer"))
        return CIMDRAW_WSYM_CURRENT_TRANSFORMER;
    if (object.className == QStringLiteral("PotentialTransformer")
        || object.className == QStringLiteral("VoltageTransformer"))
        return CIMDRAW_WSYM_VOLTAGE_TRANSFORMER;
    if (object.className == QStringLiteral("Fuse"))
        return CIMDRAW_WSYM_FUSE;
    if (object.className == QStringLiteral("Junction")
        || object.className == QStringLiteral("Connector"))
        return CIMDRAW_WSYM_JUNCTION;
    return -1;
}

bool cimSwitchClass(const QString& className)
{
    return className == QStringLiteral("Breaker")
        || className == QStringLiteral("Disconnector")
        || className == QStringLiteral("LoadBreakSwitch")
        || className == QStringLiteral("GroundDisconnector");
}

bool cimSourceClass(const QString& className)
{
    return className == QStringLiteral("SynchronousMachine")
        || className == QStringLiteral("SolarGeneratingUnit")
        || className == QStringLiteral("WindGeneratingUnit")
        || className == QStringLiteral("BatteryUnit")
        || className == QStringLiteral("ExternalNetworkInjection")
        || className == QStringLiteral("EquivalentInjection");
}

bool cimMeasurementClass(const QString& className)
{
    return className == QStringLiteral("CurrentTransformer")
        || className == QStringLiteral("PotentialTransformer")
        || className == QStringLiteral("VoltageTransformer");
}

bool cimShuntCompensationClass(const QString& className)
{
    return className == QStringLiteral("LinearShuntCompensator")
        || className == QStringLiteral("ShuntCompensator")
        || className == QStringLiteral("NonlinearShuntCompensator")
        || className == QStringLiteral("StaticVarCompensator")
        || className == QStringLiteral("PetersenCoil");
}

bool cimCapacitorLikeShuntClass(const QString& className)
{
    return className == QStringLiteral("LinearShuntCompensator")
        || className == QStringLiteral("ShuntCompensator")
        || className == QStringLiteral("NonlinearShuntCompensator");
}

CimGraphicMappingRole mappingRoleForObject(const CimObject& object)
{
    if (object.className == QStringLiteral("BusbarSection"))
        return CimGraphicMappingRole::Busbar;
    if (cimSwitchClass(object.className))
        return CimGraphicMappingRole::SwitchingDevice;
    if (object.className == QStringLiteral("PowerTransformer"))
        return CimGraphicMappingRole::Transformer;
    if (cimSourceClass(object.className))
        return CimGraphicMappingRole::Source;
    if (object.className == QStringLiteral("EnergyConsumer"))
        return CimGraphicMappingRole::Load;
    if (object.className == QStringLiteral("AsynchronousMachine"))
        return CimGraphicMappingRole::Motor;
    if (cimMeasurementClass(object.className))
        return CimGraphicMappingRole::Measurement;
    if (object.className == QStringLiteral("Fuse"))
        return CimGraphicMappingRole::Fuse;
    if (object.className == QStringLiteral("Junction")
        || object.className == QStringLiteral("Connector"))
        return CimGraphicMappingRole::Junction;
    if (object.className == QStringLiteral("ACLineSegment"))
        return CimGraphicMappingRole::Conductor;
    if (cimShuntCompensationClass(object.className))
        return CimGraphicMappingRole::ShuntCompensator;
    if (object.className == QStringLiteral("SeriesCompensator"))
        return CimGraphicMappingRole::SeriesCompensator;
    if (object.className == QStringLiteral("SurgeArrester"))
        return CimGraphicMappingRole::SurgeArrester;
    if (object.className == QStringLiteral("Ground"))
        return CimGraphicMappingRole::Ground;
    return CimGraphicMappingRole::Other;
}

}

CimGraphicMappingResult CimGraphicMapper::mapObject(const CimObject& object) const
{
    CimGraphicMappingResult result;
    result.mrid = object.mrid;
    result.className = object.className;
    result.drawType = mappedDrawTypeForObject(object);
    if (result.drawType < 0)
        return result;

    result.role = mappingRoleForObject(object);
    result.requiresDedicatedCapacitorItemFactory =
        result.drawType == CIMDRAW_WSYM_CAPACITOR
        && (cimCapacitorLikeShuntClass(object.className)
            || object.className == QStringLiteral("SeriesCompensator"));
    return result;
}

CimdrawDrawTypeId CimGraphicMapper::drawTypeForObject(const CimObject& object) const
{
    return mapObject(object).drawType;
}

bool CimGraphicMapper::canMap(const CimObject& object) const
{
    return mapObject(object).isMappable();
}
