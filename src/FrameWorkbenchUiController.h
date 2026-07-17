#ifndef FRAMEWORKBENCHUICONTROLLER_H
#define FRAMEWORKBENCHUICONTROLLER_H

#include "FrameCimWorkbenchController.h"

#include <QGraphicsItem>
#include <QList>

class QPushButton;
class QTabWidget;
class QObject;
class CimdrawItemController;
class CimdrawScene;
class CimModelBrowserDock;

class FrameWorkbenchUiController
{
public:
    void handleCurrentTabChanged(QTabWidget* tabs,
                                 QPushButton* backgroundButton,
                                 const FrameCimWorkbenchController::ImportState* cimImportState = nullptr) const;
    QObject* handleCurrentObjectChanged(CimdrawItemController* propertyController,
                                        const QList<QGraphicsItem*>& items,
                                        const FrameCimWorkbenchController::ImportState* cimImportState = nullptr) const;

private:
    QObject* syncSelectionToPropertyPanel(CimdrawItemController* propertyController,
                                          const QList<QGraphicsItem*>& items) const;
    void syncCimBrowserSelection(const FrameCimWorkbenchController::ImportState* cimImportState,
                                 CimdrawScene* scene) const;
};

#endif

