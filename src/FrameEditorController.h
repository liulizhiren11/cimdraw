#ifndef FRAMEEDITORCONTROLLER_H
#define FRAMEEDITORCONTROLLER_H

#include <QPointF>

class LzScene;
class LzView;

class FrameEditorController
{
public:
    void undo(LzView* view) const;
    void redo(LzView* view) const;

    void group(LzScene* scene) const;
    void ungroup(LzScene* scene) const;
    void removeSelection(LzScene* scene) const;
    void cut(LzScene* scene) const;
    void copy(LzScene* scene) const;
    void paste(LzScene* scene) const;

    void alignLeft(LzScene* scene) const;
    void alignRight(LzScene* scene) const;
    void alignTop(LzScene* scene) const;
    void alignBottom(LzScene* scene) const;
    void alignVCenter(LzScene* scene) const;
    void alignHCenter(LzScene* scene) const;
    void autoRow(LzScene* scene) const;
    void autoCol(LzScene* scene) const;
    void sameWidth(LzScene* scene) const;
    void sameHeight(LzScene* scene) const;
    void sameSize(LzScene* scene) const;
    void bringFront(LzScene* scene) const;
    void bringBack(LzScene* scene) const;

    void zoomIn(LzView* view) const;
    void zoomOut(LzView* view) const;
    void zoomReset(LzView* view) const;
    void bestFit(LzView* view, LzScene* scene) const;
    void normalView(LzView* view, LzScene* scene) const;

    void shortcutLeft(LzScene* scene, const QPointF& delta) const;
    void shortcutRight(LzScene* scene, const QPointF& delta) const;
    void shortcutUp(LzScene* scene, const QPointF& delta) const;
    void shortcutDown(LzScene* scene, const QPointF& delta) const;
};

#endif
