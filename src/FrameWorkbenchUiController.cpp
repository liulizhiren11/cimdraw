#include "FrameWorkbenchUiController.h"

#include <QPushButton>
#include <QTabWidget>

#include "FrameCanvasAppearanceController.h"
#include "CimdrawItemController.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

void FrameWorkbenchUiController::handleCurrentTabChanged(QTabWidget* tabs,
                                                         QPushButton* backgroundButton,
                                                         const FrameCimWorkbenchController::ImportState* cimImportState) const
{
    FrameCanvasAppearanceController appearanceController;
    appearanceController.syncBackgroundButtonFromCurrentView(tabs, backgroundButton);

    auto* view = tabs ? qobject_cast<CimdrawView*>(tabs->currentWidget()) : nullptr;
    syncCimBrowserSelection(cimImportState, view ? view->getScene() : nullptr);
}

QObject* FrameWorkbenchUiController::handleCurrentObjectChanged(CimdrawItemController* propertyController,
                                                                const QList<QGraphicsItem*>& items,
                                                                const FrameCimWorkbenchController::ImportState* cimImportState) const
{
    QObject* controlledObject = syncSelectionToPropertyPanel(propertyController, items);

    CimdrawScene* scene = nullptr;
    if (!items.isEmpty() && items.first())
        scene = qobject_cast<CimdrawScene*>(items.first()->scene());
    syncCimBrowserSelection(cimImportState, scene);
    return controlledObject;
}

QObject* FrameWorkbenchUiController::syncSelectionToPropertyPanel(CimdrawItemController* propertyController,
                                                                  const QList<QGraphicsItem*>& items) const
{
    if (!propertyController)
        return nullptr;

    if (items.isEmpty())
    {
        propertyController->setObject(nullptr);
        return nullptr;
    }

    if (items.count() > 1)
        return propertyController->getObject();

    QGraphicsItem* item = items.first();
    QObject* object = dynamic_cast<QObject*>(item);
    CimdrawScene* scene = qobject_cast<CimdrawScene*>(item ? item->scene() : nullptr);
    if (scene)
        propertyController->setView(scene->getView());
    propertyController->setObject(object);
    return object;
}

void FrameWorkbenchUiController::syncCimBrowserSelection(
    const FrameCimWorkbenchController::ImportState* cimImportState,
    CimdrawScene* scene) const
{
    if (!cimImportState)
        return;

    cimImportState->syncBrowserSelection(scene);
}

