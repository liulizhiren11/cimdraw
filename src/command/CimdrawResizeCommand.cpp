#include "CimdrawResizeCommand.h"
#include "item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawResizeCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawResizeCommand)
public:
    CimdrawResizeCommandPrivate(QGraphicsItem* item, CimdrawScene* scene, int handle, const QPointF& delta ,CimdrawResizeCommand* command)
        :q_ptr(command),scene(scene),delta(delta),bResized(true),handle(handle)
    {
        if(scene)
        {
            this->item = item;
        }
    }
    QGraphicsItem* item;
    CimdrawScene* scene;
    int handle;
    QPointF delta;
    bool bResized;
    int opposite = HANDLE_NONE;
    CimdrawResizeCommand* q_ptr;
};

CimdrawResizeCommand::CimdrawResizeCommand(QGraphicsItem* item, CimdrawScene* scene, int handle, const QPointF& delta ,QUndoCommand* command)
    :d_ptr(new CimdrawResizeCommandPrivate(item, scene,handle,delta,this))
{
    
}

void CimdrawResizeCommand::undo()
{
    TmpShape* obj = qgraphicsitem_cast<TmpShape*>(d_ptr->item);
    if(obj&&!cimdrawIsHandle(obj))
    {
        d_ptr->opposite = obj->swapHandle(d_ptr->handle,d_ptr->delta);
        int handle = d_ptr->handle;
        if(HANDLE_NONE!=d_ptr->opposite)
        {
            handle = d_ptr->opposite;
        }
        obj->stretch(handle, 1 / d_ptr->delta.x(), 1 / d_ptr->delta.y(), obj->opposite(handle));
        obj->updateCoordinate();
        obj->update();
    }
    d_ptr->bResized = false;
}

void CimdrawResizeCommand::redo()
{
    TmpShape* obj = qgraphicsitem_cast<TmpShape*>(d_ptr->item);
    if(obj&&!cimdrawIsHandle(obj))
    {
        if(!d_ptr->bResized)
        {
            obj->stretch(d_ptr->handle, d_ptr->delta.x(), d_ptr->delta.y(), obj->opposite(d_ptr->handle));
            obj->updateCoordinate();
            obj->update();
        }
    }
    d_ptr->scene->setSelections({d_ptr->item});
}
