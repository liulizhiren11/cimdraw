#include "CimDiagramObjectPointView.h"

CimDiagramObjectPointView::CimDiagramObjectPointView(const CimObject* object)
    : CimSemanticObjectView(object)
{
}

bool CimDiagramObjectPointView::isDiagramObjectPoint() const
{
    return isA(QStringLiteral("DiagramObjectPoint"));
}

QString CimDiagramObjectPointView::diagramObjectMrid() const
{
    return referenceTarget(QStringLiteral("DiagramObjectPoint.DiagramObject"));
}

bool CimDiagramObjectPointView::hasPosition() const
{
    bool xOk = false;
    bool yOk = false;
    attribute(QStringLiteral("DiagramObjectPoint.xPosition")).toDouble(&xOk);
    attribute(QStringLiteral("DiagramObjectPoint.yPosition")).toDouble(&yOk);
    return xOk && yOk;
}

QPointF CimDiagramObjectPointView::position() const
{
    return QPointF(doubleAttribute(QStringLiteral("DiagramObjectPoint.xPosition")),
                   doubleAttribute(QStringLiteral("DiagramObjectPoint.yPosition")));
}

int CimDiagramObjectPointView::sequenceNumber(int defaultValue) const
{
    return intAttribute(QStringLiteral("DiagramObjectPoint.sequenceNumber"), defaultValue);
}
