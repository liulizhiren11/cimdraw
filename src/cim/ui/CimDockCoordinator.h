#ifndef CIMDOCKCOORDINATOR_H
#define CIMDOCKCOORDINATOR_H

class QMainWindow;
class CimModelBrowserDock;

class CimDockCoordinator
{
public:
    CimModelBrowserDock* ensureBrowserDock(QMainWindow* host, CimModelBrowserDock* currentDock) const;
};

#endif
