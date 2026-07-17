#include "CimdrawShortcutLeftCommand.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"

class CimdrawShortcutLeftCommandPrivate
{
public:
    CimdrawShortcutLeftCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta, CimdrawShortcutLeftCommand* command)
        :q_ptr(command),scene(scene),delta(delta)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    QPointF delta;
    QList<QGraphicsItem*> items;
    CimdrawScene* scene;
    CimdrawShortcutLeftCommand* q_ptr;
};

CimdrawShortcutLeftCommand::CimdrawShortcutLeftCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawShortcutLeftCommandPrivate(items,scene,delta,this))
{

}   

void CimdrawShortcutLeftCommand::undo()
{
    for(auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!cimdrawIsHandle(item))
        {
            obj->moveBy(-d_ptr->delta.x(),-d_ptr->delta.y());
        }
    }
}

void CimdrawShortcutLeftCommand::redo()
{
    for(auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!cimdrawIsHandle(item))
        {
            obj->moveBy(d_ptr->delta.x(),d_ptr->delta.y());
        }
    }
}
