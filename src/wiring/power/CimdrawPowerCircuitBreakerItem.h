#ifndef CIMDRAWCIRCUITBREAKERITEM_H
#define CIMDRAWCIRCUITBREAKERITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerCircuitBreakerItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerCircuitBreakerItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerCircuitBreakerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(64, 92); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           CimdrawWiringRunState runState,
                           bool alarmPulse,
                           bool breakerOpen);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool supportsSwitchToggleOnDoubleClick() const override { return true; }

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const override;
    QPainterPath defaultRoutingObstaclePath() const override;
};

#endif // CIMDRAWCIRCUITBREAKERITEM_H
