#ifndef FRAMEACTIONSTATECONTROLLER_H
#define FRAMEACTIONSTATECONTROLLER_H

class QAction;
class LzView;

class FrameActionStateController
{
public:
    void syncUndoRedoActions(LzView* view, QAction* undoAction, QAction* redoAction) const;
    void syncFileActions(LzView* view, QAction* saveAction, QAction* saveAsAction) const;
};

#endif
