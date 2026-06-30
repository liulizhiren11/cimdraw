#include "LzVerticallAlignCommand.h"
#include "LzLayoutCommandUtils.h"
#include "LzScene.h"
#include "item/TmpBase.h"

class LzVerticallAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzVerticallAlignCommand)
public:
    LzVerticallAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzVerticallAlignCommand* command)
        :items(items),scene(scene),q_ptr(command)
    {

    }
   LzScene* scene;
   QList<QGraphicsItem*> items;
   QMap<QGraphicsItem*, QPointF> moveMap;
   QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
   bool m_hasChange = false;
   LzVerticallAlignCommand* q_ptr;
};

LzVerticallAlignCommand::LzVerticallAlignCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new LzVerticallAlignCommandPrivate(items,scene,this))
{

}

bool LzVerticallAlignCommand::validateChange()
{
    Q_D(LzVerticallAlignCommand);
    if(d->items.isEmpty())
        return false;
    qreal x = 0;
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            x += shape->centerX();
        }
    }
    x = x / d->items.count();
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                if(!qFuzzyCompare(x, shape->centerX()))
                    return true;
            }
        }
    }
    return false;
}

void LzVerticallAlignCommand::undo()
{
    Q_D(LzVerticallAlignCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void LzVerticallAlignCommand::redo()
{
    Q_D(LzVerticallAlignCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    qreal x = 0;

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!lzIsHandle(item))
            {
                qreal tmp = shape->centerX();
                x += tmp;
            }
        }
    }

    x = x / d->items.count();

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                QPointF dlt(x-shape->centerX(), 0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
