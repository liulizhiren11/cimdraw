#ifndef CIMDRAWLOADITEM_H
#define CIMDRAWLOADITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerLoadItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerLoadItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerLoadItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(96, 36); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWLOADITEM_H
