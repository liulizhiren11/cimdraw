#ifndef LZTOPALIGNCOMMAND_H
#define LZTOPALIGNCOMMAND_H

#include <QList>
#include <QScopedPointer>
#include <QUndoCommand>

class QGraphicsItem;
class LzScene;
class LzTopAlignCommandPrivate;

class LzTopAlignCommand : public QUndoCommand
{
public:
    LzTopAlignCommand(QList<QGraphicsItem*> items, LzScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzTopAlignCommand);
    QScopedPointer<LzTopAlignCommandPrivate> d_ptr;
};

#endif
