#ifndef CIMDRAWRIGHTALIGNCOMMAND_H
#define CIMDRAWRIGHTALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;;
class CimdrawScene;
class CimdrawRightAlignCommandPrivate;
class CimdrawRightAlignCommand : public QUndoCommand
{
public:
    CimdrawRightAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();

    void redo() override;

    void undo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawRightAlignCommand);
    QScopedPointer<CimdrawRightAlignCommandPrivate> d_ptr;
};
#endif
