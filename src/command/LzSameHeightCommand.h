#ifndef LZSAMEHEIGHTCOMMAND_H
#define LZSAMEHEIGHTCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class LzScene;
class QGraphicsItem;
class LzSameHeightCommandPrivate;
class LzSameHeightCommand : public QUndoCommand
{
public:
    LzSameHeightCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzSameHeightCommand);
    QScopedPointer<LzSameHeightCommandPrivate> d_ptr;
};

#endif // LZSAMEHEIGHTCOMMAND_H
