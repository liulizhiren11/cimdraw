#ifndef LZPOWERVOLTAGETRANSFORMERITEM_H
#define LZPOWERVOLTAGETRANSFORMERITEM_H

#include "LzWiringItemBase.h"

class LzPowerVoltageTransformerItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerVoltageTransformerItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerVoltageTransformerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(78, 52); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERVOLTAGETRANSFORMERITEM_H
