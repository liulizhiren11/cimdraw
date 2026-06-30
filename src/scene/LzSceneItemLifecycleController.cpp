#include "LzSceneItemLifecycleController.h"

#include "Item/LzConnectLine.h"
#include "LzScene.h"
#include "scene/LzSceneSelectionManager.h"
#include "wiring/LzShapeEdgeConnect.h"
#include "wiring/power/LzPowerBusbarSectionItem.h"

namespace {

void detachDeletedItemFromConnectLines(QGraphicsItem* item)
{
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return;

    const QVector<QGraphicsItem*> connects = shape->connectedItems();
    for (QGraphicsItem* connected : connects)
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(connected);
        if (!line)
            continue;
        if (line->getStartItem() == item)
        {
            line->setStartConnectPort(nullptr);
            line->setStartItem(nullptr);
        }
        if (line->getEndItem() == item)
        {
            line->setEndConnectPort(nullptr);
            line->setEndItem(nullptr);
        }
    }
}

}

bool LzSceneItemLifecycleController::removeSceneItem(LzScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    LzSceneSelectionManager selectionManager;
    selectionManager.removeSelection(selection, item, scene);
    if (item->scene() == scene)
        scene->QGraphicsScene::removeItem(item);
    return true;
}

bool LzSceneItemLifecycleController::insertSelection(LzScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    if (item->scene() != scene)
        scene->addItem(item);

    item->setVisible(true);
    if (item->type() == LzConnectLine::Type)
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(item);
        if (line)
        {
            line->reattachToEndpointShapes(false);
            lzReattachBusbarPortsForConnectLine(line);
        }
    }

    LzSceneSelectionManager selectionManager;
    selectionManager.addSelection(selection, item, scene);
    scene->scheduleTopologyRebuild();
    return true;
}

bool LzSceneItemLifecycleController::deleteSelection(LzScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    if (item->type() == LzConnectLine::Type)
        lzCleanupAfterConnectLineRemoved(qgraphicsitem_cast<LzConnectLine*>(item));
    else
        detachDeletedItemFromConnectLines(item);

    removeSceneItem(scene, selection, item);
    item->setVisible(false);
    scene->scheduleTopologyRebuild();
    return true;
}

int LzSceneItemLifecycleController::addSelections(LzScene* scene,
                                                  QList<QGraphicsItem*>& selection,
                                                  const QList<QGraphicsItem*>& items) const
{
    if (!scene)
        return 0;

    int changedCount = 0;
    LzSceneSelectionManager selectionManager;
    for (QGraphicsItem* item : items)
    {
        if (selectionManager.addSelection(selection, item, scene))
            ++changedCount;
    }
    return changedCount;
}

int LzSceneItemLifecycleController::removeSelections(LzScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     const QList<QGraphicsItem*>& items) const
{
    if (!scene)
        return 0;

    int changedCount = 0;
    LzSceneSelectionManager selectionManager;
    for (QGraphicsItem* item : items)
    {
        if (selectionManager.removeSelection(selection, item, scene))
            ++changedCount;
    }
    return changedCount;
}
