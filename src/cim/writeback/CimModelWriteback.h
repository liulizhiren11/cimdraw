#ifndef CIMMODELWRITEBACK_H
#define CIMMODELWRITEBACK_H

#include "cim/model/CimReference.h"

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

class CimModel;

enum class CimModelWritebackStatus
{
    Applied = 0,
    MissingObject,
    InvalidPatch,
    Rejected,
};

struct CimModelObjectPatch
{
    QString mrid;
    bool updateName = false;
    QString name;
    QHash<QString, QVariant> attributesToSet;
    QStringList attributesToRemove;
    QVector<CimReference> referencesToAdd;
    QVector<CimReference> referencesToRemove;
};

struct CimModelWritebackItemResult
{
    QString mrid;
    CimModelWritebackStatus status = CimModelWritebackStatus::Rejected;
    int changedFieldCount = 0;
    QString message;

    bool applied() const { return status == CimModelWritebackStatus::Applied; }
};

struct CimModelWritebackResult
{
    QVector<CimModelWritebackItemResult> items;
    int appliedCount = 0;
    int rejectedCount = 0;

    bool ok() const { return rejectedCount == 0; }
    QStringList messages() const;
};

CimModelWritebackResult applyCimModelObjectPatches(
    CimModel& model,
    const QVector<CimModelObjectPatch>& patches);

#endif // CIMMODELWRITEBACK_H
