#ifndef CIMDRAWPOWEREARTHSWITCHITEM_H
#define CIMDRAWPOWEREARTHSWITCHITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerEarthSwitchItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerEarthSwitchItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerEarthSwitchItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 64); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           CimdrawWiringRunState runState,
                           bool alarmPulse,
                           CimdrawWiringSymbolStandard symStd,
                           bool switchClosed);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool supportsSwitchToggleOnDoubleClick() const override { return true; }
};

#endif // CIMDRAWPOWEREARTHSWITCHITEM_H
