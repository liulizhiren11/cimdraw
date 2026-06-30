#ifndef LZHORIZONTALLALIGNCOMMAND_H
#define LZHORIZONTALLALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzHorizontallAlignCommandPrivate;
class LzHorizontallAlignCommand : public QUndoCommand
{
public:
    LzHorizontallAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzHorizontallAlignCommand);
    QScopedPointer<LzHorizontallAlignCommandPrivate> d_ptr;
};
#endif
