#ifndef LZMOVECOMMAND_H
#define LZMOVECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

#include "LzScene.h"
#include "item/LzGroup.h"

class LzMoveCommandPrivate;

class LzMoveCommand : public QUndoCommand
{
public:
    LzMoveCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF &delta ,QUndoCommand* parent = nullptr);
    LzMoveCommand(QList<QGraphicsItem*> items,
                  LzScene* scene,
                  const QPointF& delta,
                  const QVector<LzConnectLinePathSnapshot>& beforeSnapshots,
                  const QVector<LzConnectLinePathSnapshot>& afterSnapshots,
                  QUndoCommand* parent = nullptr);
    ~LzMoveCommand() override;

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(LzMoveCommand);
    QScopedPointer<LzMoveCommandPrivate> d_ptr;
};

#endif
