#ifndef CIMDRAWPOWERLOADBREAKITEM_H
#define CIMDRAWPOWERLOADBREAKITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerLoadBreakItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerLoadBreakItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerLoadBreakItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(76, 42); }
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

protected:
    QPainterPath defaultRoutingObstaclePath() const override;
};

#endif // CIMDRAWPOWERLOADBREAKITEM_H
