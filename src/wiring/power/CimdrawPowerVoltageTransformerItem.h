#ifndef CIMDRAWPOWERVOLTAGETRANSFORMERITEM_H
#define CIMDRAWPOWERVOLTAGETRANSFORMERITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerVoltageTransformerItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerVoltageTransformerItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerVoltageTransformerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(78, 52); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWPOWERVOLTAGETRANSFORMERITEM_H
