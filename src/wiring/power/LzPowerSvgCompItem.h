#ifndef LZPOWERSVGCOMPITEM_H
#define LZPOWERSVGCOMPITEM_H

#include "LzWiringItemBase.h"

class LzPowerSvgCompItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerSvgCompItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerSvgCompItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(64, 50); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERSVGCOMPITEM_H
