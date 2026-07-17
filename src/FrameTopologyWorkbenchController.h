#ifndef FRAMETOPOLOGYWORKBENCHCONTROLLER_H
#define FRAMETOPOLOGYWORKBENCHCONTROLLER_H

#include "topology/ui/FrameTopologyController.h"

#include <functional>

class QWidget;
class CimdrawScene;
class CimdrawView;

class FrameTopologyWorkbenchController
{
public:
    using GenerateContextFactory = std::function<FrameTopologyController::GenerateContext(CimdrawScene*)>;

    CimdrawScene* sceneFromView(CimdrawView* view) const;
    bool showSelectedSummary(QWidget* host, CimdrawView* view) const;
    bool highlightSelected(QWidget* host, CimdrawView* view) const;
    bool clearHighlights(CimdrawView* view) const;
    bool generateGraph(QWidget* host,
                       CimdrawView* sourceView,
                       const GenerateContextFactory& createContext,
                       FrameTopologyController::GeneratedMode mode) const;
};

#endif
