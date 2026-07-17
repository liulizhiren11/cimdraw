#ifndef CIMCONNECTIVITYNODEVIEW_H
#define CIMCONNECTIVITYNODEVIEW_H

#include "CimIdentifiedObjectView.h"

class CimConnectivityNodeView : public CimIdentifiedObjectView
{
public:
    explicit CimConnectivityNodeView(const CimObject* object = nullptr);

    bool isConnectivityNode() const;
    QString topologicalNodeMrid() const;
};

#endif // CIMCONNECTIVITYNODEVIEW_H
