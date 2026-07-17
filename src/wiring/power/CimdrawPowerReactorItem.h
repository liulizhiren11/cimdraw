#ifndef CIMDRAWPOWERREACTORITEM_H
#define CIMDRAWPOWERREACTORITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerReactorItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerReactorItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerReactorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 40); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERREACTORITEM_H
