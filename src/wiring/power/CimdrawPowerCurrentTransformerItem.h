#ifndef CIMDRAWPOWERCURRENTTRANSFORMERITEM_H
#define CIMDRAWPOWERCURRENTTRANSFORMERITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerCurrentTransformerItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerCurrentTransformerItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerCurrentTransformerItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(72, 48); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWPOWERCURRENTTRANSFORMERITEM_H
