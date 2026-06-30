#ifndef LZTRANSFORMER2WITEM_H
#define LZTRANSFORMER2WITEM_H

#include "LzWiringItemBase.h"

class LzPowerTransformer2wItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerTransformer2wItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerTransformer2wItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(112, 68); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    LzTopologyDomain topologyDomain() const override { return LzTopologyDomain::PowerSystem; }
    PowerTopologyRole powerTopologyRole() const override { return PowerTopologyRole::Transformer; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const override;
};

#endif // LZTRANSFORMER2WITEM_H
