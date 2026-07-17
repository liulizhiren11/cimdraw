#ifndef FRAMEDOCKBUILDER_H
#define FRAMEDOCKBUILDER_H

#include <QDockWidget>

class QMainWindow;
class QTabWidget;
class CimdrawFrame;
class CimdrawItemController;
class CimdrawToolBoxManager;

class FrameDockBuilder
{
public:
    struct DrawItemsDockResult
    {
        CimdrawToolBoxManager* toolBox = nullptr;
        QDockWidget* dockWidget = nullptr;
    };

    struct PropertyDockResult
    {
        QTabWidget* tabWidget = nullptr;
        CimdrawItemController* controller = nullptr;
        QDockWidget* dockWidget = nullptr;
    };

    DrawItemsDockResult createDrawItemsDock(QMainWindow* host, CimdrawFrame* frame) const;
    PropertyDockResult createPropertyDock(QMainWindow* host, CimdrawFrame* frame) const;
};

#endif

