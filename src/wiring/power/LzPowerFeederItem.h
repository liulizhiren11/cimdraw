#ifndef LZPOWERFEEDERITEM_H
#define LZPOWERFEEDERITEM_H

#include "LzWiringItemBase.h"

class LzPowerFeederItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerFeederItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerFeederItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(240, 32); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    bool supportsFlowDirectionAction() const override { return true; }
    bool wiringUsesVisualTimer() const override { return true; }
};

#endif // LZPOWERFEEDERITEM_H
