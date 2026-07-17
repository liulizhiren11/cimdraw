#include "CimTerminalView.h"

CimTerminalView::CimTerminalView(const CimObject* object)
    : CimIdentifiedObjectView(object)
{
}

bool CimTerminalView::isTerminal() const
{
    return isA(QStringLiteral("Terminal"));
}

QString CimTerminalView::conductingEquipmentMrid() const
{
    return referenceTarget(QStringLiteral("Terminal.ConductingEquipment"));
}

QString CimTerminalView::topologicalNodeMrid() const
{
    return referenceTarget(QStringLiteral("Terminal.TopologicalNode"));
}

QString CimTerminalView::connectivityNodeMrid() const
{
    return referenceTarget(QStringLiteral("Terminal.ConnectivityNode"));
}

QString CimTerminalView::transformerEndMrid() const
{
    return referenceTarget(QStringLiteral("Terminal.TransformerEnd"));
}

int CimTerminalView::sequenceNumber() const
{
    return intAttribute(QStringLiteral("ACDCTerminal.sequenceNumber"));
}
