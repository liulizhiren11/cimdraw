#ifndef LZDISCONNECTORITEM_H
#define LZDISCONNECTORITEM_H

#include "LzWiringItemBase.h"

class LzPowerDisconnectorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerDisconnectorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerDisconnectorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(168, 38); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
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

#endif // LZDISCONNECTORITEM_H
