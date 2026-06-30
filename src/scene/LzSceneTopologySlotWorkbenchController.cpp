#include "scene/LzSceneTopologySlotWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneTopologyWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneTopology(LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneTopology(const LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

bool LzSceneTopologySlotWorkbenchController::dispatch(LzScene* scene, SceneAction action) const
{
    switch (action)
    {
    case SceneAction::RebuildTopologyIndex:
        rebuildTopologyIndex(scene);
        return scene != nullptr;
    case SceneAction::ClearTopologyHighlights:
        return clearTopologyHighlights(scene);
    }

    return false;
}

bool LzSceneTopologySlotWorkbenchController::dispatch(LzScene* scene,
                                                      HighlightAction action,
                                                      const QString& stableId,
                                                      bool on,
                                                      const QColor& c) const
{
    switch (action)
    {
    case HighlightAction::Node:
        return highlightTopologyNode(scene, stableId, on, c);
    case HighlightAction::Edge:
        return highlightTopologyEdge(scene, stableId, on, c);
    case HighlightAction::Edges:
        break;
    }

    return false;
}

bool LzSceneTopologySlotWorkbenchController::dispatch(LzScene* scene,
                                                      HighlightAction action,
                                                      const QStringList& stableIds,
                                                      bool on,
                                                      const QColor& c) const
{
    switch (action)
    {
    case HighlightAction::Edges:
        return highlightTopologyEdges(scene, stableIds, on, c);
    case HighlightAction::Node:
    case HighlightAction::Edge:
        break;
    }

    return false;
}

const TopologyGraphIndex* LzSceneTopologySlotWorkbenchController::dispatch(const LzScene* scene,
                                                                           IndexAction action) const
{
    switch (action)
    {
    case IndexAction::TopologyIndex:
        return topologyIndex(scene);
    }

    return nullptr;
}

TopologyGraphIndex* LzSceneTopologySlotWorkbenchController::dispatch(LzScene* scene, IndexAction action) const
{
    switch (action)
    {
    case IndexAction::TopologyIndex:
        return topologyIndex(scene);
    }

    return nullptr;
}

QVariantMap LzSceneTopologySlotWorkbenchController::dispatchMap(LzScene* scene,
                                                                MapAction action,
                                                                const QVariantMap& request) const
{
    switch (action)
    {
    case MapAction::BuildPowerTopologyBindingSnapshot:
        return buildPowerTopologyBindingSnapshot(scene);
    case MapAction::BuildTopologyBindingSnapshot:
        return buildTopologyBindingSnapshot(scene);
    case MapAction::QueryTopology:
        return queryTopology(scene, request);
    }

    return QVariantMap();
}

PowerTopologyAnalysisSnapshot LzSceneTopologySlotWorkbenchController::dispatchSnapshot(
    LzScene* scene,
    SnapshotAction action) const
{
    switch (action)
    {
    case SnapshotAction::BuildPowerTopologySnapshot:
        return buildPowerTopologySnapshot(scene);
    case SnapshotAction::BuildPowerTopologyDocumentExport:
        break;
    }

    return PowerTopologyAnalysisSnapshot{};
}

PowerTopologyDocumentExport LzSceneTopologySlotWorkbenchController::dispatchDocumentExport(
    LzScene* scene,
    SnapshotAction action) const
{
    switch (action)
    {
    case SnapshotAction::BuildPowerTopologyDocumentExport:
        return buildPowerTopologyDocumentExport(scene);
    case SnapshotAction::BuildPowerTopologySnapshot:
        break;
    }

    return PowerTopologyDocumentExport{};
}

QVector<int> LzSceneTopologySlotWorkbenchController::dispatchDeviceNodeIds(LzScene* scene,
                                                                           DeviceQueryAction action,
                                                                           const QString& deviceId) const
{
    switch (action)
    {
    case DeviceQueryAction::PowerDeviceNodeIds:
        return powerDeviceNodeIds(scene, deviceId);
    case DeviceQueryAction::PowerDeviceConductorIds:
    case DeviceQueryAction::ConnectedPowerDevices:
    case DeviceQueryAction::BusbarAttachedPowerDevices:
    case DeviceQueryAction::ReachablePowerDevices:
    case DeviceQueryAction::DirectedReachablePowerDevices:
        break;
    }

    return QVector<int>();
}

QStringList LzSceneTopologySlotWorkbenchController::dispatchDeviceQuery(LzScene* scene,
                                                                        DeviceQueryAction action,
                                                                        const QString& deviceId) const
{
    switch (action)
    {
    case DeviceQueryAction::PowerDeviceConductorIds:
        return powerDeviceConductorIds(scene, deviceId);
    case DeviceQueryAction::ConnectedPowerDevices:
        return connectedPowerDevices(scene, deviceId);
    case DeviceQueryAction::BusbarAttachedPowerDevices:
        return busbarAttachedPowerDevices(scene, deviceId);
    case DeviceQueryAction::ReachablePowerDevices:
        return reachablePowerDevices(scene, deviceId);
    case DeviceQueryAction::DirectedReachablePowerDevices:
        return directedReachablePowerDevices(scene, deviceId);
    case DeviceQueryAction::PowerDeviceNodeIds:
        break;
    }

    return QStringList();
}

PowerTopologyIslandAnalysis LzSceneTopologySlotWorkbenchController::dispatchIslandAnalysis(
    LzScene* scene,
    AnalysisAction action) const
{
    switch (action)
    {
    case AnalysisAction::AnalyzePowerIslands:
        return analyzePowerIslands(scene);
    case AnalysisAction::AnalyzePowerBranches:
    case AnalysisAction::AnalyzePowerLoops:
        break;
    }

    return PowerTopologyIslandAnalysis{};
}

PowerTopologyBranchAnalysis LzSceneTopologySlotWorkbenchController::dispatchBranchAnalysis(
    LzScene* scene,
    AnalysisAction action,
    const QString& sourceDeviceId) const
{
    switch (action)
    {
    case AnalysisAction::AnalyzePowerBranches:
        return analyzePowerBranches(scene, sourceDeviceId);
    case AnalysisAction::AnalyzePowerIslands:
    case AnalysisAction::AnalyzePowerLoops:
        break;
    }

    return PowerTopologyBranchAnalysis{};
}

PowerTopologyLoopAnalysis LzSceneTopologySlotWorkbenchController::dispatchLoopAnalysis(
    LzScene* scene,
    AnalysisAction action) const
{
    switch (action)
    {
    case AnalysisAction::AnalyzePowerLoops:
        return analyzePowerLoops(scene);
    case AnalysisAction::AnalyzePowerIslands:
    case AnalysisAction::AnalyzePowerBranches:
        break;
    }

    return PowerTopologyLoopAnalysis{};
}

PowerTopologyPath LzSceneTopologySlotWorkbenchController::dispatchPath(LzScene* scene,
                                                                       PathAction action,
                                                                       const QString& sourceDeviceId,
                                                                       const QString& targetDeviceId) const
{
    switch (action)
    {
    case PathAction::ShortestPowerSupplyPath:
        return shortestPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
    case PathAction::ShortestDirectedPowerSupplyPath:
        return shortestDirectedPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
    case PathAction::ShortestPowerGroundPath:
    case PathAction::PreviewPowerSwitchTopologyChange:
    case PathAction::PreviewPowerProtectionRange:
    case PathAction::PreviewPowerSwitchOperation:
        break;
    }

    return PowerTopologyPath{};
}

PowerTopologyGroundPath LzSceneTopologySlotWorkbenchController::dispatchGroundPath(
    LzScene* scene,
    PathAction action,
    const QString& sourceDeviceId) const
{
    switch (action)
    {
    case PathAction::ShortestPowerGroundPath:
        return shortestPowerGroundPath(scene, sourceDeviceId);
    case PathAction::ShortestPowerSupplyPath:
    case PathAction::ShortestDirectedPowerSupplyPath:
    case PathAction::PreviewPowerSwitchTopologyChange:
    case PathAction::PreviewPowerProtectionRange:
    case PathAction::PreviewPowerSwitchOperation:
        break;
    }

    return PowerTopologyGroundPath{};
}

PowerTopologySwitchChangePreview LzSceneTopologySlotWorkbenchController::dispatchSwitchChangePreview(
    LzScene* scene,
    PathAction action,
    const QString& deviceId,
    int toSwitchPosition) const
{
    switch (action)
    {
    case PathAction::PreviewPowerSwitchTopologyChange:
        return previewPowerSwitchTopologyChange(scene, deviceId, toSwitchPosition);
    case PathAction::ShortestPowerSupplyPath:
    case PathAction::ShortestDirectedPowerSupplyPath:
    case PathAction::ShortestPowerGroundPath:
    case PathAction::PreviewPowerProtectionRange:
    case PathAction::PreviewPowerSwitchOperation:
        break;
    }

    return PowerTopologySwitchChangePreview{};
}

PowerTopologyProtectionRangePreview
LzSceneTopologySlotWorkbenchController::dispatchProtectionRangePreview(
    LzScene* scene,
    PathAction action,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    switch (action)
    {
    case PathAction::PreviewPowerProtectionRange:
        return previewPowerProtectionRange(scene, deviceId, toSwitchPosition, sourceDeviceId);
    case PathAction::ShortestPowerSupplyPath:
    case PathAction::ShortestDirectedPowerSupplyPath:
    case PathAction::ShortestPowerGroundPath:
    case PathAction::PreviewPowerSwitchTopologyChange:
    case PathAction::PreviewPowerSwitchOperation:
        break;
    }

    return PowerTopologyProtectionRangePreview{};
}

PowerTopologyOperationPreview LzSceneTopologySlotWorkbenchController::dispatchOperationPreview(
    LzScene* scene,
    PathAction action,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    switch (action)
    {
    case PathAction::PreviewPowerSwitchOperation:
        return previewPowerSwitchOperation(scene,
                                           deviceId,
                                           toSwitchPosition,
                                           supplySourceDeviceId,
                                           supplyTargetDeviceId,
                                           groundSourceDeviceId);
    case PathAction::ShortestPowerSupplyPath:
    case PathAction::ShortestDirectedPowerSupplyPath:
    case PathAction::ShortestPowerGroundPath:
    case PathAction::PreviewPowerSwitchTopologyChange:
    case PathAction::PreviewPowerProtectionRange:
        break;
    }

    return PowerTopologyOperationPreview{};
}

void LzSceneTopologySlotWorkbenchController::rebuildTopologyIndex(LzScene* scene) const
{
    runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        controller.rebuildTopologyIndex(currentScene, currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

const TopologyGraphIndex* LzSceneTopologySlotWorkbenchController::topologyIndex(const LzScene* scene) const
{
    return runConstSceneTopology(scene, [](const LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.topologyIndex(currentScene->topologyStateStorage());
    }, []() -> const TopologyGraphIndex*
    {
        return nullptr;
    });
}

TopologyGraphIndex* LzSceneTopologySlotWorkbenchController::topologyIndex(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.topologyIndex(currentScene->topologyStateStorage());
    }, []() -> TopologyGraphIndex*
    {
        return nullptr;
    });
}

bool LzSceneTopologySlotWorkbenchController::highlightTopologyNode(LzScene* scene,
                                                                   const QString& nodeStableId,
                                                                   bool on,
                                                                   const QColor& c) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        controller.highlightTopologyNode(currentScene->topologyStateStorage(), nodeStableId, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneTopologySlotWorkbenchController::highlightTopologyEdge(LzScene* scene,
                                                                   const QString& edgeStableId,
                                                                   bool on,
                                                                   const QColor& c) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        controller.highlightTopologyEdge(currentScene->topologyStateStorage(), edgeStableId, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneTopologySlotWorkbenchController::highlightTopologyEdges(LzScene* scene,
                                                                    const QStringList& edgeStableIds,
                                                                    bool on,
                                                                    const QColor& c) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        controller.highlightTopologyEdges(currentScene->topologyStateStorage(), edgeStableIds, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneTopologySlotWorkbenchController::clearTopologyHighlights(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        controller.clearTopologyHighlights(currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

PowerTopologyAnalysisSnapshot LzSceneTopologySlotWorkbenchController::buildPowerTopologySnapshot(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.buildPowerTopologySnapshot(currentScene, currentScene->topologyStateStorage());
    }, []()
    {
        return PowerTopologyAnalysisSnapshot{};
    });
}

PowerTopologyDocumentExport LzSceneTopologySlotWorkbenchController::buildPowerTopologyDocumentExport(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.buildPowerTopologyDocumentExport(currentScene, currentScene->topologyStateStorage());
    }, []()
    {
        return PowerTopologyDocumentExport{};
    });
}

QVector<int> LzSceneTopologySlotWorkbenchController::powerDeviceNodeIds(LzScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.powerDeviceNodeIds(currentScene, deviceId);
    }, []()
    {
        return QVector<int>();
    });
}

QStringList LzSceneTopologySlotWorkbenchController::powerDeviceConductorIds(LzScene* scene,
                                                                            const QString& deviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.powerDeviceConductorIds(currentScene, deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList LzSceneTopologySlotWorkbenchController::connectedPowerDevices(LzScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.connectedPowerDevices(currentScene, deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList LzSceneTopologySlotWorkbenchController::busbarAttachedPowerDevices(LzScene* scene,
                                                                               const QString& deviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.busbarAttachedPowerDevices(currentScene, deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList LzSceneTopologySlotWorkbenchController::reachablePowerDevices(LzScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.reachablePowerDevices(currentScene, deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList LzSceneTopologySlotWorkbenchController::directedReachablePowerDevices(LzScene* scene,
                                                                                  const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.directedReachablePowerDevices(currentScene, sourceDeviceId);
    }, []()
    {
        return QStringList();
    });
}

PowerTopologyIslandAnalysis LzSceneTopologySlotWorkbenchController::analyzePowerIslands(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.analyzePowerIslands(currentScene);
    }, []()
    {
        return PowerTopologyIslandAnalysis{};
    });
}

PowerTopologyBranchAnalysis LzSceneTopologySlotWorkbenchController::analyzePowerBranches(
    LzScene* scene,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.analyzePowerBranches(currentScene, sourceDeviceId);
    }, []()
    {
        return PowerTopologyBranchAnalysis{};
    });
}

PowerTopologyLoopAnalysis LzSceneTopologySlotWorkbenchController::analyzePowerLoops(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.analyzePowerLoops(currentScene);
    }, []()
    {
        return PowerTopologyLoopAnalysis{};
    });
}

PowerTopologyPath LzSceneTopologySlotWorkbenchController::shortestPowerSupplyPath(
    LzScene* scene,
    const QString& sourceDeviceId,
    const QString& targetDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.shortestPowerSupplyPath(currentScene, sourceDeviceId, targetDeviceId);
    }, []()
    {
        return PowerTopologyPath{};
    });
}

PowerTopologyPath LzSceneTopologySlotWorkbenchController::shortestDirectedPowerSupplyPath(
    LzScene* scene,
    const QString& sourceDeviceId,
    const QString& targetDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.shortestDirectedPowerSupplyPath(currentScene, sourceDeviceId, targetDeviceId);
    }, []()
    {
        return PowerTopologyPath{};
    });
}

PowerTopologyGroundPath LzSceneTopologySlotWorkbenchController::shortestPowerGroundPath(
    LzScene* scene,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.shortestPowerGroundPath(currentScene, sourceDeviceId);
    }, []()
    {
        return PowerTopologyGroundPath{};
    });
}

PowerTopologySwitchChangePreview LzSceneTopologySlotWorkbenchController::previewPowerSwitchTopologyChange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.previewPowerSwitchTopologyChange(currentScene, deviceId, toSwitchPosition);
    }, []()
    {
        return PowerTopologySwitchChangePreview{};
    });
}

PowerTopologyProtectionRangePreview LzSceneTopologySlotWorkbenchController::previewPowerProtectionRange(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.previewPowerProtectionRange(currentScene, deviceId, toSwitchPosition, sourceDeviceId);
    }, []()
    {
        return PowerTopologyProtectionRangePreview{};
    });
}

PowerTopologyOperationPreview LzSceneTopologySlotWorkbenchController::previewPowerSwitchOperation(
    LzScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.previewPowerSwitchOperation(currentScene,
                                                      deviceId,
                                                      toSwitchPosition,
                                                      supplySourceDeviceId,
                                                      supplyTargetDeviceId,
                                                      groundSourceDeviceId);
    }, []()
    {
        return PowerTopologyOperationPreview{};
    });
}

QVariantMap LzSceneTopologySlotWorkbenchController::buildPowerTopologyBindingSnapshot(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.buildPowerTopologyBindingSnapshot(currentScene);
    }, []()
    {
        return QVariantMap();
    });
}

QVariantMap LzSceneTopologySlotWorkbenchController::buildTopologyBindingSnapshot(LzScene* scene) const
{
    return runSceneTopology(scene, [](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.buildTopologyBindingSnapshot(currentScene);
    }, []()
    {
        return QVariantMap();
    });
}

QVariantMap LzSceneTopologySlotWorkbenchController::queryTopology(LzScene* scene, const QVariantMap& request) const
{
    return runSceneTopology(scene, [&](LzScene* currentScene)
    {
        LzSceneTopologyWorkbenchController controller;
        return controller.queryTopology(currentScene, request);
    }, []()
    {
        return QVariantMap();
    });
}
