#include "FrameTopologySlotWorkbenchController.h"

#include "LzCenterWidget.h"
#include "LzView.h"

LzView* FrameTopologySlotWorkbenchController::currentView(LzCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

LzScene* FrameTopologySlotWorkbenchController::currentScene(LzCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.sceneFromView(currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::dispatch(QWidget* host,
                                                    LzCenterWidget* centerWidget,
                                                    Action action) const
{
    switch (action)
    {
    case Action::ShowSelectedSummary:
        return showSelectedSummary(host, centerWidget);
    case Action::HighlightSelected:
        return highlightSelected(host, centerWidget);
    case Action::ClearHighlights:
        return clearHighlights(centerWidget);
    }

    return false;
}

bool FrameTopologySlotWorkbenchController::dispatchGenerateGraph(
    QWidget* host,
    LzCenterWidget* centerWidget,
    const GenerateContextFactory& createContext,
    FrameTopologyController::GeneratedMode mode) const
{
    return generateGraph(host, centerWidget, createContext, mode);
}

bool FrameTopologySlotWorkbenchController::showSelectedSummary(QWidget* host,
                                                               LzCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.showSelectedSummary(host, currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::highlightSelected(QWidget* host,
                                                             LzCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.highlightSelected(host, currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::clearHighlights(LzCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.clearHighlights(currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::generateGraph(
    QWidget* host,
    LzCenterWidget* centerWidget,
    const GenerateContextFactory& createContext,
    FrameTopologyController::GeneratedMode mode) const
{
    FrameTopologyWorkbenchController controller;
    return controller.generateGraph(host, currentView(centerWidget), createContext, mode);
}
