#ifndef CIMDRAWSCENETOPOLOGYSLOTWORKBENCHCONTROLLER_H
#define CIMDRAWSCENETOPOLOGYSLOTWORKBENCHCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

#include <QStringList>
#include <QVariantMap>

class QColor;
class QString;
class CimdrawScene;

class CimdrawSceneTopologySlotWorkbenchController
{
public:
    enum class SceneAction
    {
        RebuildTopologyIndex,
        ClearTopologyHighlights
    };

    enum class HighlightAction
    {
        Node,
        Edge,
        Edges
    };

    enum class SnapshotAction
    {
        BuildPowerTopologySnapshot,
        BuildPowerTopologyDocumentExport
    };

    enum class DeviceQueryAction
    {
        PowerDeviceNodeIds,
        PowerDeviceConductorIds,
        ConnectedPowerDevices,
        BusbarAttachedPowerDevices,
        ReachablePowerDevices,
        DirectedReachablePowerDevices
    };

    enum class AnalysisAction
    {
        AnalyzePowerIslands,
        AnalyzePowerBranches,
        AnalyzePowerLoops
    };

    enum class PathAction
    {
        ShortestPowerSupplyPath,
        ShortestDirectedPowerSupplyPath,
        ShortestPowerGroundPath,
        PreviewPowerSwitchTopologyChange,
        PreviewPowerProtectionRange,
        PreviewPowerSwitchOperation
    };

    bool dispatch(CimdrawScene* scene, SceneAction action) const;
    bool dispatch(CimdrawScene* scene,
                  HighlightAction action,
                  const QString& stableId,
                  bool on,
                  const QColor& c) const;
    bool dispatch(CimdrawScene* scene,
                  HighlightAction action,
                  const QStringList& stableIds,
                  bool on,
                  const QColor& c) const;
    PowerTopologyAnalysisSnapshot dispatchSnapshot(CimdrawScene* scene, SnapshotAction action) const;
    PowerTopologyDocumentExport dispatchDocumentExport(CimdrawScene* scene, SnapshotAction action) const;
    QVector<int> dispatchDeviceNodeIds(CimdrawScene* scene,
                                       DeviceQueryAction action,
                                       const QString& deviceId) const;
    QStringList dispatchDeviceQuery(CimdrawScene* scene,
                                    DeviceQueryAction action,
                                    const QString& deviceId) const;
    PowerTopologyIslandAnalysis dispatchIslandAnalysis(CimdrawScene* scene, AnalysisAction action) const;
    PowerTopologyBranchAnalysis dispatchBranchAnalysis(CimdrawScene* scene,
                                                       AnalysisAction action,
                                                       const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis dispatchLoopAnalysis(CimdrawScene* scene, AnalysisAction action) const;
    PowerTopologyPath dispatchPath(CimdrawScene* scene,
                                   PathAction action,
                                   const QString& sourceDeviceId,
                                   const QString& targetDeviceId = QString()) const;
    PowerTopologyGroundPath dispatchGroundPath(CimdrawScene* scene,
                                               PathAction action,
                                               const QString& sourceDeviceId) const;
    PowerTopologySwitchChangePreview dispatchSwitchChangePreview(CimdrawScene* scene,
                                                                 PathAction action,
                                                                 const QString& deviceId,
                                                                 int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview dispatchProtectionRangePreview(
        CimdrawScene* scene,
        PathAction action,
        const QString& deviceId,
        int toSwitchPosition,
        const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview dispatchOperationPreview(CimdrawScene* scene,
                                                           PathAction action,
                                                           const QString& deviceId,
                                                           int toSwitchPosition,
                                                           const QString& supplySourceDeviceId,
                                                           const QString& supplyTargetDeviceId,
                                                           const QString& groundSourceDeviceId) const;

    void rebuildTopologyIndex(CimdrawScene* scene) const;
    bool highlightTopologyNode(CimdrawScene* scene, const QString& nodeStableId, bool on, const QColor& c) const;
    bool highlightTopologyEdge(CimdrawScene* scene, const QString& edgeStableId, bool on, const QColor& c) const;
    bool highlightTopologyEdges(CimdrawScene* scene, const QStringList& edgeStableIds, bool on, const QColor& c) const;
    bool clearTopologyHighlights(CimdrawScene* scene) const;
    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(CimdrawScene* scene) const;
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(CimdrawScene* scene) const;

    QVector<int> powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList directedReachablePowerDevices(CimdrawScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyIslandAnalysis analyzePowerIslands(CimdrawScene* scene) const;
    PowerTopologyBranchAnalysis analyzePowerBranches(CimdrawScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis analyzePowerLoops(CimdrawScene* scene) const;
    PowerTopologyPath shortestPowerSupplyPath(CimdrawScene* scene,
                                              const QString& sourceDeviceId,
                                              const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                      const QString& sourceDeviceId,
                                                      const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestPowerGroundPath(CimdrawScene* scene, const QString& sourceDeviceId) const;
    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(CimdrawScene* scene,
                                                                      const QString& deviceId,
                                                                      int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(CimdrawScene* scene,
                                                                    const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview previewPowerSwitchOperation(CimdrawScene* scene,
                                                              const QString& deviceId,
                                                              int toSwitchPosition,
                                                              const QString& supplySourceDeviceId,
                                                              const QString& supplyTargetDeviceId,
                                                              const QString& groundSourceDeviceId) const;

};

#endif
