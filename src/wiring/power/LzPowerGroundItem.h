#ifndef LZGROUNDITEM_H
#define LZGROUNDITEM_H

#include "LzWiringItemBase.h"

class LzPowerGroundItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerGroundItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerGroundItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(40, 96); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           LzWiringRunState runState,
                           bool alarmPulse,
                           LzWiringSymbolStandard symStd);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    LzTopologyDomain topologyDomain() const override { return LzTopologyDomain::PowerSystem; }
    PowerTopologyRole powerTopologyRole() const override { return PowerTopologyRole::Ground; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // LZGROUNDITEM_H
