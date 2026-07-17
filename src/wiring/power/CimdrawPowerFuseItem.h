#ifndef CIMDRAWPOWERFUSEITEM_H
#define CIMDRAWPOWERFUSEITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerFuseItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerFuseItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerFuseItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(84, 32); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWPOWERFUSEITEM_H
