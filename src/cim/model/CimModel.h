#ifndef CIMMODEL_H
#define CIMMODEL_H

#include "CimObject.h"

#include <QHash>
#include <QString>
#include <QVector>

class CimModel
{
public:
    bool addObject(const CimObject& object);
    bool contains(const QString& mrid) const;
    int objectCount() const;
    void clear();

    const CimObject* objectByMrid(const QString& mrid) const;
    QVector<CimObject> objectsByClassName(const QString& className) const;
    QVector<CimObject> allObjects() const;

private:
    QHash<QString, CimObject> objectsByMrid_;
};

#endif // CIMMODEL_H
