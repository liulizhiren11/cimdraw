#include "CimdrawHorizontallAlignCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"

class CimdrawHorizontallAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawHorizontallAlignCommand)
public:
    CimdrawHorizontallAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawHorizontallAlignCommand* command)
        :q_ptr(command),scene(scene),items(items)
    {
        
    }
    CimdrawScene* scene;
    QMap<QGraphicsItem*, QPointF> moveMap;
    QList<QGraphicsItem*> items;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    CimdrawHorizontallAlignCommand* q_ptr;
};

CimdrawHorizontallAlignCommand::CimdrawHorizontallAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* command)
    :d_ptr(new CimdrawHorizontallAlignCommandPrivate(items,scene,this))
{
    
}

bool CimdrawHorizontallAlignCommand::validateChange()
{
    Q_D(CimdrawHorizontallAlignCommand);
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
            if(!cimdrawIsHandle(item))
            {
                if(!qFuzzyCompare(y, shape->centerY()))
                    return true;
            }
        }
    }
    return false;
}

void CimdrawHorizontallAlignCommand::undo()
{
    Q_D(CimdrawHorizontallAlignCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void CimdrawHorizontallAlignCommand::redo()
{
    Q_D(CimdrawHorizontallAlignCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    qreal y = 0;

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!cimdrawIsHandle(item))
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
            if(!cimdrawIsHandle(item))
            {
                QPointF dlt(0, y-shape->centerY());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
