#include "cim/behavior/CimBehaviorResultVariant.h"

namespace {

QString terminalConnectivityKey(CimBehaviorTerminalConnectivity connectivity)
{
    switch (connectivity)
    {
    case CimBehaviorTerminalConnectivity::Disconnected:
        return QStringLiteral("disconnected");
    case CimBehaviorTerminalConnectivity::Partial:
        return QStringLiteral("partial");
    case CimBehaviorTerminalConnectivity::Connected:
        return QStringLiteral("connected");
    case CimBehaviorTerminalConnectivity::Unknown:
    default:
        return QStringLiteral("unknown");
    }
}

CimBehaviorTerminalConnectivity terminalConnectivityFromKey(const QString& key)
{
    const QString normalized = key.trimmed().toLower();
    if (normalized == QStringLiteral("disconnected"))
        return CimBehaviorTerminalConnectivity::Disconnected;
    if (normalized == QStringLiteral("partial"))
        return CimBehaviorTerminalConnectivity::Partial;
    if (normalized == QStringLiteral("connected"))
        return CimBehaviorTerminalConnectivity::Connected;
    return CimBehaviorTerminalConnectivity::Unknown;
}

QString runStateKey(CimBehaviorRunState runState)
{
    switch (runState)
    {
    case CimBehaviorRunState::Normal:
        return QStringLiteral("normal");
    case CimBehaviorRunState::Energized:
        return QStringLiteral("energized");
    case CimBehaviorRunState::Deenergized:
        return QStringLiteral("deenergized");
    case CimBehaviorRunState::Alarm:
        return QStringLiteral("alarm");
    case CimBehaviorRunState::Unknown:
    default:
        return QStringLiteral("unknown");
    }
}

CimBehaviorRunState runStateFromKey(const QString& key)
{
    const QString normalized = key.trimmed().toLower();
    if (normalized == QStringLiteral("normal"))
        return CimBehaviorRunState::Normal;
    if (normalized == QStringLiteral("energized"))
        return CimBehaviorRunState::Energized;
    if (normalized == QStringLiteral("deenergized"))
        return CimBehaviorRunState::Deenergized;
    if (normalized == QStringLiteral("alarm"))
        return CimBehaviorRunState::Alarm;
    return CimBehaviorRunState::Unknown;
}

} // namespace

QVariantMap cimBehaviorResultToVariantMap(const CimBehaviorResult& result)
{
    return {
        {QStringLiteral("objectId"), result.objectId},
        {QStringLiteral("valid"), result.valid},
        {QStringLiteral("runState"), runStateKey(result.runState)},
        {QStringLiteral("available"), result.available},
        {QStringLiteral("conductivity"), result.conductivity},
        {QStringLiteral("canOperate"), result.canOperate},
        {QStringLiteral("energized"), result.energized},
        {QStringLiteral("terminalConnectivity"), terminalConnectivityKey(result.terminalConnectivity)},
    };
}

CimBehaviorResult cimBehaviorResultFromVariantMap(const QVariantMap& map)
{
    CimBehaviorResult result;
    result.objectId = map.value(QStringLiteral("objectId")).toString();
    result.valid = map.value(QStringLiteral("valid")).toBool();
    result.runState = runStateFromKey(map.value(QStringLiteral("runState")).toString());
    result.available = map.value(QStringLiteral("available")).toBool();
    result.conductivity = map.value(QStringLiteral("conductivity")).toBool();
    result.canOperate = map.value(QStringLiteral("canOperate")).toBool();
    result.energized = map.value(QStringLiteral("energized")).toBool();
    result.terminalConnectivity =
        terminalConnectivityFromKey(map.value(QStringLiteral("terminalConnectivity")).toString());
    return result;
}

bool isCimBehaviorResultVariantMap(const QVariantMap& map)
{
    return map.contains(QStringLiteral("valid"))
        || map.contains(QStringLiteral("runState"))
        || map.contains(QStringLiteral("available"))
        || map.contains(QStringLiteral("conductivity"))
        || map.contains(QStringLiteral("canOperate"))
        || map.contains(QStringLiteral("energized"))
        || map.contains(QStringLiteral("terminalConnectivity"));
}
