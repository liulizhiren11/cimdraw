#ifndef FRAMEDOCKBUILDER_H
#define FRAMEDOCKBUILDER_H

#include <QDockWidget>

class QMainWindow;
class QTabWidget;
class LzFrame;
class LzItemController;
class LzToolBoxManager;

class FrameDockBuilder
{
public:
    struct DrawItemsDockResult
    {
        LzToolBoxManager* toolBox = nullptr;
        QDockWidget* dockWidget = nullptr;
    };

    struct PropertyDockResult
    {
        QTabWidget* tabWidget = nullptr;
        LzItemController* controller = nullptr;
        QDockWidget* dockWidget = nullptr;
    };

    DrawItemsDockResult createDrawItemsDock(QMainWindow* host, LzFrame* frame) const;
    PropertyDockResult createPropertyDock(QMainWindow* host, LzFrame* frame) const;
};

#endif
