#include "CimIdentifiedObjectView.h"

CimIdentifiedObjectView::CimIdentifiedObjectView(const CimObject* object)
    : CimSemanticObjectView(object)
{
}

QString CimIdentifiedObjectView::description() const
{
    return attribute(QStringLiteral("IdentifiedObject.description")).toString();
}

QString CimIdentifiedObjectView::aliasName() const
{
    return attribute(QStringLiteral("IdentifiedObject.aliasName")).toString();
}

QString CimIdentifiedObjectView::shortName() const
{
    return attribute(QStringLiteral("IdentifiedObject.shortName")).toString();
}
