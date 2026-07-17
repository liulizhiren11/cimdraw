#ifndef CIMDRAWPOWERSTATIONXFMRITEM_H
#define CIMDRAWPOWERSTATIONXFMRITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerStationXfmrItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerStationXfmrItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerStationXfmrItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(80, 52); }
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

#endif // CIMDRAWPOWERSTATIONXFMRITEM_H
