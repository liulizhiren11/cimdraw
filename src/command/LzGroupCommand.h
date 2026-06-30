#ifndef LZGROUPCOMMAND_H  
#define LZGROUPCOMMAND_H

#include <QUndoCommand>

class LzGroup;
class QGraphicsItemGroup;
class LzScene;
class QGraphicsItem;
class LzGroupCommandPrivate;
class LzGroupCommand : public QUndoCommand
{
public:
    LzGroupCommand(LzScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzGroupCommand)
    LzGroupCommandPrivate* d_ptr;
};

#endif
