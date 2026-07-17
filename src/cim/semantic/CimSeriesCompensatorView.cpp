#include "CimSeriesCompensatorView.h"

#include <QStringList>

#include <initializer_list>

namespace {

QString searchableText(const CimSeriesCompensatorView& view)
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

bool containsAnyToken(const CimSeriesCompensatorView& view, std::initializer_list<const char*> tokens)
{
    const QString folded = searchableText(view);
    for (const char* token : tokens)
    {
        if (folded.contains(QString::fromUtf8(token).toLower()))
            return true;
    }
    return false;
}

bool reactanceValue(const CimSeriesCompensatorView& view, double* valueOut)
{
    const QStringList keys{
        QStringLiteral("SeriesCompensator.x"),
        QStringLiteral("x"),
    };

    for (const QString& key : keys)
    {
        bool ok = false;
        const double value = view.attribute(key).toDouble(&ok);
        if (ok)
        {
            if (valueOut)
                *valueOut = value;
            return true;
        }
    }

    return false;
}

} // namespace

CimSeriesCompensatorView::CimSeriesCompensatorView(const CimObject* object)
    : CimEquipmentView(object)
{
}

bool CimSeriesCompensatorView::isSeriesCompensator() const
{
    return className() == QStringLiteral("SeriesCompensator");
}

bool CimSeriesCompensatorView::looksLikeCapacitor() const
{
    if (containsAnyToken(*this, {"series capacitor", "series cap", "串补", "串联电容"}))
        return true;

    double x = 0.0;
    return reactanceValue(*this, &x) && x < 0.0;
}

bool CimSeriesCompensatorView::looksLikeReactor() const
{
    if (containsAnyToken(*this, {"series reactor", "current limiting reactor", "串联电抗", "限流电抗"}))
        return true;

    double x = 0.0;
    return reactanceValue(*this, &x) && x > 0.0;
}
