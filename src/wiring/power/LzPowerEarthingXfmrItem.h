#ifndef LZPOWEREARTHINGXFMRITEM_H
#define LZPOWEREARTHINGXFMRITEM_H

#include "LzWiringItemBase.h"

class LzPowerEarthingXfmrItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerEarthingXfmrItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerEarthingXfmrItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(92, 72); }
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

#endif // LZPOWEREARTHINGXFMRITEM_H
