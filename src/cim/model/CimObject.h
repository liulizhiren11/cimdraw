#ifndef CIMOBJECT_H
#define CIMOBJECT_H

#include "CimReference.h"

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariant>

class CimObject
{
public:
    QString mrid;
    QString className;
    QString name;
    QString sourceFile;
    QString sourceNamespace;
    QStringList profileTags;
    QHash<QString, QVariant> attributeMap;
    QVector<CimReference> references;

    bool isValid() const;
    QVariant attribute(const QString& key) const;
    void setAttribute(const QString& key, const QVariant& value);
    void addReference(const QString& relationName, const QString& targetMrid);
};

#endif // CIMOBJECT_H
