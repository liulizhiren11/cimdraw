#ifndef LZVERTICALLALIGNCOMMAND_H
#define LZVERTICALLALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class LzScene;
class LzVerticallAlignCommandPrivate;
class LzVerticallAlignCommand : public QUndoCommand
{
public:
    LzVerticallAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzVerticallAlignCommand);
    QScopedPointer<LzVerticallAlignCommandPrivate> d_ptr;
};
#endif
