#include "CimdrawGroupCommand.h"
#include "item/CimdrawGroup.h"
#include "item/CimdrawConnectLine.h"
#include "item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawGroupCommandPrivate
{
public:
    CimdrawGroupCommandPrivate(CimdrawGroupCommand* command,CimdrawScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items)
        : q_ptr(command),m_scene(scene),m_group(group),m_items(items),m_isGroup(true)
    {
        CimdrawGroup::collectConnectLineSnapshots(m_items, m_lineSnapshots);
    }
    ~CimdrawGroupCommandPrivate();
    bool m_isGroup;
    CimdrawGroupCommand* q_ptr;
    QGraphicsItemGroup* m_group;
    QList<QGraphicsItem*> m_items;
    CimdrawScene* m_scene;
    QVector<CimdrawConnectLinePathSnapshot> m_lineSnapshots;
};

CimdrawGroupCommand::CimdrawGroupCommand(CimdrawScene* scene,QGraphicsItemGroup* group,QList<QGraphicsItem*> items, QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawGroupCommandPrivate(this,scene,group,items))
{

}

void CimdrawGroupCommand::undo()
{
    CimdrawGroup* group = qgraphicsitem_cast<CimdrawGroup*>(d_ptr->m_group);
    if(group&&!cimdrawIsHandle(group))
    {
        group->setPreserveLinePathsDuringGeometryUpdate(true);
        d_ptr->m_scene->setInteractiveTransformActive(true);
        for(auto item : d_ptr->m_items)
        {
             if(item&&!cimdrawIsHandle(item))
              {
                group->removeFromGroup(item);
                d_ptr->m_scene->addSelection(item);
              }
        }
        CimdrawGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        CimdrawGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
        for(auto item : d_ptr->m_items)
        {
            if (item && !cimdrawIsHandle(item))
                CimdrawGroup::refreshConnectionsForItemTree(item, true);
        }
        CimdrawGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        d_ptr->m_scene->removeSelection(group);
        d_ptr->m_scene->removeSceneItem(group);
        d_ptr->m_scene->setInteractiveTransformActive(false);
        group->setPreserveLinePathsDuringGeometryUpdate(false);
    }
    d_ptr->m_isGroup = false;
    d_ptr->m_scene->rebuildTopologyIndex();
}

void CimdrawGroupCommand::redo()
{
    CimdrawGroup* group = qgraphicsitem_cast<CimdrawGroup*>(d_ptr->m_group);
    if(group&&!cimdrawIsHandle(group))
    {
        if(!d_ptr->m_isGroup)
        {
            group->setPreserveLinePathsDuringGeometryUpdate(true);
            d_ptr->m_scene->setInteractiveTransformActive(true);
            for(auto item : d_ptr->m_items)
            {
                if(item&&!cimdrawIsHandle(item))
                {
                    group->addToGroup(item);
                    item->setSelected(false);
                }
            }
            group->updateCoordinate();
            CimdrawGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
            CimdrawGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
            d_ptr->m_scene->setInteractiveTransformActive(false);
        }
        d_ptr->m_scene->cleanSelection();
        d_ptr->m_scene->addItem(group);
        CimdrawGroup::refreshConnectionsForItemTree(group, true);
        CimdrawGroup::restoreConnectLineSnapshotsExact(d_ptr->m_lineSnapshots);
        group->setPreserveLinePathsDuringGeometryUpdate(false);
        d_ptr->m_scene->addSelection(group);
    }
    d_ptr->m_scene->rebuildTopologyIndex();
}
