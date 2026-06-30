#ifndef LZLOADITEM_H
#define LZLOADITEM_H

#include "LzWiringItemBase.h"

class LzPowerLoadItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerLoadItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerLoadItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 36); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZLOADITEM_H
