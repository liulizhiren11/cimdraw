#include "CimModelDiff.h"

#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"

#include <QSet>

namespace {

QString variantToDiffString(const QVariant& value)
{
    if (!value.isValid())
        return {};
    if (value.typeId() == QMetaType::Bool)
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    return value.toString();
}

QString referenceKey(const CimReference& reference)
{
    return reference.relationName + QStringLiteral(" -> ") + reference.targetMrid;
}

QStringList sortedReferenceKeys(const QVector<CimReference>& references)
{
    QStringList result;
    result.reserve(references.size());
    for (const CimReference& reference : references)
        result << referenceKey(reference);
    result.removeDuplicates();
    result.sort();
    return result;
}

QString changeKindLabel(CimModelDiffChangeKind kind)
{
    switch (kind)
    {
    case CimModelDiffChangeKind::AddedObject:
        return QStringLiteral("新增对象");
    case CimModelDiffChangeKind::RemovedObject:
        return QStringLiteral("删除对象");
    case CimModelDiffChangeKind::ClassChanged:
        return QStringLiteral("类名变化");
    case CimModelDiffChangeKind::NameChanged:
        return QStringLiteral("名称变化");
    case CimModelDiffChangeKind::AttributeAdded:
        return QStringLiteral("新增属性");
    case CimModelDiffChangeKind::AttributeRemoved:
        return QStringLiteral("删除属性");
    case CimModelDiffChangeKind::AttributeChanged:
        return QStringLiteral("属性变化");
    case CimModelDiffChangeKind::ReferenceAdded:
        return QStringLiteral("新增引用");
    case CimModelDiffChangeKind::ReferenceRemoved:
        return QStringLiteral("删除引用");
    default:
        return QStringLiteral("未知变化");
    }
}

void appendChange(CimModelDiffReport& report,
                  CimModelDiffChangeKind kind,
                  const QString& mrid,
                  const QString& className,
                  const QString& fieldName = {},
                  const QString& oldValue = {},
                  const QString& newValue = {})
{
    CimModelDiffChange change;
    change.kind = kind;
    change.mrid = mrid;
    change.className = className;
    change.fieldName = fieldName;
    change.oldValue = oldValue;
    change.newValue = newValue;
    report.changes.push_back(change);
    ++report.summary.changeCount;
}

void diffObjectFields(CimModelDiffReport& report,
                      const CimObject& before,
                      const CimObject& after)
{
    if (before.className != after.className)
    {
        appendChange(report,
                     CimModelDiffChangeKind::ClassChanged,
                     before.mrid,
                     after.className,
                     QStringLiteral("className"),
                     before.className,
                     after.className);
    }

    if (before.name != after.name)
    {
        appendChange(report,
                     CimModelDiffChangeKind::NameChanged,
                     before.mrid,
                     after.className,
                     QStringLiteral("name"),
                     before.name,
                     after.name);
    }

    QStringList attributeKeys = before.attributeMap.keys();
    attributeKeys << after.attributeMap.keys();
    attributeKeys.removeDuplicates();
    attributeKeys.sort();
    for (const QString& key : attributeKeys)
    {
        const bool beforeHasKey = before.attributeMap.contains(key);
        const bool afterHasKey = after.attributeMap.contains(key);
        if (!beforeHasKey && afterHasKey)
        {
            appendChange(report,
                         CimModelDiffChangeKind::AttributeAdded,
                         before.mrid,
                         after.className,
                         key,
                         {},
                         variantToDiffString(after.attributeMap.value(key)));
            continue;
        }
        if (beforeHasKey && !afterHasKey)
        {
            appendChange(report,
                         CimModelDiffChangeKind::AttributeRemoved,
                         before.mrid,
                         after.className,
                         key,
                         variantToDiffString(before.attributeMap.value(key)),
                         {});
            continue;
        }
        if (before.attributeMap.value(key) != after.attributeMap.value(key))
        {
            appendChange(report,
                         CimModelDiffChangeKind::AttributeChanged,
                         before.mrid,
                         after.className,
                         key,
                         variantToDiffString(before.attributeMap.value(key)),
                         variantToDiffString(after.attributeMap.value(key)));
        }
    }

    const QStringList beforeReferenceKeys = sortedReferenceKeys(before.references);
    const QStringList afterReferenceKeys = sortedReferenceKeys(after.references);
    const QSet<QString> beforeRefs(beforeReferenceKeys.begin(), beforeReferenceKeys.end());
    const QSet<QString> afterRefs(afterReferenceKeys.begin(), afterReferenceKeys.end());
    QSet<QString> removedRefSet = beforeRefs;
    removedRefSet.subtract(afterRefs);
    QSet<QString> addedRefSet = afterRefs;
    addedRefSet.subtract(beforeRefs);
    QStringList removedRefs = removedRefSet.values();
    QStringList addedRefs = addedRefSet.values();
    removedRefs.sort();
    addedRefs.sort();

    for (const QString& ref : removedRefs)
    {
        appendChange(report,
                     CimModelDiffChangeKind::ReferenceRemoved,
                     before.mrid,
                     after.className,
                     QStringLiteral("reference"),
                     ref,
                     {});
    }
    for (const QString& ref : addedRefs)
    {
        appendChange(report,
                     CimModelDiffChangeKind::ReferenceAdded,
                     before.mrid,
                     after.className,
                     QStringLiteral("reference"),
                     {},
                     ref);
    }
}

} // namespace

QStringList CimModelDiffReport::changedMrids() const
{
    QStringList result;
    for (const CimModelDiffChange& change : changes)
        result << change.mrid;
    result.removeDuplicates();
    result.sort();
    return result;
}

QStringList CimModelDiffReport::reportLines() const
{
    QStringList lines;
    lines << QStringLiteral("新增对象数：%1").arg(summary.addedObjectCount);
    lines << QStringLiteral("删除对象数：%1").arg(summary.removedObjectCount);
    lines << QStringLiteral("变更对象数：%1").arg(summary.changedObjectCount);
    lines << QStringLiteral("变更条目数：%1").arg(summary.changeCount);
    for (const CimModelDiffChange& change : changes)
    {
        QString line = QStringLiteral("%1[%2]").arg(changeKindLabel(change.kind), change.mrid);
        if (!change.fieldName.isEmpty())
            line += QStringLiteral(" %1").arg(change.fieldName);
        if (!change.oldValue.isEmpty() || !change.newValue.isEmpty())
            line += QStringLiteral(": %1 -> %2").arg(change.oldValue, change.newValue);
        lines << line;
    }
    return lines;
}

CimModelDiffReport diffCimModels(const CimModel& beforeModel, const CimModel& afterModel)
{
    CimModelDiffReport report;

    QStringList beforeMrids;
    for (const CimObject& object : beforeModel.allObjects())
        beforeMrids << object.mrid;
    QStringList afterMrids;
    for (const CimObject& object : afterModel.allObjects())
        afterMrids << object.mrid;

    QSet<QString> beforeSet(beforeMrids.begin(), beforeMrids.end());
    QSet<QString> afterSet(afterMrids.begin(), afterMrids.end());

    QStringList removedMrids = beforeSet.subtract(afterSet).values();
    beforeSet = QSet<QString>(beforeMrids.begin(), beforeMrids.end());
    QStringList addedMrids = afterSet.subtract(beforeSet).values();
    removedMrids.sort();
    addedMrids.sort();

    for (const QString& mrid : addedMrids)
    {
        const CimObject* object = afterModel.objectByMrid(mrid);
        appendChange(report,
                     CimModelDiffChangeKind::AddedObject,
                     mrid,
                     object ? object->className : QString());
        ++report.summary.addedObjectCount;
    }

    for (const QString& mrid : removedMrids)
    {
        const CimObject* object = beforeModel.objectByMrid(mrid);
        appendChange(report,
                     CimModelDiffChangeKind::RemovedObject,
                     mrid,
                     object ? object->className : QString());
        ++report.summary.removedObjectCount;
    }

    QStringList commonMrids;
    for (const QString& mrid : beforeMrids)
    {
        if (afterModel.contains(mrid))
            commonMrids << mrid;
    }
    commonMrids.sort();

    QSet<QString> changedObjects;
    for (const QString& mrid : commonMrids)
    {
        const int beforeChangeCount = report.summary.changeCount;
        const CimObject* before = beforeModel.objectByMrid(mrid);
        const CimObject* after = afterModel.objectByMrid(mrid);
        if (!before || !after)
            continue;
        diffObjectFields(report, *before, *after);
        if (report.summary.changeCount != beforeChangeCount)
            changedObjects.insert(mrid);
    }
    report.summary.changedObjectCount =
        changedObjects.size() + report.summary.addedObjectCount + report.summary.removedObjectCount;
    return report;
}
