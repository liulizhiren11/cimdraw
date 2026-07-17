#include "CimdrawControlCommand.h"

class CimdrawControlCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawControlCommand)
public:
    CimdrawControlCommandPrivate(CimdrawControlCommand* command)
        :q_ptr(command)
    {

    }
    CimdrawControlCommand* q_ptr;
};

CimdrawControlCommand::CimdrawControlCommand(QUndoCommand* parent)
    :d_ptr(new CimdrawControlCommandPrivate(this))
{

}

void CimdrawControlCommand::undo()
{

}

void CimdrawControlCommand::redo()
{

}
