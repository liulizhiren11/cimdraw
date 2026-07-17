#ifndef FRAMETOPOLOGYCONTROLLER_H
#define FRAMETOPOLOGYCONTROLLER_H

#include <functional>

class QString;
class QWidget;
class CimdrawScene;
class CimdrawView;

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
        CimdrawScene* sourceScene = nullptr;
        std::function<CimdrawView*()> createTargetView;
        std::function<void(const QString&)> renameCurrentTab;
        std::function<void()> onGenerated;
    };

    void showSelectedSummary(QWidget* host, CimdrawScene* scene) const;
    void highlightSelected(QWidget* host, CimdrawScene* scene) const;
    void clearHighlights(CimdrawScene* scene) const;
    void generateGraph(const GenerateContext& context, GeneratedMode mode) const;
};

#endif
