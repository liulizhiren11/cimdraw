#include "LzResizeCommand.h"
#include "item/TmpBase.h"
#include "LzScene.h"

class LzResizeCommandPrivate
{
    Q_DECLARE_PUBLIC(LzResizeCommand)
public:
    LzResizeCommandPrivate(QGraphicsItem* item, LzScene* scene, int handle, const QPointF& delta ,LzResizeCommand* command)
        :q_ptr(command),scene(scene),delta(delta),bResized(true),handle(handle)
    {
        if(scene)
        {
            this->item = item;
        }
    }
    QGraphicsItem* item;
    LzScene* scene;
    int handle;
    QPointF delta;
    bool bResized;
    int opposite = HANDLE_NONE;
    LzResizeCommand* q_ptr;
};

LzResizeCommand::LzResizeCommand(QGraphicsItem* item, LzScene* scene, int handle, const QPointF& delta ,QUndoCommand* command)
    :d_ptr(new LzResizeCommandPrivate(item, scene,handle,delta,this))
{
    
}

void LzResizeCommand::undo()
{
    TmpShape* obj = qgraphicsitem_cast<TmpShape*>(d_ptr->item);
    if(obj&&!lzIsHandle(obj))
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

void LzResizeCommand::redo()
{
    TmpShape* obj = qgraphicsitem_cast<TmpShape*>(d_ptr->item);
    if(obj&&!lzIsHandle(obj))
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
