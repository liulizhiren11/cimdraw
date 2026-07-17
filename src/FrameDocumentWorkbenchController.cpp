#include "FrameDocumentWorkbenchController.h"

#include "CimdrawScene.h"
#include "CimdrawView.h"

CimdrawView* FrameDocumentWorkbenchController::createPowerScene(const NewSceneContextFactory& createContext) const
{
    if (!createContext)
        return nullptr;

    FrameDocumentController controller;
    return controller.createPowerScene(createContext());
}

bool FrameDocumentWorkbenchController::openFile(QWidget* host,
                                                const OpenSceneContextFactory& createContext) const
{
    if (!createContext)
        return false;

    FrameDocumentController controller;
    return controller.chooseAndOpenScene(host, createContext());
}

bool FrameDocumentWorkbenchController::openFile(const QString& fileNamePath,
                                                const OpenSceneContextFactory& createContext) const
{
    if (fileNamePath.isEmpty() || !createContext)
        return false;

    FrameDocumentController controller;
    const FrameDocumentController::OpenSceneContext context = createContext();
    CimdrawView* view = controller.openScene(fileNamePath, context);
    return controller.finalizeOpenedScene(view, fileNamePath, context.renameCurrentTab, context.afterOpen);
}

bool FrameDocumentWorkbenchController::saveCurrentView(CimdrawView* view,
                                                       const RenameCurrentTabFn& renameCurrentTab,
                                                       const AfterDocumentActionFn& afterSave) const
{
    FrameFileController controller;
    return applySaveResult(controller.saveCurrentView(view), renameCurrentTab, afterSave);
}

bool FrameDocumentWorkbenchController::saveCurrentViewAs(
    QWidget* host,
    CimdrawView* view,
    const RenameCurrentTabFn& renameCurrentTab,
    const AfterDocumentActionFn& afterSave) const
{
    FrameFileController controller;
    return applySaveResult(controller.saveCurrentViewAs(host, view, view ? view->getScene() : nullptr),
                           renameCurrentTab,
                           afterSave);
}

bool FrameDocumentWorkbenchController::applySaveResult(const FrameFileController::SaveResult& result,
                                                       const RenameCurrentTabFn& renameCurrentTab,
                                                       const AfterDocumentActionFn& afterSave) const
{
    if (!result.success)
        return false;

    if (renameCurrentTab && !result.tabName.isEmpty())
        renameCurrentTab(result.tabName);
    if (afterSave)
        afterSave();
    return true;
}
