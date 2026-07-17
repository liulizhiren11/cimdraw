#ifndef CIMDRAWPOWERWINDINFEEDITEM_H
#define CIMDRAWPOWERWINDINFEEDITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerWindInfeedItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerWindInfeedItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerWindInfeedItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
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

#endif // CIMDRAWPOWERWINDINFEEDITEM_H
