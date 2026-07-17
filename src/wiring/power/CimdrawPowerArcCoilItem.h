#ifndef CIMDRAWPOWERARCCOILITEM_H
#define CIMDRAWPOWERARCCOILITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerArcCoilItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerArcCoilItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerArcCoilItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(92, 44); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERARCCOILITEM_H
