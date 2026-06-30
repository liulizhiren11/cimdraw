#ifndef FRAMETOPOLOGYWORKBENCHCONTROLLER_H
#define FRAMETOPOLOGYWORKBENCHCONTROLLER_H

#include "topology/ui/FrameTopologyController.h"

#include <functional>

class QWidget;
class LzScene;
class LzView;

class FrameTopologyWorkbenchController
{
public:
    using GenerateContextFactory = std::function<FrameTopologyController::GenerateContext(LzScene*)>;

    LzScene* sceneFromView(LzView* view) const;
    bool showSelectedSummary(QWidget* host, LzView* view) const;
    bool highlightSelected(QWidget* host, LzView* view) const;
    bool clearHighlights(LzView* view) const;
    bool generateGraph(QWidget* host,
                       LzView* sourceView,
                       const GenerateContextFactory& createContext,
                       FrameTopologyController::GeneratedMode mode) const;
};

#endif
