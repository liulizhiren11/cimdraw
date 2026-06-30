#include "LzShortcutLeftCommand.h"
#include "LzScene.h"
#include "item/TmpBase.h"

class LzShortcutLeftCommandPrivate
{
public:
    LzShortcutLeftCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta, LzShortcutLeftCommand* command)
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
    LzShortcutLeftCommand* q_ptr;
};

LzShortcutLeftCommand::LzShortcutLeftCommand(QList<QGraphicsItem*> items,LzScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzShortcutLeftCommandPrivate(items,scene,delta,this))
{

}   

void LzShortcutLeftCommand::undo()
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

void LzShortcutLeftCommand::redo()
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
