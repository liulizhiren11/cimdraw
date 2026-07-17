#ifndef CIMDRAWUNGROUPCOMMAND_H  
#define CIMDRAWUNGROUPCOMMAND_H

#include <QUndoCommand>

class CimdrawGroup;
class CimdrawScene;
class CimdrawUngroupCommandPrivate;
class CimdrawUngroupCommand : public QUndoCommand
{
public:
    CimdrawUngroupCommand(CimdrawGroup* item,CimdrawScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawUngroupCommand)
    CimdrawUngroupCommandPrivate* d_ptr;
};

#endif
