#include "CimdrawRightAlignCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "Item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawRightAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawRightAlignCommand)
public:
    CimdrawRightAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawRightAlignCommand* command)
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
    CimdrawRightAlignCommand* q_ptr;
};


CimdrawRightAlignCommand::CimdrawRightAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent)
    :d_ptr(new CimdrawRightAlignCommandPrivate(items,scene,this))
{

}

bool CimdrawRightAlignCommand::validateChange()
{
    Q_D(CimdrawRightAlignCommand);
    if(d->items.isEmpty())
        return false;
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal right = shape->itemRight();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
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

void CimdrawRightAlignCommand::undo()
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

void CimdrawRightAlignCommand::redo()
{
    d_ptr->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d_ptr->items);
    cimdrawBeginLayoutCommand(d_ptr->scene);
    TmpShape* shape = qgraphicsitem_cast<TmpShape*>(d_ptr->items.first());
    qreal right = shape->itemRight();
    for (auto item : d_ptr->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if(!cimdrawIsHandle(item))
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
            if(!cimdrawIsHandle(item))
            {
                QPointF dlt(right- shape->itemRight(),0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d_ptr->lineSnapshots);
    cimdrawEndLayoutCommand(d_ptr->scene);
}
