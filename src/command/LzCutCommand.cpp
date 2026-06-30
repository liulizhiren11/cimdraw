#include "LzCutCommand.h"
#include "LzScene.h"
#include "item/TmpBase.h"
#include "LzMimeData.h"
#include <QApplication>
#include <QClipboard>

class LzCutCommandPrivate
{
    Q_DECLARE_PUBLIC(LzCutCommand)
public:
    LzCutCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, LzCutCommand* parent)
        :q_ptr(parent),scene(scene)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    QList<QGraphicsItem*> items;
    LzScene* scene;
    LzCutCommand* q_ptr;
};

LzCutCommand::LzCutCommand(QList<QGraphicsItem*> items, LzScene* scene, QUndoCommand* command)
    :d_ptr(new LzCutCommandPrivate(items, scene, this))
{

}

void LzCutCommand::undo()
{
    for (auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!lzIsHandle(item))
        {
            d_ptr->scene->insertSelection(item);
        }
    }
    emit d_ptr->scene->currentObjectChanged(d_ptr->items);
}

void LzCutCommand::redo()
{
    QList<QGraphicsItem* > items;
    for (auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!lzIsHandle(item))
        {
            d_ptr->scene->deleteSelection(item);
            items.append(item);
        }
    }
    LzMimeData* data = new LzMimeData(items);
    QApplication::clipboard()->setMimeData(data);
    QList<QGraphicsItem*> list;
    emit d_ptr->scene->currentObjectChanged(list);
}
