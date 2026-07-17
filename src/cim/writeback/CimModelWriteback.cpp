#include "CimModelWriteback.h"

#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"

#include <QSet>

namespace {

bool sameReference(const CimReference& lhs, const CimReference& rhs)
{
    return lhs.relationName == rhs.relationName
        && lhs.targetMrid == rhs.targetMrid;
}

bool containsReference(const QVector<CimReference>& references, const CimReference& reference)
{
    for (const CimReference& existing : references)
    {
        if (sameReference(existing, reference))
            return true;
    }
    return false;
}

CimModelWritebackItemResult rejectedResult(const QString& mrid,
                                           CimModelWritebackStatus status,
                                           const QString& message)
{
    CimModelWritebackItemResult result;
    result.mrid = mrid;
    result.status = status;
    result.message = message;
    return result;
}

void addItemResult(CimModelWritebackResult& result, const CimModelWritebackItemResult& item)
{
    result.items.push_back(item);
    if (item.applied())
        ++result.appliedCount;
    else
        ++result.rejectedCount;
}

QString statusLabel(CimModelWritebackStatus status)
{
    switch (status)
    {
    case CimModelWritebackStatus::Applied:
        return QStringLiteral("已应用");
    case CimModelWritebackStatus::MissingObject:
        return QStringLiteral("对象不存在");
    case CimModelWritebackStatus::InvalidPatch:
        return QStringLiteral("补丁无效");
    case CimModelWritebackStatus::Rejected:
    default:
        return QStringLiteral("已拒绝");
    }
}

} // namespace

QStringList CimModelWritebackResult::messages() const
{
    QStringList lines;
    for (const CimModelWritebackItemResult& item : items)
    {
        lines << QStringLiteral("%1[%2]: %3")
                     .arg(statusLabel(item.status), item.mrid, item.message);
    }
    return lines;
}

CimModelWritebackResult applyCimModelObjectPatches(
    CimModel& model,
    const QVector<CimModelObjectPatch>& patches)
{
    CimModelWritebackResult result;
    for (const CimModelObjectPatch& patch : patches)
    {
        const QString mrid = patch.mrid.trimmed();
        if (mrid.isEmpty())
        {
            addItemResult(result,
                          rejectedResult({},
                                         CimModelWritebackStatus::InvalidPatch,
                                         QStringLiteral("回写补丁缺少 mRID。")));
            continue;
        }

        const CimObject* existingObject = model.objectByMrid(mrid);
        if (!existingObject)
        {
            addItemResult(result,
                          rejectedResult(mrid,
                                         CimModelWritebackStatus::MissingObject,
                                         QStringLiteral("只能回写已存在的关键对象。")));
            continue;
        }

        CimObject updated = *existingObject;
        int changedFields = 0;

        if (patch.updateName && updated.name != patch.name)
        {
            updated.name = patch.name;
            ++changedFields;
        }

        QStringList removeKeys = patch.attributesToRemove;
        removeKeys.removeDuplicates();
        for (const QString& key : removeKeys)
        {
            const QString normalizedKey = key.trimmed();
            if (normalizedKey.isEmpty())
                continue;
            if (updated.attributeMap.remove(normalizedKey))
                ++changedFields;
        }

        for (auto it = patch.attributesToSet.constBegin(); it != patch.attributesToSet.constEnd(); ++it)
        {
            const QString key = it.key().trimmed();
            if (key.isEmpty())
                continue;
            if (!updated.attributeMap.contains(key) || updated.attributeMap.value(key) != it.value())
            {
                updated.attributeMap.insert(key, it.value());
                ++changedFields;
            }
        }

        for (const CimReference& reference : patch.referencesToRemove)
        {
            QVector<CimReference> kept;
            kept.reserve(updated.references.size());
            bool removed = false;
            for (const CimReference& existing : updated.references)
            {
                if (sameReference(existing, reference))
                {
                    removed = true;
                    continue;
                }
                kept.push_back(existing);
            }
            if (removed)
            {
                updated.references = kept;
                ++changedFields;
            }
        }

        for (const CimReference& reference : patch.referencesToAdd)
        {
            if (reference.relationName.trimmed().isEmpty() || reference.targetMrid.trimmed().isEmpty())
                continue;
            if (!containsReference(updated.references, reference))
            {
                updated.references.push_back(reference);
                ++changedFields;
            }
        }

        CimModelWritebackItemResult item;
        item.mrid = mrid;
        item.status = CimModelWritebackStatus::Applied;
        item.changedFieldCount = changedFields;
        item.message = QStringLiteral("已回写 %1 个字段。").arg(changedFields);
        if (!model.replaceObject(updated))
        {
            item.status = CimModelWritebackStatus::Rejected;
            item.message = QStringLiteral("模型拒绝替换对象，可能存在 className 或 mRID 漂移。");
        }
        addItemResult(result, item);
    }
    return result;
}
