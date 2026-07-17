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
    bool replaceObject(const CimObject& object);
    QVector<CimObject> objectsByClassName(const QString& className) const;
    QVector<CimObject> objectsBySourceFile(const QString& sourceFile) const;
    QVector<CimObject> allObjects() const;
    QHash<QString, int> classDistribution() const;
    QHash<QString, int> sourceFileDistribution() const;
    int referenceCount() const;

private:
    QHash<QString, CimObject> objectsByMrid_;
};

#endif // CIMMODEL_H
