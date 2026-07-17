#include "CimdrawShortcutRightCommand.h"
#include "CimdrawScene.h"
#include "item/TmpBase.h"
    
class CimdrawShortcutRightCommandPrivate
{
public:
    CimdrawShortcutRightCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta, CimdrawShortcutRightCommand* command)
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
    CimdrawShortcutRightCommand* q_ptr;
};

CimdrawShortcutRightCommand::CimdrawShortcutRightCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, const QPointF& delta,QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawShortcutRightCommandPrivate(items,scene,delta,this))
{

}   

void CimdrawShortcutRightCommand::undo()
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

void CimdrawShortcutRightCommand::redo()
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
