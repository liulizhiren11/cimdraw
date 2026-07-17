#include "CimdrawSceneItemLifecycleController.h"

#include "Item/CimdrawConnectLine.h"
#include "CimdrawScene.h"
#include "scene/CimdrawSceneSelectionManager.h"
#include "wiring/CimdrawShapeEdgeConnect.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"

namespace {

void detachDeletedItemFromConnectLines(QGraphicsItem* item)
{
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return;

    const QVector<QGraphicsItem*> connects = shape->connectedItems();
    for (QGraphicsItem* connected : connects)
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(connected);
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

bool CimdrawSceneItemLifecycleController::removeSceneItem(CimdrawScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    CimdrawSceneSelectionManager selectionManager;
    selectionManager.removeSelection(selection, item, scene);
    if (item->scene() == scene)
        scene->QGraphicsScene::removeItem(item);
    return true;
}

bool CimdrawSceneItemLifecycleController::insertSelection(CimdrawScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    if (item->scene() != scene)
        scene->addItem(item);

    item->setVisible(true);
    if (item->type() == CimdrawConnectLine::Type)
    {
        auto* line = qgraphicsitem_cast<CimdrawConnectLine*>(item);
        if (line)
        {
            line->reattachToEndpointShapes(false);
            cimdrawReattachBusbarPortsForConnectLine(line);
        }
    }

    CimdrawSceneSelectionManager selectionManager;
    selectionManager.addSelection(selection, item, scene);
    scene->scheduleTopologyRebuild();
    return true;
}

bool CimdrawSceneItemLifecycleController::deleteSelection(CimdrawScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     QGraphicsItem* item) const
{
    if (!scene || !item)
        return false;

    if (item->type() == CimdrawConnectLine::Type)
        cimdrawCleanupAfterConnectLineRemoved(qgraphicsitem_cast<CimdrawConnectLine*>(item));
    else
        detachDeletedItemFromConnectLines(item);

    removeSceneItem(scene, selection, item);
    item->setVisible(false);
    scene->scheduleTopologyRebuild();
    return true;
}

int CimdrawSceneItemLifecycleController::addSelections(CimdrawScene* scene,
                                                  QList<QGraphicsItem*>& selection,
                                                  const QList<QGraphicsItem*>& items) const
{
    if (!scene)
        return 0;

    int changedCount = 0;
    CimdrawSceneSelectionManager selectionManager;
    for (QGraphicsItem* item : items)
    {
        if (selectionManager.addSelection(selection, item, scene))
            ++changedCount;
    }
    return changedCount;
}

int CimdrawSceneItemLifecycleController::removeSelections(CimdrawScene* scene,
                                                     QList<QGraphicsItem*>& selection,
                                                     const QList<QGraphicsItem*>& items) const
{
    if (!scene)
        return 0;

    int changedCount = 0;
    CimdrawSceneSelectionManager selectionManager;
    for (QGraphicsItem* item : items)
    {
        if (selectionManager.removeSelection(selection, item, scene))
            ++changedCount;
    }
    return changedCount;
}
