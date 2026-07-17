#include "FrameTopologyWorkbenchController.h"

#include <QMessageBox>

#include "CimdrawScene.h"
#include "CimdrawView.h"

CimdrawScene* FrameTopologyWorkbenchController::sceneFromView(CimdrawView* view) const
{
    return view ? view->getScene() : nullptr;
}

bool FrameTopologyWorkbenchController::showSelectedSummary(QWidget* host, CimdrawView* view) const
{
    CimdrawScene* scene = sceneFromView(view);
    if (!scene)
    {
        if (host)
            QMessageBox::information(host, QObject::tr("拓扑摘要"), QObject::tr("当前没有打开的场景。"));
        return false;
    }

    FrameTopologyController controller;
    controller.showSelectedSummary(host, scene);
    return true;
}

bool FrameTopologyWorkbenchController::highlightSelected(QWidget* host, CimdrawView* view) const
{
    CimdrawScene* scene = sceneFromView(view);
    if (!scene)
        return false;

    FrameTopologyController controller;
    controller.highlightSelected(host, scene);
    return true;
}

bool FrameTopologyWorkbenchController::clearHighlights(CimdrawView* view) const
{
    CimdrawScene* scene = sceneFromView(view);
    if (!scene)
        return false;

    FrameTopologyController controller;
    controller.clearHighlights(scene);
    return true;
}

bool FrameTopologyWorkbenchController::generateGraph(
    QWidget* host,
    CimdrawView* sourceView,
    const GenerateContextFactory& createContext,
    FrameTopologyController::GeneratedMode mode) const
{
    CimdrawScene* scene = sceneFromView(sourceView);
    if (!scene)
    {
        if (host)
            QMessageBox::information(host, QObject::tr("生成拓扑图"), QObject::tr("当前没有打开的场景。"));
        return false;
    }
    if (!createContext)
        return false;

    FrameTopologyController controller;
    controller.generateGraph(createContext(scene), mode);
    return true;
}
