#ifndef CIMDRAWDISCONNECTORITEM_H
#define CIMDRAWDISCONNECTORITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerDisconnectorItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerDisconnectorItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerDisconnectorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(168, 38); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           CimdrawWiringRunState runState,
                           bool alarmPulse,
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

#endif // CIMDRAWDISCONNECTORITEM_H
