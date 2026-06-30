#include "LzControlCommand.h"

class LzControlCommandPrivate
{
    Q_DECLARE_PUBLIC(LzControlCommand)
public:
    LzControlCommandPrivate(LzControlCommand* command)
        :q_ptr(command)
    {

    }
    LzControlCommand* q_ptr;
};

LzControlCommand::LzControlCommand(QUndoCommand* parent)
    :d_ptr(new LzControlCommandPrivate(this))
{

}

void LzControlCommand::undo()
{

}

void LzControlCommand::redo()
{

}
