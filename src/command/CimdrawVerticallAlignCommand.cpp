#include "CimdrawVerticallAlignCommand.h"
#include "CimdrawLayoutCommandUtils.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"

class CimdrawVerticallAlignCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawVerticallAlignCommand)
public:
    CimdrawVerticallAlignCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawVerticallAlignCommand* command)
        :items(items),scene(scene),q_ptr(command)
    {

    }
   CimdrawScene* scene;
   QList<QGraphicsItem*> items;
   QMap<QGraphicsItem*, QPointF> moveMap;
   QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
   bool m_hasChange = false;
   CimdrawVerticallAlignCommand* q_ptr;
};

CimdrawVerticallAlignCommand::CimdrawVerticallAlignCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new CimdrawVerticallAlignCommandPrivate(items,scene,this))
{

}

bool CimdrawVerticallAlignCommand::validateChange()
{
    Q_D(CimdrawVerticallAlignCommand);
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
            if(!cimdrawIsHandle(item))
            {
                if(!qFuzzyCompare(x, shape->centerX()))
                    return true;
            }
        }
    }
    return false;
}

void CimdrawVerticallAlignCommand::undo()
{
    Q_D(CimdrawVerticallAlignCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto item : d->items)
    {
        item->moveBy(- d->moveMap[item].x(), - d->moveMap[item].y());
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->moveMap.clear();
}

void CimdrawVerticallAlignCommand::redo()
{
    Q_D(CimdrawVerticallAlignCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    qreal x = 0;

    for (auto item : d->items)
    {
        if(TmpShape* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            if (!cimdrawIsHandle(item))
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
            if(!cimdrawIsHandle(item))
            {
                QPointF dlt(x-shape->centerX(), 0);
                d_ptr->moveMap[item] = dlt;
                item->moveBy(dlt.x(),dlt.y());
            }
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
