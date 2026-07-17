#ifndef FRAMEEDITORSLOTWORKBENCHCONTROLLER_H
#define FRAMEEDITORSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class CimdrawCenterWidget;
class CimdrawView;

class FrameEditorSlotWorkbenchController
{
public:
    enum class Action
    {
        Undo,
        Redo,
        Group,
        Ungroup,
        AlignLeft,
        AlignRight,
        AlignTop,
        AlignBottom,
        AlignVCenter,
        AlignHCenter,
        AutoRow,
        AutoCol,
        SameWidth,
        SameHeight,
        SameSize,
        BringFront,
        BringBack,
        RemoveSelection,
        ZoomIn,
        ZoomOut,
        ZoomReset,
        Cut,
        Copy,
        Paste,
        BestFit,
        NormalView
    };

    enum class ShortcutDirection
    {
        Left,
        Right,
        Up,
        Down
    };

    CimdrawView* currentView(CimdrawCenterWidget* centerWidget) const;
    bool dispatch(CimdrawCenterWidget* centerWidget, Action action) const;
    QPointF shortcutDelta(const QPointF& distance, ShortcutDirection direction) const;
    bool dispatchShortcut(CimdrawCenterWidget* centerWidget,
                          const QPointF& distance,
                          ShortcutDirection direction) const;

    bool undo(CimdrawCenterWidget* centerWidget) const;
    bool redo(CimdrawCenterWidget* centerWidget) const;
    bool group(CimdrawCenterWidget* centerWidget) const;
    bool ungroup(CimdrawCenterWidget* centerWidget) const;

    bool alignLeft(CimdrawCenterWidget* centerWidget) const;
    bool alignRight(CimdrawCenterWidget* centerWidget) const;
    bool alignTop(CimdrawCenterWidget* centerWidget) const;
    bool alignBottom(CimdrawCenterWidget* centerWidget) const;
    bool alignVCenter(CimdrawCenterWidget* centerWidget) const;
    bool alignHCenter(CimdrawCenterWidget* centerWidget) const;
    bool autoRow(CimdrawCenterWidget* centerWidget) const;
    bool autoCol(CimdrawCenterWidget* centerWidget) const;
    bool sameWidth(CimdrawCenterWidget* centerWidget) const;
    bool sameHeight(CimdrawCenterWidget* centerWidget) const;
    bool sameSize(CimdrawCenterWidget* centerWidget) const;
    bool bringFront(CimdrawCenterWidget* centerWidget) const;
    bool bringBack(CimdrawCenterWidget* centerWidget) const;

    bool removeSelection(CimdrawCenterWidget* centerWidget) const;
    bool zoomIn(CimdrawCenterWidget* centerWidget) const;
    bool zoomOut(CimdrawCenterWidget* centerWidget) const;
    bool zoomReset(CimdrawCenterWidget* centerWidget) const;
    bool cut(CimdrawCenterWidget* centerWidget) const;
    bool copy(CimdrawCenterWidget* centerWidget) const;
    bool paste(CimdrawCenterWidget* centerWidget) const;
    bool bestFit(CimdrawCenterWidget* centerWidget) const;
    bool normalView(CimdrawCenterWidget* centerWidget) const;
    bool shortcutLeft(CimdrawCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutRight(CimdrawCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutUp(CimdrawCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutDown(CimdrawCenterWidget* centerWidget, const QPointF& delta) const;
};

#endif
