#include "CimdrawAddCommand.h"
#include "Item/TmpBase.h"
#include "Item/CimdrawConnectLine.h"
#include "CimdrawConnectConfig.h"
#include "CimdrawScene.h"

class CimdrawAddCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawAddCommand)
public:
    CimdrawAddCommandPrivate(CimdrawAddCommand* command)
        :q_ptr(command)
    {

    }
    QGraphicsItem* item;
    CimdrawScene* scene;
    QList<QGraphicsItem*> items;
    bool add;
    CimdrawAddCommand* q_ptr;
};


CimdrawAddCommand::CimdrawAddCommand(QGraphicsItem* item,CimdrawScene* scene,QList<QGraphicsItem*> items,QUndoCommand* parent)
    :d_ptr(new CimdrawAddCommandPrivate(this))
{
    Q_D(CimdrawAddCommand);
    d->item = item;
    d->scene = scene;
    d->add = true;
    d->items = items;
}

CimdrawAddCommand::~CimdrawAddCommand()
{
    Q_D(CimdrawAddCommand);
}

void CimdrawAddCommand::undo()
{
    Q_D(CimdrawAddCommand);
    d->scene->deleteSelection(d->item);
    d->item->setZValue(0);
    d->scene->setSelections(d->items);
    emit d->scene->currentObjectChanged(d->items);
    d->add = false;
    d->scene->rebuildTopologyIndex();
}

void CimdrawAddCommand::redo()
{
    Q_D(CimdrawAddCommand);
    if(!d->add)
    {
        d->scene->cleanSelection();
        d->scene->insertSelection(d->item);
    }
    QList<QGraphicsItem*> list;
    list.append(d->item);
    emit d->scene->currentObjectChanged(list);

    QPainterPath itemShape = d->item->shape();
    itemShape.translate(d->item->pos());

    QList<QGraphicsItem*> overlappingItems;
    foreach(QGraphicsItem* item, d->scene->items())
    {
        if (item != d->item && item->shape().intersects(itemShape))
        {
            overlappingItems.append(item);
        }
    }

    if (dynamic_cast<CimdrawConnectLine*>(d->item))
    {
        d->item->setZValue(CimdrawConnectLineLayerZ);
    }
    else
    {
        qreal maxZValue = 0;
        for (QGraphicsItem* item : overlappingItems)
            maxZValue = qMax(maxZValue, item->zValue());
        if (maxZValue > 0)
            d->item->setZValue(maxZValue + 1);
    }
    d->scene->rebuildTopologyIndex();
}
