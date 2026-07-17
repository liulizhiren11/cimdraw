#ifndef CIMDRAWPOWERAUTOXFMRITEM_H
#define CIMDRAWPOWERAUTOXFMRITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerAutoXfmrItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerAutoXfmrItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerAutoXfmrItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
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

#endif // CIMDRAWPOWERAUTOXFMRITEM_H
