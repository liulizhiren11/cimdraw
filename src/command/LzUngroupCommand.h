#ifndef LZUNGROUPCOMMAND_H  
#define LZUNGROUPCOMMAND_H

#include <QUndoCommand>

class LzGroup;
class LzScene;
class LzUngroupCommandPrivate;
class LzUngroupCommand : public QUndoCommand
{
public:
    LzUngroupCommand(LzGroup* item,LzScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzUngroupCommand)
    LzUngroupCommandPrivate* d_ptr;
};

#endif
