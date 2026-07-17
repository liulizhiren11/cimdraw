#include "CimTransformerEndView.h"

CimTransformerEndView::CimTransformerEndView(const CimObject* object)
    : CimIdentifiedObjectView(object)
{
}

bool CimTransformerEndView::isTransformerEnd() const
{
    return isA(QStringLiteral("TransformerEnd"));
}

QString CimTransformerEndView::powerTransformerMrid() const
{
    return referenceTarget(QStringLiteral("PowerTransformerEnd.PowerTransformer"));
}

QString CimTransformerEndView::terminalMrid() const
{
    return referenceTarget(QStringLiteral("TransformerEnd.Terminal"));
}

int CimTransformerEndView::endNumber() const
{
    return intAttribute(QStringLiteral("TransformerEnd.endNumber"), -1);
}
