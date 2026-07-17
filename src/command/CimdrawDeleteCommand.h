#ifndef CIMDRAWDELETECOMMAND_H
#define CIMDRAWDELETECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawDeleteCommandPrivate;

class CimdrawDeleteCommand : public QUndoCommand
{
public:
    CimdrawDeleteCommand(QList<QGraphicsItem*> items, CimdrawScene* scene, QUndoCommand* command = nullptr);

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawDeleteCommand);
    QScopedPointer<CimdrawDeleteCommandPrivate> d_ptr;
};

#endif
