#include "FrameDocumentSlotWorkbenchController.h"

#include "FrameWindowStateSlotWorkbenchController.h"
#include "LzCenterWidget.h"

LzView* FrameDocumentSlotWorkbenchController::currentView(LzCenterWidget* centerWidget) const
{
    return centerWidget ? centerWidget->getCurrentView() : nullptr;
}

LzView* FrameDocumentSlotWorkbenchController::createPowerScene(const NewSceneContextFactory& createContext) const
{
    FrameDocumentWorkbenchController controller;
    return controller.createPowerScene(createContext);
}

bool FrameDocumentSlotWorkbenchController::openFile(QWidget* host,
                                                    const OpenSceneContextFactory& createContext) const
{
    FrameDocumentWorkbenchController controller;
    return controller.openFile(host, createContext);
}

bool FrameDocumentSlotWorkbenchController::openFile(const QString& fileNamePath,
                                                    const OpenSceneContextFactory& createContext) const
{
    FrameDocumentWorkbenchController controller;
    return controller.openFile(fileNamePath, createContext);
}

bool FrameDocumentSlotWorkbenchController::saveCurrentView(
    LzCenterWidget* centerWidget,
    const RenameCurrentTabFn& renameCurrentTab,
    const AfterDocumentActionFn& afterSave) const
{
    FrameDocumentWorkbenchController controller;
    return controller.saveCurrentView(currentView(centerWidget), renameCurrentTab, afterSave);
}

bool FrameDocumentSlotWorkbenchController::saveCurrentViewAs(
    QWidget* host,
    LzCenterWidget* centerWidget,
    const RenameCurrentTabFn& renameCurrentTab,
    const AfterDocumentActionFn& afterSave) const
{
    FrameDocumentWorkbenchController controller;
    return controller.saveCurrentViewAs(host, currentView(centerWidget), renameCurrentTab, afterSave);
}

bool FrameDocumentSlotWorkbenchController::saveCurrentViewAndSyncUi(
    LzCenterWidget* centerWidget,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateSlotWorkbenchController windowStateController;
    return saveCurrentView(centerWidget,
                           [this, centerWidget](const QString& title)
                           {
                               FrameWindowStateSlotWorkbenchController controller;
                               controller.renameCurrentTab(centerWidget, title);
                           },
                           [windowStateController,
                            centerWidget,
                            undoAction,
                            redoAction,
                            saveAction,
                            saveAsAction]()
                           {
                               windowStateController.syncActionStates(centerWidget,
                                                                      undoAction,
                                                                      redoAction,
                                                                      saveAction,
                                                                      saveAsAction);
                           });
}

bool FrameDocumentSlotWorkbenchController::saveCurrentViewAsAndSyncUi(
    QWidget* host,
    LzCenterWidget* centerWidget,
    QAction* undoAction,
    QAction* redoAction,
    QAction* saveAction,
    QAction* saveAsAction) const
{
    FrameWindowStateSlotWorkbenchController windowStateController;
    return saveCurrentViewAs(host,
                             centerWidget,
                             [this, centerWidget](const QString& title)
                             {
                                 FrameWindowStateSlotWorkbenchController controller;
                                 controller.renameCurrentTab(centerWidget, title);
                             },
                             [windowStateController,
                              centerWidget,
                              undoAction,
                              redoAction,
                              saveAction,
                              saveAsAction]()
                             {
                                 windowStateController.syncActionStates(centerWidget,
                                                                        undoAction,
                                                                        redoAction,
                                                                        saveAction,
                                                                        saveAsAction);
                             });
}
