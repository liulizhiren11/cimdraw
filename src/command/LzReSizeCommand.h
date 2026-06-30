#ifndef LZRESIZECOMMAND_H
#define LZRESIZECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class LzScene;
class QGraphicsItem;
class LzResizeCommandPrivate;

class LzResizeCommand : public QUndoCommand
{
public:
    LzResizeCommand(QGraphicsItem* item, LzScene* scene, int handle, const QPointF& delta , QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzResizeCommand);
    QScopedPointer<LzResizeCommandPrivate> d_ptr; 
};

#endif
