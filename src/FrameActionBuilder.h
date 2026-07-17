#ifndef FRAMEACTIONBUILDER_H
#define FRAMEACTIONBUILDER_H

class QAction;
class QMainWindow;
class CimdrawFrame;

class FrameActionBuilder
{
public:
    struct FileActionsResult
    {
        QAction* actionFileSave = nullptr;
        QAction* actionFileSaveAs = nullptr;
    };

    struct EditActionsResult
    {
        QAction* actionEditUndo = nullptr;
        QAction* actionEditRedo = nullptr;
    };

    FileActionsResult installFileActions(QMainWindow* host, CimdrawFrame* frame) const;
    EditActionsResult installEditActions(QMainWindow* host, CimdrawFrame* frame) const;
    void installViewActions(QMainWindow* host, CimdrawFrame* frame) const;
    void installTopologyActions(QMainWindow* host, CimdrawFrame* frame) const;
    void installAlignActions(QMainWindow* host, CimdrawFrame* frame) const;
    void installHelpMenu(QMainWindow* host, CimdrawFrame* frame) const;
};

#endif
