#ifndef CIMDRAWVERTICALLALIGNCOMMAND_H
#define CIMDRAWVERTICALLALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawVerticallAlignCommandPrivate;
class CimdrawVerticallAlignCommand : public QUndoCommand
{
public:
    CimdrawVerticallAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawVerticallAlignCommand);
    QScopedPointer<CimdrawVerticallAlignCommandPrivate> d_ptr;
};
#endif
