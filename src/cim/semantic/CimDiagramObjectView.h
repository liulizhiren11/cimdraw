#ifndef CIMDIAGRAMOBJECTVIEW_H
#define CIMDIAGRAMOBJECTVIEW_H

#include "CimSemanticObjectView.h"

class CimDiagramObjectView : public CimSemanticObjectView
{
public:
    explicit CimDiagramObjectView(const CimObject* object = nullptr);

    bool isDiagramObject() const;
    QString identifiedObjectMrid() const;
    bool hasRotation() const;
    double rotation() const;
};

#endif // CIMDIAGRAMOBJECTVIEW_H
