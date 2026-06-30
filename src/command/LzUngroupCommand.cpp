#include "LzUngroupCommand.h"
#include "item/LzGroup.h"
#include "item/LzConnectLine.h"
#include "item/TmpBase.h"
#include "LzScene.h"

class LzUngroupCommandPrivate
{
public:
    LzUngroupCommandPrivate(LzGroup* group,LzScene* scene,LzUngroupCommand* command)
        : q_ptr(command),m_item(group),m_scene(scene)
    {
        for(auto item:group->childItems())
        {
            if(item&&!lzIsHandle(item))
            m_items.append(item);
        }
    }
    ~LzUngroupCommandPrivate();
    LzScene* m_scene;
    LzUngroupCommand* q_ptr;
    QList<QGraphicsItem*> m_items;
    LzGroup* m_item;
};

LzUngroupCommand::LzUngroupCommand(LzGroup* item,LzScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent),d_ptr(new LzUngroupCommandPrivate(item ,scene,this))
{
    
}

void LzUngroupCommand::undo()
{
    d_ptr->m_scene->setInteractiveTransformActive(true);

    if (d_ptr->m_item && d_ptr->m_item->scene() != d_ptr->m_scene)
        d_ptr->m_scene->addItem(d_ptr->m_item);

    QVector<LzConnectLinePathSnapshot> lineSnapshots;
    LzGroup::collectConnectLineSnapshots(d_ptr->m_items, lineSnapshots);

    d_ptr->m_scene->cleanSelection();
    for(auto item : d_ptr->m_items)
    {
        if (item && !lzIsHandle(item))
        {
            d_ptr->m_scene->removeSelection(item);
            d_ptr->m_item->addToGroup(item);
        }
    }
    d_ptr->m_item->updateCoordinate();
    LzGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    LzGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
    LzGroup::refreshConnectionsForItemTree(d_ptr->m_item, true);
    LzGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    d_ptr->m_scene->addSelection(d_ptr->m_item);
    d_ptr->m_scene->setInteractiveTransformActive(false);
    d_ptr->m_scene->rebuildTopologyIndex();
}

void LzUngroupCommand::redo()
{
    d_ptr->m_scene->setInteractiveTransformActive(true);

    QVector<LzConnectLinePathSnapshot> lineSnapshots;
    LzGroup::collectConnectLineSnapshots(d_ptr->m_items, lineSnapshots);

    d_ptr->m_scene->removeSelection(d_ptr->m_item);
    for(auto item : d_ptr->m_items)
    {
        if (item && !lzIsHandle(item))
        {
            d_ptr->m_item->removeFromGroup(item);
            d_ptr->m_scene->addSelection(item);
        }
    }
    LzGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    LzGroup::reattachConnectLinesAmongItems(d_ptr->m_items);
    for(auto item : d_ptr->m_items)
    {
        if (item && !lzIsHandle(item))
            LzGroup::refreshConnectionsForItemTree(item, true);
    }
    LzGroup::restoreConnectLineSnapshotsExact(lineSnapshots);
    d_ptr->m_scene->removeSceneItem(d_ptr->m_item);
    d_ptr->m_scene->setInteractiveTransformActive(false);
    d_ptr->m_scene->rebuildTopologyIndex();
}
