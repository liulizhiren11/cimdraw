#include "LzShortcutRightCommand.h"
#include "LzScene.h"
#include "item/TmpBase.h"
    
class LzShortcutRightCommandPrivate
{
public:
    LzShortcutRightCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta, LzShortcutRightCommand* command)
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
    LzShortcutRightCommand* q_ptr;
};

LzShortcutRightCommand::LzShortcutRightCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzShortcutRightCommandPrivate(items,scene,delta,this))
{

}   

void LzShortcutRightCommand::undo()
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

void LzShortcutRightCommand::redo()
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
