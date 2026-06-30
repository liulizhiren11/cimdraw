#include "FrameWorkbenchUiController.h"

#include <QPushButton>
#include <QTabWidget>

#include "FrameCanvasAppearanceController.h"
#include "LzItemController.h"
#include "LzScene.h"
#include "LzView.h"
#include "cim/ui/CimBrowserBridge.h"
#include "cim/ui/CimModelBrowserDock.h"

void FrameWorkbenchUiController::handleCurrentTabChanged(QTabWidget* tabs,
                                                         QPushButton* backgroundButton,
                                                         CimModelBrowserDock* browserDock) const
{
    FrameCanvasAppearanceController appearanceController;
    appearanceController.syncBackgroundButtonFromCurrentView(tabs, backgroundButton);

    auto* view = tabs ? qobject_cast<LzView*>(tabs->currentWidget()) : nullptr;
    syncCimBrowserSelection(browserDock, view ? view->getScene() : nullptr);
}

QObject* FrameWorkbenchUiController::handleCurrentObjectChanged(LzItemController* propertyController,
                                                                const QList<QGraphicsItem*>& items,
                                                                CimModelBrowserDock* browserDock) const
{
    QObject* controlledObject = syncSelectionToPropertyPanel(propertyController, items);

    LzScene* scene = nullptr;
    if (!items.isEmpty() && items.first())
        scene = qobject_cast<LzScene*>(items.first()->scene());
    syncCimBrowserSelection(browserDock, scene);
    return controlledObject;
}

QObject* FrameWorkbenchUiController::syncSelectionToPropertyPanel(LzItemController* propertyController,
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
    LzScene* scene = qobject_cast<LzScene*>(item ? item->scene() : nullptr);
    if (scene)
        propertyController->setView(scene->getView());
    propertyController->setObject(object);
    return object;
}

void FrameWorkbenchUiController::syncCimBrowserSelection(CimModelBrowserDock* browserDock, LzScene* scene) const
{
    CimBrowserBridge bridge;
    bridge.syncDockSelectionFromScene(browserDock, scene);
}
