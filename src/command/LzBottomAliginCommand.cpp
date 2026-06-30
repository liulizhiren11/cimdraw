#include "LzBottomAliginCommand.h"
#include "LzLayoutCommandUtils.h"
#include "item/TmpBase.h"
#include "LzScene.h"

class LzBottomAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(LzBottomAlignCommand)
public:
    LzBottomAlignCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzBottomAlignCommand* command)
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
    LzBottomAlignCommand* q_ptr;
};

LzBottomAlignCommand::LzBottomAlignCommand(QList<QGraphicsItem*>items,LzScene* scene, QUndoCommand* parent)
    :d_ptr(new LzBottomAlignCommandPrivate(items,scene,this))
{

}

bool LzBottomAlignCommand::validateChange()
{
    Q_D(LzBottomAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal bottom = shape->itemBottom();

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!lzIsHandle(item))
            {
                qreal tmp = shape->itemBottom();
                if (tmp > bottom)
                {
                    bottom = tmp;
                }
            }
        }
    }
    for(auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!qFuzzyCompare(bottom,shape->itemBottom()))
                return true;
        }
    }
    return false;
}

void LzBottomAlignCommand::undo()
{
    Q_D(LzBottomAlignCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void LzBottomAlignCommand::redo()
{
    Q_D(LzBottomAlignCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal bottom = shape->itemBottom();

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!lzIsHandle(item))
            {
                qreal tmp = shape->itemBottom();
                if (tmp > bottom)
                {
                    bottom = tmp;
                }
            }
        }
    }

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!lzIsHandle(item))
            {
                QPointF dlt(0,bottom-shape->itemBottom());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
