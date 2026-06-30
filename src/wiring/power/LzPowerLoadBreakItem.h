#ifndef LZPOWERLOADBREAKITEM_H
#define LZPOWERLOADBREAKITEM_H

#include "LzWiringItemBase.h"

class LzPowerLoadBreakItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerLoadBreakItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerLoadBreakItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(76, 42); }
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

protected:
    QPainterPath defaultRoutingObstaclePath() const override;
};

#endif // LZPOWERLOADBREAKITEM_H
