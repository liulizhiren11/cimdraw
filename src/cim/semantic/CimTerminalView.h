#ifndef CIMTERMINALVIEW_H
#define CIMTERMINALVIEW_H

#include "CimIdentifiedObjectView.h"

class CimTerminalView : public CimIdentifiedObjectView
{
public:
    explicit CimTerminalView(const CimObject* object = nullptr);

    bool isTerminal() const;
    QString conductingEquipmentMrid() const;
    QString topologicalNodeMrid() const;
    QString connectivityNodeMrid() const;
    QString transformerEndMrid() const;
    int sequenceNumber() const;
};

#endif // CIMTERMINALVIEW_H
