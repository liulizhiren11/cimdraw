#ifndef LZPOWERFUSEITEM_H
#define LZPOWERFUSEITEM_H

#include "LzWiringItemBase.h"

class LzPowerFuseItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerFuseItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerFuseItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(84, 32); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERFUSEITEM_H
