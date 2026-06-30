#include "CimGraphicMapper.h"

#include <QtGlobal>

namespace {

bool cimNameContainsAny(const CimObject& object, std::initializer_list<const char*> tokens);

int powerTransformerEndCount(const CimObject& object)
{
    int count = 0;
    for (const CimReference& reference : object.references)
    {
        if (reference.relationName == QStringLiteral("PowerTransformer.PowerTransformerEnd"))
            ++count;
    }
    return count;
}

bool powerTransformerLooksLikeAutoXfmr(const CimObject& object)
{
    return cimNameContainsAny(object,
                              {"autoxfmr",
                               "auto xfmr",
                               "autotransformer",
                               "auto transformer",
                               "self coupled",
                               "self-coupled",
                               "自耦"});
}

bool powerTransformerLooksLikeEarthingXfmr(const CimObject& object)
{
    return cimNameContainsAny(object,
                              {"earthing transformer",
                               "grounding transformer",
                               "grounded transformer",
                               "zigzag transformer",
                               "zig-zag transformer",
                               "neutral grounding transformer",
                               "earthing xfmr",
                               "grounding xfmr",
                               "zigzag",
                               "zig-zag",
                               "接地变",
                               "接地变压器"});
}

bool powerTransformerLooksLikeStationXfmr(const CimObject& object)
{
    return cimNameContainsAny(object,
                              {"stationxfmr",
                               "station xfmr",
                               "station transformer",
                               "service transformer",
                               "aux transformer",
                               "auxiliary transformer",
                               "station service",
                               "aux service",
                               "站用变",
                               "厂用变",
                               "所用变"});
}

bool cimNameContainsAny(const CimObject& object, std::initializer_list<const char*> tokens)
{
    QString haystack = object.className + QLatin1Char(' ') + object.name;
    for (auto it = object.attributeMap.constBegin(); it != object.attributeMap.constEnd(); ++it)
        haystack += QLatin1Char(' ') + it.value().toString();
    const QString folded = haystack.toLower();

    for (const char* token : tokens)
    {
        if (folded.contains(QString::fromUtf8(token).toLower()))
            return true;
    }
    return false;
}

double firstNumericAttribute(const CimObject& object,
                             std::initializer_list<const char*> keys,
                             bool* okOut = nullptr)
{
    for (const char* key : keys)
    {
        bool ok = false;
        const double value = object.attribute(QString::fromLatin1(key)).toDouble(&ok);
        if (ok)
        {
            if (okOut)
                *okOut = true;
            return value;
        }
    }

    if (okOut)
        *okOut = false;
    return 0.0;
}

int inferredSectionCount(const CimObject& object)
{
    bool ok = false;
    const double maximumSections = firstNumericAttribute(object,
                                                         {"ShuntCompensator.maximumSections",
                                                          "LinearShuntCompensator.maximumSections",
                                                          "maximumSections"},
                                                         &ok);
    if (ok)
        return qRound(maximumSections);

    const double normalSections = firstNumericAttribute(object,
                                                        {"ShuntCompensator.normalSections",
                                                         "LinearShuntCompensator.normalSections",
                                                         "normalSections"},
                                                        &ok);
    if (ok)
        return qRound(normalSections);

    const double sections = firstNumericAttribute(object,
                                                  {"SvShuntCompensatorSections.sections",
                                                   "sections"},
                                                  &ok);
    if (ok)
        return qRound(sections);

    return 0;
}

bool shuntCompensatorLooksLikeSvg(const CimObject& object)
{
    if (object.className == QStringLiteral("StaticVarCompensator"))
        return true;

    return cimNameContainsAny(object, {"svg", "svc", "statcom", "staticvar"});
}

bool shuntCompensatorLooksLikeArcCoil(const CimObject& object)
{
    if (object.className == QStringLiteral("PetersenCoil"))
        return true;

    return cimNameContainsAny(object, {"petersen", "arccoil", "arc coil", "消弧", "消弧线圈"});
}

bool shuntCompensatorLooksLikeReactor(const CimObject& object)
{
    if (cimNameContainsAny(object, {"reactor", "shunt reactor", "电抗", "电抗器"}))
        return true;

    bool ok = false;
    const double bPerSection = firstNumericAttribute(object,
                                                     {"ShuntCompensator.bPerSection",
                                                      "LinearShuntCompensator.bPerSection",
                                                      "bPerSection"},
                                                     &ok);
    return ok && bPerSection < 0.0;
}

bool shuntCompensatorLooksLikeSplitReactor(const CimObject& object)
{
    if (!shuntCompensatorLooksLikeReactor(object))
        return false;

    if (cimNameContainsAny(object, {"splitreactor", "split reactor", "分裂"}))
        return true;

    return inferredSectionCount(object) >= 2;
}

bool seriesCompensatorLooksLikeCapacitor(const CimObject& object)
{
    if (cimNameContainsAny(object, {"series capacitor", "series cap", "串补", "串联电容"}))
        return true;

    bool ok = false;
    const double x = firstNumericAttribute(object,
                                           {"SeriesCompensator.x",
                                            "x"},
                                           &ok);
    return ok && x < 0.0;
}

bool seriesCompensatorLooksLikeReactor(const CimObject& object)
{
    if (cimNameContainsAny(object, {"series reactor", "current limiting reactor", "串联电抗", "限流电抗"}))
        return true;

    bool ok = false;
    const double x = firstNumericAttribute(object,
                                           {"SeriesCompensator.x",
                                            "x"},
                                           &ok);
    return ok && x > 0.0;
}

LzDrawTypeId drawTypeForSeriesCompensator(const CimObject& object)
{
    if (seriesCompensatorLooksLikeCapacitor(object))
        return LZ_WSYM_CAPACITOR;
    if (seriesCompensatorLooksLikeReactor(object))
        return LZ_WSYM_REACTOR;
    return LZ_WSYM_REACTOR;
}

LzDrawTypeId drawTypeForShuntCompensator(const CimObject& object)
{
    if (shuntCompensatorLooksLikeSvg(object))
        return LZ_WSYM_SVG_COMP;
    if (shuntCompensatorLooksLikeArcCoil(object))
        return LZ_WSYM_ARC_COIL;
    if (shuntCompensatorLooksLikeSplitReactor(object))
        return LZ_WSYM_SPLIT_REACTOR;
    if (shuntCompensatorLooksLikeReactor(object))
        return LZ_WSYM_REACTOR;
    return LZ_WSYM_CAPACITOR;
}

}

LzDrawTypeId CimGraphicMapper::drawTypeForObject(const CimObject& object) const
{
    if (object.className == QStringLiteral("Breaker"))
        return LZ_SLD_CIRCUIT_BREAKER;
    if (object.className == QStringLiteral("Disconnector"))
        return LZ_SLD_DISCONNECTOR;
    if (object.className == QStringLiteral("BusbarSection"))
        return LZ_SLD_BUSBAR_SECTION;
    if (object.className == QStringLiteral("PowerTransformer"))
    {
        if (powerTransformerLooksLikeAutoXfmr(object))
            return LZ_WSYM_AUTO_XFMR;
        if (powerTransformerLooksLikeEarthingXfmr(object))
            return LZ_WSYM_EARTHING_XFMR;
        if (powerTransformerLooksLikeStationXfmr(object))
            return LZ_WSYM_STATION_XFMR;
        return powerTransformerEndCount(object) >= 3 ? LZ_WSYM_TRANSFORMER_3W : LZ_SLD_TRANSFORMER_2W;
    }
    if (object.className == QStringLiteral("EnergyConsumer"))
        return LZ_SLD_LOAD;
    if (object.className == QStringLiteral("SynchronousMachine"))
        return LZ_SLD_GENERATOR;
    if (object.className == QStringLiteral("SolarGeneratingUnit"))
        return LZ_WSYM_PV_INFEED;
    if (object.className == QStringLiteral("WindGeneratingUnit"))
        return LZ_WSYM_WIND_INFEED;
    if (object.className == QStringLiteral("BatteryUnit"))
        return LZ_WSYM_ESS;
    if (object.className == QStringLiteral("ACLineSegment"))
        return LZ_WSYM_CABLE;
    if (object.className == QStringLiteral("LoadBreakSwitch"))
        return LZ_WSYM_LOAD_BREAK;
    if (object.className == QStringLiteral("GroundDisconnector"))
        return LZ_WSYM_EARTH_SWITCH;
    if (object.className == QStringLiteral("LinearShuntCompensator")
        || object.className == QStringLiteral("ShuntCompensator")
        || object.className == QStringLiteral("NonlinearShuntCompensator")
        || object.className == QStringLiteral("StaticVarCompensator")
        || object.className == QStringLiteral("PetersenCoil"))
        return drawTypeForShuntCompensator(object);
    if (object.className == QStringLiteral("SeriesCompensator"))
        return drawTypeForSeriesCompensator(object);
    if (object.className == QStringLiteral("AsynchronousMachine"))
        return LZ_SLD_MOTOR;
    if (object.className == QStringLiteral("ExternalNetworkInjection"))
        return LZ_WSYM_GRID;
    if (object.className == QStringLiteral("EquivalentInjection"))
        return LZ_WSYM_GRID;
    if (object.className == QStringLiteral("SurgeArrester"))
        return LZ_WSYM_ARRESTER;
    if (object.className == QStringLiteral("Ground"))
        return LZ_SLD_GROUND;
    if (object.className == QStringLiteral("CurrentTransformer"))
        return LZ_WSYM_CURRENT_TRANSFORMER;
    if (object.className == QStringLiteral("PotentialTransformer")
        || object.className == QStringLiteral("VoltageTransformer"))
        return LZ_WSYM_VOLTAGE_TRANSFORMER;
    if (object.className == QStringLiteral("Fuse"))
        return LZ_WSYM_FUSE;
    if (object.className == QStringLiteral("Junction")
        || object.className == QStringLiteral("Connector"))
        return LZ_WSYM_JUNCTION;
    return -1;
}

bool CimGraphicMapper::canMap(const CimObject& object) const
{
    return drawTypeForObject(object) >= 0;
}
