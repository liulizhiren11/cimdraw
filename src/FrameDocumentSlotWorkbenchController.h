#ifndef FRAMEDOCUMENTSLOTWORKBENCHCONTROLLER_H
#define FRAMEDOCUMENTSLOTWORKBENCHCONTROLLER_H

#include "FrameDocumentWorkbenchController.h"

class LzCenterWidget;
class LzView;
class QWidget;
class QAction;

class FrameDocumentSlotWorkbenchController
{
public:
    using NewSceneContextFactory = FrameDocumentWorkbenchController::NewSceneContextFactory;
    using OpenSceneContextFactory = FrameDocumentWorkbenchController::OpenSceneContextFactory;
    using RenameCurrentTabFn = FrameDocumentWorkbenchController::RenameCurrentTabFn;
    using AfterDocumentActionFn = FrameDocumentWorkbenchController::AfterDocumentActionFn;

    LzView* currentView(LzCenterWidget* centerWidget) const;
    LzView* createPowerScene(const NewSceneContextFactory& createContext) const;
    bool openFile(QWidget* host, const OpenSceneContextFactory& createContext) const;
    bool openFile(const QString& fileNamePath, const OpenSceneContextFactory& createContext) const;
    bool saveCurrentView(LzCenterWidget* centerWidget,
                         const RenameCurrentTabFn& renameCurrentTab,
                         const AfterDocumentActionFn& afterSave) const;
    bool saveCurrentViewAs(QWidget* host,
                           LzCenterWidget* centerWidget,
                           const RenameCurrentTabFn& renameCurrentTab,
                           const AfterDocumentActionFn& afterSave) const;
    bool saveCurrentViewAndSyncUi(LzCenterWidget* centerWidget,
                                  QAction* undoAction,
                                  QAction* redoAction,
                                  QAction* saveAction,
                                  QAction* saveAsAction) const;
    bool saveCurrentViewAsAndSyncUi(QWidget* host,
                                    LzCenterWidget* centerWidget,
                                    QAction* undoAction,
                                    QAction* redoAction,
                                    QAction* saveAction,
                                    QAction* saveAsAction) const;
};

#endif
