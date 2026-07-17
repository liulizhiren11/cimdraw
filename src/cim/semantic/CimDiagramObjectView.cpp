#include "CimDiagramObjectView.h"

CimDiagramObjectView::CimDiagramObjectView(const CimObject* object)
    : CimSemanticObjectView(object)
{
}

bool CimDiagramObjectView::isDiagramObject() const
{
    return isA(QStringLiteral("DiagramObject"));
}

QString CimDiagramObjectView::identifiedObjectMrid() const
{
    return referenceTarget(QStringLiteral("DiagramObject.IdentifiedObject"));
}

bool CimDiagramObjectView::hasRotation() const
{
    bool ok = false;
    attribute(QStringLiteral("DiagramObject.rotation")).toDouble(&ok);
    return ok;
}

double CimDiagramObjectView::rotation() const
{
    return doubleAttribute(QStringLiteral("DiagramObject.rotation"));
}
