#ifndef LZCIRCUITBREAKERITEM_H
#define LZCIRCUITBREAKERITEM_H

#include "LzWiringItemBase.h"

class LzPowerCircuitBreakerItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerCircuitBreakerItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerCircuitBreakerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(64, 92); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
                           bool breakerOpen);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool supportsSwitchToggleOnDoubleClick() const override { return true; }

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const override;
    QPainterPath defaultRoutingObstaclePath() const override;
};

#endif // LZCIRCUITBREAKERITEM_H
