#ifndef LZPOWERREACTORITEM_H
#define LZPOWERREACTORITEM_H

#include "LzWiringItemBase.h"

class LzPowerReactorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerReactorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerReactorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 40); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERREACTORITEM_H
