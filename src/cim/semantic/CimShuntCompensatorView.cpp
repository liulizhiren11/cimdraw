#include "CimShuntCompensatorView.h"

#include <QStringList>

#include <initializer_list>
#include <QtGlobal>

namespace {

QString searchableText(const CimShuntCompensatorView& view)
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

bool containsAnyToken(const CimShuntCompensatorView& view, std::initializer_list<const char*> tokens)
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

CimShuntCompensatorView::CimShuntCompensatorView(const CimObject* object)
    : CimEquipmentView(object)
{
}

bool CimShuntCompensatorView::isShuntCompensatorFamily() const
{
    return className() == QStringLiteral("LinearShuntCompensator")
        || className() == QStringLiteral("ShuntCompensator")
        || className() == QStringLiteral("NonlinearShuntCompensator")
        || className() == QStringLiteral("StaticVarCompensator")
        || className() == QStringLiteral("PetersenCoil");
}

bool CimShuntCompensatorView::looksLikeSvg() const
{
    if (className() == QStringLiteral("StaticVarCompensator"))
        return true;

    return containsAnyToken(*this, {"svg", "svc", "statcom", "staticvar"});
}

bool CimShuntCompensatorView::looksLikeArcCoil() const
{
    if (className() == QStringLiteral("PetersenCoil"))
        return true;

    return containsAnyToken(*this, {"petersen", "arccoil", "arc coil", "消弧", "消弧线圈"});
}

bool CimShuntCompensatorView::looksLikeReactor() const
{
    if (containsAnyToken(*this, {"reactor", "shunt reactor", "电抗", "电抗器"}))
        return true;

    bool ok = false;
    const double bPerSection = firstNumericAttribute({"ShuntCompensator.bPerSection",
                                                      "LinearShuntCompensator.bPerSection",
                                                      "bPerSection"},
                                                     &ok);
    return ok && bPerSection < 0.0;
}

bool CimShuntCompensatorView::looksLikeSplitReactor() const
{
    if (!looksLikeReactor())
        return false;

    if (containsAnyToken(*this, {"splitreactor", "split reactor", "分裂"}))
        return true;

    return inferredSectionCount() >= 2;
}

double CimShuntCompensatorView::firstNumericAttribute(std::initializer_list<const char*> keys,
                                                      bool* okOut) const
{
    for (const char* key : keys)
    {
        bool ok = false;
        const double value = attribute(QString::fromLatin1(key)).toDouble(&ok);
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

int CimShuntCompensatorView::inferredSectionCount() const
{
    bool ok = false;
    const double maximumSections = firstNumericAttribute({"ShuntCompensator.maximumSections",
                                                          "LinearShuntCompensator.maximumSections",
                                                          "maximumSections"},
                                                         &ok);
    if (ok)
        return qRound(maximumSections);

    const double normalSections = firstNumericAttribute({"ShuntCompensator.normalSections",
                                                         "LinearShuntCompensator.normalSections",
                                                         "normalSections"},
                                                        &ok);
    if (ok)
        return qRound(normalSections);

    const double sections = firstNumericAttribute({"SvShuntCompensatorSections.sections", "sections"},
                                                  &ok);
    if (ok)
        return qRound(sections);

    return 0;
}
