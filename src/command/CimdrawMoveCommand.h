#ifndef CIMDRAWMOVECOMMAND_H
#define CIMDRAWMOVECOMMAND_H

#include <QUndoCommand>
#include <QScopedPointer>

#include "CimdrawScene.h"
#include "item/CimdrawGroup.h"

class CimdrawMoveCommandPrivate;

class CimdrawMoveCommand : public QUndoCommand
{
public:
    CimdrawMoveCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF &delta ,QUndoCommand* parent = nullptr);
    CimdrawMoveCommand(QList<QGraphicsItem*> items,
                  CimdrawScene* scene,
                  const QPointF& delta,
                  const QVector<CimdrawConnectLinePathSnapshot>& beforeSnapshots,
                  const QVector<CimdrawConnectLinePathSnapshot>& afterSnapshots,
                  QUndoCommand* parent = nullptr);
    ~CimdrawMoveCommand() override;

    void undo() override;

    void redo() override;
private:
    Q_DECLARE_PRIVATE(CimdrawMoveCommand);
    QScopedPointer<CimdrawMoveCommandPrivate> d_ptr;
};

#endif
