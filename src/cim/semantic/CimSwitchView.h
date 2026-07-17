#ifndef CIMSWITCHVIEW_H
#define CIMSWITCHVIEW_H

#include "CimEquipmentView.h"

class CimSwitchView : public CimEquipmentView
{
public:
    explicit CimSwitchView(const CimObject* object = nullptr);

    bool isSwitch() const;
    bool isOpen() const;
    bool normalOpen() const;
};

#endif // CIMSWITCHVIEW_H
