#ifndef CIMDRAWPOWERTRANSFORMER3WITEM_H
#define CIMDRAWPOWERTRANSFORMER3WITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerTransformer3wItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerTransformer3wItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerTransformer3wItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 72); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const override;
};

#endif // CIMDRAWPOWERTRANSFORMER3WITEM_H
