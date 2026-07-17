#include "FrameEditorWorkbenchController.h"

#include "FrameEditorController.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

namespace {

template <typename SceneFn>
bool runSceneEdit(CimdrawView* view, SceneFn&& fn)
{
    if (!view || !view->getScene())
        return false;

    FrameEditorController controller;
    fn(controller, view->getScene());
    return true;
}

template <typename ViewFn>
bool runViewEdit(CimdrawView* view, ViewFn&& fn)
{
    if (!view)
        return false;

    FrameEditorController controller;
    fn(controller, view);
    return true;
}

} // namespace

CimdrawScene* FrameEditorWorkbenchController::sceneFromView(CimdrawView* view) const
{
    return view ? view->getScene() : nullptr;
}

bool FrameEditorWorkbenchController::undo(CimdrawView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, CimdrawView* currentView)
    {
        controller.undo(currentView);
    });
}

bool FrameEditorWorkbenchController::redo(CimdrawView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, CimdrawView* currentView)
    {
        controller.redo(currentView);
    });
}

bool FrameEditorWorkbenchController::group(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.group(scene);
    });
}

bool FrameEditorWorkbenchController::ungroup(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.ungroup(scene);
    });
}

bool FrameEditorWorkbenchController::removeSelection(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.removeSelection(scene);
    });
}

bool FrameEditorWorkbenchController::cut(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.cut(scene);
    });
}

bool FrameEditorWorkbenchController::copy(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.copy(scene);
    });
}

bool FrameEditorWorkbenchController::paste(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.paste(scene);
    });
}

bool FrameEditorWorkbenchController::alignLeft(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignLeft(scene);
    });
}

bool FrameEditorWorkbenchController::alignRight(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignRight(scene);
    });
}

bool FrameEditorWorkbenchController::alignTop(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignTop(scene);
    });
}

bool FrameEditorWorkbenchController::alignBottom(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignBottom(scene);
    });
}

bool FrameEditorWorkbenchController::alignVCenter(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignVCenter(scene);
    });
}

bool FrameEditorWorkbenchController::alignHCenter(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.alignHCenter(scene);
    });
}

bool FrameEditorWorkbenchController::autoRow(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.autoRow(scene);
    });
}

bool FrameEditorWorkbenchController::autoCol(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.autoCol(scene);
    });
}

bool FrameEditorWorkbenchController::sameWidth(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.sameWidth(scene);
    });
}

bool FrameEditorWorkbenchController::sameHeight(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.sameHeight(scene);
    });
}

bool FrameEditorWorkbenchController::sameSize(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.sameSize(scene);
    });
}

bool FrameEditorWorkbenchController::bringFront(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.bringFront(scene);
    });
}

bool FrameEditorWorkbenchController::bringBack(CimdrawView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.bringBack(scene);
    });
}

bool FrameEditorWorkbenchController::zoomIn(CimdrawView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, CimdrawView* currentView)
    {
        controller.zoomIn(currentView);
    });
}

bool FrameEditorWorkbenchController::zoomOut(CimdrawView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, CimdrawView* currentView)
    {
        controller.zoomOut(currentView);
    });
}

bool FrameEditorWorkbenchController::zoomReset(CimdrawView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, CimdrawView* currentView)
    {
        controller.zoomReset(currentView);
    });
}

bool FrameEditorWorkbenchController::bestFit(CimdrawView* view) const
{
    return runSceneEdit(view, [view](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.bestFit(view, scene);
    });
}

bool FrameEditorWorkbenchController::normalView(CimdrawView* view) const
{
    return runSceneEdit(view, [view](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.normalView(view, scene);
    });
}

bool FrameEditorWorkbenchController::shortcutLeft(CimdrawView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.shortcutLeft(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutRight(CimdrawView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.shortcutRight(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutUp(CimdrawView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.shortcutUp(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutDown(CimdrawView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, CimdrawScene* scene)
    {
        controller.shortcutDown(scene, delta);
    });
}
