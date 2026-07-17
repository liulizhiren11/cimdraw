#include "CimdrawAppConfig.h"

#include "wiring/base/CimdrawWiringSymbolStyle.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace CimdrawAppConfig {

QString symbolStandardToString(CimdrawWiringSymbolStandard standard)
{
    switch (standard)
    {
    case CimdrawWiringSymbolStandard::IEC: return QStringLiteral("IEC");
    case CimdrawWiringSymbolStandard::ANSI: return QStringLiteral("ANSI");
    case CimdrawWiringSymbolStandard::GB: return QStringLiteral("GB");
    }
    return QStringLiteral("GB");
}

CimdrawWiringSymbolStandard symbolStandardFromString(const QString& text, bool* ok)
{
    const QString k = text.trimmed().toUpper();
    if (k == QLatin1String("IEC") || k == QLatin1String("EU") || k == QLatin1String("EURO"))
    {
        if (ok)
            *ok = true;
        return CimdrawWiringSymbolStandard::IEC;
    }
    if (k == QLatin1String("ANSI") || k == QLatin1String("US") || k == QLatin1String("IEEE"))
    {
        if (ok)
            *ok = true;
        return CimdrawWiringSymbolStandard::ANSI;
    }
    if (k == QLatin1String("GB") || k == QLatin1String("CN"))
    {
        if (ok)
            *ok = true;
        return CimdrawWiringSymbolStandard::GB;
    }
    if (ok)
        *ok = false;
    return CimdrawWiringSymbolStandard::GB;
}

QString symbolStandardDisplayName(CimdrawWiringSymbolStandard standard)
{
    switch (standard)
    {
    case CimdrawWiringSymbolStandard::IEC: return QStringLiteral("欧标 IEC");
    case CimdrawWiringSymbolStandard::ANSI: return QStringLiteral("美标 ANSI");
    case CimdrawWiringSymbolStandard::GB: return QStringLiteral("国标 GB");
    }
    return QStringLiteral("国标 GB");
}

void applyFromJsonRoot(const QJsonObject& root)
{
    CimdrawWiringSymbolStandard std = CimdrawWiringSymbolStandard::GB;
    const QJsonObject wiring = root.value(QStringLiteral("wiring")).toObject();
    if (!wiring.isEmpty())
    {
        bool ok = false;
        std = symbolStandardFromString(wiring.value(QStringLiteral("symbolStandard")).toString(QStringLiteral("GB")), &ok);
        if (!ok)
            std = CimdrawWiringSymbolStandard::GB;
    }
    CimdrawWiringSymbolStyle::instance().setStandard(std);
}

bool saveWiringSymbolStandard(const QString& absolutePath, CimdrawWiringSymbolStandard standard)
{
    QJsonObject root;
    QFile f(absolutePath);
    if (f.exists() && f.open(QIODevice::ReadOnly))
    {
        QJsonParseError pe{};
        const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &pe);
        f.close();
        if (pe.error == QJsonParseError::NoError && doc.isObject())
            root = doc.object();
    }

    QJsonObject wiring = root.value(QStringLiteral("wiring")).toObject();
    wiring.insert(QStringLiteral("symbolStandard"), symbolStandardToString(standard));
    root.insert(QStringLiteral("wiring"), wiring);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    f.close();
    return true;
}

} // namespace CimdrawAppConfig
