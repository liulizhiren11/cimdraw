#ifndef CIMDRAWTRANSFORMER2WITEM_H
#define CIMDRAWTRANSFORMER2WITEM_H

#include "CimdrawWiringItemBase.h"

class CimdrawPowerTransformer2wItem : public CimdrawWiringItemBase
{
    Q_OBJECT
public:
    explicit CimdrawPowerTransformer2wItem(QGraphicsItem* parent = nullptr);
    explicit CimdrawPowerTransformer2wItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(112, 68); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    CimdrawDrawTypeId drawTypeForXml() const override;
    CimdrawWiringItemBase* cloneForDuplicate() const override;
    CimdrawTopologyDomain topologyDomain() const override { return CimdrawTopologyDomain::PowerSystem; }
    PowerTopologyRole powerTopologyRole() const override { return PowerTopologyRole::Transformer; }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    int defaultWiringConnectPointCount() const override;
    void configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const override;
};

#endif // CIMDRAWTRANSFORMER2WITEM_H
