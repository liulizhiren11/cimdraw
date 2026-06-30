#ifndef FRAMETOPOLOGYSLOTWORKBENCHCONTROLLER_H
#define FRAMETOPOLOGYSLOTWORKBENCHCONTROLLER_H

#include "FrameTopologyWorkbenchController.h"

class LzCenterWidget;
class LzScene;
class LzView;
class QWidget;

class FrameTopologySlotWorkbenchController
{
public:
    using GenerateContextFactory = FrameTopologyWorkbenchController::GenerateContextFactory;

    enum class Action
    {
        ShowSelectedSummary,
        HighlightSelected,
        ClearHighlights
    };

    LzView* currentView(LzCenterWidget* centerWidget) const;
    LzScene* currentScene(LzCenterWidget* centerWidget) const;

    bool dispatch(QWidget* host, LzCenterWidget* centerWidget, Action action) const;
    bool dispatchGenerateGraph(QWidget* host,
                               LzCenterWidget* centerWidget,
                               const GenerateContextFactory& createContext,
                               FrameTopologyController::GeneratedMode mode) const;

    bool showSelectedSummary(QWidget* host, LzCenterWidget* centerWidget) const;
    bool highlightSelected(QWidget* host, LzCenterWidget* centerWidget) const;
    bool clearHighlights(LzCenterWidget* centerWidget) const;
    bool generateGraph(QWidget* host,
                       LzCenterWidget* centerWidget,
                       const GenerateContextFactory& createContext,
                       FrameTopologyController::GeneratedMode mode) const;
};

#endif
