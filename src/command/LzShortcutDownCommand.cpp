#include "LzShortcutDownCommand.h"
#include "LzScene.h"
#include "item/TmpBase.h"
    
class LzShortcutDownCommandPrivate
{
public:
    LzShortcutDownCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta, LzShortcutDownCommand* command)
        :q_ptr(command),scene(scene),delta(delta)
    {
        if(scene)
        {
            this->items = items;
        }
    }
    QPointF delta;
    QList<QGraphicsItem*> items;
    LzScene* scene;
    LzShortcutDownCommand* q_ptr;
};

LzShortcutDownCommand::LzShortcutDownCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzShortcutDownCommandPrivate(items,scene,delta,this))
{
    
}   

void LzShortcutDownCommand::undo()
{
    for(auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!lzIsHandle(item))
        {
            obj->moveBy(-d_ptr->delta.x(),-d_ptr->delta.y());
        }   
    }
}

void LzShortcutDownCommand::redo()
{
    for(auto item : d_ptr->items)
    {
        TmpShape* obj = qgraphicsitem_cast<TmpShape*>(item);
        if(obj&&!lzIsHandle(item))
        {
            obj->moveBy(d_ptr->delta.x(),d_ptr->delta.y());
        }
    }
}
