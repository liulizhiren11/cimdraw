#include "FrameCimWorkbenchController.h"

#include <QCoreApplication>
#include <QDir>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>

#include "LzScene.h"
#include "cim/importer/CimImportResult.h"
#include "cim/ui/CimBrowserBridge.h"
#include "cim/ui/CimDockCoordinator.h"
#include "cim/ui/CimFrameCoordinator.h"
#include "cim/ui/CimImportController.h"
#include "cim/ui/CimSceneBuilder.h"

QString FrameCimWorkbenchController::defaultImportPath() const
{
    return QDir::cleanPath(
        QCoreApplication::applicationDirPath() + QStringLiteral("/../../tests/data/cim/minimal/EQ_SSH_sample"));
}

QString FrameCimWorkbenchController::chooseImportPath(QWidget* host) const
{
    CimFrameCoordinator coordinator;
    const CimFrameCoordinator::ImportDialogResult result =
        coordinator.chooseImportPath(host, defaultImportPath());
    return result.accepted ? result.selectedPath : QString();
}

bool FrameCimWorkbenchController::importFromPath(const QString& path,
                                                 const ImportContext& context,
                                                 ImportState* state) const
{
    if (!state || path.trimmed().isEmpty())
        return false;

    CimImportController importController;
    const CimImportController::ExecuteResult executeResult = importController.executeImport(path);
    if (!executeResult.accepted)
    {
        if (!executeResult.validationError.isEmpty())
        {
            if (context.messageLineEdit)
                context.messageLineEdit->setText(QObject::tr("CIM 导入失败"));
            QMessageBox::warning(context.host, QObject::tr("导入 CIM/CGMES"), executeResult.validationError);
        }
        return false;
    }

    state->browserDock = ensureBrowserDock(context.host, state->browserDock, context.onBrowserDockReady);
    state->lastImportedModel = executeResult.importResult.model;

    CimFrameCoordinator coordinator;
    CimFrameCoordinator::ImportExecutionContext executionContext;
    executionContext.host = context.host;
    executionContext.messageLineEdit = context.messageLineEdit;
    executionContext.browserDock = state->browserDock;
    executionContext.createTargetView = context.createTargetView;
    executionContext.renameCurrentTab = context.renameCurrentTab;
    executionContext.populateScene = [this, state](LzScene* scene, const CimModel& model)
    {
        populateScene(scene, model, state->shapeByMrid, state->generatedHelperLineCount);
    };
    executionContext.afterSceneCreated = context.afterSceneCreated;
    const bool imported = coordinator.applyImportResult(executeResult.importResult, executionContext);
    if (imported && state->browserDock)
    {
        CimBrowserBridge bridge;
        bridge.updateDock(state->browserDock,
                          executeResult.importResult,
                          state->shapeByMrid,
                          state->generatedHelperLineCount);
    }
    return imported;
}

void FrameCimWorkbenchController::activateObjectByMrid(const QString& mrid,
                                                       LzScene* scene,
                                                       const ImportState& state) const
{
    CimBrowserBridge bridge;
    bridge.activateSceneItemByMrid(mrid, scene, state.shapeByMrid);
}

CimModelBrowserDock* FrameCimWorkbenchController::ensureBrowserDock(
    QWidget* host,
    CimModelBrowserDock* currentDock,
    const std::function<void(CimModelBrowserDock*)>& onBrowserDockReady) const
{
    auto* mainWindow = qobject_cast<QMainWindow*>(host);
    if (!mainWindow)
        return currentDock;

    const bool created = (currentDock == nullptr);
    CimDockCoordinator coordinator;
    CimModelBrowserDock* dock = coordinator.ensureBrowserDock(mainWindow, currentDock);
    if (created && dock && onBrowserDockReady)
        onBrowserDockReady(dock);
    return dock;
}

void FrameCimWorkbenchController::populateScene(LzScene* scene,
                                                const CimModel& model,
                                                QHash<QString, QGraphicsItem*>& shapeByMrid,
                                                int& generatedHelperLineCount) const
{
    shapeByMrid.clear();
    generatedHelperLineCount = 0;
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(scene, model);
    shapeByMrid = buildResult.shapeByMrid;
    generatedHelperLineCount = buildResult.generatedHelperLineCount;
}
