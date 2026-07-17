#include "CimdrawTopAlignCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawTopAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawTopAlignCommand)
public:
    CimdrawTopAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawTopAlignCommand* command)
        :q_ptr(command),scene(scene)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    CimdrawScene* scene;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QPointF> moveMap;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    CimdrawTopAlignCommand* q_ptr;
};

CimdrawTopAlignCommand::CimdrawTopAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent)
    :d_ptr(new CimdrawTopAlignCommandPrivate(items,scene,this))
{

}

bool CimdrawTopAlignCommand::validateChange()
{
    Q_D(CimdrawTopAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d->items.first());
    qreal top = shape->itemTop();
    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                qreal tmp = shape->itemTop();
                if(tmp < top)
                {
                    top = tmp;
                }
            }
        }
    }
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!qFuzzyCompare(top,shape->itemTop()))
                return true;
        }
    }
    return false;
}

void CimdrawTopAlignCommand::undo()
{
    cimdrawBeginLayoutCommand(d_ptr->scene);
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                item->moveBy(- d_ptr->moveMap[item].x(), - d_ptr->moveMap[item].y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    cimdrawEndLayoutCommand(d_ptr->scene);
    d_ptr->moveMap.clear();
}

void CimdrawTopAlignCommand::redo()
{
    d_ptr->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d_ptr->items);
    cimdrawBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal top = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                qreal tmp = qgraphicsitem_cast<TmpShape*>(item)->itemTop();
                if (tmp < top)
                {
                    top = tmp;
                }
            }
        }
    }

    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                QPointF dlt(0, top- shape->itemTop());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    cimdrawEndLayoutCommand(d_ptr->scene);
}
