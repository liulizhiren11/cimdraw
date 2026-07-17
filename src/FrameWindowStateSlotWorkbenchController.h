#ifndef FRAMEWINDOWSTATESLOTWORKBENCHCONTROLLER_H
#define FRAMEWINDOWSTATESLOTWORKBENCHCONTROLLER_H

#include "FrameCimWorkbenchController.h"
#include "FrameWindowStateWorkbenchController.h"

class QAction;
class CimdrawCenterWidget;
class CimdrawView;
class QPushButton;
class QString;
class QTabWidget;

class FrameWindowStateSlotWorkbenchController
{
public:
    CimdrawView* currentView(CimdrawCenterWidget* centerWidget) const;
    QTabWidget* tabWidget(CimdrawCenterWidget* centerWidget) const;

    void syncActionStates(CimdrawCenterWidget* centerWidget,
                          QAction* undoAction,
                          QAction* redoAction,
                          QAction* saveAction,
                          QAction* saveAsAction) const;
    void syncCurrentTabState(CimdrawCenterWidget* centerWidget,
                             QPushButton* backgroundButton,
                             const FrameCimWorkbenchController::ImportState* cimImportState,
                             QAction* undoAction,
                             QAction* redoAction,
                             QAction* saveAction,
                             QAction* saveAsAction) const;
    bool renameCurrentTab(CimdrawCenterWidget* centerWidget, const QString& title) const;
};

#endif
