#ifndef LZPOWERCABLEITEM_H
#define LZPOWERCABLEITEM_H

#include "LzWiringItemBase.h"

class LzPowerCableItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerCableItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerCableItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(128, 32); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
                           LzWiringSymbolStandard symStd);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERCABLEITEM_H
