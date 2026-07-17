#ifndef FRAMEEDITORWORKBENCHCONTROLLER_H
#define FRAMEEDITORWORKBENCHCONTROLLER_H

#include <QPointF>

class CimdrawScene;
class CimdrawView;

class FrameEditorWorkbenchController
{
public:
    CimdrawScene* sceneFromView(CimdrawView* view) const;

    bool undo(CimdrawView* view) const;
    bool redo(CimdrawView* view) const;

    bool group(CimdrawView* view) const;
    bool ungroup(CimdrawView* view) const;
    bool removeSelection(CimdrawView* view) const;
    bool cut(CimdrawView* view) const;
    bool copy(CimdrawView* view) const;
    bool paste(CimdrawView* view) const;

    bool alignLeft(CimdrawView* view) const;
    bool alignRight(CimdrawView* view) const;
    bool alignTop(CimdrawView* view) const;
    bool alignBottom(CimdrawView* view) const;
    bool alignVCenter(CimdrawView* view) const;
    bool alignHCenter(CimdrawView* view) const;
    bool autoRow(CimdrawView* view) const;
    bool autoCol(CimdrawView* view) const;
    bool sameWidth(CimdrawView* view) const;
    bool sameHeight(CimdrawView* view) const;
    bool sameSize(CimdrawView* view) const;
    bool bringFront(CimdrawView* view) const;
    bool bringBack(CimdrawView* view) const;

    bool zoomIn(CimdrawView* view) const;
    bool zoomOut(CimdrawView* view) const;
    bool zoomReset(CimdrawView* view) const;
    bool bestFit(CimdrawView* view) const;
    bool normalView(CimdrawView* view) const;

    bool shortcutLeft(CimdrawView* view, const QPointF& delta) const;
    bool shortcutRight(CimdrawView* view, const QPointF& delta) const;
    bool shortcutUp(CimdrawView* view, const QPointF& delta) const;
    bool shortcutDown(CimdrawView* view, const QPointF& delta) const;
};

#endif
