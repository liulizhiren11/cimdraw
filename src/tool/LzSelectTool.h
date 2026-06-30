#ifndef LZSELECTTOOL_H
#define LZSELECTTOOL_H

#include <QGraphicsRectItem>
#include <QPointer>
#include <QVector>
#include <QPointF>

#include "LzTool.h"
#include "LzConnectConfig.h"
#include "command/LzEditConnectLinePathCommand.h"
#include "item/LzGroup.h"
#include "item/TmpBase.h"

class LzPowerBusbarSectionItem;
class LzConnectLine;
class LzConnectPoint;

class LzSelectTool : public LzTool
{
    Q_OBJECT
public:
    explicit LzSelectTool(QObject* parent);
    virtual ~LzSelectTool() override;

    virtual LzDrawTypeId getDrawType() const override;
    virtual QGraphicsItem* createObject(LzScene* scene, QRectF* pos, bool addToScene = true) override;
    virtual QIcon getIcon(const QSize& size, const QVariant& para = QVariant()) const override;
    virtual void onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseRelease(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt) override;
    virtual void onMouseDoubleClick(LzScene* scene,QGraphicsSceneMouseEvent* evt) override;
protected:
    QPointF opposite;
    bool isDragging = false;
    bool segmentDragPending = false;  // 未命中线段时回退为插点+拖折点
    int segmentDragSegmentIndex = -1;
    QVector<QPointF> segmentDragBasePath;
    bool pendingBusbarWire = false;   // Ctrl + 母线沿边按下：拖动则拉线
    bool pendingBusbarPortNeedsCleanup = false;
    QPointF pendingBusbarWireDown;
    QPointer<LzPowerBusbarSectionItem> pendingBusbar;
    LzConnectPoint* pendingBusbarTouchPort = nullptr;
    QPointF lastPoint;
    QPointF downPoint;
    QGraphicsRectItem* selectionRect = nullptr;
private:
    void resetState();
    void handleMove(LzScene* scene, const QPointF& delta);
    void handleResize(LzScene* scene, const QPointF& pos);
    void handleEdit(LzScene* scene, const QPointF& pos);
    void updateCursor(LzScene* scene);
    void handleEditorMode(TmpShape* shape, QGraphicsSceneMouseEvent* evt);
    void handleMoveMode(TmpShape* shape, QGraphicsSceneMouseEvent* evt);
    void updateHoverState(LzScene* scene, TmpShape* hoverShape);
    void enableRubberBandSelection(LzScene* scene);
    void attachConnectLineEndpointIfNearPort(LzScene* scene, LzConnectLine* line, int handleDir,
                                            const QPointF& scenePos);
    void beginConnectLinePathEdit(LzConnectLine* line);
    void endConnectLinePathEdit(LzScene* scene);
    void setupSegmentDrag(LzConnectLine* line, const QPointF& scenePressPos);

    QPointer<LzConnectLine> connectLinePathEditTarget;
    LzEditConnectLinePathCommand::State connectLineEditStateBefore;
    QVector<QPointF> connectLineEditBasePath;
    bool connectLinePathEditChanged = false;
    QVector<LzConnectLinePathSnapshot> moveLineSnapshotsBefore;
};

#endif
