#include "scene/CimdrawSceneTopologySlotWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneTopology(CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneTopology(const CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

bool CimdrawSceneTopologySlotWorkbenchController::dispatch(CimdrawScene* scene, SceneAction action) const
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

bool CimdrawSceneTopologySlotWorkbenchController::dispatch(CimdrawScene* scene,
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

bool CimdrawSceneTopologySlotWorkbenchController::dispatch(CimdrawScene* scene,
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
PowerTopologyAnalysisSnapshot CimdrawSceneTopologySlotWorkbenchController::dispatchSnapshot(
    CimdrawScene* scene,
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

PowerTopologyDocumentExport CimdrawSceneTopologySlotWorkbenchController::dispatchDocumentExport(
    CimdrawScene* scene,
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

QVector<int> CimdrawSceneTopologySlotWorkbenchController::dispatchDeviceNodeIds(CimdrawScene* scene,
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

QStringList CimdrawSceneTopologySlotWorkbenchController::dispatchDeviceQuery(CimdrawScene* scene,
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

PowerTopologyIslandAnalysis CimdrawSceneTopologySlotWorkbenchController::dispatchIslandAnalysis(
    CimdrawScene* scene,
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

PowerTopologyBranchAnalysis CimdrawSceneTopologySlotWorkbenchController::dispatchBranchAnalysis(
    CimdrawScene* scene,
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

PowerTopologyLoopAnalysis CimdrawSceneTopologySlotWorkbenchController::dispatchLoopAnalysis(
    CimdrawScene* scene,
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

PowerTopologyPath CimdrawSceneTopologySlotWorkbenchController::dispatchPath(CimdrawScene* scene,
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

PowerTopologyGroundPath CimdrawSceneTopologySlotWorkbenchController::dispatchGroundPath(
    CimdrawScene* scene,
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

PowerTopologySwitchChangePreview CimdrawSceneTopologySlotWorkbenchController::dispatchSwitchChangePreview(
    CimdrawScene* scene,
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
CimdrawSceneTopologySlotWorkbenchController::dispatchProtectionRangePreview(
    CimdrawScene* scene,
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

PowerTopologyOperationPreview CimdrawSceneTopologySlotWorkbenchController::dispatchOperationPreview(
    CimdrawScene* scene,
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

void CimdrawSceneTopologySlotWorkbenchController::rebuildTopologyIndex(CimdrawScene* scene) const
{
    runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneTopologyWorkbenchController controller;
        controller.rebuildTopologyIndex(currentScene, currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneTopologySlotWorkbenchController::highlightTopologyNode(CimdrawScene* scene,
                                                                   const QString& nodeStableId,
                                                                   bool on,
                                                                   const QColor& c) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneTopologyWorkbenchController controller;
        controller.highlightTopologyNode(currentScene->topologyStateStorage(), nodeStableId, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneTopologySlotWorkbenchController::highlightTopologyEdge(CimdrawScene* scene,
                                                                   const QString& edgeStableId,
                                                                   bool on,
                                                                   const QColor& c) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneTopologyWorkbenchController controller;
        controller.highlightTopologyEdge(currentScene->topologyStateStorage(), edgeStableId, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneTopologySlotWorkbenchController::highlightTopologyEdges(CimdrawScene* scene,
                                                                    const QStringList& edgeStableIds,
                                                                    bool on,
                                                                    const QColor& c) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneTopologyWorkbenchController controller;
        controller.highlightTopologyEdges(currentScene->topologyStateStorage(), edgeStableIds, on, c);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneTopologySlotWorkbenchController::clearTopologyHighlights(CimdrawScene* scene) const
{
    return runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneTopologyWorkbenchController controller;
        controller.clearTopologyHighlights(currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

PowerTopologyAnalysisSnapshot CimdrawSceneTopologySlotWorkbenchController::buildPowerTopologySnapshot(CimdrawScene* scene) const
{
    return runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        return currentScene->buildPowerTopologySnapshot();
    }, []()
    {
        return PowerTopologyAnalysisSnapshot{};
    });
}

PowerTopologyDocumentExport CimdrawSceneTopologySlotWorkbenchController::buildPowerTopologyDocumentExport(CimdrawScene* scene) const
{
    return runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        return currentScene->buildPowerTopologyDocumentExport();
    }, []()
    {
        return PowerTopologyDocumentExport{};
    });
}

QVector<int> CimdrawSceneTopologySlotWorkbenchController::powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->powerDeviceNodeIds(deviceId);
    }, []()
    {
        return QVector<int>();
    });
}

QStringList CimdrawSceneTopologySlotWorkbenchController::powerDeviceConductorIds(CimdrawScene* scene,
                                                                            const QString& deviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->powerDeviceConductorIds(deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList CimdrawSceneTopologySlotWorkbenchController::connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->connectedPowerDevices(deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList CimdrawSceneTopologySlotWorkbenchController::busbarAttachedPowerDevices(CimdrawScene* scene,
                                                                               const QString& deviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->busbarAttachedPowerDevices(deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList CimdrawSceneTopologySlotWorkbenchController::reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->reachablePowerDevices(deviceId);
    }, []()
    {
        return QStringList();
    });
}

QStringList CimdrawSceneTopologySlotWorkbenchController::directedReachablePowerDevices(CimdrawScene* scene,
                                                                                  const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->directedReachablePowerDevices(sourceDeviceId);
    }, []()
    {
        return QStringList();
    });
}

PowerTopologyIslandAnalysis CimdrawSceneTopologySlotWorkbenchController::analyzePowerIslands(CimdrawScene* scene) const
{
    return runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        return currentScene->analyzePowerIslands();
    }, []()
    {
        return PowerTopologyIslandAnalysis{};
    });
}

PowerTopologyBranchAnalysis CimdrawSceneTopologySlotWorkbenchController::analyzePowerBranches(
    CimdrawScene* scene,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->analyzePowerBranches(sourceDeviceId);
    }, []()
    {
        return PowerTopologyBranchAnalysis{};
    });
}

PowerTopologyLoopAnalysis CimdrawSceneTopologySlotWorkbenchController::analyzePowerLoops(CimdrawScene* scene) const
{
    return runSceneTopology(scene, [](CimdrawScene* currentScene)
    {
        return currentScene->analyzePowerLoops();
    }, []()
    {
        return PowerTopologyLoopAnalysis{};
    });
}

PowerTopologyPath CimdrawSceneTopologySlotWorkbenchController::shortestPowerSupplyPath(
    CimdrawScene* scene,
    const QString& sourceDeviceId,
    const QString& targetDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->shortestPowerSupplyPath(sourceDeviceId, targetDeviceId);
    }, []()
    {
        return PowerTopologyPath{};
    });
}

PowerTopologyPath CimdrawSceneTopologySlotWorkbenchController::shortestDirectedPowerSupplyPath(
    CimdrawScene* scene,
    const QString& sourceDeviceId,
    const QString& targetDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->shortestDirectedPowerSupplyPath(sourceDeviceId, targetDeviceId);
    }, []()
    {
        return PowerTopologyPath{};
    });
}

PowerTopologyGroundPath CimdrawSceneTopologySlotWorkbenchController::shortestPowerGroundPath(
    CimdrawScene* scene,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->shortestPowerGroundPath(sourceDeviceId);
    }, []()
    {
        return PowerTopologyGroundPath{};
    });
}

PowerTopologySwitchChangePreview CimdrawSceneTopologySlotWorkbenchController::previewPowerSwitchTopologyChange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->previewPowerSwitchTopologyChange(deviceId, toSwitchPosition);
    }, []()
    {
        return PowerTopologySwitchChangePreview{};
    });
}

PowerTopologyProtectionRangePreview CimdrawSceneTopologySlotWorkbenchController::previewPowerProtectionRange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->previewPowerProtectionRange(deviceId, toSwitchPosition, sourceDeviceId);
    }, []()
    {
        return PowerTopologyProtectionRangePreview{};
    });
}

PowerTopologyOperationPreview CimdrawSceneTopologySlotWorkbenchController::previewPowerSwitchOperation(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    return runSceneTopology(scene, [&](CimdrawScene* currentScene)
    {
        return currentScene->previewPowerSwitchOperation(deviceId,
                                                         toSwitchPosition,
                                                         supplySourceDeviceId,
                                                         supplyTargetDeviceId,
                                                         groundSourceDeviceId);
    }, []()
    {
        return PowerTopologyOperationPreview{};
    });
}
