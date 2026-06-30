#ifndef LZPOWERCAPACITORITEM_H
#define LZPOWERCAPACITORITEM_H

#include "LzWiringItemBase.h"

class LzPowerCapacitorItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerCapacitorItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerCapacitorItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(44, 48); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    QString displayName() const override;
    LzTopologyDomain topologyDomain() const override { return LzTopologyDomain::PowerSystem; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool topologyGraphNodeEnabled() const override { return false; }
};

#endif // LZPOWERCAPACITORITEM_H
