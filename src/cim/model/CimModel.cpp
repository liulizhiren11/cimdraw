#include "CimModel.h"

#include <algorithm>

bool CimModel::addObject(const CimObject& object)
{
    if (!object.isValid() || objectsByMrid_.contains(object.mrid))
        return false;
    objectsByMrid_.insert(object.mrid, object);
    return true;
}

bool CimModel::contains(const QString& mrid) const
{
    return objectsByMrid_.contains(mrid);
}

int CimModel::objectCount() const
{
    return objectsByMrid_.size();
}

void CimModel::clear()
{
    objectsByMrid_.clear();
}

const CimObject* CimModel::objectByMrid(const QString& mrid) const
{
    auto it = objectsByMrid_.constFind(mrid);
    if (it == objectsByMrid_.constEnd())
        return nullptr;
    return &it.value();
}

QVector<CimObject> CimModel::objectsByClassName(const QString& className) const
{
    QVector<CimObject> result;
    for (auto it = objectsByMrid_.constBegin(); it != objectsByMrid_.constEnd(); ++it)
    {
        if (it.value().className == className)
            result.push_back(it.value());
    }
    std::sort(result.begin(), result.end(), [](const CimObject& lhs, const CimObject& rhs) {
        return lhs.mrid < rhs.mrid;
    });
    return result;
}

QVector<CimObject> CimModel::allObjects() const
{
    QVector<CimObject> result = objectsByMrid_.values().toVector();
    std::sort(result.begin(), result.end(), [](const CimObject& lhs, const CimObject& rhs) {
        return lhs.mrid < rhs.mrid;
    });
    return result;
}
