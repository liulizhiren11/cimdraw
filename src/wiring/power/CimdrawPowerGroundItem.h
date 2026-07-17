#ifndef CIMDRAWGROUNDITEM_H
#define CIMDRAWGROUNDITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerGroundItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerGroundItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerGroundItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(40, 96); }
    static void drawSymbol(QPainter* painter,
                           const QRectF& rect,
                           CimdrawWiringRunState runState,
                           bool alarmPulse,
                           CimdrawWiringSymbolStandard symStd);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    CimdrawTopologyDomain topologyDomain() const override { return CimdrawTopologyDomain::PowerSystem; }
    PowerTopologyRole powerTopologyRole() const override { return PowerTopologyRole::Ground; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // CIMDRAWGROUNDITEM_H
