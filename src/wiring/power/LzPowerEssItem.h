#ifndef LZPOWERESSITEM_H
#define LZPOWERESSITEM_H

#include "LzWiringItemBase.h"

class LzPowerEssItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerEssItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerEssItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(72, 52); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZPOWERESSITEM_H
