#ifndef FRAMEWINDOWSTATEWORKBENCHCONTROLLER_H
#define FRAMEWINDOWSTATEWORKBENCHCONTROLLER_H

#include "FrameCimWorkbenchController.h"

class QAction;
class QPushButton;
class QTabWidget;
class QString;
class CimdrawView;

class FrameWindowStateWorkbenchController
{
public:
    void syncActionStates(CimdrawView* view,
                          QAction* undoAction,
                          QAction* redoAction,
                          QAction* saveAction,
                          QAction* saveAsAction) const;
    void syncCurrentTabState(QTabWidget* tabs,
                             QPushButton* backgroundButton,
                             const FrameCimWorkbenchController::ImportState* cimImportState,
                             QAction* undoAction,
                             QAction* redoAction,
                             QAction* saveAction,
                             QAction* saveAsAction) const;
    bool renameCurrentTab(QTabWidget* tabs, const QString& title) const;
};

#endif
