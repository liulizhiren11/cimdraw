#ifndef LZRIGHTALIGNCOMMAND_H
#define LZRIGHTALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;;
class LzScene;
class LzRightAlignCommandPrivate;
class LzRightAlignCommand : public QUndoCommand
{
public:
    LzRightAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();

    void redo() override;

    void undo() override;
private:
    Q_DECLARE_PRIVATE(LzRightAlignCommand);
    QScopedPointer<LzRightAlignCommandPrivate> d_ptr;
};
#endif
