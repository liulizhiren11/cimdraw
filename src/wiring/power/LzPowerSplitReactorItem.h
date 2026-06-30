#ifndef LZPOWERSPLITREACTORITEM_H
#define LZPOWERSPLITREACTORITEM_H

#include "LzWiringItemBase.h"

class LzPowerSplitReactorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerSplitReactorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerSplitReactorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(120, 44); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERSPLITREACTORITEM_H
