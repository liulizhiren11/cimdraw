#ifndef FRAMEEDITORWORKBENCHCONTROLLER_H
#define FRAMEEDITORWORKBENCHCONTROLLER_H

#include <QPointF>

class LzScene;
class LzView;

class FrameEditorWorkbenchController
{
public:
    LzScene* sceneFromView(LzView* view) const;

    bool undo(LzView* view) const;
    bool redo(LzView* view) const;

    bool group(LzView* view) const;
    bool ungroup(LzView* view) const;
    bool removeSelection(LzView* view) const;
    bool cut(LzView* view) const;
    bool copy(LzView* view) const;
    bool paste(LzView* view) const;

    bool alignLeft(LzView* view) const;
    bool alignRight(LzView* view) const;
    bool alignTop(LzView* view) const;
    bool alignBottom(LzView* view) const;
    bool alignVCenter(LzView* view) const;
    bool alignHCenter(LzView* view) const;
    bool autoRow(LzView* view) const;
    bool autoCol(LzView* view) const;
    bool sameWidth(LzView* view) const;
    bool sameHeight(LzView* view) const;
    bool sameSize(LzView* view) const;
    bool bringFront(LzView* view) const;
    bool bringBack(LzView* view) const;

    bool zoomIn(LzView* view) const;
    bool zoomOut(LzView* view) const;
    bool zoomReset(LzView* view) const;
    bool bestFit(LzView* view) const;
    bool normalView(LzView* view) const;

    bool shortcutLeft(LzView* view, const QPointF& delta) const;
    bool shortcutRight(LzView* view, const QPointF& delta) const;
    bool shortcutUp(LzView* view, const QPointF& delta) const;
    bool shortcutDown(LzView* view, const QPointF& delta) const;
};

#endif
