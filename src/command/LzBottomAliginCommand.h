#ifndef LZBOTTOMALIGINCOMMAND_H
#define LZBOTTOMALIGINCOMMAND_H

#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class QGraphicsItem;
class LzScene;
class LzBottomAlignCommandPrivate;
class LzBottomAlignCommand : public QUndoCommand
{
public:
    LzBottomAlignCommand(QList<QGraphicsItem*>items,LzScene* scene, QUndoCommand* parent = nullptr);
    
    bool validateChange();

    void undo();

    void redo();

private:
    Q_DECLARE_PRIVATE(LzBottomAlignCommand);
    QScopedPointer<LzBottomAlignCommandPrivate> d_ptr;
};
#endif
