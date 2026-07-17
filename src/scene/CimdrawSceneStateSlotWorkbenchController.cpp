#include "scene/CimdrawSceneStateSlotWorkbenchController.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneStateWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneState(CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneState(const CimdrawScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

CimdrawView* CimdrawSceneStateSlotWorkbenchController::view(CimdrawScene* scene) const
{
    return runSceneState(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.view(currentScene->viewStorage());
    }, []() -> CimdrawView*
    {
        return nullptr;
    });
}

CimdrawView* CimdrawSceneStateSlotWorkbenchController::view(const CimdrawScene* scene) const
{
    return runConstSceneState(scene, [](const CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.view(currentScene->viewStorage());
    }, []() -> CimdrawView*
    {
        return nullptr;
    });
}

CimdrawView* CimdrawSceneStateSlotWorkbenchController::dispatch(CimdrawScene* scene, ViewAction action) const
{
    switch (action)
    {
    case ViewAction::Get:
        return view(scene);
    }

    return nullptr;
}

CimdrawView* CimdrawSceneStateSlotWorkbenchController::dispatch(const CimdrawScene* scene, ViewAction action) const
{
    switch (action)
    {
    case ViewAction::Get:
        return view(scene);
    }

    return nullptr;
}

bool CimdrawSceneStateSlotWorkbenchController::flagState(const CimdrawScene* scene, FlagStateAction action) const
{
    switch (action)
    {
    case FlagStateAction::PaintState:
        return paintState(scene);
    case FlagStateAction::InteractiveTransformActive:
        return interactiveTransformActive(scene);
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::setView(CimdrawScene* scene, CimdrawView* nextView) const
{
    return runSceneState(scene, [nextView](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.setView(currentScene->viewStorage(), nextView);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                   FlagStateAction action,
                                                   bool nextState) const
{
    switch (action)
    {
    case FlagStateAction::PaintState:
        return setPaintState(scene, nextState);
    case FlagStateAction::InteractiveTransformActive:
        return setInteractiveTransformActive(scene, nextState);
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::dispatch(CimdrawScene* scene, DeferredSceneAction action) const
{
    switch (action)
    {
    case DeferredSceneAction::InvalidateTopologySnapshots:
        return invalidateTopologySnapshots(scene);
    case DeferredSceneAction::ScheduleTopologyRebuild:
        return scheduleTopologyRebuild(scene);
    case DeferredSceneAction::RequestDeferredItemPropertyPanel:
        return requestDeferredItemPropertyPanel(scene);
    case DeferredSceneAction::FlushDeferredItemPropertyPanel:
        return flushDeferredItemPropertyPanel(scene);
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                   DeferredLineAction action,
                                                   CimdrawConnectLine* line) const
{
    switch (action)
    {
    case DeferredLineAction::CancelConnectLinePathRecompute:
        return cancelConnectLinePathRecompute(scene, line);
    case DeferredLineAction::ScheduleConnectLinesPathRecompute:
    case DeferredLineAction::ScheduleConnectorPostprocess:
        break;
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::dispatch(CimdrawScene* scene,
                                                   DeferredLineAction action,
                                                   const QList<CimdrawConnectLine*>& lines) const
{
    switch (action)
    {
    case DeferredLineAction::ScheduleConnectLinesPathRecompute:
        return scheduleConnectLinesPathRecompute(scene, lines);
    case DeferredLineAction::ScheduleConnectorPostprocess:
        return scheduleConnectorPostprocess(scene, lines);
    case DeferredLineAction::CancelConnectLinePathRecompute:
        break;
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::dispatch(const CimdrawScene* scene, DeferredQueryAction action) const
{
    switch (action)
    {
    case DeferredQueryAction::IsConnectLinePathUpdatePending:
        return isConnectLinePathUpdatePending(scene);
    }

    return false;
}

bool CimdrawSceneStateSlotWorkbenchController::dispatchWiringData(CimdrawScene* scene,
                                                             const QString& key,
                                                             const QVariant& value) const
{
    return setWiringData(scene, key, value);
}

QVariant CimdrawSceneStateSlotWorkbenchController::dispatch(const CimdrawScene* scene,
                                                       WiringDataAction action,
                                                       const QString& key) const
{
    switch (action)
    {
    case WiringDataAction::Get:
        return wiringData(scene, key);
    }

    return QVariant();
}

bool CimdrawSceneStateSlotWorkbenchController::paintState(const CimdrawScene* scene) const
{
    return runConstSceneState(scene, [](const CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.paintState(currentScene->paintStateStorage());
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::setPaintState(CimdrawScene* scene, bool nextState) const
{
    return runSceneState(scene, [nextState](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.setPaintState(currentScene->paintStateStorage(), nextState);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::interactiveTransformActive(const CimdrawScene* scene) const
{
    return runConstSceneState(scene, [](const CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.interactiveTransformActive(currentScene->interactiveTransformStorage());
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::setInteractiveTransformActive(CimdrawScene* scene, bool nextState) const
{
    return runSceneState(scene, [nextState](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.setInteractiveTransformActive(currentScene->interactiveTransformStorage(), nextState);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::invalidateTopologySnapshots(CimdrawScene* scene) const
{
    return runSceneState(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.invalidateTopologySnapshots(currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::scheduleTopologyRebuild(CimdrawScene* scene) const
{
    return runSceneState(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.scheduleTopologyRebuild(currentScene,
                                           currentScene->deferredUpdateStorage(),
                                           currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::cancelConnectLinePathRecompute(CimdrawScene* scene, CimdrawConnectLine* line) const
{
    return runSceneState(scene, [line](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.cancelConnectLinePathRecompute(currentScene->deferredUpdateStorage(), line);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::isConnectLinePathUpdatePending(const CimdrawScene* scene) const
{
    return runConstSceneState(scene, [](const CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.isConnectLinePathUpdatePending(currentScene->deferredUpdateStorage());
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::requestDeferredItemPropertyPanel(CimdrawScene* scene) const
{
    return runSceneState(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.requestDeferredItemPropertyPanel(currentScene->deferredUpdateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::flushDeferredItemPropertyPanel(CimdrawScene* scene) const
{
    return runSceneState(scene, [](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.flushDeferredItemPropertyPanel(currentScene, currentScene->deferredUpdateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::scheduleConnectLinesPathRecompute(
    CimdrawScene* scene,
    const QList<CimdrawConnectLine*>& lines) const
{
    return runSceneState(scene, [&lines](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.scheduleConnectLinesPathRecompute(currentScene, currentScene->deferredUpdateStorage(), lines);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::scheduleConnectorPostprocess(
    CimdrawScene* scene,
    const QList<CimdrawConnectLine*>& affectedLines) const
{
    return runSceneState(scene, [&affectedLines](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        controller.scheduleConnectorPostprocess(currentScene,
                                                currentScene->deferredUpdateStorage(),
                                                affectedLines);
        return true;
    }, []()
    {
        return false;
    });
}

bool CimdrawSceneStateSlotWorkbenchController::setWiringData(CimdrawScene* scene, const QString& key, const QVariant& value) const
{
    return runSceneState(scene, [&](CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.setWiringData(currentScene->wiringDataStorage(),
                                        currentScene->topologyStateStorage(),
                                        key,
                                        value);
    }, []()
    {
        return false;
    });
}

QVariant CimdrawSceneStateSlotWorkbenchController::wiringData(const CimdrawScene* scene, const QString& key) const
{
    return runConstSceneState(scene, [&](const CimdrawScene* currentScene)
    {
        CimdrawSceneStateWorkbenchController controller;
        return controller.wiringData(currentScene->wiringDataStorage(), key);
    }, []()
    {
        return QVariant();
    });
}
