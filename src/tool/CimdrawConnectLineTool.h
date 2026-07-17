#ifndef CIMDRAWLINKTOOL_H
#define CIMDRAWLINKTOOL_H

#include "CimdrawTool.h"

#include "Item/TmpBase.h"

class CimdrawConnectLineToolPrivate;
class CimdrawConnectPoint;
class CimdrawScene;
class CimdrawConnectLine;

/** 接线图端点是否允许相连（母线/SLD 可与开关等拓扑图元互连） */
bool cimdrawWiringConnectEndpointsCompatible(TmpShape* start, TmpShape* end);

class CimdrawConnectLineTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawConnectLineTool(QObject* parent);
    virtual ~CimdrawConnectLineTool() override;

    /** 选择工具从母线拖出连线时：在连接点处开始预览折线 */
    void beginWireFromConnectPoint(CimdrawScene* scene, CimdrawConnectPoint* connectPoint, const QPointF& scenePos);
    bool hasActiveWire() const;
    void abortCurrentWire(CimdrawScene* scene, bool switchToSelection = true);

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseRelease(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
private:
    void finishActiveWire(CimdrawScene* scene, const QPointF& scenePos, CimdrawConnectPoint* endPort,
                          TmpShape* endShape);

    CimdrawConnectLineToolPrivate* d_ptr;
};
#endif
