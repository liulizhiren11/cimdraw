#ifndef CIMDRAWCUTCOMMAND_H
#define CIMDRAWCUTCOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class QGraphicsItem;
class CimdrawScene;
class CimdrawCutCommandPrivate;
class CimdrawCutCommand : public QUndoCommand
{
public:
    CimdrawCutCommand(QList<QGraphicsItem*> items, CimdrawScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawCutCommand);
    QScopedPointer<CimdrawCutCommandPrivate> d_ptr;
};
#endif
