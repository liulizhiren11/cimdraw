#ifndef LZGENERATORITEM_H
#define LZGENERATORITEM_H

#include "LzWiringItemBase.h"

class LzPowerGeneratorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerGeneratorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerGeneratorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(60, 60); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZGENERATORITEM_H
