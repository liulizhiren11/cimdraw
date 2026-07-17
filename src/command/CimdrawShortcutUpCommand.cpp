#include "CimdrawShortcutUpCommand.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"
    
class CimdrawShortcutUpCommandPrivate
{
public:
    CimdrawShortcutUpCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta, CimdrawShortcutUpCommand* command)
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
    CimdrawShortcutUpCommand* q_ptr;
};

CimdrawShortcutUpCommand::CimdrawShortcutUpCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawShortcutUpCommandPrivate(items,scene,delta,this))
{

}   

void CimdrawShortcutUpCommand::undo()
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

void CimdrawShortcutUpCommand::redo()
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
