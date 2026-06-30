#ifndef FRAMEWINDOWSTATESLOTWORKBENCHCONTROLLER_H
#define FRAMEWINDOWSTATESLOTWORKBENCHCONTROLLER_H

#include "FrameWindowStateWorkbenchController.h"

class QAction;
class LzCenterWidget;
class LzView;
class QPushButton;
class QString;
class QTabWidget;
class CimModelBrowserDock;

class FrameWindowStateSlotWorkbenchController
{
public:
    LzView* currentView(LzCenterWidget* centerWidget) const;
    QTabWidget* tabWidget(LzCenterWidget* centerWidget) const;

    void syncActionStates(LzCenterWidget* centerWidget,
                          QAction* undoAction,
                          QAction* redoAction,
                          QAction* saveAction,
                          QAction* saveAsAction) const;
    void syncCurrentTabState(LzCenterWidget* centerWidget,
                             QPushButton* backgroundButton,
                             CimModelBrowserDock* browserDock,
                             QAction* undoAction,
                             QAction* redoAction,
                             QAction* saveAction,
                             QAction* saveAsAction) const;
    bool renameCurrentTab(LzCenterWidget* centerWidget, const QString& title) const;
};

#endif
