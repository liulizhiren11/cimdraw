#ifndef LZSCENEDEFERREDUPDATECONTROLLER_H
#define LZSCENEDEFERREDUPDATECONTROLLER_H

#include <QList>
#include <QSet>

class LzConnectLine;
class LzScene;

class LzSceneDeferredUpdateController
{
public:
    void scheduleTopologyRebuild(LzScene* scene);
    void scheduleConnectorPostprocess(LzScene* scene, const QList<LzConnectLine*>& affectedLines);
    void scheduleConnectLinesPathRecompute(LzScene* scene, const QList<LzConnectLine*>& lines);

    void cancelConnectLinePathRecompute(LzConnectLine* line);
    bool isConnectLinePathUpdatePending() const;

    void requestDeferredItemPropertyPanel();
    void flushDeferredItemPropertyPanel(LzScene* scene);

private:
    bool connectorPostprocessPending_ = false;
    bool topologyRebuildPending_ = false;
    bool pathRecomputePending_ = false;
    bool itemPropertyPanelFlushPending_ = false;
    QSet<LzConnectLine*> pendingConnectorLines_;
    QSet<LzConnectLine*> pendingPathRecomputeLines_;
};

#endif
