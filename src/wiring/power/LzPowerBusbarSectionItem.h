#ifndef LZBUSBARSECTIONITEM_H
#define LZBUSBARSECTIONITEM_H

#include "LzWiringItemBase.h"
#include "LzConnectConfig.h"

class LzPowerBusbarSectionItem : public LzWiringItemBase
{
    Q_OBJECT
public:
    explicit LzPowerBusbarSectionItem(QGraphicsItem* parent = nullptr);
    explicit LzPowerBusbarSectionItem(const QRectF& pos, QGraphicsItem* parent = nullptr);
    static QSizeF defaultSize() { return QSizeF(300, 12); }
    static void drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse);
    QString className() override;
    QString shapeName() const override;
    LzDrawTypeId drawTypeForXml() const override;
    LzWiringItemBase* cloneForDuplicate() const override;
    QGraphicsItem* duplicate() override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    bool wiringUsesAlongEdgeConnectPoints() const override { return true; }
    bool topologyGraphNodeEnabled() const override { return false; }
    LzConnectPoint* ensureConnectPointAtScene(const QPointF& scenePos) override;
    void rebindAlongEdgeConnectPorts() override;

    /** 与 paint 中母线条带一致（item 局部坐标） */
    QRectF busbarBodyRect() const;
    bool hitBusbarPickRegion(const QPointF& scenePos) const;
    /** 将场景坐标吸附到母线上下沿（draw.io）；成功时 anchorScene 为沿边连接点圆心 */
    bool snapSceneToEdgePort(const QPointF& scenePos, qreal maxDist, QPointF& anchorScene) const;
    /** 在 maxSnapDist 内吸附到母线沿边并复用/创建端口 */
    LzConnectPoint* ensureConnectPointNearScene(const QPointF& scenePos, qreal maxSnapDist);

    QPainterPath shape() const override;
    void stretch(int handle, double sx, double sy, const QPointF& origin) override;

    bool isConnectPointUsed(const LzConnectPoint* port) const;
    void removeConnectPointIfUnused(LzConnectPoint* port);
    /** 在已有沿边端口中找距 scenePos 最近者（粘贴时按复制位置匹配，避免吸到母线另一沿） */
    LzConnectPoint* findConnectPortNearScene(const QPointF& scenePos, qreal maxDistPx = 36.0) const;

private:
    void copyAlongEdgeConnectPointsFrom(const LzPowerBusbarSectionItem* source);
    bool projectSceneToConnectAnchor(const QPointF& scenePos, QPointF& norm, CONNECT_DIRECTION& dir) const;
};

class LzConnectLine;

/** 删除/隐藏连线：从端图元摘掉连线，并删除母线上无其它连线的沿边端口 */
/** 按路径端点恢复母线沿边端口；preservePathShape 为 true 时不重算折线轨迹 */
void lzReattachBusbarPortsForConnectLine(LzConnectLine* line, bool preservePathShape = true);

#endif // LZBUSBARSECTIONITEM_H
