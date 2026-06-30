#include "LzMoveCommand.h"
#include "item/TmpBase.h"
#include "item/LzConnectLine.h"

class LzMoveCommandPrivate
{
    Q_DECLARE_PUBLIC(LzMoveCommand)
public:
    LzMoveCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, const QPointF &delta ,LzMoveCommand* command)
        :q_ptr(command),scene(scene),delta(delta),bMoved(true)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    LzMoveCommandPrivate(QList<QGraphicsItem*> items,
                         LzScene* scene,
                         const QPointF& delta,
                         const QVector<LzConnectLinePathSnapshot>& beforeSnapshots,
                         const QVector<LzConnectLinePathSnapshot>& afterSnapshots,
                         LzMoveCommand* command)
        : q_ptr(command)
        , items(items)
        , delta(delta)
        , scene(scene)
        , bMoved(true)
        , beforeLineSnapshots(beforeSnapshots)
        , afterLineSnapshots(afterSnapshots)
    {
    }
    QList<QGraphicsItem*> items;
    QPointF delta;
    LzScene* scene;
    bool bMoved;
    QVector<LzConnectLinePathSnapshot> beforeLineSnapshots;
    QVector<LzConnectLinePathSnapshot> afterLineSnapshots;
    LzMoveCommand* q_ptr;
};

LzMoveCommand::LzMoveCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF &delta ,QUndoCommand* parent)
    :d_ptr(new LzMoveCommandPrivate(items,scene,delta,this))
{

}

LzMoveCommand::LzMoveCommand(QList<QGraphicsItem*> items,
                             LzScene* scene,
                             const QPointF& delta,
                             const QVector<LzConnectLinePathSnapshot>& beforeSnapshots,
                             const QVector<LzConnectLinePathSnapshot>& afterSnapshots,
                             QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzMoveCommandPrivate(items, scene, delta, beforeSnapshots, afterSnapshots, this))
{
}

LzMoveCommand::~LzMoveCommand() = default;

void LzMoveCommand::undo()
{
    if (d_ptr->scene)
        d_ptr->scene->setInteractiveTransformActive(true);
    for(auto item : d_ptr->items)
    {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                shape->move( - d_ptr->delta);
            }
        }
    }
    LzGroup::restoreConnectLineSnapshots(d_ptr->beforeLineSnapshots);
    if (d_ptr->scene)
        d_ptr->scene->setInteractiveTransformActive(false);
    d_ptr->bMoved = false;
}

void LzMoveCommand::redo()
{
    if(!d_ptr->bMoved)
    {
        if (d_ptr->scene)
            d_ptr->scene->setInteractiveTransformActive(true);
        for(auto item : d_ptr->items)
        {
            if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
            {
                if(!lzIsHandle(item))
                {
                    shape->move(d_ptr->delta);
                }
            }
        }
        LzGroup::restoreConnectLineSnapshots(d_ptr->afterLineSnapshots);
        if (d_ptr->scene)
            d_ptr->scene->setInteractiveTransformActive(false);
    }
}
