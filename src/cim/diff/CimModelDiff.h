#ifndef CIMMODELDIFF_H
#define CIMMODELDIFF_H

#include <QString>
#include <QStringList>
#include <QVector>

class CimModel;

enum class CimModelDiffChangeKind
{
    AddedObject = 0,
    RemovedObject,
    ClassChanged,
    NameChanged,
    AttributeAdded,
    AttributeRemoved,
    AttributeChanged,
    ReferenceAdded,
    ReferenceRemoved,
};

struct CimModelDiffChange
{
    CimModelDiffChangeKind kind = CimModelDiffChangeKind::AddedObject;
    QString mrid;
    QString className;
    QString fieldName;
    QString oldValue;
    QString newValue;
};

struct CimModelDiffSummary
{
    int addedObjectCount = 0;
    int removedObjectCount = 0;
    int changedObjectCount = 0;
    int changeCount = 0;

    bool empty() const { return changeCount == 0; }
};

struct CimModelDiffReport
{
    CimModelDiffSummary summary;
    QVector<CimModelDiffChange> changes;

    bool empty() const { return summary.empty(); }
    QStringList changedMrids() const;
    QStringList reportLines() const;
};

CimModelDiffReport diffCimModels(const CimModel& beforeModel, const CimModel& afterModel);

#endif // CIMMODELDIFF_H
