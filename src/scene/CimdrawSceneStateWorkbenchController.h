#ifndef CIMDRAWSCENESTATEWORKBENCHCONTROLLER_H
#define CIMDRAWSCENESTATEWORKBENCHCONTROLLER_H

#include <QHash>
#include <QList>
#include <QString>
#include <QVariant>

class CimdrawConnectLine;
class CimdrawScene;
class CimdrawSceneDeferredUpdateController;
class CimdrawSceneTopologyStateController;
class CimdrawView;

class CimdrawSceneStateWorkbenchController
{
public:
    CimdrawView* view(CimdrawView* currentView) const;
    void setView(CimdrawView*& currentView, CimdrawView* nextView) const;

    bool paintState(bool currentState) const;
    void setPaintState(bool& currentState, bool nextState) const;

    bool interactiveTransformActive(bool currentState) const;
    void setInteractiveTransformActive(bool& currentState, bool nextState) const;

    void invalidateTopologySnapshots(CimdrawSceneTopologyStateController& topologyStateController) const;
    void scheduleTopologyRebuild(CimdrawScene* scene,
                                 CimdrawSceneDeferredUpdateController& deferredUpdateController,
                                 CimdrawSceneTopologyStateController& topologyStateController) const;
    void cancelConnectLinePathRecompute(CimdrawSceneDeferredUpdateController& deferredUpdateController,
                                        CimdrawConnectLine* line) const;
    bool isConnectLinePathUpdatePending(const CimdrawSceneDeferredUpdateController& deferredUpdateController) const;
    void requestDeferredItemPropertyPanel(CimdrawSceneDeferredUpdateController& deferredUpdateController) const;
    void flushDeferredItemPropertyPanel(CimdrawScene* scene,
                                        CimdrawSceneDeferredUpdateController& deferredUpdateController) const;
    void scheduleConnectLinesPathRecompute(CimdrawScene* scene,
                                           CimdrawSceneDeferredUpdateController& deferredUpdateController,
                                           const QList<CimdrawConnectLine*>& lines) const;
    void scheduleConnectorPostprocess(CimdrawScene* scene,
                                      CimdrawSceneDeferredUpdateController& deferredUpdateController,
                                      const QList<CimdrawConnectLine*>& affectedLines) const;

    bool setWiringData(QHash<QString, QVariant>& wiringData,
                       CimdrawSceneTopologyStateController& topologyStateController,
                       const QString& key,
                       const QVariant& value) const;
    QVariant wiringData(const QHash<QString, QVariant>& wiringData,
                        const QString& key) const;
};

#endif
