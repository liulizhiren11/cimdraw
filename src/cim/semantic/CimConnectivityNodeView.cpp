#include "CimConnectivityNodeView.h"

CimConnectivityNodeView::CimConnectivityNodeView(const CimObject* object)
    : CimIdentifiedObjectView(object)
{
}

bool CimConnectivityNodeView::isConnectivityNode() const
{
    return isA(QStringLiteral("ConnectivityNode"));
}

QString CimConnectivityNodeView::topologicalNodeMrid() const
{
    return referenceTarget(QStringLiteral("ConnectivityNode.TopologicalNode"));
}
