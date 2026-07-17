#ifndef CIMDRAWGROUPCOMMAND_H  
#define CIMDRAWGROUPCOMMAND_H

#include <QUndoCommand>

class CimdrawGroup;
class QGraphicsItemGroup;
class CimdrawScene;
class QGraphicsItem;
class CimdrawGroupCommandPrivate;
class CimdrawGroupCommand : public QUndoCommand
{
public:
    CimdrawGroupCommand(CimdrawScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawGroupCommand)
    CimdrawGroupCommandPrivate* d_ptr;
};

#endif
