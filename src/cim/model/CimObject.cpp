#include "CimObject.h"

bool CimObject::isValid() const
{
    return !mrid.trimmed().isEmpty() && !className.trimmed().isEmpty();
}

QVariant CimObject::attribute(const QString& key) const
{
    return attributeMap.value(key);
}

void CimObject::setAttribute(const QString& key, const QVariant& value)
{
    if (key.trimmed().isEmpty())
        return;
    attributeMap.insert(key, value);
}

void CimObject::addReference(const QString& relationName, const QString& targetMrid)
{
    if (relationName.trimmed().isEmpty() || targetMrid.trimmed().isEmpty())
        return;
    references.push_back({relationName, targetMrid});
}
