#include "FrameWindowStateSlotWorkbenchController.h"

#include "CimdrawCenterWidget.h"

CimdrawView* FrameWindowStateSlotWorkbenchController::currentView(CimdrawCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

QTabWidget* FrameWindowStateSlotWorkbenchController::tabWidget(CimdrawCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getTabWidget() : nullptr;
}

void FrameWindowStateSlotWorkbenchController::syncActionStates(
    CimdrawCenterWidget* centerWidget,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateWorkbenchController controller;
    controller.syncActionStates(currentView(centerWidget), undoAction, redoAction, saveAction, saveAsAction);
}

void FrameWindowStateSlotWorkbenchController::syncCurrentTabState(
    CimdrawCenterWidget* centerWidget,
    QPushButton* backgroundButton,
    const FrameCimWorkbenchController::ImportState* cimImportState,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateWorkbenchController controller;
    controller.syncCurrentTabState(tabWidget(centerWidget),
                                   backgroundButton,
                                   cimImportState,
                                   undoAction,
                                   redoAction,
                                   saveAction,
                                   saveAsAction);
}

bool FrameWindowStateSlotWorkbenchController::renameCurrentTab(CimdrawCenterWidget* centerWidget,
                                                               const QString& title) const
{
    FrameWindowStateWorkbenchController controller;
    return controller.renameCurrentTab(tabWidget(centerWidget), title);
}
