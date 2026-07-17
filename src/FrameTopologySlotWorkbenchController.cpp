#include "FrameTopologySlotWorkbenchController.h"

#include "CimdrawCenterWidget.h"
#include "CimdrawView.h"

CimdrawView* FrameTopologySlotWorkbenchController::currentView(CimdrawCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

CimdrawScene* FrameTopologySlotWorkbenchController::currentScene(CimdrawCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.sceneFromView(currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::dispatch(QWidget* host,
                                                    CimdrawCenterWidget* centerWidget,
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
    CimdrawCenterWidget* centerWidget,
    const GenerateContextFactory& createContext,
    FrameTopologyController::GeneratedMode mode) const
{
    return generateGraph(host, centerWidget, createContext, mode);
}

bool FrameTopologySlotWorkbenchController::showSelectedSummary(QWidget* host,
                                                               CimdrawCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.showSelectedSummary(host, currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::highlightSelected(QWidget* host,
                                                             CimdrawCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.highlightSelected(host, currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::clearHighlights(CimdrawCenterWidget* centerWidget) const
{
    FrameTopologyWorkbenchController controller;
    return controller.clearHighlights(currentView(centerWidget));
}

bool FrameTopologySlotWorkbenchController::generateGraph(
    QWidget* host,
    CimdrawCenterWidget* centerWidget,
    const GenerateContextFactory& createContext,
    FrameTopologyController::GeneratedMode mode) const
{
    FrameTopologyWorkbenchController controller;
    return controller.generateGraph(host, currentView(centerWidget), createContext, mode);
}
