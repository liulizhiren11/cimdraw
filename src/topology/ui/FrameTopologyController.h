#ifndef FRAMETOPOLOGYCONTROLLER_H
#define FRAMETOPOLOGYCONTROLLER_H

#include <functional>

class QString;
class QWidget;
class LzScene;
class LzView;

class FrameTopologyController
{
public:
    enum class GeneratedMode : int
    {
        Combined = 0,
        PowerSystem,
    };

    struct GenerateContext
    {
        QWidget* host = nullptr;
        LzScene* sourceScene = nullptr;
        std::function<LzView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> onGenerated;
    };

    void showSelectedSummary(QWidget* host, LzScene* scene) const;
    void highlightSelected(QWidget* host, LzScene* scene) const;
    void clearHighlights(LzScene* scene) const;
    void generateGraph(const GenerateContext& context, GeneratedMode mode) const;
};

#endif
