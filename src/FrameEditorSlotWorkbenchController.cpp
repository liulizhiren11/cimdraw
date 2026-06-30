#include "FrameEditorSlotWorkbenchController.h"

#include "FrameEditorWorkbenchController.h"
#include "LzCenterWidget.h"

namespace {

template <typename Fn>
bool runWithCurrentView(LzCenterWidget* centerWidget, Fn&& fn)
{
    FrameEditorSlotWorkbenchController controller;
    if (LzView* view = controller.currentView(centerWidget))
        return fn(view);
    return false;
}

} // namespace

LzView* FrameEditorSlotWorkbenchController::currentView(LzCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

bool FrameEditorSlotWorkbenchController::dispatch(LzCenterWidget* centerWidget, Action action) const
{
    switch (action)
    {
    case Action::Undo:
        return undo(centerWidget);
    case Action::Redo:
        return redo(centerWidget);
    case Action::Group:
        return group(centerWidget);
    case Action::Ungroup:
        return ungroup(centerWidget);
    case Action::AlignLeft:
        return alignLeft(centerWidget);
    case Action::AlignRight:
        return alignRight(centerWidget);
    case Action::AlignTop:
        return alignTop(centerWidget);
    case Action::AlignBottom:
        return alignBottom(centerWidget);
    case Action::AlignVCenter:
        return alignVCenter(centerWidget);
    case Action::AlignHCenter:
        return alignHCenter(centerWidget);
    case Action::AutoRow:
        return autoRow(centerWidget);
    case Action::AutoCol:
        return autoCol(centerWidget);
    case Action::SameWidth:
        return sameWidth(centerWidget);
    case Action::SameHeight:
        return sameHeight(centerWidget);
    case Action::SameSize:
        return sameSize(centerWidget);
    case Action::BringFront:
        return bringFront(centerWidget);
    case Action::BringBack:
        return bringBack(centerWidget);
    case Action::RemoveSelection:
        return removeSelection(centerWidget);
    case Action::ZoomIn:
        return zoomIn(centerWidget);
    case Action::ZoomOut:
        return zoomOut(centerWidget);
    case Action::ZoomReset:
        return zoomReset(centerWidget);
    case Action::Cut:
        return cut(centerWidget);
    case Action::Copy:
        return copy(centerWidget);
    case Action::Paste:
        return paste(centerWidget);
    case Action::BestFit:
        return bestFit(centerWidget);
    case Action::NormalView:
        return normalView(centerWidget);
    }

    return false;
}

QPointF FrameEditorSlotWorkbenchController::shortcutDelta(const QPointF& distance,
                                                          ShortcutDirection direction) const
{
    switch (direction)
    {
    case ShortcutDirection::Left:
        return QPointF(-distance.x(), 0);
    case ShortcutDirection::Right:
        return QPointF(distance.x(), 0);
    case ShortcutDirection::Up:
        return QPointF(0, -distance.y());
    case ShortcutDirection::Down:
        return QPointF(0, distance.y());
    }

    return QPointF();
}

bool FrameEditorSlotWorkbenchController::dispatchShortcut(LzCenterWidget* centerWidget,
                                                          const QPointF& distance,
                                                          ShortcutDirection direction) const
{
    const QPointF delta = shortcutDelta(distance, direction);
    switch (direction)
    {
    case ShortcutDirection::Left:
        return shortcutLeft(centerWidget, delta);
    case ShortcutDirection::Right:
        return shortcutRight(centerWidget, delta);
    case ShortcutDirection::Up:
        return shortcutUp(centerWidget, delta);
    case ShortcutDirection::Down:
        return shortcutDown(centerWidget, delta);
    }

    return false;
}

bool FrameEditorSlotWorkbenchController::undo(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.undo(view);
    });
}

bool FrameEditorSlotWorkbenchController::redo(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.redo(view);
    });
}

bool FrameEditorSlotWorkbenchController::group(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.group(view);
    });
}

bool FrameEditorSlotWorkbenchController::ungroup(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.ungroup(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignLeft(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignLeft(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignRight(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignRight(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignTop(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignTop(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignBottom(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignBottom(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignVCenter(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignVCenter(view);
    });
}

bool FrameEditorSlotWorkbenchController::alignHCenter(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.alignHCenter(view);
    });
}

bool FrameEditorSlotWorkbenchController::autoRow(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.autoRow(view);
    });
}

bool FrameEditorSlotWorkbenchController::autoCol(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.autoCol(view);
    });
}

bool FrameEditorSlotWorkbenchController::sameWidth(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.sameWidth(view);
    });
}

bool FrameEditorSlotWorkbenchController::sameHeight(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.sameHeight(view);
    });
}

bool FrameEditorSlotWorkbenchController::sameSize(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.sameSize(view);
    });
}

bool FrameEditorSlotWorkbenchController::bringFront(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.bringFront(view);
    });
}

bool FrameEditorSlotWorkbenchController::bringBack(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.bringBack(view);
    });
}

bool FrameEditorSlotWorkbenchController::removeSelection(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.removeSelection(view);
    });
}

bool FrameEditorSlotWorkbenchController::zoomIn(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.zoomIn(view);
    });
}

bool FrameEditorSlotWorkbenchController::zoomOut(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.zoomOut(view);
    });
}

bool FrameEditorSlotWorkbenchController::zoomReset(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.zoomReset(view);
    });
}

bool FrameEditorSlotWorkbenchController::cut(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.cut(view);
    });
}

bool FrameEditorSlotWorkbenchController::copy(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.copy(view);
    });
}

bool FrameEditorSlotWorkbenchController::paste(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.paste(view);
    });
}

bool FrameEditorSlotWorkbenchController::bestFit(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.bestFit(view);
    });
}

bool FrameEditorSlotWorkbenchController::normalView(LzCenterWidget* centerWidget) const
{
    return runWithCurrentView(centerWidget, [](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.normalView(view);
    });
}

bool FrameEditorSlotWorkbenchController::shortcutLeft(LzCenterWidget* centerWidget, const QPointF& delta) const
{
    return runWithCurrentView(centerWidget, [&delta](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.shortcutLeft(view, delta);
    });
}

bool FrameEditorSlotWorkbenchController::shortcutRight(LzCenterWidget* centerWidget, const QPointF& delta) const
{
    return runWithCurrentView(centerWidget, [&delta](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.shortcutRight(view, delta);
    });
}

bool FrameEditorSlotWorkbenchController::shortcutUp(LzCenterWidget* centerWidget, const QPointF& delta) const
{
    return runWithCurrentView(centerWidget, [&delta](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.shortcutUp(view, delta);
    });
}

bool FrameEditorSlotWorkbenchController::shortcutDown(LzCenterWidget* centerWidget, const QPointF& delta) const
{
    return runWithCurrentView(centerWidget, [&delta](LzView* view)
    {
        FrameEditorWorkbenchController controller;
        return controller.shortcutDown(view, delta);
    });
}
