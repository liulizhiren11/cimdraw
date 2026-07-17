#ifndef CIMDRAWTOPALIGNCOMMAND_H
#define CIMDRAWTOPALIGNCOMMAND_H

#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class QGraphicsItem;
class CimdrawScene;
class CimdrawTopAlignCommandPrivate;

class CimdrawTopAlignCommand : public QUndoCommand
{
public:
    CimdrawTopAlignCommand(QList<QGraphicsItem*> items, CimdrawScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawTopAlignCommand);
    QScopedPointer<CimdrawTopAlignCommandPrivate> d_ptr;
};

#endif
