#ifndef CIMDRAWPOWERPVINFEEDITEM_H
#define CIMDRAWPOWERPVINFEEDITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerPvInfeedItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerPvInfeedItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerPvInfeedItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(168, 36); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    bool flowSignAffectsConnectPoints() const override { return true; }
    bool supportsFlowDirectionAction() const override { return true; }
    bool wiringUsesVisualTimer() const override { return true; }
};

#endif // CIMDRAWPOWERPVINFEEDITEM_H
