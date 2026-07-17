#ifndef CIMDRAWPOWEREARTHINGXFMRITEM_H
#define CIMDRAWPOWEREARTHINGXFMRITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerEarthingXfmrItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerEarthingXfmrItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerEarthingXfmrItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(92, 72); }
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

#endif // CIMDRAWPOWEREARTHINGXFMRITEM_H
