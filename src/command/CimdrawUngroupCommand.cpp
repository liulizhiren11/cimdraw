#include "CimdrawUngroupCommand.h"
#include "item/CimdrawGroup.h"
#include "item/CimdrawConnectLine.h"
#include "item/TmpBase.h"
#include "CimdrawScene.h"

class CimdrawUngroupCommandPrivate
{
public:
    CimdrawUngroupCommandPrivate(CimdrawGroup* group,CimdrawScene* scene,CimdrawUngroupCommand* command)
        : q_ptr(command),m_item(group),m_scene(scene)
    {
        for(auto item:group->childItems())
        {
            if(item&&!cimdrawIsHandle(item))
            m_items.append(item);
        }
    }
    ~CimdrawUngroupCommandPrivate();
    CimdrawScene* m_scene;
    CimdrawUngroupCommand* q_ptr;
    QList<QGraphicsItem*> m_items;
    CimdrawGroup* m_item;
};

CimdrawUngroupCommand::CimdrawUngroupCommand(CimdrawGroup* item,CimdrawScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new CimdrawUngroupCommandPrivate(item ,scene,this))
{
    
}

void CimdrawUngroupCommand::undo()
{
    d_ptr->m_scene->setInteractiveTransformActive(true);

    if (d_ptr->m_item && d_ptr->m_item->scene() != d_ptr->m_scene)
        d_ptr->m_scene->addItem(d_ptr->m_item);

    QVector<CimdrawConnectLinePathSnapshot> lineSnapshots;
    CimdrawGroup::collectConnectLineSnapshots(d_ptr->m_items, lineSnapshots);

    d_ptr->m_scene->cleanSelection();
    for(auto item : d_ptr->m_items)
    {
        if (item && !cimdrawIsHandle(item))
        {
            d_ptr->m_scene->removeSelection(item);
            d_ptr->m_item->addToGroup(item);
        }
    }
    d_ptr->m_item->updateCoordinate();
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    CimdrawGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
    CimdrawGroup::refreshConnectionsForItemTree(d_ptr->m_item, true);
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    d_ptr->m_scene->addSelection(d_ptr->m_item);
    d_ptr->m_scene->setInteractiveTransformActive(false);
    d_ptr->m_scene->rebuildTopologyIndex();
}

void CimdrawUngroupCommand::redo()
{
    d_ptr->m_scene->setInteractiveTransformActive(true);

    QVector<CimdrawConnectLinePathSnapshot> lineSnapshots;
    CimdrawGroup::collectConnectLineSnapshots(d_ptr->m_items, lineSnapshots);

    d_ptr->m_scene->removeSelection(d_ptr->m_item);
    for(auto item : d_ptr->m_items)
    {
        if (item && !cimdrawIsHandle(item))
        {
            d_ptr->m_item->removeFromGroup(item);
            d_ptr->m_scene->addSelection(item);
        }
    }
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    CimdrawGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
    for(auto item : d_ptr->m_items)
    {
        if (item && !cimdrawIsHandle(item))
            CimdrawGroup::refreshConnectionsForItemTree(item, true);
    }
    CimdrawGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    d_ptr->m_scene->removeSceneItem(d_ptr->m_item);
    d_ptr->m_scene->setInteractiveTransformActive(false);
    d_ptr->m_scene->rebuildTopologyIndex();
}
