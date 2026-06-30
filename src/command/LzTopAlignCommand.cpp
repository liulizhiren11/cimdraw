#include "LzTopAlignCommand.h"
#include "LzLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "LzScene.h"

class LzTopAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzTopAlignCommand)
public:
    LzTopAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzTopAlignCommand* command)
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
    LzTopAlignCommand* q_ptr;
};

LzTopAlignCommand::LzTopAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent)
    :d_ptr(new LzTopAlignCommandPrivate(items,scene,this))
{

}

bool LzTopAlignCommand::validateChange()
{
    Q_D(LzTopAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d->items.first());
    qreal top = shape->itemTop();
    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
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

void LzTopAlignCommand::undo()
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

void LzTopAlignCommand::redo()
{
    d_ptr->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d_ptr->items);
    lzBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal top = shape->itemLeft();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
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
            if(!lzIsHandle(item))
            {
                QPointF dlt(0, top- shape->itemTop());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    lzEndLayoutCommand(d_ptr->scene);
}
