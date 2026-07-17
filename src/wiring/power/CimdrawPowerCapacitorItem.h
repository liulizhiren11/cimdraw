#ifndef CIMDRAWPOWERCAPACITORITEM_H
#define CIMDRAWPOWERCAPACITORITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerCapacitorItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerCapacitorItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerCapacitorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(44, 48); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    QString displayName() const override;
    CimdrawTopologyDomain topologyDomain() const override { return CimdrawTopologyDomain::PowerSystem; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // CIMDRAWPOWERCAPACITORITEM_H
