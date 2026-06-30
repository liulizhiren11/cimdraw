#include "LzAddCommand.h"
#include "Item/TmpBase.h"
#include "Item/LzConnectLine.h"
#include "LzConnectConfig.h"
#include "LzScene.h"

class LzAddCommandPrivate
{
    Q_DECLARE_PUBLIC(LzAddCommand)
public:
    LzAddCommandPrivate(LzAddCommand* command)
        :q_ptr(command)
    {

    }
    QGraphicsItem* item;
    LzScene* scene;
    QList<QGraphicsItem*> items;
    bool add;
    LzAddCommand* q_ptr;
};


LzAddCommand::LzAddCommand(QGraphicsItem* item,LzScene* scene,QList<QGraphicsItem*> items,QUndoCommand* parent)
    :d_ptr(new LzAddCommandPrivate(this))
{
    Q_D(LzAddCommand);
    d->item = item;
    d->scene = scene;
    d->add = true;
    d->items = items;
}

LzAddCommand::~LzAddCommand()
{
    Q_D(LzAddCommand);
}

void LzAddCommand::undo()
{
    Q_D(LzAddCommand);
    d->scene->deleteSelection(d->item);
    d->item->setZValue(0);
    d->scene->setSelections(d->items);
    emit d->scene->currentObjectChanged(d->items);
    d->add = false;
    d->scene->rebuildTopologyIndex();
}

void LzAddCommand::redo()
{
    Q_D(LzAddCommand);
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

    if (dynamic_cast<LzConnectLine*>(d->item))
    {
        d->item->setZValue(LzConnectLineLayerZ);
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
