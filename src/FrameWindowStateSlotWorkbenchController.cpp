#include "FrameWindowStateSlotWorkbenchController.h"

#include "LzCenterWidget.h"

LzView* FrameWindowStateSlotWorkbenchController::currentView(LzCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

QTabWidget* FrameWindowStateSlotWorkbenchController::tabWidget(LzCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getTabWidget() : nullptr;
}

void FrameWindowStateSlotWorkbenchController::syncActionStates(
    LzCenterWidget* centerWidget,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateWorkbenchController controller;
    controller.syncActionStates(currentView(centerWidget), undoAction, redoAction, saveAction, saveAsAction);
}

void FrameWindowStateSlotWorkbenchController::syncCurrentTabState(
    LzCenterWidget* centerWidget,
    QPushButton* backgroundButton,
    CimModelBrowserDock* browserDock,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateWorkbenchController controller;
    controller.syncCurrentTabState(tabWidget(centerWidget),
                                   backgroundButton,
                                   browserDock,
                                   undoAction,
                                   redoAction,
                                   saveAction,
                                   saveAsAction);
}

bool FrameWindowStateSlotWorkbenchController::renameCurrentTab(LzCenterWidget* centerWidget,
                                                               const QString& title) const
{
    FrameWindowStateWorkbenchController controller;
    return controller.renameCurrentTab(tabWidget(centerWidget), title);
}
