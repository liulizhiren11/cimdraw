#ifndef CIMDRAWPOWERARRESTERITEM_H
#define CIMDRAWPOWERARRESTERITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerArresterItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerArresterItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerArresterItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(52, 58); }
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
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERARRESTERITEM_H
