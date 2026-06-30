#ifndef LZSCENESTATESLOTWORKBENCHCONTROLLER_H
#define LZSCENESTATESLOTWORKBENCHCONTROLLER_H

#include <QList>
#include <QString>
#include <QVariant>

class LzConnectLine;
class LzScene;
class LzView;

class LzSceneStateSlotWorkbenchController
{
public:
    enum class ViewAction
    {
        Get
    };

    enum class FlagStateAction
    {
        PaintState,
        InteractiveTransformActive
    };

    enum class DeferredSceneAction
    {
        InvalidateTopologySnapshots,
        ScheduleTopologyRebuild,
        RequestDeferredItemPropertyPanel,
        FlushDeferredItemPropertyPanel
    };

    enum class DeferredLineAction
    {
        CancelConnectLinePathRecompute,
        ScheduleConnectLinesPathRecompute,
        ScheduleConnectorPostprocess
    };

    enum class DeferredQueryAction
    {
        IsConnectLinePathUpdatePending
    };

    enum class WiringDataAction
    {
        Get
    };

    LzView* view(LzScene* scene) const;
    LzView* view(const LzScene* scene) const;
    LzView* dispatch(LzScene* scene, ViewAction action) const;
    LzView* dispatch(const LzScene* scene, ViewAction action) const;
    bool setView(LzScene* scene, LzView* nextView) const;
    bool flagState(const LzScene* scene, FlagStateAction action) const;
    bool dispatch(LzScene* scene, FlagStateAction action, bool nextState) const;
    bool dispatch(LzScene* scene, DeferredSceneAction action) const;
    bool dispatch(LzScene* scene, DeferredLineAction action, LzConnectLine* line) const;
    bool dispatch(LzScene* scene,
                  DeferredLineAction action,
                  const QList<LzConnectLine*>& lines) const;
    bool dispatch(const LzScene* scene, DeferredQueryAction action) const;
    bool dispatchWiringData(LzScene* scene, const QString& key, const QVariant& value) const;
    QVariant dispatch(const LzScene* scene, WiringDataAction action, const QString& key) const;

    bool paintState(const LzScene* scene) const;
    bool setPaintState(LzScene* scene, bool nextState) const;

    bool interactiveTransformActive(const LzScene* scene) const;
    bool setInteractiveTransformActive(LzScene* scene, bool nextState) const;

    bool invalidateTopologySnapshots(LzScene* scene) const;
    bool scheduleTopologyRebuild(LzScene* scene) const;
    bool cancelConnectLinePathRecompute(LzScene* scene, LzConnectLine* line) const;
    bool isConnectLinePathUpdatePending(const LzScene* scene) const;
    bool requestDeferredItemPropertyPanel(LzScene* scene) const;
    bool flushDeferredItemPropertyPanel(LzScene* scene) const;
    bool scheduleConnectLinesPathRecompute(LzScene* scene, const QList<LzConnectLine*>& lines) const;
    bool scheduleConnectorPostprocess(LzScene* scene, const QList<LzConnectLine*>& affectedLines) const;

    bool setWiringData(LzScene* scene, const QString& key, const QVariant& value) const;
    QVariant wiringData(const LzScene* scene, const QString& key) const;
};

#endif
