#ifndef LZSCENETOPOLOGYSLOTWORKBENCHCONTROLLER_H
#define LZSCENETOPOLOGYSLOTWORKBENCHCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

#include <QStringList>
#include <QVariantMap>

class QColor;
class QString;
class LzScene;
class TopologyGraphIndex;

class LzSceneTopologySlotWorkbenchController
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

    enum class MapAction
    {
        BuildPowerTopologyBindingSnapshot,
        BuildTopologyBindingSnapshot,
        QueryTopology
    };

    enum class IndexAction
    {
        TopologyIndex
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

    bool dispatch(LzScene* scene, SceneAction action) const;
    bool dispatch(LzScene* scene,
                  HighlightAction action,
                  const QString& stableId,
                  bool on,
                  const QColor& c) const;
    bool dispatch(LzScene* scene,
                  HighlightAction action,
                  const QStringList& stableIds,
                  bool on,
                  const QColor& c) const;
    const TopologyGraphIndex* dispatch(const LzScene* scene, IndexAction action) const;
    TopologyGraphIndex* dispatch(LzScene* scene, IndexAction action) const;
    QVariantMap dispatchMap(LzScene* scene,
                            MapAction action,
                            const QVariantMap& request = QVariantMap{}) const;
    PowerTopologyAnalysisSnapshot dispatchSnapshot(LzScene* scene, SnapshotAction action) const;
    PowerTopologyDocumentExport dispatchDocumentExport(LzScene* scene, SnapshotAction action) const;
    QVector<int> dispatchDeviceNodeIds(LzScene* scene,
                                       DeviceQueryAction action,
                                       const QString& deviceId) const;
    QStringList dispatchDeviceQuery(LzScene* scene,
                                    DeviceQueryAction action,
                                    const QString& deviceId) const;
    PowerTopologyIslandAnalysis dispatchIslandAnalysis(LzScene* scene, AnalysisAction action) const;
    PowerTopologyBranchAnalysis dispatchBranchAnalysis(LzScene* scene,
                                                       AnalysisAction action,
                                                       const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis dispatchLoopAnalysis(LzScene* scene, AnalysisAction action) const;
    PowerTopologyPath dispatchPath(LzScene* scene,
                                   PathAction action,
                                   const QString& sourceDeviceId,
                                   const QString& targetDeviceId = QString()) const;
    PowerTopologyGroundPath dispatchGroundPath(LzScene* scene,
                                               PathAction action,
                                               const QString& sourceDeviceId) const;
    PowerTopologySwitchChangePreview dispatchSwitchChangePreview(LzScene* scene,
                                                                 PathAction action,
                                                                 const QString& deviceId,
                                                                 int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview dispatchProtectionRangePreview(
        LzScene* scene,
        PathAction action,
        const QString& deviceId,
        int toSwitchPosition,
        const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview dispatchOperationPreview(LzScene* scene,
                                                           PathAction action,
                                                           const QString& deviceId,
                                                           int toSwitchPosition,
                                                           const QString& supplySourceDeviceId,
                                                           const QString& supplyTargetDeviceId,
                                                           const QString& groundSourceDeviceId) const;

    void rebuildTopologyIndex(LzScene* scene) const;
    const TopologyGraphIndex* topologyIndex(const LzScene* scene) const;
    TopologyGraphIndex* topologyIndex(LzScene* scene) const;
    bool highlightTopologyNode(LzScene* scene, const QString& nodeStableId, bool on, const QColor& c) const;
    bool highlightTopologyEdge(LzScene* scene, const QString& edgeStableId, bool on, const QColor& c) const;
    bool highlightTopologyEdges(LzScene* scene, const QStringList& edgeStableIds, bool on, const QColor& c) const;
    bool clearTopologyHighlights(LzScene* scene) const;
    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(LzScene* scene) const;
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(LzScene* scene) const;

    QVector<int> powerDeviceNodeIds(LzScene* scene, const QString& deviceId) const;
    QStringList powerDeviceConductorIds(LzScene* scene, const QString& deviceId) const;
    QStringList connectedPowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList busbarAttachedPowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList reachablePowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList directedReachablePowerDevices(LzScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyIslandAnalysis analyzePowerIslands(LzScene* scene) const;
    PowerTopologyBranchAnalysis analyzePowerBranches(LzScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis analyzePowerLoops(LzScene* scene) const;
    PowerTopologyPath shortestPowerSupplyPath(LzScene* scene,
                                              const QString& sourceDeviceId,
                                              const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedPowerSupplyPath(LzScene* scene,
                                                      const QString& sourceDeviceId,
                                                      const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestPowerGroundPath(LzScene* scene, const QString& sourceDeviceId) const;
    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(LzScene* scene,
                                                                      const QString& deviceId,
                                                                      int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(LzScene* scene,
                                                                    const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview previewPowerSwitchOperation(LzScene* scene,
                                                              const QString& deviceId,
                                                              int toSwitchPosition,
                                                              const QString& supplySourceDeviceId,
                                                              const QString& supplyTargetDeviceId,
                                                              const QString& groundSourceDeviceId) const;

    QVariantMap buildPowerTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap buildTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap queryTopology(LzScene* scene, const QVariantMap& request) const;
};

#endif
