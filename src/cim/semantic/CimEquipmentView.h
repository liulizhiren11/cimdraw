#ifndef CIMEQUIPMENTVIEW_H
#define CIMEQUIPMENTVIEW_H

#include "CimIdentifiedObjectView.h"

class CimEquipmentView : public CimIdentifiedObjectView
{
public:
    explicit CimEquipmentView(const CimObject* object = nullptr);

    bool isEquipment() const;
    QString equipmentContainerMrid() const;
};

#endif // CIMEQUIPMENTVIEW_H
