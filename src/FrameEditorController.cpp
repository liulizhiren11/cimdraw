#include "FrameEditorController.h"

#include <QRectF>

#include "LzScene.h"
#include "LzView.h"

void FrameEditorController::undo(LzView* view) const
{
    if (view)
        view->editUndo();
}

void FrameEditorController::redo(LzView* view) const
{
    if (view)
        view->editRedo();
}

void FrameEditorController::group(LzScene* scene) const
{
    if (scene)
        scene->editGroup();
}

void FrameEditorController::ungroup(LzScene* scene) const
{
    if (scene)
        scene->editUnGroup();
}

void FrameEditorController::removeSelection(LzScene* scene) const
{
    if (scene)
        scene->editDelete();
}

void FrameEditorController::cut(LzScene* scene) const
{
    if (scene)
        scene->editCut();
}

void FrameEditorController::copy(LzScene* scene) const
{
    if (scene)
        scene->editCopy();
}

void FrameEditorController::paste(LzScene* scene) const
{
    if (scene)
        scene->editPaste();
}

void FrameEditorController::alignLeft(LzScene* scene) const
{
    if (scene)
        scene->onLeftAlign();
}

void FrameEditorController::alignRight(LzScene* scene) const
{
    if (scene)
        scene->onRightAlign();
}

void FrameEditorController::alignTop(LzScene* scene) const
{
    if (scene)
        scene->onTopAlign();
}

void FrameEditorController::alignBottom(LzScene* scene) const
{
    if (scene)
        scene->onBottomAlign();
}

void FrameEditorController::alignVCenter(LzScene* scene) const
{
    if (scene)
        scene->onVcenterAlign();
}

void FrameEditorController::alignHCenter(LzScene* scene) const
{
    if (scene)
        scene->onHcenterAlign();
}

void FrameEditorController::autoRow(LzScene* scene) const
{
    if (scene)
        scene->onAutoRow();
}

void FrameEditorController::autoCol(LzScene* scene) const
{
    if (scene)
        scene->onAutoCol();
}

void FrameEditorController::sameWidth(LzScene* scene) const
{
    if (scene)
        scene->onSameWidth();
}

void FrameEditorController::sameHeight(LzScene* scene) const
{
    if (scene)
        scene->onSameHeight();
}

void FrameEditorController::sameSize(LzScene* scene) const
{
    if (scene)
        scene->onSameSize();
}

void FrameEditorController::bringFront(LzScene* scene) const
{
    if (scene)
        scene->onBringFont();
}

void FrameEditorController::bringBack(LzScene* scene) const
{
    if (scene)
        scene->onBringBack();
}

void FrameEditorController::zoomIn(LzView* view) const
{
    if (view)
        view->zoomIn();
}

void FrameEditorController::zoomOut(LzView* view) const
{
    if (view)
        view->zoomOut();
}

void FrameEditorController::zoomReset(LzView* view) const
{
    if (view)
        view->zoomReset();
}

void FrameEditorController::bestFit(LzView* view, LzScene* scene) const
{
    if (!view || !scene)
        return;
    const QRectF contentsRect = scene->getContentsRect();
    if (!contentsRect.isNull() && !contentsRect.isEmpty())
        view->fitSceneToView(contentsRect);
}

void FrameEditorController::normalView(LzView* view, LzScene* scene) const
{
    if (!view || !scene)
        return;
    view->zoomReset();
    const QRectF contentsRect = scene->getContentsRect();
    if (!contentsRect.isNull() && !contentsRect.isEmpty())
    {
        scene->setSceneRect(contentsRect);
        view->ensureVisible(contentsRect);
    }
}

void FrameEditorController::shortcutLeft(LzScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutLeft(delta);
}

void FrameEditorController::shortcutRight(LzScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutRight(delta);
}

void FrameEditorController::shortcutUp(LzScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutUp(delta);
}

void FrameEditorController::shortcutDown(LzScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutDown(delta);
}
