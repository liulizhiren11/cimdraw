#ifndef FRAMECIMSLOTWORKBENCHCONTROLLER_H
#define FRAMECIMSLOTWORKBENCHCONTROLLER_H

#include "FrameCimWorkbenchController.h"

class QGraphicsItem;
template <typename Key, typename T> class QHash;
class CimModelBrowserDock;
class LzScene;
class QWidget;

class FrameCimSlotWorkbenchController
{
public:
    QString defaultImportPath() const;
    QString chooseImportPath(QWidget* host) const;
    FrameCimWorkbenchController::ImportState makeImportState(
        CimModelBrowserDock* browserDock,
        const CimModel& lastImportedModel,
        const QHash<QString, QGraphicsItem*>& shapeByMrid) const;
    FrameCimWorkbenchController::ImportState makeActivationState(
        const QHash<QString, QGraphicsItem*>& shapeByMrid) const;
    void storeImportState(const FrameCimWorkbenchController::ImportState& state,
                          CimModelBrowserDock*& browserDock,
                          CimModel& lastImportedModel,
                          QHash<QString, QGraphicsItem*>& shapeByMrid) const;
    bool importFromPath(const QString& path,
                        const FrameCimWorkbenchController::ImportContext& context,
                        FrameCimWorkbenchController::ImportState* state) const;
    void activateObjectByMrid(const QString& mrid,
                              LzScene* scene,
                              const FrameCimWorkbenchController::ImportState& state) const;
};

#endif
