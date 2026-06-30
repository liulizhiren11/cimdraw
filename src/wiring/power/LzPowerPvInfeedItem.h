#ifndef LZPOWERPVINFEEDITEM_H
#define LZPOWERPVINFEEDITEM_H

#include "LzWiringItemBase.h"

class LzPowerPvInfeedItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerPvInfeedItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerPvInfeedItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(168, 36); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    bool flowSignAffectsConnectPoints() const override { return true; }
    bool supportsFlowDirectionAction() const override { return true; }
    bool wiringUsesVisualTimer() const override { return true; }
};

#endif // LZPOWERPVINFEEDITEM_H
