#ifndef CIMDRAWGENERATORITEM_H
#define CIMDRAWGENERATORITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerGeneratorItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerGeneratorItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerGeneratorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(60, 60); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWGENERATORITEM_H
