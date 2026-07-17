#include "CimdrawBottomAliginCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawBottomAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawBottomAlignCommand)
public:
    CimdrawBottomAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawBottomAlignCommand* command)
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
    CimdrawBottomAlignCommand* q_ptr;
};

CimdrawBottomAlignCommand::CimdrawBottomAlignCommand(QList<QGraphicsItem*>items,CimdrawScene* scene, QUndoCommand* parent)
    :d_ptr(new CimdrawBottomAlignCommandPrivate(items,scene,this))
{

}

bool CimdrawBottomAlignCommand::validateChange()
{
    Q_D(CimdrawBottomAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal bottom = shape->itemBottom();

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!cimdrawIsHandle(item))
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

void CimdrawBottomAlignCommand::undo()
{
    Q_D(CimdrawBottomAlignCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void CimdrawBottomAlignCommand::redo()
{
    Q_D(CimdrawBottomAlignCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal bottom = shape->itemBottom();

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!cimdrawIsHandle(item))
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
            if(!cimdrawIsHandle(item))
            {
                QPointF dlt(0,bottom-shape->itemBottom());
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
