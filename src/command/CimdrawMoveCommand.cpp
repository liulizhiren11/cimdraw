#include "CimdrawMoveCommand.h"
#include "item/TmpBase.h"
#include "item/CimdrawConnectLine.h"

class CimdrawMoveCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawMoveCommand)
public:
    CimdrawMoveCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF &delta ,CimdrawMoveCommand* command)
        :q_ptr(command),scene(scene),delta(delta),bMoved(true)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    CimdrawMoveCommandPrivate(QList<QGraphicsItem*> items,
                         CimdrawScene* scene,
                         const QPointF& delta,
                         const QVector<CimdrawConnectLinePathSnapshot>& beforeSnapshots,
                         const QVector<CimdrawConnectLinePathSnapshot>& afterSnapshots,
                         CimdrawMoveCommand* command)
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
    CimdrawScene* scene;
    bool bMoved;
    QVector<CimdrawConnectLinePathSnapshot> beforeLineSnapshots;
    QVector<CimdrawConnectLinePathSnapshot> afterLineSnapshots;
    CimdrawMoveCommand* q_ptr;
};

CimdrawMoveCommand::CimdrawMoveCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF &delta ,QUndoCommand* parent)
    :d_ptr(new CimdrawMoveCommandPrivate(items,scene,delta,this))
{

}

CimdrawMoveCommand::CimdrawMoveCommand(QList<QGraphicsItem*> items,
                             CimdrawScene* scene,
                             const QPointF& delta,
                             const QVector<CimdrawConnectLinePathSnapshot>& beforeSnapshots,
                             const QVector<CimdrawConnectLinePathSnapshot>& afterSnapshots,
                             QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new CimdrawMoveCommandPrivate(items, scene, delta, beforeSnapshots, afterSnapshots, this))
{
}

CimdrawMoveCommand::~CimdrawMoveCommand() = default;

void CimdrawMoveCommand::undo()
{
    if (d_ptr->scene)
        d_ptr->scene->setInteractiveTransformActive(true);
    for(auto item : d_ptr->items)
    {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                shape->move( - d_ptr->delta);
            }
        }
    }
    CimdrawGroup::restoreConnectLineSnapshots(d_ptr->beforeLineSnapshots);
    if (d_ptr->scene)
        d_ptr->scene->setInteractiveTransformActive(false);
    d_ptr->bMoved = false;
}

void CimdrawMoveCommand::redo()
{
    if(!d_ptr->bMoved)
    {
        if (d_ptr->scene)
            d_ptr->scene->setInteractiveTransformActive(true);
        for(auto item : d_ptr->items)
        {
            if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
            {
                if(!cimdrawIsHandle(item))
                {
                    shape->move(d_ptr->delta);
                }
            }
        }
        CimdrawGroup::restoreConnectLineSnapshots(d_ptr->afterLineSnapshots);
        if (d_ptr->scene)
            d_ptr->scene->setInteractiveTransformActive(false);
    }
}
