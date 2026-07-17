#ifndef CIMDRAWPASTECOMMAND_H
#define CIMDRAWPASTECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QList>

class CimdrawScene;
class CimdrawPasteCommandPrivate;

class CimdrawPasteCommand : public QUndoCommand
{
public:
    CimdrawPasteCommand(CimdrawScene* scene, QList<QGraphicsItem*> pastedItems, QList<QGraphicsItem*> previousSelection,
                   QUndoCommand* parent = nullptr);

    ~CimdrawPasteCommand() override;

    void undo() override;

    void redo() override;

private:
    Q_DECLARE_PRIVATE(CimdrawPasteCommand);
    QScopedPointer<CimdrawPasteCommandPrivate> d_ptr;
};
#endif
