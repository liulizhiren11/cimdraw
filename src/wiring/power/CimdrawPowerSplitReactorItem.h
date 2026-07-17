#ifndef CIMDRAWPOWERSPLITREACTORITEM_H
#define CIMDRAWPOWERSPLITREACTORITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerSplitReactorItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerSplitReactorItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerSplitReactorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(120, 44); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERSPLITREACTORITEM_H
