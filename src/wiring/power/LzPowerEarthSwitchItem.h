#ifndef LZPOWEREARTHSWITCHITEM_H
#define LZPOWEREARTHSWITCHITEM_H

#include "LzWiringItemBase.h"

class LzPowerEarthSwitchItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerEarthSwitchItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerEarthSwitchItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 64); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
                           LzWiringSymbolStandard symStd,
                           bool switchClosed);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool supportsSwitchToggleOnDoubleClick() const override { return true; }
};

#endif // LZPOWEREARTHSWITCHITEM_H
