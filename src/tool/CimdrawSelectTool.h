#ifndef CIMDRAWSELECTTOOL_H
#define CIMDRAWSELECTTOOL_H

#include <QGraphicsRectItem>
#include <QPointer>
#include <QVector>
#include <QPointF>

#include "CimdrawTool.h"
#include "CimdrawConnectConfig.h"
#include "command/CimdrawEditConnectLinePathCommand.h"
#include "item/CimdrawGroup.h"
#include "item/TmpBase.h"

class CimdrawPowerBusbarSectionItem;
class CimdrawConnectLine;
class CimdrawConnectPoint;

class CimdrawSelectTool : public CimdrawTool
{
    Q_OBJECT
public:
    explicit CimdrawSelectTool(QObject* parent);
    virtual ~CimdrawSelectTool() override;

    virtual CimdrawDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(CimdrawScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseRelease(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseDoubleClick(CimdrawScene* scene,QGraphicsSceneMouseEvent* evt) override;
protected:
    QPointF opposite;
    bool isDragging = false;
    bool segmentDragPending = false;  // 未命中线段时回退为插点+拖折点
    int segmentDragSegmentIndex = -1;
    QVector<QPointF> segmentDragBasePath;
    bool pendingBusbarWire = false;   // Ctrl + 母线沿边按下：拖动则拉线
    bool pendingBusbarPortNeedsCleanup = false;
    QPointF pendingBusbarWireDown;
    QPointer<CimdrawPowerBusbarSectionItem> pendingBusbar;
    CimdrawConnectPoint* pendingBusbarTouchPort = nullptr;
    QPointF lastPoint;
    QPointF downPoint;
    QGraphicsRectItem* selectionRect = nullptr;
private:
    void resetState();
    void handleMove(CimdrawScene* scene, const QPointF& delta);
    void handleResize(CimdrawScene* scene, const QPointF& pos);
    void handleEdit(CimdrawScene* scene, const QPointF& pos);
    void updateCursor(CimdrawScene* scene);
    void handleEditorMode(TmpShape* shape, QGraphicsSceneMouseEvent* evt);
    void handleMoveMode(TmpShape* shape, QGraphicsSceneMouseEvent* evt);
    void updateHoverState(CimdrawScene* scene, TmpShape* hoverShape);
    void enableRubberBandSelection(CimdrawScene* scene);
    void attachConnectLineEndpointIfNearPort(CimdrawScene* scene, CimdrawConnectLine* line, int handleDir,
                                            const QPointF& scenePos);
    void beginConnectLinePathEdit(CimdrawConnectLine* line);
    void endConnectLinePathEdit(CimdrawScene* scene);
    void setupSegmentDrag(CimdrawConnectLine* line, const QPointF& scenePressPos);

    QPointer<CimdrawConnectLine> connectLinePathEditTarget;
    CimdrawEditConnectLinePathCommand::State connectLineEditStateBefore;
    QVector<QPointF> connectLineEditBasePath;
    bool connectLinePathEditChanged = false;
    QVector<CimdrawConnectLinePathSnapshot> moveLineSnapshotsBefore;
};

#endif
