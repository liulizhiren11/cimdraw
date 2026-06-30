#include "LzShortcutUpCommand.h"
#include "LzScene.h"
#include "item/TmpBase.h"
    
class LzShortcutUpCommandPrivate
{
public:
    LzShortcutUpCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta, LzShortcutUpCommand* command)
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
    LzShortcutUpCommand* q_ptr;
};

LzShortcutUpCommand::LzShortcutUpCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzShortcutUpCommandPrivate(items,scene,delta,this))
{

}   

void LzShortcutUpCommand::undo()
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

void LzShortcutUpCommand::redo()
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
