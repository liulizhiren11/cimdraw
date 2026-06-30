#ifndef LZPOWERTRANSFORMER3WITEM_H
#define LZPOWERTRANSFORMER3WITEM_H

#include "LzWiringItemBase.h"

class LzPowerTransformer3wItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerTransformer3wItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerTransformer3wItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 72); }
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

#endif // LZPOWERTRANSFORMER3WITEM_H
