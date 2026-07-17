#include "CimSemanticObjectView.h"

CimSemanticObjectView::CimSemanticObjectView(const CimObject* object)
    : object_(object)
{
}

bool CimSemanticObjectView::isValid() const
{
    return object_ != nullptr && object_->isValid();
}

const CimObject* CimSemanticObjectView::object() const
{
    return object_;
}

QString CimSemanticObjectView::mrid() const
{
    return isValid() ? object_->mrid : QString();
}

QString CimSemanticObjectView::className() const
{
    return isValid() ? object_->className : QString();
}

QString CimSemanticObjectView::name() const
{
    return isValid() ? object_->name : QString();
}

QString CimSemanticObjectView::sourceFile() const
{
    return isValid() ? object_->sourceFile : QString();
}

QVariant CimSemanticObjectView::attribute(const QString& key) const
{
    return isValid() ? object_->attribute(key) : QVariant();
}

QString CimSemanticObjectView::referenceTarget(const QString& relationName) const
{
    if (!isValid() || relationName.trimmed().isEmpty())
        return QString();

    for (const CimReference& reference : object_->references)
    {
        if (reference.relationName == relationName)
            return reference.targetMrid;
    }

    return QString();
}

bool CimSemanticObjectView::isA(const QString& ancestorClassName) const
{
    return isValid() && CimClassLineage::isA(object_->className, ancestorClassName);
}

bool CimSemanticObjectView::boolAttribute(const QString& key, bool defaultValue) const
{
    const QVariant value = attribute(key);
    if (!value.isValid())
        return defaultValue;

    if (value.metaType().id() == QMetaType::Bool)
        return value.toBool();

    const QString normalized = value.toString().trimmed().toLower();
    if (normalized == QStringLiteral("true") || normalized == QStringLiteral("1"))
        return true;
    if (normalized == QStringLiteral("false") || normalized == QStringLiteral("0"))
        return false;
    return defaultValue;
}

int CimSemanticObjectView::intAttribute(const QString& key, int defaultValue) const
{
    bool ok = false;
    const int value = attribute(key).toInt(&ok);
    return ok ? value : defaultValue;
}

double CimSemanticObjectView::doubleAttribute(const QString& key, double defaultValue) const
{
    bool ok = false;
    const double value = attribute(key).toDouble(&ok);
    return ok ? value : defaultValue;
}
