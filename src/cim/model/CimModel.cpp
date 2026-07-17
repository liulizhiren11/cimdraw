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

bool CimModel::replaceObject(const CimObject& object)
{
    if (!object.isValid())
        return false;
    auto it = objectsByMrid_.find(object.mrid);
    if (it == objectsByMrid_.end())
        return false;
    if (it.value().className != object.className)
        return false;
    it.value() = object;
    return true;
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

QVector<CimObject> CimModel::objectsBySourceFile(const QString& sourceFile) const
{
    QVector<CimObject> result;
    for (auto it = objectsByMrid_.constBegin(); it != objectsByMrid_.constEnd(); ++it)
    {
        if (it.value().sourceFile == sourceFile)
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

QHash<QString, int> CimModel::classDistribution() const
{
    QHash<QString, int> result;
    for (auto it = objectsByMrid_.constBegin(); it != objectsByMrid_.constEnd(); ++it)
        result[it.value().className] += 1;
    return result;
}

QHash<QString, int> CimModel::sourceFileDistribution() const
{
    QHash<QString, int> result;
    for (auto it = objectsByMrid_.constBegin(); it != objectsByMrid_.constEnd(); ++it)
    {
        const QString key = it.value().sourceFile.trimmed().isEmpty()
                                ? QStringLiteral("<unknown>")
                                : it.value().sourceFile;
        result[key] += 1;
    }
    return result;
}

int CimModel::referenceCount() const
{
    int total = 0;
    for (auto it = objectsByMrid_.constBegin(); it != objectsByMrid_.constEnd(); ++it)
        total += it.value().references.size();
    return total;
}
