#ifndef CIMDRAWSCENESTATESLOTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENESTATESLOTWORKBENCHCONTROLLER_H

#include <QList>
#include <QString>
#include <QVariant>

class CimdrawConnectLine;
class CimdrawScene;
class CimdrawView;

class CimdrawSceneStateSlotWorkbenchController
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

    CimdrawView* view(CimdrawScene* scene) const;
    CimdrawView* view(const CimdrawScene* scene) const;
    CimdrawView* dispatch(CimdrawScene* scene, ViewAction action) const;
    CimdrawView* dispatch(const CimdrawScene* scene, ViewAction action) const;
    bool setView(CimdrawScene* scene, CimdrawView* nextView) const;
    bool flagState(const CimdrawScene* scene, FlagStateAction action) const;
    bool dispatch(CimdrawScene* scene, FlagStateAction action, bool nextState) const;
    bool dispatch(CimdrawScene* scene, DeferredSceneAction action) const;
    bool dispatch(CimdrawScene* scene, DeferredLineAction action, CimdrawConnectLine* line) const;
    bool dispatch(CimdrawScene* scene,
                  DeferredLineAction action,
                  const QList<CimdrawConnectLine*>& lines) const;
    bool dispatch(const CimdrawScene* scene, DeferredQueryAction action) const;
    bool dispatchWiringData(CimdrawScene* scene, const QString& key, const QVariant& value) const;
    QVariant dispatch(const CimdrawScene* scene, WiringDataAction action, const QString& key) const;

    bool paintState(const CimdrawScene* scene) const;
    bool setPaintState(CimdrawScene* scene, bool nextState) const;

    bool interactiveTransformActive(const CimdrawScene* scene) const;
    bool setInteractiveTransformActive(CimdrawScene* scene, bool nextState) const;

    bool invalidateTopologySnapshots(CimdrawScene* scene) const;
    bool scheduleTopologyRebuild(CimdrawScene* scene) const;
    bool cancelConnectLinePathRecompute(CimdrawScene* scene, CimdrawConnectLine* line) const;
    bool isConnectLinePathUpdatePending(const CimdrawScene* scene) const;
    bool requestDeferredItemPropertyPanel(CimdrawScene* scene) const;
    bool flushDeferredItemPropertyPanel(CimdrawScene* scene) const;
    bool scheduleConnectLinesPathRecompute(CimdrawScene* scene, const QList<CimdrawConnectLine*>& lines) const;
    bool scheduleConnectorPostprocess(CimdrawScene* scene, const QList<CimdrawConnectLine*>& affectedLines) const;

    bool setWiringData(CimdrawScene* scene, const QString& key, const QVariant& value) const;
    QVariant wiringData(const CimdrawScene* scene, const QString& key) const;
};

#endif
