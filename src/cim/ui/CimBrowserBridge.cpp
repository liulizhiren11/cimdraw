#include "CimBrowserBridge.h"

#include <QDockWidget>
#include <QMainWindow>

#include "CimdrawScene.h"
#include "cim/ui/CimModelBrowserDock.h"

CimModelBrowserDock* CimBrowserBridge::ensureDock(QMainWindow* host, CimModelBrowserDock* currentDock) const
{
    if (!host)
        return currentDock;
    if (currentDock)
        return currentDock;

    auto* dock = new CimModelBrowserDock(host);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    host->addDockWidget(Qt::RightDockWidgetArea, dock);
    if (QDockWidget* propertyDock = host->findChild<QDockWidget*>(QStringLiteral("propertyDock")))
        host->tabifyDockWidget(propertyDock, dock);
    dock->raise();
    return dock;
}

void CimBrowserBridge::updateDock(CimModelBrowserDock* dock,
                                  const CimImportResult& result,
                                  const CimGraphicVisualSummary& visualSummary) const
{
    if (!dock)
        return;

    dock->setImportResult(result.model,
                          result.loadedProfiles,
                          result.logs,
                          result.warnings,
                          result.errors,
                          visualSummary,
                          result.topologyProjection);
    dock->show();
    dock->raise();
}
