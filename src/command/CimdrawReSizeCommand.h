#ifndef CIMDRAWRESIZECOMMAND_H
#define CIMDRAWRESIZECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

class CimdrawScene;
class QGraphicsItem;
class CimdrawResizeCommandPrivate;

class CimdrawResizeCommand : public QUndoCommand
{
public:
    CimdrawResizeCommand(QGraphicsItem* item, CimdrawScene* scene, int handle, const QPointF& delta , QUndoCommand* parent = nullptr);

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawResizeCommand);
    QScopedPointer<CimdrawResizeCommandPrivate> d_ptr; 
};

#endif
