#ifndef CIMSEMANTICOBJECTVIEW_H
#define CIMSEMANTICOBJECTVIEW_H

#include "cim/model/CimClassLineage.h"
#include "cim/model/CimObject.h"

#include <QString>
#include <QVariant>

class CimSemanticObjectView
{
public:
    explicit CimSemanticObjectView(const CimObject* object = nullptr);

    bool isValid() const;
    const CimObject* object() const;
    QString mrid() const;
    QString className() const;
    QString name() const;
    QString sourceFile() const;
    QVariant attribute(const QString& key) const;
    QString referenceTarget(const QString& relationName) const;
    bool isA(const QString& ancestorClassName) const;

protected:
    bool boolAttribute(const QString& key, bool defaultValue = false) const;
    int intAttribute(const QString& key, int defaultValue = 0) const;
    double doubleAttribute(const QString& key, double defaultValue = 0.0) const;

private:
    const CimObject* object_ = nullptr;
};

#endif // CIMSEMANTICOBJECTVIEW_H
