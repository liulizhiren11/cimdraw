#ifndef CIMDRAWPOWERGRIDITEM_H
#define CIMDRAWPOWERGRIDITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerGridItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerGridItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerGridItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
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

#endif // CIMDRAWPOWERGRIDITEM_H
