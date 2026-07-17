#ifndef CIMDRAWPOWERCABLEITEM_H
#define CIMDRAWPOWERCABLEITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerCableItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerCableItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerCableItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(128, 32); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           CimdrawWiringRunState runState,
                           bool alarmPulse,
                           CimdrawWiringSymbolStandard symStd);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWPOWERCABLEITEM_H
