#include "CimdrawLeftAlignCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawLeftAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawLeftAlignCommand)
public:
    CimdrawLeftAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawLeftAlignCommand* command)
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
    CimdrawLeftAlignCommand* q_ptr;
};

CimdrawLeftAlignCommand::CimdrawLeftAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* command)
    :d_ptr(new CimdrawLeftAlignCommandPrivate(items,scene,this))
{

}

bool CimdrawLeftAlignCommand::validateChange()
{
    Q_D(CimdrawLeftAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal left = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                qreal tmp = qgraphicsitem_cast<TmpShape*>(item)->itemLeft();
                if (tmp < left)
                {
                    left = tmp;
                }
            }
        }
    }
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!qFuzzyCompare(left,shape->itemLeft()))
                return true;
        }
    }
    return false;
}

void CimdrawLeftAlignCommand::undo()
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

void CimdrawLeftAlignCommand::redo()
{
    d_ptr->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d_ptr->items);
    cimdrawBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal left = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
            {
                qreal tmp = qgraphicsitem_cast<TmpShape*>(item)->itemLeft();
                if (tmp < left)
                {
                    left = tmp;
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
                QPointF dlt(left- shape->itemLeft(),0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    cimdrawEndLayoutCommand(d_ptr->scene);
}
