#ifndef CIMDRAWSCENEDEFERREDUPDATECONTROLLER_H
#define CIMDRAWSCENEDEFERREDUPDATECONTROLLER_H

#include <QList>
#include <QSet>

class CimdrawConnectLine;
class CimdrawScene;

class CimdrawSceneDeferredUpdateController
{
public:
    void scheduleTopologyRebuild(CimdrawScene* scene);
    void scheduleConnectorPostprocess(CimdrawScene* scene, const QList<CimdrawConnectLine*>& affectedLines);
    void scheduleConnectLinesPathRecompute(CimdrawScene* scene, const QList<CimdrawConnectLine*>& lines);

    void cancelConnectLinePathRecompute(CimdrawConnectLine* line);
    bool isConnectLinePathUpdatePending() const;

    void requestDeferredItemPropertyPanel();
    void flushDeferredItemPropertyPanel(CimdrawScene* scene);

private:
    bool connectorPostprocessPending_ = false;
    bool topologyRebuildPending_ = false;
    bool pathRecomputePending_ = false;
    bool itemPropertyPanelFlushPending_ = false;
    QSet<CimdrawConnectLine*> pendingConnectorLines_;
    QSet<CimdrawConnectLine*> pendingPathRecomputeLines_;
};

#endif
