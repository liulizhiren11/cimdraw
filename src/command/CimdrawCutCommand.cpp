#include "CimdrawCutCommand.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"
#include "CimdrawMimeData.h"
#include <QApplication>
#include <QClipboard>

class CimdrawCutCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawCutCommand)
public:
    CimdrawCutCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, CimdrawCutCommand* parent)
        :q_ptr(parent),scene(scene)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    QList<QGraphicsItem*> items;
    CimdrawScene* scene;
    CimdrawCutCommand* q_ptr;
};

CimdrawCutCommand::CimdrawCutCommand(QList<QGraphicsItem*> items, CimdrawScene* scene, QUndoCommand* command)
    :d_ptr(new CimdrawCutCommandPrivate(items, scene, this))
{

}

void CimdrawCutCommand::undo()
{
    for (auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!cimdrawIsHandle(item))
        {
            d_ptr->scene->insertSelection(item);
        }
    }
    emit d_ptr->scene->currentObjectChanged(d_ptr->items);
}

void CimdrawCutCommand::redo()
{
    QList<QGraphicsItem* > items;
    for (auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!cimdrawIsHandle(item))
        {
            d_ptr->scene->deleteSelection(item);
            items.append(item);
        }
    }
    CimdrawMimeData* data = new CimdrawMimeData(items);
    QApplication::clipboard()->setMimeData(data);
    QList<QGraphicsItem*> list;
    emit d_ptr->scene->currentObjectChanged(list);
}
