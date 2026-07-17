#ifndef FRAMEDOCUMENTSLOTWORKBENCHCONTROLLER_H
#define FRAMEDOCUMENTSLOTWORKBENCHCONTROLLER_H

#include "FrameDocumentWorkbenchController.h"

class CimdrawCenterWidget;
class CimdrawView;
class QWidget;
class QAction;

class FrameDocumentSlotWorkbenchController
{
public:
    using NewSceneContextFactory = FrameDocumentWorkbenchController::NewSceneContextFactory;
    using OpenSceneContextFactory = FrameDocumentWorkbenchController::OpenSceneContextFactory;
    using RenameCurrentTabFn = FrameDocumentWorkbenchController::RenameCurrentTabFn;
    using AfterDocumentActionFn = FrameDocumentWorkbenchController::AfterDocumentActionFn;

    CimdrawView* currentView(CimdrawCenterWidget* centerWidget) const;
    CimdrawView* createPowerScene(const NewSceneContextFactory& createContext) const;
    bool openFile(QWidget* host, const OpenSceneContextFactory& createContext) const;
    bool openFile(const QString& fileNamePath, const OpenSceneContextFactory& createContext) const;
    bool saveCurrentView(CimdrawCenterWidget* centerWidget,
                         const RenameCurrentTabFn& renameCurrentTab,
                         const AfterDocumentActionFn& afterSave) const;
    bool saveCurrentViewAs(QWidget* host,
                           CimdrawCenterWidget* centerWidget,
                           const RenameCurrentTabFn& renameCurrentTab,
                           const AfterDocumentActionFn& afterSave) const;
    bool saveCurrentViewAndSyncUi(CimdrawCenterWidget* centerWidget,
                                  QAction* undoAction,
                                  QAction* redoAction,
                                  QAction* saveAction,
                                  QAction* saveAsAction) const;
    bool saveCurrentViewAsAndSyncUi(QWidget* host,
                                    CimdrawCenterWidget* centerWidget,
                                    QAction* undoAction,
                                    QAction* redoAction,
                                    QAction* saveAction,
                                    QAction* saveAsAction) const;
};

#endif
