#ifndef LZPOWERARRESTERITEM_H
#define LZPOWERARRESTERITEM_H

#include "LzWiringItemBase.h"

class LzPowerArresterItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerArresterItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerArresterItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(52, 58); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
                           LzWiringSymbolStandard symStd);

    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERARRESTERITEM_H
