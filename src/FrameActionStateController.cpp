#include "FrameActionStateController.h"

#include <QAction>

#include "LzView.h"

void FrameActionStateController::syncUndoRedoActions(LzView* view, QAction* undoAction, QAction* redoAction) const
{
    if (!undoAction || !redoAction)
        return;

    if (!view)
    {
        undoAction->setEnabled(false);
        redoAction->setEnabled(false);
        return;
    }

    undoAction->setEnabled(view->getStack()->canUndo());
    redoAction->setEnabled(view->getStack()->canRedo());
}

void FrameActionStateController::syncFileActions(LzView* view, QAction* saveAction, QAction* saveAsAction) const
{
    const bool hasView = (view != nullptr);
    if (saveAction)
        saveAction->setEnabled(hasView && view->isModified());
    if (saveAsAction)
        saveAsAction->setEnabled(hasView);
}
