#ifndef CIMCLASSLINEAGE_H
#define CIMCLASSLINEAGE_H

#include <QHash>
#include <QString>
#include <QStringList>

namespace CimClassLineage {

inline const QHash<QString, QString>& parentMap()
{
    static const QHash<QString, QString> kParents = [] {
        QHash<QString, QString> parents;
        #include "CimClassLineageGenerated.inc"
        return parents;
    }();

    return kParents;
}

inline QStringList lineageFor(const QString& className)
{
    QStringList lineage;
    QString current = className.trimmed();
    while (!current.isEmpty())
    {
        lineage.push_back(current);
        current = parentMap().value(current);
    }
    return lineage;
}

inline bool isA(const QString& className, const QString& ancestorClassName)
{
    if (className.trimmed().isEmpty() || ancestorClassName.trimmed().isEmpty())
        return false;

    return lineageFor(className).contains(ancestorClassName.trimmed());
}

}

#endif // CIMCLASSLINEAGE_H
