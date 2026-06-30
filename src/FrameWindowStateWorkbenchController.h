#ifndef FRAMEWINDOWSTATEWORKBENCHCONTROLLER_H
#define FRAMEWINDOWSTATEWORKBENCHCONTROLLER_H

class QAction;
class QPushButton;
class QTabWidget;
class QString;
class LzView;
class CimModelBrowserDock;

class FrameWindowStateWorkbenchController
{
public:
    void syncActionStates(LzView* view,
                          QAction* undoAction,
                          QAction* redoAction,
                          QAction* saveAction,
                          QAction* saveAsAction) const;
    void syncCurrentTabState(QTabWidget* tabs,
                             QPushButton* backgroundButton,
                             CimModelBrowserDock* browserDock,
                             QAction* undoAction,
                             QAction* redoAction,
                             QAction* saveAction,
                             QAction* saveAsAction) const;
    bool renameCurrentTab(QTabWidget* tabs, const QString& title) const;
};

#endif
