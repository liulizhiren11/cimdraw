#ifndef FRAMEACTIONBUILDER_H
#define FRAMEACTIONBUILDER_H

class QAction;
class QMainWindow;
class LzFrame;

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

    FileActionsResult installFileActions(QMainWindow* host, LzFrame* frame) const;
    EditActionsResult installEditActions(QMainWindow* host, LzFrame* frame) const;
    void installViewActions(QMainWindow* host, LzFrame* frame) const;
    void installTopologyActions(QMainWindow* host, LzFrame* frame) const;
    void installAlignActions(QMainWindow* host, LzFrame* frame) const;
    void installHelpMenu(QMainWindow* host, LzFrame* frame) const;
};

#endif
