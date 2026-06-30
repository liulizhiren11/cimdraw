#ifndef LZSCENESTATEWORKBENCHCONTROLLER_H
#define LZSCENESTATEWORKBENCHCONTROLLER_H

#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

class LzConnectLine;
class LzScene;
class LzSceneDeferredUpdateController;
class LzSceneTopologyStateController;
class LzView;

class LzSceneStateWorkbenchController
{
public:
    LzView* view(LzView* currentView) const;
    void setView(LzView*& currentView, LzView* nextView) const;

    bool paintState(bool currentState) const;
    void setPaintState(bool& currentState, bool nextState) const;

    bool interactiveTransformActive(bool currentState) const;
    void setInteractiveTransformActive(bool& currentState, bool nextState) const;

    void invalidateTopologySnapshots(LzSceneTopologyStateController& topologyStateController) const;
    void scheduleTopologyRebuild(LzScene* scene,
                                 LzSceneDeferredUpdateController& deferredUpdateController,
                                 LzSceneTopologyStateController& topologyStateController) const;
    void cancelConnectLinePathRecompute(LzSceneDeferredUpdateController& deferredUpdateController,
                                        LzConnectLine* line) const;
    bool isConnectLinePathUpdatePending(const LzSceneDeferredUpdateController& deferredUpdateController) const;
    void requestDeferredItemPropertyPanel(LzSceneDeferredUpdateController& deferredUpdateController) const;
    void flushDeferredItemPropertyPanel(LzScene* scene,
                                        LzSceneDeferredUpdateController& deferredUpdateController) const;
    void scheduleConnectLinesPathRecompute(LzScene* scene,
                                           LzSceneDeferredUpdateController& deferredUpdateController,
                                           const QList<LzConnectLine*>& lines) const;
    void scheduleConnectorPostprocess(LzScene* scene,
                                      LzSceneDeferredUpdateController& deferredUpdateController,
                                      const QList<LzConnectLine*>& affectedLines) const;

    bool setWiringData(QHash<QString, QVariant>& wiringData,
                       LzSceneTopologyStateController& topologyStateController,
                       const QString& key,
                       const QVariant& value) const;
    QVariant wiringData(const QHash<QString, QVariant>& wiringData,
                        const QString& key) const;
};

#endif
