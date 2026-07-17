#include "CimSwitchView.h"

CimSwitchView::CimSwitchView(const CimObject* object)
    : CimEquipmentView(object)
{
}

bool CimSwitchView::isSwitch() const
{
    return isA(QStringLiteral("Switch"));
}

bool CimSwitchView::isOpen() const
{
    return boolAttribute(QStringLiteral("Switch.open"));
}

bool CimSwitchView::normalOpen() const
{
    return boolAttribute(QStringLiteral("Switch.normalOpen"));
}
