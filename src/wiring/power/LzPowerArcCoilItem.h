#ifndef LZPOWERARCCOILITEM_H
#define LZPOWERARCCOILITEM_H

#include "LzWiringItemBase.h"

class LzPowerArcCoilItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerArcCoilItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerArcCoilItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(92, 44); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERARCCOILITEM_H
