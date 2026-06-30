#include "LzGroupCommand.h"
#include "item/LzGroup.h"
#include "item/LzConnectLine.h"
#include "item/TmpBase.h"
#include "LzScene.h"

class LzGroupCommandPrivate
{
public:
    LzGroupCommandPrivate(LzGroupCommand* command,LzScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items)
        : q_ptr(command),m_scene(scene),m_group(group),m_items(items),m_isGroup(true)
    {
        LzGroup::collectConnectLineSnapshots(m_items, m_lineSnapshots);
    }
    ~LzGroupCommandPrivate();
    bool m_isGroup;
    LzGroupCommand* q_ptr;
    QGraphicsItemGroup* m_group;
    QList<QGraphicsItem*> m_items;
    LzScene* m_scene;
    QVector<LzConnectLinePathSnapshot> m_lineSnapshots;
};

LzGroupCommand::LzGroupCommand(LzScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items, QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzGroupCommandPrivate(this,scene,group,items))
{

}

void LzGroupCommand::undo()
{
    LzGroup* group = qgraphicsitem_cast<LzGroup*>(d_ptr->m_group);
    if(group&&!lzIsHandle(group))
    {
        group->setPreserveLinePathsDuringGeometryUpdate(true);
        d_ptr->m_scene->setInteractiveTransformActive(true);
        for(auto item : d_ptr->m_items)
        {
             if(item&&!lzIsHandle(item))
              {
                group->removeFromGroup(item);
                d_ptr->m_scene->addSelection(item);
              }
        }
        LzGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        LzGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
        for(auto item : d_ptr->m_items)
        {
            if (item && !lzIsHandle(item))
                LzGroup::refreshConnectionsForItemTree(item, true);
        }
        LzGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        d_ptr->m_scene->removeSelection(group);
        d_ptr->m_scene->removeSceneItem(group);
        d_ptr->m_scene->setInteractiveTransformActive(false);
        group->setPreserveLinePathsDuringGeometryUpdate(false);
    }
    d_ptr->m_isGroup = false;
    d_ptr->m_scene->rebuildTopologyIndex();
}

void LzGroupCommand::redo()
{
    LzGroup* group = qgraphicsitem_cast<LzGroup*>(d_ptr->m_group);
    if(group&&!lzIsHandle(group))
    {
        if(!d_ptr->m_isGroup)
        {
            group->setPreserveLinePathsDuringGeometryUpdate(true);
            d_ptr->m_scene->setInteractiveTransformActive(true);
            for(auto item : d_ptr->m_items)
            {
                if(item&&!lzIsHandle(item))
                {
                    group->addToGroup(item);
                    item->setSelected(false);
                }
            }
            group->updateCoordinate();
            LzGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
            LzGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
            d_ptr->m_scene->setInteractiveTransformActive(false);
        }
        d_ptr->m_scene->cleanSelection();
        d_ptr->m_scene->addItem(group);
        LzGroup::refreshConnectionsForItemTree(group, true);
        LzGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        group->setPreserveLinePathsDuringGeometryUpdate(false);
        d_ptr->m_scene->addSelection(group);
    }
    d_ptr->m_scene->rebuildTopologyIndex();
}
