#ifndef LZPOWERJUNCTIONITEM_H
#define LZPOWERJUNCTIONITEM_H

#include "LzWiringItemBase.h"

class LzPowerJunctionItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerJunctionItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerJunctionItem(const QRectF& pos, QGraphicsItem* parent = nullptr);

    static QSizeF defaultSize() { return QSizeF(28, 28); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);

    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const override;
};

#endif // LZPOWERJUNCTIONITEM_H
