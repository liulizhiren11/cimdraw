#ifndef LZLINKTOOL_H
#define LZLINKTOOL_H

#include "LzTool.h"

#include "Item/TmpBase.h"

class LzConnectLineToolPrivate;
class LzConnectPoint;
class LzScene;
class LzConnectLine;

/** 接线图端点是否允许相连（母线/SLD 可与开关等拓扑图元互连） */
bool lzWiringConnectEndpointsCompatible(TmpShape* start, TmpShape* end);

class LzConnectLineTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzConnectLineTool(QObject* parent);
    virtual ~LzConnectLineTool() override;

    /** 选择工具从母线拖出连线时：在连接点处开始预览折线 */
    void beginWireFromConnectPoint(LzScene* scene, LzConnectPoint* connectPoint, const QPointF& scenePos);
    bool hasActiveWire() const;
    void abortCurrentWire(LzScene* scene, bool switchToSelection = true);

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    void finishActiveWire(LzScene* scene, const QPointF& scenePos, LzConnectPoint* endPort,
                          TmpShape* endShape);

    LzConnectLineToolPrivate* d_ptr;
};
#endif
