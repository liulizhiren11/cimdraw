#include "FrameActionStateController.h"

#include <QAction>

#include "CimdrawView.h"

void FrameActionStateController::syncUndoRedoActions(CimdrawView* view, QAction* undoAction, QAction* redoAction) const
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

void FrameActionStateController::syncFileActions(CimdrawView* view, QAction* saveAction, QAction* saveAsAction) const
{
    const bool hasView = (view != nullptr);
    if (saveAction)
        saveAction->setEnabled(hasView && view->isModified());
    if (saveAsAction)
        saveAsAction->setEnabled(hasView);
}
