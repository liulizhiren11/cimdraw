#ifndef FRAMECIMSLOTWORKBENCHCONTROLLER_H
#define FRAMECIMSLOTWORKBENCHCONTROLLER_H

#include "FrameCimWorkbenchController.h"

class CimModelBrowserDock;
class CimdrawScene;
class QWidget;

class FrameCimSlotWorkbenchController
{
public:
    QString defaultImportPath() const;
    QString chooseImportPath(QWidget* host) const;
    FrameCimWorkbenchController::ImportState makeImportState(
        CimModelBrowserDock* browserDock,
        const CimModel& lastImportedModel,
        const CimSceneBuilder::BuildResult& buildResult) const;
    FrameCimWorkbenchController::ImportState makeActivationState(
        const CimSceneBuilder::BuildResult& buildResult) const;
    bool importFromPath(const QString& path,
                        const FrameCimWorkbenchController::ImportContext& context,
                        FrameCimWorkbenchController::ImportState* state) const;
    void activateObjectByMrid(const QString& mrid,
                              CimdrawScene* scene,
                              const FrameCimWorkbenchController::ImportState& state) const;
};

#endif
