#ifndef LZPOWERSTATIONXFMRITEM_H
#define LZPOWERSTATIONXFMRITEM_H

#include "LzWiringItemBase.h"

class LzPowerStationXfmrItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerStationXfmrItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerStationXfmrItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(80, 52); }
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

#endif // LZPOWERSTATIONXFMRITEM_H
