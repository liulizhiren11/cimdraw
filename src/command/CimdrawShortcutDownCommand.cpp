#include "CimdrawShortcutDownCommand.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"
    
class CimdrawShortcutDownCommandPrivate
{
public:
    CimdrawShortcutDownCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta, CimdrawShortcutDownCommand* command)
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
    CimdrawShortcutDownCommand* q_ptr;
};

CimdrawShortcutDownCommand::CimdrawShortcutDownCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawShortcutDownCommandPrivate(items,scene,delta,this))
{
    
}   

void CimdrawShortcutDownCommand::undo()
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

void CimdrawShortcutDownCommand::redo()
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
