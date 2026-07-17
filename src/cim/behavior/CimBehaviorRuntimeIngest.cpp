#include "cim/behavior/CimBehaviorRuntimeIngest.h"

#include "CimdrawScene.h"
#include "cim/behavior/CimBehaviorResultVariant.h"

namespace {

CimBehaviorResult behaviorResultForRunState(CimBehaviorRunState runState)
{
    CimBehaviorResult result;
    if (runState == CimBehaviorRunState::Unknown)
        return result;

    result.valid = true;
    result.runState = runState;
    result.available = true;
    result.energized = (runState == CimBehaviorRunState::Energized);
    result.terminalConnectivity = CimBehaviorTerminalConnectivity::Unknown;
    return result;
}

CimBehaviorRunState runStateFromLegacyInt(int value)
{
    switch (value)
    {
    case 0:
        return CimBehaviorRunState::Normal;
    case 1:
        return CimBehaviorRunState::Energized;
    case 2:
        return CimBehaviorRunState::Deenergized;
    case 3:
        return CimBehaviorRunState::Alarm;
    default:
        return CimBehaviorRunState::Unknown;
    }
}

CimBehaviorRunState runStateFromLegacyString(const QString& value)
{
    const QString normalized = value.trimmed().toLower();
    if (normalized == QLatin1String("0"))
        return CimBehaviorRunState::Normal;
    if (normalized == QLatin1String("1"))
        return CimBehaviorRunState::Energized;
    if (normalized == QLatin1String("2"))
        return CimBehaviorRunState::Deenergized;
    if (normalized == QLatin1String("3"))
        return CimBehaviorRunState::Alarm;
    if (normalized == QLatin1String("normal") || normalized == QStringLiteral("正常"))
        return CimBehaviorRunState::Normal;
    if (normalized == QLatin1String("energized") || normalized == QLatin1String("on")
        || normalized == QStringLiteral("带电"))
        return CimBehaviorRunState::Energized;
    if (normalized == QLatin1String("off") || normalized == QStringLiteral("停运")
        || normalized == QStringLiteral("分闸"))
        return CimBehaviorRunState::Deenergized;
    if (normalized == QLatin1String("alarm") || normalized == QStringLiteral("告警"))
        return CimBehaviorRunState::Alarm;
    return CimBehaviorRunState::Unknown;
}

CimBehaviorResult behaviorResultFromLegacyRuntimeValue(const QVariant& value)
{
    const int typeId = value.metaType().id();
    if (typeId == QMetaType::Int || typeId == QMetaType::UInt
        || typeId == QMetaType::LongLong || typeId == QMetaType::ULongLong)
        return behaviorResultForRunState(runStateFromLegacyInt(value.toInt()));
    if (typeId == QMetaType::Double)
    {
        const double numericValue = value.toDouble();
        const int intValue = static_cast<int>(numericValue);
        if (numericValue == static_cast<double>(intValue))
            return behaviorResultForRunState(runStateFromLegacyInt(intValue));
        return {};
    }
    return behaviorResultForRunState(runStateFromLegacyString(value.toString()));
}

} // namespace

CimBehaviorRuntimeIngestResult cimApplyWiringRuntimeValue(CimdrawScene* scene,
                                                          const QString& key,
                                                          const QVariant& value)
{
    CimBehaviorRuntimeIngestResult result;
    if (!scene || key.trimmed().isEmpty())
        return result;

    const QVariantMap behaviorMap = value.toMap();
    if (!behaviorMap.isEmpty() && isCimBehaviorResultVariantMap(behaviorMap))
    {
        scene->setWiringBehaviorResult(key, cimBehaviorResultFromVariantMap(behaviorMap));
        result.kind = CimBehaviorRuntimeIngestKind::BehaviorResultPayload;
        return result;
    }

    const CimBehaviorResult legacyBehaviorResult = behaviorResultFromLegacyRuntimeValue(value);
    if (legacyBehaviorResult.valid)
    {
        scene->setWiringBehaviorResult(key, legacyBehaviorResult);
        result.kind = CimBehaviorRuntimeIngestKind::BehaviorResultPayload;
        return result;
    }

    scene->setWiringData(key, value);
    result.kind = CimBehaviorRuntimeIngestKind::RawRuntimeValue;
    return result;
}
