#ifndef CIMDRAWPOWERSVGCOMPITEM_H
#define CIMDRAWPOWERSVGCOMPITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerSvgCompItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerSvgCompItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerSvgCompItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(64, 50); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERSVGCOMPITEM_H
