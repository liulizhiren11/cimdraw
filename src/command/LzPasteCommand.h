#ifndef LZPASTECOMMAND_H
#define LZPASTECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QList>

class LzScene;
class LzPasteCommandPrivate;

class LzPasteCommand : public QUndoCommand
{
public:
    LzPasteCommand(LzScene* scene, QList<QGraphicsItem*> pastedItems, QList<QGraphicsItem*> previousSelection,
                   QUndoCommand* parent = nullptr);

    ~LzPasteCommand() override;

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(LzPasteCommand);
    QScopedPointer<LzPasteCommandPrivate> d_ptr;
};
#endif
