#ifndef LZCUTCOMMAND_H
#define LZCUTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzCutCommandPrivate;
class LzCutCommand : public QUndoCommand
{
public:
    LzCutCommand(QList<QGraphicsItem*> items, LzScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzCutCommand);
    QScopedPointer<LzCutCommandPrivate> d_ptr;
};
#endif
