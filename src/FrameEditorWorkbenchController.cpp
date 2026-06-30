#include "FrameEditorWorkbenchController.h"

#include "FrameEditorController.h"
#include "LzScene.h"
#include "LzView.h"

namespace {

template <typename SceneFn>
bool runSceneEdit(LzView* view, SceneFn&& fn)
{
    if (!view || !view->getScene())
        return false;

    FrameEditorController controller;
    fn(controller, view->getScene());
    return true;
}

template <typename ViewFn>
bool runViewEdit(LzView* view, ViewFn&& fn)
{
    if (!view)
        return false;

    FrameEditorController controller;
    fn(controller, view);
    return true;
}

} // namespace

LzScene* FrameEditorWorkbenchController::sceneFromView(LzView* view) const
{
    return view ? view->getScene() : nullptr;
}

bool FrameEditorWorkbenchController::undo(LzView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, LzView* currentView)
    {
        controller.undo(currentView);
    });
}

bool FrameEditorWorkbenchController::redo(LzView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, LzView* currentView)
    {
        controller.redo(currentView);
    });
}

bool FrameEditorWorkbenchController::group(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.group(scene);
    });
}

bool FrameEditorWorkbenchController::ungroup(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.ungroup(scene);
    });
}

bool FrameEditorWorkbenchController::removeSelection(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.removeSelection(scene);
    });
}

bool FrameEditorWorkbenchController::cut(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.cut(scene);
    });
}

bool FrameEditorWorkbenchController::copy(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.copy(scene);
    });
}

bool FrameEditorWorkbenchController::paste(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.paste(scene);
    });
}

bool FrameEditorWorkbenchController::alignLeft(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignLeft(scene);
    });
}

bool FrameEditorWorkbenchController::alignRight(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignRight(scene);
    });
}

bool FrameEditorWorkbenchController::alignTop(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignTop(scene);
    });
}

bool FrameEditorWorkbenchController::alignBottom(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignBottom(scene);
    });
}

bool FrameEditorWorkbenchController::alignVCenter(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignVCenter(scene);
    });
}

bool FrameEditorWorkbenchController::alignHCenter(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.alignHCenter(scene);
    });
}

bool FrameEditorWorkbenchController::autoRow(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.autoRow(scene);
    });
}

bool FrameEditorWorkbenchController::autoCol(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.autoCol(scene);
    });
}

bool FrameEditorWorkbenchController::sameWidth(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.sameWidth(scene);
    });
}

bool FrameEditorWorkbenchController::sameHeight(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.sameHeight(scene);
    });
}

bool FrameEditorWorkbenchController::sameSize(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.sameSize(scene);
    });
}

bool FrameEditorWorkbenchController::bringFront(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.bringFront(scene);
    });
}

bool FrameEditorWorkbenchController::bringBack(LzView* view) const
{
    return runSceneEdit(view, [](FrameEditorController& controller, LzScene* scene)
    {
        controller.bringBack(scene);
    });
}

bool FrameEditorWorkbenchController::zoomIn(LzView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, LzView* currentView)
    {
        controller.zoomIn(currentView);
    });
}

bool FrameEditorWorkbenchController::zoomOut(LzView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, LzView* currentView)
    {
        controller.zoomOut(currentView);
    });
}

bool FrameEditorWorkbenchController::zoomReset(LzView* view) const
{
    return runViewEdit(view, [](FrameEditorController& controller, LzView* currentView)
    {
        controller.zoomReset(currentView);
    });
}

bool FrameEditorWorkbenchController::bestFit(LzView* view) const
{
    return runSceneEdit(view, [view](FrameEditorController& controller, LzScene* scene)
    {
        controller.bestFit(view, scene);
    });
}

bool FrameEditorWorkbenchController::normalView(LzView* view) const
{
    return runSceneEdit(view, [view](FrameEditorController& controller, LzScene* scene)
    {
        controller.normalView(view, scene);
    });
}

bool FrameEditorWorkbenchController::shortcutLeft(LzView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, LzScene* scene)
    {
        controller.shortcutLeft(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutRight(LzView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, LzScene* scene)
    {
        controller.shortcutRight(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutUp(LzView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, LzScene* scene)
    {
        controller.shortcutUp(scene, delta);
    });
}

bool FrameEditorWorkbenchController::shortcutDown(LzView* view, const QPointF& delta) const
{
    return runSceneEdit(view, [&delta](FrameEditorController& controller, LzScene* scene)
    {
        controller.shortcutDown(scene, delta);
    });
}
