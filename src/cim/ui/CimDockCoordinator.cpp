#include "CimDockCoordinator.h"

#include <QMainWindow>

#include "cim/ui/CimBrowserBridge.h"
#include "cim/ui/CimModelBrowserDock.h"

CimModelBrowserDock* CimDockCoordinator::ensureBrowserDock(QMainWindow* host,
                                                           CimModelBrowserDock* currentDock) const
{
    CimBrowserBridge bridge;
    return bridge.ensureDock(host, currentDock);
}
