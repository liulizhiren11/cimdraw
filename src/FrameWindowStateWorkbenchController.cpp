#include "FrameWindowStateWorkbenchController.h"

#include <QAction>
#include <QPushButton>
#include <QTabWidget>

#include "FrameActionStateController.h"
#include "FrameWorkbenchUiController.h"
#include "FrameWorkbenchStateController.h"

void FrameWindowStateWorkbenchController::syncActionStates(CimdrawView* view,
                                                           QAction* undoAction,
                                                           QAction* redoAction,
                                                           QAction* saveAction,
                                                           QAction* saveAsAction) const
{
    FrameActionStateController controller;
    controller.syncUndoRedoActions(view, undoAction, redoAction);
    controller.syncFileActions(view, saveAction, saveAsAction);
}

void FrameWindowStateWorkbenchController::syncCurrentTabState(QTabWidget* tabs,
                                                              QPushButton* backgroundButton,
                                                              const FrameCimWorkbenchController::ImportState* cimImportState,
                                                              QAction* undoAction,
                                                              QAction* redoAction,
                                                              QAction* saveAction,
                                                              QAction* saveAsAction) const
{
    FrameWorkbenchStateController stateController;
    syncActionStates(stateController.currentViewFromTabs(tabs),
                     undoAction,
                     redoAction,
                     saveAction,
                     saveAsAction);

    FrameWorkbenchUiController uiController;
    uiController.handleCurrentTabChanged(tabs, backgroundButton, cimImportState);
}

bool FrameWindowStateWorkbenchController::renameCurrentTab(QTabWidget* tabs, const QString& title) const
{
    if (!tabs || tabs->currentIndex() < 0)
        return false;

    tabs->setTabText(tabs->currentIndex(), title);
    return true;
}
