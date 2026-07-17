#ifndef CIMDIAGRAMOBJECTPOINTVIEW_H
#define CIMDIAGRAMOBJECTPOINTVIEW_H

#include "CimSemanticObjectView.h"

#include <QPointF>

class CimDiagramObjectPointView : public CimSemanticObjectView
{
public:
    explicit CimDiagramObjectPointView(const CimObject* object = nullptr);

    bool isDiagramObjectPoint() const;
    QString diagramObjectMrid() const;
    bool hasPosition() const;
    QPointF position() const;
    int sequenceNumber(int defaultValue = -1) const;
};

#endif // CIMDIAGRAMOBJECTPOINTVIEW_H
