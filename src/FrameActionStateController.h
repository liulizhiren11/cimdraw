#ifndef FRAMEACTIONSTATECONTROLLER_H
#define FRAMEACTIONSTATECONTROLLER_H

class QAction;
class CimdrawView;

class FrameActionStateController
{
public:
    void syncUndoRedoActions(CimdrawView* view, QAction* undoAction, QAction* redoAction) const;
    void syncFileActions(CimdrawView* view, QAction* saveAction, QAction* saveAsAction) const;
};

#endif
