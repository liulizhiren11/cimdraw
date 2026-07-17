#include "CimPowerTransformerView.h"

#include <QStringList>

#include <initializer_list>

namespace {

QString searchableText(const CimPowerTransformerView& view)
{
    if (!view.isValid())
        return QString();

    QStringList fields{
        view.className(),
        view.name(),
        view.description(),
        view.aliasName(),
        view.shortName(),
    };

    if (const CimObject* object = view.object())
    {
        for (auto it = object->attributeMap.constBegin(); it != object->attributeMap.constEnd(); ++it)
            fields.push_back(it.value().toString());
    }

    return fields.join(QLatin1Char(' ')).toLower();
}

bool containsAnyToken(const CimPowerTransformerView& view, std::initializer_list<const char*> tokens)
{
    const QString folded = searchableText(view);
    for (const char* token : tokens)
    {
        if (folded.contains(QString::fromUtf8(token).toLower()))
            return true;
    }
    return false;
}

} // namespace

CimPowerTransformerView::CimPowerTransformerView(const CimObject* object)
    : CimEquipmentView(object)
{
}

bool CimPowerTransformerView::isPowerTransformer() const
{
    return className() == QStringLiteral("PowerTransformer");
}

int CimPowerTransformerView::transformerEndCount() const
{
    if (!isValid())
        return 0;

    int count = 0;
    for (const CimReference& reference : object()->references)
    {
        if (reference.relationName == QStringLiteral("PowerTransformer.PowerTransformerEnd"))
            ++count;
    }
    return count;
}

bool CimPowerTransformerView::looksLikeAutoTransformer() const
{
    return containsAnyToken(*this,
                            {"autoxfmr",
                             "auto xfmr",
                             "autotransformer",
                             "auto transformer",
                             "self coupled",
                             "self-coupled",
                             "自耦"});
}

bool CimPowerTransformerView::looksLikeEarthingTransformer() const
{
    return containsAnyToken(*this,
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

bool CimPowerTransformerView::looksLikeStationTransformer() const
{
    return containsAnyToken(*this,
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
