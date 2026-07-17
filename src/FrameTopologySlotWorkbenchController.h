#ifndef FRAMETOPOLOGYSLOTWORKBENCHCONTROLLER_H
#define FRAMETOPOLOGYSLOTWORKBENCHCONTROLLER_H

#include "FrameTopologyWorkbenchController.h"

class CimdrawCenterWidget;
class CimdrawScene;
class CimdrawView;
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

    CimdrawView* currentView(CimdrawCenterWidget* centerWidget) const;
    CimdrawScene* currentScene(CimdrawCenterWidget* centerWidget) const;

    bool dispatch(QWidget* host, CimdrawCenterWidget* centerWidget, Action action) const;
    bool dispatchGenerateGraph(QWidget* host,
                               CimdrawCenterWidget* centerWidget,
                               const GenerateContextFactory& createContext,
                               FrameTopologyController::GeneratedMode mode) const;

    bool showSelectedSummary(QWidget* host, CimdrawCenterWidget* centerWidget) const;
    bool highlightSelected(QWidget* host, CimdrawCenterWidget* centerWidget) const;
    bool clearHighlights(CimdrawCenterWidget* centerWidget) const;
    bool generateGraph(QWidget* host,
                       CimdrawCenterWidget* centerWidget,
                       const GenerateContextFactory& createContext,
                       FrameTopologyController::GeneratedMode mode) const;
};

#endif
