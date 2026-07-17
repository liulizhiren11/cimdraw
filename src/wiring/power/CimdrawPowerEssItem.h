#ifndef CIMDRAWPOWERESSITEM_H
#define CIMDRAWPOWERESSITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerEssItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerEssItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerEssItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(72, 52); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse, int flowSign);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWPOWERESSITEM_H
