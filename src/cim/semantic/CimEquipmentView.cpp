#include "CimEquipmentView.h"

CimEquipmentView::CimEquipmentView(const CimObject* object)
    : CimIdentifiedObjectView(object)
{
}

bool CimEquipmentView::isEquipment() const
{
    return isA(QStringLiteral("Equipment"));
}

QString CimEquipmentView::equipmentContainerMrid() const
{
    return referenceTarget(QStringLiteral("Equipment.EquipmentContainer"));
}
