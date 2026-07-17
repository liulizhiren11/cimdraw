#include "FrameEditorController.h"

#include <QRectF>

#include "CimdrawScene.h"
#include "CimdrawView.h"

void FrameEditorController::undo(CimdrawView* view) const
{
    if (view)
        view->editUndo();
}

void FrameEditorController::redo(CimdrawView* view) const
{
    if (view)
        view->editRedo();
}

void FrameEditorController::group(CimdrawScene* scene) const
{
    if (scene)
        scene->editGroup();
}

void FrameEditorController::ungroup(CimdrawScene* scene) const
{
    if (scene)
        scene->editUnGroup();
}

void FrameEditorController::removeSelection(CimdrawScene* scene) const
{
    if (scene)
        scene->editDelete();
}

void FrameEditorController::cut(CimdrawScene* scene) const
{
    if (scene)
        scene->editCut();
}

void FrameEditorController::copy(CimdrawScene* scene) const
{
    if (scene)
        scene->editCopy();
}

void FrameEditorController::paste(CimdrawScene* scene) const
{
    if (scene)
        scene->editPaste();
}

void FrameEditorController::alignLeft(CimdrawScene* scene) const
{
    if (scene)
        scene->onLeftAlign();
}

void FrameEditorController::alignRight(CimdrawScene* scene) const
{
    if (scene)
        scene->onRightAlign();
}

void FrameEditorController::alignTop(CimdrawScene* scene) const
{
    if (scene)
        scene->onTopAlign();
}

void FrameEditorController::alignBottom(CimdrawScene* scene) const
{
    if (scene)
        scene->onBottomAlign();
}

void FrameEditorController::alignVCenter(CimdrawScene* scene) const
{
    if (scene)
        scene->onVcenterAlign();
}

void FrameEditorController::alignHCenter(CimdrawScene* scene) const
{
    if (scene)
        scene->onHcenterAlign();
}

void FrameEditorController::autoRow(CimdrawScene* scene) const
{
    if (scene)
        scene->onAutoRow();
}

void FrameEditorController::autoCol(CimdrawScene* scene) const
{
    if (scene)
        scene->onAutoCol();
}

void FrameEditorController::sameWidth(CimdrawScene* scene) const
{
    if (scene)
        scene->onSameWidth();
}

void FrameEditorController::sameHeight(CimdrawScene* scene) const
{
    if (scene)
        scene->onSameHeight();
}

void FrameEditorController::sameSize(CimdrawScene* scene) const
{
    if (scene)
        scene->onSameSize();
}

void FrameEditorController::bringFront(CimdrawScene* scene) const
{
    if (scene)
        scene->onBringFont();
}

void FrameEditorController::bringBack(CimdrawScene* scene) const
{
    if (scene)
        scene->onBringBack();
}

void FrameEditorController::zoomIn(CimdrawView* view) const
{
    if (view)
        view->zoomIn();
}

void FrameEditorController::zoomOut(CimdrawView* view) const
{
    if (view)
        view->zoomOut();
}

void FrameEditorController::zoomReset(CimdrawView* view) const
{
    if (view)
        view->zoomReset();
}

void FrameEditorController::bestFit(CimdrawView* view, CimdrawScene* scene) const
{
    if (!view || !scene)
        return;
    const QRectF contentsRect = scene->getContentsRect();
    if (!contentsRect.isNull() && !contentsRect.isEmpty())
        view->fitSceneToView(contentsRect);
}

void FrameEditorController::normalView(CimdrawView* view, CimdrawScene* scene) const
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

void FrameEditorController::shortcutLeft(CimdrawScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutLeft(delta);
}

void FrameEditorController::shortcutRight(CimdrawScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutRight(delta);
}

void FrameEditorController::shortcutUp(CimdrawScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutUp(delta);
}

void FrameEditorController::shortcutDown(CimdrawScene* scene, const QPointF& delta) const
{
    if (scene)
        scene->onShortcutDown(delta);
}
