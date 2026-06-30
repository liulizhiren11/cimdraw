#ifndef FRAMEEDITORSLOTWORKBENCHCONTROLLER_H
#define FRAMEEDITORSLOTWORKBENCHCONTROLLER_H

#include <QPointF>

class LzCenterWidget;
class LzView;

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

    LzView* currentView(LzCenterWidget* centerWidget) const;
    bool dispatch(LzCenterWidget* centerWidget, Action action) const;
    QPointF shortcutDelta(const QPointF& distance, ShortcutDirection direction) const;
    bool dispatchShortcut(LzCenterWidget* centerWidget,
                          const QPointF& distance,
                          ShortcutDirection direction) const;

    bool undo(LzCenterWidget* centerWidget) const;
    bool redo(LzCenterWidget* centerWidget) const;
    bool group(LzCenterWidget* centerWidget) const;
    bool ungroup(LzCenterWidget* centerWidget) const;

    bool alignLeft(LzCenterWidget* centerWidget) const;
    bool alignRight(LzCenterWidget* centerWidget) const;
    bool alignTop(LzCenterWidget* centerWidget) const;
    bool alignBottom(LzCenterWidget* centerWidget) const;
    bool alignVCenter(LzCenterWidget* centerWidget) const;
    bool alignHCenter(LzCenterWidget* centerWidget) const;
    bool autoRow(LzCenterWidget* centerWidget) const;
    bool autoCol(LzCenterWidget* centerWidget) const;
    bool sameWidth(LzCenterWidget* centerWidget) const;
    bool sameHeight(LzCenterWidget* centerWidget) const;
    bool sameSize(LzCenterWidget* centerWidget) const;
    bool bringFront(LzCenterWidget* centerWidget) const;
    bool bringBack(LzCenterWidget* centerWidget) const;

    bool removeSelection(LzCenterWidget* centerWidget) const;
    bool zoomIn(LzCenterWidget* centerWidget) const;
    bool zoomOut(LzCenterWidget* centerWidget) const;
    bool zoomReset(LzCenterWidget* centerWidget) const;
    bool cut(LzCenterWidget* centerWidget) const;
    bool copy(LzCenterWidget* centerWidget) const;
    bool paste(LzCenterWidget* centerWidget) const;
    bool bestFit(LzCenterWidget* centerWidget) const;
    bool normalView(LzCenterWidget* centerWidget) const;
    bool shortcutLeft(LzCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutRight(LzCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutUp(LzCenterWidget* centerWidget, const QPointF& delta) const;
    bool shortcutDown(LzCenterWidget* centerWidget, const QPointF& delta) const;
};

#endif
