#ifndef LZSCENE_H
#define LZSCENE_H

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QVariant>
#include <QVariantMap>
#include <QPair>
#include <QSizeF>
#include <QMenu>
#include <QAction>
#include <QDomDocument>
#include <QScopedPointer>
#include "LzViewConfig.h"
#include "topology/PowerTopologyTypes.h"

class LzGroup;
class LzView;
class LzScenePrivate;
class LzConnectLine;
class TopologyGraphIndex;
class LzSceneDocumentSlotWorkbenchController;
class LzSceneSelectionSlotWorkbenchController;
class LzSceneStateSlotWorkbenchController;
class LzSceneTopologySlotWorkbenchController;
class LzSceneTopologyStateController;
class LzSceneDeferredUpdateController;
class LzSceneViewportController;
class LzScene : public QGraphicsScene
{
    Q_OBJECT
public:
    LzScene();
    ~LzScene();
    LzView* getView();

    void setView(LzView* view);

    bool load(const QString& filename, QString* inout_error_msg);

    bool toDomDocument(const QString& filename, QDomDocument * dom, QString* inout_error_msg);

    QRectF getContentsRect();

    //void setImageFile(const QString&file);

    void onLeftAlign();//左对齐
    void onRightAlign();//右对齐
    void onTopAlign();//上对齐
    void onBottomAlign();//下对齐
    void onVcenterAlign();//垂直对齐
    void onHcenterAlign();//水平对齐
    void onAutoCol();//垂直对齐
    void onAutoRow();//水平对齐
    void onSameWidth();//等宽
    void onSameHeight();//等高
    void onSameSize();//等大小
    void onBringFont();//置于顶层
    void onBringBack();//置于底层

    void onShortcutLeft(QPointF delta);
    void onShortcutRight(QPointF delta);
    void onShortcutUp(QPointF delta);
    void onShortcutDown(QPointF delta);

    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editLevelUp();
    void editLevelDown();
    void editToTop();
    void editToBottom();
    void itemRotate(int value);

    void editGroup();
    void editUnGroup();

    QList<QGraphicsItem*> getSelections();
    void setSelections(const QList<QGraphicsItem*>& selections);

    QGraphicsItem* getSelection();

    void cleanSelection();

    void addSelection(QGraphicsItem* item);
    void removeSelection(QGraphicsItem* item);

    void insertSelection(QGraphicsItem* item);
    void deleteSelection(QGraphicsItem* item);

    void addSelections(QList<QGraphicsItem*> items);
    void removeSelections(QList<QGraphicsItem*> items);

    /** 从场景移除图元并同步清理 selectList（勿对已 delete 的指针调用） */
    void removeSceneItem(QGraphicsItem* item);

    bool getPaintState();
    void setPaintState(bool state);
    bool isInteractiveTransformActive() const;
    void setInteractiveTransformActive(bool active);
    void scheduleTopologyRebuild();
    void scheduleConnectorPostprocess(const QList<LzConnectLine*>& affectedLines = {});
    /** 批量延迟重算连线路径（拖动/缩放结束后合并为一次，避免 N 条线卡顿） */
    void scheduleConnectLinesPathRecompute(const QList<LzConnectLine*>& lines);
    void cancelConnectLinePathRecompute(LzConnectLine* line);
    bool isConnectLinePathUpdatePending() const;
    /** 在连线路径批量更新完成后再刷新属性面板，避免松手时同步重建属性树卡顿 */
    void flushDeferredItemPropertyPanel();
    void requestDeferredItemPropertyPanel();

    QGraphicsItemGroup* createGroup(QList<QGraphicsItem* > m_items);

    virtual bool save(const QString& pFileName);

    void rebuildTopologyIndex();
    const TopologyGraphIndex* topologyIndex() const;
    TopologyGraphIndex* topologyIndex();
    void highlightTopologyNode(const QString& nodeStableId, bool on, const QColor& c = QColor());
    void highlightTopologyEdge(const QString& edgeStableId, bool on, const QColor& c = QColor());
    void clearTopologyHighlights();
    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot();
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport();
    QVector<int> powerDeviceNodeIds(const QString& deviceId);
    QStringList powerDeviceConductorIds(const QString& deviceId);
    QStringList connectedPowerDevices(const QString& deviceId);
    QStringList busbarAttachedPowerDevices(const QString& deviceId);
    QStringList reachablePowerDevices(const QString& deviceId);
    QStringList directedReachablePowerDevices(const QString& sourceDeviceId);
    PowerTopologyIslandAnalysis analyzePowerIslands();
    PowerTopologyBranchAnalysis analyzePowerBranches(const QString& sourceDeviceId);
    PowerTopologyLoopAnalysis analyzePowerLoops();
    PowerTopologyPath shortestPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId);
    PowerTopologyPath shortestDirectedPowerSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId);
    PowerTopologyGroundPath shortestPowerGroundPath(const QString& sourceDeviceId);
    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(const QString& deviceId, int toSwitchPosition);
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& sourceDeviceId);
    PowerTopologyOperationPreview previewPowerSwitchOperation(const QString& deviceId,
                                                              int toSwitchPosition,
                                                              const QString& supplySourceDeviceId,
                                                              const QString& supplyTargetDeviceId,
                                                              const QString& groundSourceDeviceId);
    QVariantMap buildPowerTopologyBindingSnapshot();
    QVariantMap buildTopologyBindingSnapshot();
    QVariantMap queryTopology(const QVariantMap& request);
    void highlightTopologyEdges(const QStringList& edgeStableIds, bool on, const QColor& c = QColor());

    /// 对场景内所有连接线做共享段 Nudging（proposal-phase1 任务 1.2）
    void applyConnectorNudging(const QList<LzConnectLine*>& seedLines);

    FILE_TYPE getSceneType();
    void setSceneType(FILE_TYPE);

    /** 接线图 / 一次设备数据绑定：写入后对所有匹配 wiringDataKey 的 SLD 图元发出 wiringDataChanged */
    void setWiringData(const QString& key, const QVariant& value);
    QVariant wiringData(const QString& key) const;

signals:
    void currentObjectChanged(QList<QGraphicsItem*> items);
    void itemPropertyChanged();
    void wiringDataChanged(const QString& key, const QVariant& value);
public slots:
    void slotMenuClicked(QAction* action);
    QAction* execContextMenu(const QPoint& screenPos);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
    friend class LzSceneDocumentSlotWorkbenchController;
    friend class LzSceneSelectionSlotWorkbenchController;
    friend class LzSceneStateSlotWorkbenchController;
    friend class LzSceneTopologySlotWorkbenchController;
    friend class LzSceneViewportController;
    void pruneSelectionList();
    void invalidateTopologySnapshots();
    QList<QGraphicsItem*>& selectionStorage();
    const QList<QGraphicsItem*>& selectionStorage() const;
    LzView*& viewStorage();
    LzView* viewStorage() const;
    bool& paintStateStorage();
    bool paintStateStorage() const;
    bool& interactiveTransformStorage();
    bool interactiveTransformStorage() const;
    LzSceneDeferredUpdateController& deferredUpdateStorage();
    const LzSceneDeferredUpdateController& deferredUpdateStorage() const;
    FILE_TYPE& sceneTypeStorage();
    FILE_TYPE sceneTypeStorage() const;
    QHash<QString, QVariant>& wiringDataStorage();
    const QHash<QString, QVariant>& wiringDataStorage() const;
    LzSceneTopologyStateController& topologyStateStorage();
    const LzSceneTopologyStateController& topologyStateStorage() const;
    static bool canTouchSelectionItem(QGraphicsItem* item, const QGraphicsScene* scene);
    Q_DECLARE_PRIVATE(LzScene);
    QScopedPointer<LzScenePrivate> d_ptr;
};
#endif // LZSCENE_H
