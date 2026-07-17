#ifndef CIMDRAWPOWERJUNCTIONITEM_H
#define CIMDRAWPOWERJUNCTIONITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerJunctionItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerJunctionItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerJunctionItem(const QRectF& pos, QGraphicsItem* parent = nullptr);

    static QSizeF defaultSize() { return QSizeF(28, 28); }
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

#endif // CIMDRAWPOWERJUNCTIONITEM_H
