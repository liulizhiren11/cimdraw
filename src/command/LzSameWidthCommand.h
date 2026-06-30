#ifndef LZSAMEWIDTHCOMMAND_H
#define LZSAMEWIDTHCOMMAND_H

#include <QUndoCommand>
#include <QList>
#include <QScopedPointer>

class LzScene;
class QGraphicsItem;
class LzSameWidthCommandPrivate;
class LzSameWidthCommand : public QUndoCommand
{
public:
    LzSameWidthCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent = nullptr);

    bool validateChange();
    
    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzSameWidthCommand);
    QScopedPointer<LzSameWidthCommandPrivate> d_ptr;
};
#endif
