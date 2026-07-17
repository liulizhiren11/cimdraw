#ifndef FRAMEEDITORCONTROLLER_H
#define FRAMEEDITORCONTROLLER_H

#include <QPointF>

class CimdrawScene;
class CimdrawView;

class FrameEditorController
{
public:
    void undo(CimdrawView* view) const;
    void redo(CimdrawView* view) const;

    void group(CimdrawScene* scene) const;
    void ungroup(CimdrawScene* scene) const;
    void removeSelection(CimdrawScene* scene) const;
    void cut(CimdrawScene* scene) const;
    void copy(CimdrawScene* scene) const;
    void paste(CimdrawScene* scene) const;

    void alignLeft(CimdrawScene* scene) const;
    void alignRight(CimdrawScene* scene) const;
    void alignTop(CimdrawScene* scene) const;
    void alignBottom(CimdrawScene* scene) const;
    void alignVCenter(CimdrawScene* scene) const;
    void alignHCenter(CimdrawScene* scene) const;
    void autoRow(CimdrawScene* scene) const;
    void autoCol(CimdrawScene* scene) const;
    void sameWidth(CimdrawScene* scene) const;
    void sameHeight(CimdrawScene* scene) const;
    void sameSize(CimdrawScene* scene) const;
    void bringFront(CimdrawScene* scene) const;
    void bringBack(CimdrawScene* scene) const;

    void zoomIn(CimdrawView* view) const;
    void zoomOut(CimdrawView* view) const;
    void zoomReset(CimdrawView* view) const;
    void bestFit(CimdrawView* view, CimdrawScene* scene) const;
    void normalView(CimdrawView* view, CimdrawScene* scene) const;

    void shortcutLeft(CimdrawScene* scene, const QPointF& delta) const;
    void shortcutRight(CimdrawScene* scene, const QPointF& delta) const;
    void shortcutUp(CimdrawScene* scene, const QPointF& delta) const;
    void shortcutDown(CimdrawScene* scene, const QPointF& delta) const;
};

#endif
