#ifndef FRAMEDOCUMENTWORKBENCHCONTROLLER_H
#define FRAMEDOCUMENTWORKBENCHCONTROLLER_H

#include "FrameDocumentController.h"
#include "FrameFileController.h"

#include <functional>

class QString;
class QWidget;
class LzView;

class FrameDocumentWorkbenchController
{
public:
    using NewSceneContextFactory = std::function<FrameDocumentController::NewSceneContext()>;
    using OpenSceneContextFactory = std::function<FrameDocumentController::OpenSceneContext()>;
    using RenameCurrentTabFn = std::function<void(const QString&)>;
    using AfterDocumentActionFn = std::function<void()>;

    LzView* createPowerScene(const NewSceneContextFactory& createContext) const;
    bool openFile(QWidget* host, const OpenSceneContextFactory& createContext) const;
    bool openFile(const QString& fileNamePath, const OpenSceneContextFactory& createContext) const;
    bool saveCurrentView(LzView* view,
                         const RenameCurrentTabFn& renameCurrentTab,
                         const AfterDocumentActionFn& afterSave) const;
    bool saveCurrentViewAs(QWidget* host,
                           LzView* view,
                           const RenameCurrentTabFn& renameCurrentTab,
                           const AfterDocumentActionFn& afterSave) const;
    bool applySaveResult(const FrameFileController::SaveResult& result,
                         const RenameCurrentTabFn& renameCurrentTab,
                         const AfterDocumentActionFn& afterSave) const;
};

#endif
