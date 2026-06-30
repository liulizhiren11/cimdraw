#ifndef LZSAMESIZECOMMAND_H
#define LZSAMESIZECOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class LzScene;
class QGraphicsItem;
class LzSameSizeCommandPrivate;
class LzSameSizeCommand : public QUndoCommand
{
public:
    LzSameSizeCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzSameSizeCommand);
    QScopedPointer<LzSameSizeCommandPrivate> d_ptr;
};

#endif
