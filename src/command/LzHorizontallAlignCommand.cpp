#include "LzHorizontallAlignCommand.h"
#include "LzLayoutCommandUtils.h"
#include "LzScene.h"
#include "item/TmpBase.h"

class LzHorizontallAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzHorizontallAlignCommand)
public:
    LzHorizontallAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzHorizontallAlignCommand* command)
        :q_ptr(command),scene(scene),items(items)
    {
        
    }
    LzScene* scene;
    QMap<QGraphicsItem*, QPointF> moveMap;
    QList<QGraphicsItem*> items;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    LzHorizontallAlignCommand* q_ptr;
};

LzHorizontallAlignCommand::LzHorizontallAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* command)
    :d_ptr(new LzHorizontallAlignCommandPrivate(items,scene,this))
{
    
}

bool LzHorizontallAlignCommand::validateChange()
{
    Q_D(LzHorizontallAlignCommand);
    if(d->items.isEmpty())
        return false;
    qreal y = 0;
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            y += shape->centerY();
        }
    }
    y = y / d->items.count();
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                if(!qFuzzyCompare(y, shape->centerY()))
                    return true;
            }
        }
    }
    return false;
}

void LzHorizontallAlignCommand::undo()
{
    Q_D(LzHorizontallAlignCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void LzHorizontallAlignCommand::redo()
{
    Q_D(LzHorizontallAlignCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    qreal y = 0;

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!lzIsHandle(item))
            {
                qreal tmp = shape->centerY();
                y += tmp;
            }
        }
    }
    y = y / d->items.count();
    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                QPointF dlt(0, y-shape->centerY());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
