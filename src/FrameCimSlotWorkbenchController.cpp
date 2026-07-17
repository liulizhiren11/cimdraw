#include "FrameCimSlotWorkbenchController.h"

FrameCimWorkbenchController::ImportState FrameCimSlotWorkbenchController::makeImportState(
    CimModelBrowserDock* browserDock,
    const CimModel& lastImportedModel,
    const CimSceneBuilder::BuildResult& buildResult) const
{
    FrameCimWorkbenchController::ImportState state;
    state.browserDock = browserDock;
    state.lastImportedModel = lastImportedModel;
    state.applyBuildResult(buildResult);
    return state;
}

QString FrameCimSlotWorkbenchController::defaultImportPath() const
{
    FrameCimWorkbenchController controller;
    return controller.defaultImportPath();
}

QString FrameCimSlotWorkbenchController::chooseImportPath(QWidget* host) const
{
    FrameCimWorkbenchController controller;
    return controller.chooseImportPath(host);
}

FrameCimWorkbenchController::ImportState FrameCimSlotWorkbenchController::makeActivationState(
    const CimSceneBuilder::BuildResult& buildResult) const
{
    return makeImportState(nullptr, CimModel{}, buildResult);
}

bool FrameCimSlotWorkbenchController::importFromPath(
    const QString& path,
    const FrameCimWorkbenchController::ImportContext& context,
    FrameCimWorkbenchController::ImportState* state) const
{
    FrameCimWorkbenchController controller;
    return controller.importFromPath(path, context, state);
}

void FrameCimSlotWorkbenchController::activateObjectByMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const FrameCimWorkbenchController::ImportState& state) const
{
    FrameCimWorkbenchController controller;
    controller.activateObjectByMrid(mrid, scene, state);
}
