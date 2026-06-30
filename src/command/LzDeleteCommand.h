#ifndef LZDELETECOMMAND_H
#define LZDELETECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzDeleteCommandPrivate;

class LzDeleteCommand : public QUndoCommand
{
public:
    LzDeleteCommand(QList<QGraphicsItem*> items, LzScene* scene, QUndoCommand* command = nullptr);

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzDeleteCommand);
    QScopedPointer<LzDeleteCommandPrivate> d_ptr;
};

#endif
