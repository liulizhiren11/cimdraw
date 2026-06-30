#include "scene/LzSceneStateSlotWorkbenchController.h"

#include "LzScene.h"
#include "scene/LzSceneStateWorkbenchController.h"

namespace {

template <typename Fn, typename Fallback>
auto runSceneState(LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

template <typename Fn, typename Fallback>
auto runConstSceneState(const LzScene* scene, Fn&& fn, Fallback&& fallback) -> decltype(fn(scene))
{
    if (!scene)
        return fallback();
    return fn(scene);
}

} // namespace

LzView* LzSceneStateSlotWorkbenchController::view(LzScene* scene) const
{
    return runSceneState(scene, [](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.view(currentScene->viewStorage());
    }, []() -> LzView*
    {
        return nullptr;
    });
}

LzView* LzSceneStateSlotWorkbenchController::view(const LzScene* scene) const
{
    return runConstSceneState(scene, [](const LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.view(currentScene->viewStorage());
    }, []() -> LzView*
    {
        return nullptr;
    });
}

LzView* LzSceneStateSlotWorkbenchController::dispatch(LzScene* scene, ViewAction action) const
{
    switch (action)
    {
    case ViewAction::Get:
        return view(scene);
    }

    return nullptr;
}

LzView* LzSceneStateSlotWorkbenchController::dispatch(const LzScene* scene, ViewAction action) const
{
    switch (action)
    {
    case ViewAction::Get:
        return view(scene);
    }

    return nullptr;
}

bool LzSceneStateSlotWorkbenchController::flagState(const LzScene* scene, FlagStateAction action) const
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

bool LzSceneStateSlotWorkbenchController::setView(LzScene* scene, LzView* nextView) const
{
    return runSceneState(scene, [nextView](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.setView(currentScene->viewStorage(), nextView);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::dispatch(LzScene* scene,
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

bool LzSceneStateSlotWorkbenchController::dispatch(LzScene* scene, DeferredSceneAction action) const
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

bool LzSceneStateSlotWorkbenchController::dispatch(LzScene* scene,
                                                   DeferredLineAction action,
                                                   LzConnectLine* line) const
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

bool LzSceneStateSlotWorkbenchController::dispatch(LzScene* scene,
                                                   DeferredLineAction action,
                                                   const QList<LzConnectLine*>& lines) const
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

bool LzSceneStateSlotWorkbenchController::dispatch(const LzScene* scene, DeferredQueryAction action) const
{
    switch (action)
    {
    case DeferredQueryAction::IsConnectLinePathUpdatePending:
        return isConnectLinePathUpdatePending(scene);
    }

    return false;
}

bool LzSceneStateSlotWorkbenchController::dispatchWiringData(LzScene* scene,
                                                             const QString& key,
                                                             const QVariant& value) const
{
    return setWiringData(scene, key, value);
}

QVariant LzSceneStateSlotWorkbenchController::dispatch(const LzScene* scene,
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

bool LzSceneStateSlotWorkbenchController::paintState(const LzScene* scene) const
{
    return runConstSceneState(scene, [](const LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.paintState(currentScene->paintStateStorage());
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::setPaintState(LzScene* scene, bool nextState) const
{
    return runSceneState(scene, [nextState](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.setPaintState(currentScene->paintStateStorage(), nextState);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::interactiveTransformActive(const LzScene* scene) const
{
    return runConstSceneState(scene, [](const LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.interactiveTransformActive(currentScene->interactiveTransformStorage());
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::setInteractiveTransformActive(LzScene* scene, bool nextState) const
{
    return runSceneState(scene, [nextState](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.setInteractiveTransformActive(currentScene->interactiveTransformStorage(), nextState);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::invalidateTopologySnapshots(LzScene* scene) const
{
    return runSceneState(scene, [](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.invalidateTopologySnapshots(currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::scheduleTopologyRebuild(LzScene* scene) const
{
    return runSceneState(scene, [](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.scheduleTopologyRebuild(currentScene,
                                           currentScene->deferredUpdateStorage(),
                                           currentScene->topologyStateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::cancelConnectLinePathRecompute(LzScene* scene, LzConnectLine* line) const
{
    return runSceneState(scene, [line](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.cancelConnectLinePathRecompute(currentScene->deferredUpdateStorage(), line);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::isConnectLinePathUpdatePending(const LzScene* scene) const
{
    return runConstSceneState(scene, [](const LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.isConnectLinePathUpdatePending(currentScene->deferredUpdateStorage());
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::requestDeferredItemPropertyPanel(LzScene* scene) const
{
    return runSceneState(scene, [](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.requestDeferredItemPropertyPanel(currentScene->deferredUpdateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::flushDeferredItemPropertyPanel(LzScene* scene) const
{
    return runSceneState(scene, [](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.flushDeferredItemPropertyPanel(currentScene, currentScene->deferredUpdateStorage());
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::scheduleConnectLinesPathRecompute(
    LzScene* scene,
    const QList<LzConnectLine*>& lines) const
{
    return runSceneState(scene, [&lines](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.scheduleConnectLinesPathRecompute(currentScene, currentScene->deferredUpdateStorage(), lines);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::scheduleConnectorPostprocess(
    LzScene* scene,
    const QList<LzConnectLine*>& affectedLines) const
{
    return runSceneState(scene, [&affectedLines](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        controller.scheduleConnectorPostprocess(currentScene,
                                                currentScene->deferredUpdateStorage(),
                                                affectedLines);
        return true;
    }, []()
    {
        return false;
    });
}

bool LzSceneStateSlotWorkbenchController::setWiringData(LzScene* scene, const QString& key, const QVariant& value) const
{
    return runSceneState(scene, [&](LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.setWiringData(currentScene->wiringDataStorage(),
                                        currentScene->topologyStateStorage(),
                                        key,
                                        value);
    }, []()
    {
        return false;
    });
}

QVariant LzSceneStateSlotWorkbenchController::wiringData(const LzScene* scene, const QString& key) const
{
    return runConstSceneState(scene, [&](const LzScene* currentScene)
    {
        LzSceneStateWorkbenchController controller;
        return controller.wiringData(currentScene->wiringDataStorage(), key);
    }, []()
    {
        return QVariant();
    });
}
