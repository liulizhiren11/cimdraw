#ifndef CIMDRAWHORIZONTALLALIGNCOMMAND_H
#define CIMDRAWHORIZONTALLALIGNCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawHorizontallAlignCommandPrivate;
class CimdrawHorizontallAlignCommand : public QUndoCommand
{
public:
    CimdrawHorizontallAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawHorizontallAlignCommand);
    QScopedPointer<CimdrawHorizontallAlignCommandPrivate> d_ptr;
};
#endif
