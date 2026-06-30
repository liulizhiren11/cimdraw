#ifndef LZPOWERMOTORITEM_H
#define LZPOWERMOTORITEM_H

#include "LzWiringItemBase.h"

class LzPowerMotorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerMotorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerMotorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(60, 60); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    PowerTopologyRole powerTopologyRole() const override { return PowerTopologyRole::GenericEquipment; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERMOTORITEM_H
