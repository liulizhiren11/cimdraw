#ifndef CIMIDENTIFIEDOBJECTVIEW_H
#define CIMIDENTIFIEDOBJECTVIEW_H

#include "CimSemanticObjectView.h"

class CimIdentifiedObjectView : public CimSemanticObjectView
{
public:
    explicit CimIdentifiedObjectView(const CimObject* object = nullptr);

    QString description() const;
    QString aliasName() const;
    QString shortName() const;
};

#endif // CIMIDENTIFIEDOBJECTVIEW_H
