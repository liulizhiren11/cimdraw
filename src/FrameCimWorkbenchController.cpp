#include "FrameCimWorkbenchController.h"

#include <QCoreApplication>
#include <QDir>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>

#include "CimdrawScene.h"
#include "cim/importer/CimImportResult.h"
#include "cim/ui/CimBrowserBridge.h"
#include "cim/ui/CimDockCoordinator.h"
#include "cim/ui/CimFrameCoordinator.h"
#include "cim/ui/CimImportController.h"
#include "cim/ui/CimModelBrowserDock.h"

CimGraphicVisualSummary FrameCimWorkbenchController::ImportState::buildGraphicVisualSummaryForQueryState(
    const CimModel& model,
    const CimGraphicQueryState& queryState)
{
    return CimdrawScene::buildGraphicVisualSummary(model, queryState);
}

CimGraphicVisualSummary FrameCimWorkbenchController::ImportState::buildGraphicVisualSummary(
    const CimModel& model) const
{
    return buildGraphicVisualSummaryForQueryState(model, graphicQueryState());
}

void FrameCimWorkbenchController::ImportState::syncBrowserSelection(CimdrawScene* scene) const
{
    if (!browserDock || !scene)
        return;

    const CimGraphicObjectSummary summary = scene->selectedGraphicObjectSummary(graphicQueryState());
    if (!summary.mrid.isEmpty())
        browserDock->selectObjectByMrid(summary.mrid);
}

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
    executionContext.populateScene = [this, state](CimdrawScene* scene, const CimModel& model)
    {
        populateScene(scene, model, state);
    };
    executionContext.afterSceneCreated = context.afterSceneCreated;
    const bool imported = coordinator.applyImportResult(executeResult.importResult, executionContext);
    if (imported && state->browserDock)
    {
        CimBrowserBridge bridge;
        const CimGraphicVisualSummary visualSummary =
            state->buildGraphicVisualSummary(executeResult.importResult.model);
        bridge.updateDock(state->browserDock,
                          executeResult.importResult,
                          visualSummary);
    }
    return imported;
}

void FrameCimWorkbenchController::activateObjectByMrid(const QString& mrid,
                                                       CimdrawScene* scene,
                                                       const ImportState& state) const
{
    if (!scene)
        return;

    scene->activateGraphicObjectByMrid(mrid, state.graphicQueryState());
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

void FrameCimWorkbenchController::populateScene(CimdrawScene* scene,
                                                const CimModel& model,
                                                ImportState* state) const
{
    if (!state)
        return;

    state->clearGraphicQueryState();
    CimSceneBuilder builder;
    const CimSceneBuilder::BuildResult buildResult = builder.populateSceneFromCimModel(scene, model);
    state->applyBuildResult(buildResult);
}
