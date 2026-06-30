#include "LzLeftAlignCommand.h"
#include "LzLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "LzScene.h"

class LzLeftAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzLeftAlignCommand)
public:
    LzLeftAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzLeftAlignCommand* command)
        :q_ptr(command),scene(scene)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    LzScene* scene;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QPointF> moveMap;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    LzLeftAlignCommand* q_ptr;
};

LzLeftAlignCommand::LzLeftAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* command)
    :d_ptr(new LzLeftAlignCommandPrivate(items,scene,this))
{

}

bool LzLeftAlignCommand::validateChange()
{
    Q_D(LzLeftAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal left = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
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

void LzLeftAlignCommand::undo()
{
    lzBeginLayoutCommand(d_ptr->scene);
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                item->moveBy(- d_ptr->moveMap[item].x(), - d_ptr->moveMap[item].y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    lzEndLayoutCommand(d_ptr->scene);
    d_ptr->moveMap.clear();
}

void LzLeftAlignCommand::redo()
{
    d_ptr->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d_ptr->items);
    lzBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal left = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
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
            if(!lzIsHandle(item))
            {
                QPointF dlt(left- shape->itemLeft(),0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    lzEndLayoutCommand(d_ptr->scene);
}
