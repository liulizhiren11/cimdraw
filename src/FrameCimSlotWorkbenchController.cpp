#include "FrameCimSlotWorkbenchController.h"

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

FrameCimWorkbenchController::ImportState FrameCimSlotWorkbenchController::makeImportState(
    CimModelBrowserDock* browserDock,
    const CimModel& lastImportedModel,
    const QHash<QString, QGraphicsItem*>& shapeByMrid) const
{
    FrameCimWorkbenchController::ImportState state;
    state.browserDock = browserDock;
    state.lastImportedModel = lastImportedModel;
    state.shapeByMrid = shapeByMrid;
    return state;
}

FrameCimWorkbenchController::ImportState FrameCimSlotWorkbenchController::makeActivationState(
    const QHash<QString, QGraphicsItem*>& shapeByMrid) const
{
    return makeImportState(nullptr, CimModel{}, shapeByMrid);
}

void FrameCimSlotWorkbenchController::storeImportState(
    const FrameCimWorkbenchController::ImportState& state,
    CimModelBrowserDock*& browserDock,
    CimModel& lastImportedModel,
    QHash<QString, QGraphicsItem*>& shapeByMrid) const
{
    browserDock = state.browserDock;
    lastImportedModel = state.lastImportedModel;
    shapeByMrid = state.shapeByMrid;
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
    LzScene* scene,
    const FrameCimWorkbenchController::ImportState& state) const
{
    FrameCimWorkbenchController controller;
    controller.activateObjectByMrid(mrid, scene, state);
}
