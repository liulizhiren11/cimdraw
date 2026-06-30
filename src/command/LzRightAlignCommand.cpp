#include "LzRightAlignCommand.h"
#include "LzLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "LzScene.h"

class LzRightAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzRightAlignCommand)
public:
    LzRightAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzRightAlignCommand* command)
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
    LzRightAlignCommand* q_ptr;
};


LzRightAlignCommand::LzRightAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent)
    :d_ptr(new LzRightAlignCommandPrivate(items,scene,this))
{

}

bool LzRightAlignCommand::validateChange()
{
    Q_D(LzRightAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal right = shape->itemRight();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                qreal tmp = qgraphicsitem_cast<TmpShape*>(item)->itemRight();
                if (tmp > right)
                {
                    right = tmp;
                }
            }
        }
    }
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!qFuzzyCompare(right,shape->itemRight()))
                return true;
        }
    }
    return false;
}

void LzRightAlignCommand::undo()
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

void LzRightAlignCommand::redo()
{
    d_ptr->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d_ptr->items);
    lzBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal right = shape->itemRight();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                qreal tmp = qgraphicsitem_cast<TmpShape*>(item)->itemRight();
                if (tmp > right)
                {
                    right = tmp;
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
                QPointF dlt(right- shape->itemRight(),0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    lzEndLayoutCommand(d_ptr->scene);
}
