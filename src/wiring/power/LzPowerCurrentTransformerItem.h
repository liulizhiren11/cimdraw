#ifndef LZPOWERCURRENTTRANSFORMERITEM_H
#define LZPOWERCURRENTTRANSFORMERITEM_H

#include "LzWiringItemBase.h"

class LzPowerCurrentTransformerItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerCurrentTransformerItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerCurrentTransformerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(72, 48); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERCURRENTTRANSFORMERITEM_H
