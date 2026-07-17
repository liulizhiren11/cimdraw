#include "CimdrawSceneEditController.h"

#include <QApplication>
#include <QClipboard>
#include <QGraphicsScene>
#include <QMimeData>
#include <QUndoCommand>
#include <QUndoStack>
#include <algorithm>

#include "Command/CimdrawBottomAliginCommand.h"
#include "Command/CimdrawCutCommand.h"
#include "Command/CimdrawDeleteCommand.h"
#include "Command/CimdrawGroupCommand.h"
#include "Command/CimdrawHorizontallAlignCommand.h"
#include "Command/CimdrawHorizontalDistributionCommand.h"
#include "Command/CimdrawLeftAlignCommand.h"
#include "Command/CimdrawPasteCommand.h"
#include "Command/CimdrawRightAlignCommand.h"
#include "Command/CimdrawRotateCommand.h"
#include "Command/CimdrawSameHeightCommand.h"
#include "Command/CimdrawSameSizeCommand.h"
#include "Command/CimdrawSameWidthCommand.h"
#include "Command/CimdrawShortcutDownCommand.h"
#include "Command/CimdrawShortcutLeftCommand.h"
#include "Command/CimdrawShortcutRightCommand.h"
#include "Command/CimdrawShortcutUpCommand.h"
#include "Command/CimdrawTopAlignCommand.h"
#include "Command/CimdrawUngroupCommand.h"
#include "Command/CimdrawVerticalDistributionCommand.h"
#include "Command/CimdrawVerticallAlignCommand.h"
#include "Item/CimdrawGroup.h"
#include "Item/CimdrawHandle.h"
#include "CimdrawMimeData.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

namespace {

template <typename T>
void pushValidated(T* command, QUndoStack* stack)
{
    if (!command || !stack)
    {
        delete command;
        return;
    }
    if (command->validateChange())
        stack->push(command);
    else
        delete command;
}

void markSceneModified(CimdrawScene* scene)
{
    if (!scene)
        return;
    if (CimdrawView* view = scene->getView())
    {
        view->setModified(true);
        emit view->editChanged();
    }
}

QList<QGraphicsItem*> movableSelectionItems(const QList<QGraphicsItem*>& items, const QGraphicsScene* scene)
{
    QList<QGraphicsItem*> result;
    for (QGraphicsItem* item : items)
    {
        if (!item || item->scene() != scene || cimdrawIsHandle(item))
            continue;
        result.append(item);
    }
    return result;
}

QList<QGraphicsItem*> groupableSelectionItems(const QList<QGraphicsItem*>& items, const QGraphicsScene* scene)
{
    QList<QGraphicsItem*> result;
    for (QGraphicsItem* item : items)
    {
        if (!item || item->scene() != scene || cimdrawIsHandle(item))
            continue;
        if (item->parentItem())
            continue;
        result.append(item);
    }
    return result;
}

} // namespace

void CimdrawSceneEditController::alignLeft(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawLeftAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::alignRight(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawRightAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::alignTop(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawTopAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::alignBottom(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawBottomAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::alignVCenter(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawVerticallAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::alignHCenter(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new CimdrawHorizontallAlignCommand(selection, scene), stack);
}

void CimdrawSceneEditController::autoCol(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() >= 3)
        pushValidated(new CimdrawVerticalDistributionCommand(selection, scene), stack);
}

void CimdrawSceneEditController::autoRow(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (selection.count() >= 3)
        pushValidated(new CimdrawHorizontalDistributionCommand(selection, scene), stack);
}

void CimdrawSceneEditController::sameWidth(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    pushValidated(new CimdrawSameWidthCommand(selection, scene), stack);
    markSceneModified(scene);
}

void CimdrawSceneEditController::sameHeight(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    pushValidated(new CimdrawSameHeightCommand(selection, scene), stack);
    markSceneModified(scene);
}

void CimdrawSceneEditController::sameSize(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    pushValidated(new CimdrawSameSizeCommand(selection, scene), stack);
    markSceneModified(scene);
}

void CimdrawSceneEditController::shortcutLeft(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawShortcutLeftCommand(selection, scene, delta));
}

void CimdrawSceneEditController::shortcutRight(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawShortcutRightCommand(selection, scene, delta));
}

void CimdrawSceneEditController::shortcutUp(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawShortcutUpCommand(selection, scene, delta));
}

void CimdrawSceneEditController::shortcutDown(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawShortcutDownCommand(selection, scene, delta));
}

void CimdrawSceneEditController::cut(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawCutCommand(selection, scene));
}

void CimdrawSceneEditController::copy(const QList<QGraphicsItem*>& selection) const
{
    if (!selection.isEmpty())
        QApplication::clipboard()->setMimeData(new CimdrawMimeData(selection));
}

void CimdrawSceneEditController::paste(CimdrawScene* scene, QUndoStack* stack) const
{
    if (!scene)
        return;
    const QMimeData* md = QApplication::clipboard()->mimeData();
    if (!md)
        return;
    const auto* mimeData = dynamic_cast<const CimdrawMimeData*>(md);
    if (!mimeData || scene->items().isEmpty())
        return;

    CimdrawView* view = scene->getView();
    if (!view)
        return;

    const QList<QGraphicsItem*> prev = scene->getSelections();
    scene->setInteractiveTransformActive(true);
    const QList<QGraphicsItem*> batch = mimeData->instantiatePasteBatch(QPointF(20, 20));
    scene->setInteractiveTransformActive(false);
    if (batch.isEmpty())
        return;

    if (stack)
        stack->push(new CimdrawPasteCommand(scene, batch, prev));
}

void CimdrawSceneEditController::remove(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new CimdrawDeleteCommand(selection, scene));
}

void CimdrawSceneEditController::levelUp(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty())
        return;
    std::sort(selected.begin(), selected.end(), [](QGraphicsItem* lhs, QGraphicsItem* rhs) {
        return lhs->zValue() > rhs->zValue();
    });
    for (QGraphicsItem* item : selected)
        item->setZValue(item->zValue() + 1.0);
    if (scene)
        scene->update();
    markSceneModified(scene);
}

void CimdrawSceneEditController::levelDown(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty())
        return;
    std::sort(selected.begin(), selected.end(), [](QGraphicsItem* lhs, QGraphicsItem* rhs) {
        return lhs->zValue() < rhs->zValue();
    });
    for (QGraphicsItem* item : selected)
        item->setZValue(item->zValue() - 1.0);
    if (scene)
        scene->update();
    markSceneModified(scene);
}

void CimdrawSceneEditController::toTop(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty() || !graphicsScene)
        return;
    qreal maxZValue = 0.0;
    for (QGraphicsItem* item : graphicsScene->items())
    {
        if (!item || cimdrawIsHandle(item))
            continue;
        maxZValue = qMax(maxZValue, item->zValue());
    }
    for (int i = 0; i < selected.size(); ++i)
        selected.at(i)->setZValue(maxZValue + 1.0 + i);
    if (scene)
        scene->update();
    markSceneModified(scene);
}

void CimdrawSceneEditController::toBottom(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty() || !graphicsScene)
        return;
    qreal minZValue = 0.0;
    bool initialized = false;
    for (QGraphicsItem* item : graphicsScene->items())
    {
        if (!item || cimdrawIsHandle(item))
            continue;
        if (!initialized)
        {
            minZValue = item->zValue();
            initialized = true;
        }
        else
        {
            minZValue = qMin(minZValue, item->zValue());
        }
    }
    for (int i = 0; i < selected.size(); ++i)
        selected.at(i)->setZValue(minZValue - selected.size() + i);
    if (scene)
        scene->update();
    markSceneModified(scene);
}

void CimdrawSceneEditController::rotate(const QList<QGraphicsItem*>& selection,
                                   CimdrawScene* scene,
                                   QUndoStack* stack,
                                   qreal angleDegrees) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, scene);
    if (selected.isEmpty() || !stack || qFuzzyIsNull(angleDegrees))
        return;

    auto* command = new CimdrawRotateCommand(selected, scene, angleDegrees);
    if (!command->validateChange())
    {
        delete command;
        return;
    }

    stack->push(command);
    markSceneModified(scene);
}

void CimdrawSceneEditController::group(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (!scene || !stack)
        return;
    QList<QGraphicsItem*> groupItems = groupableSelectionItems(selection, scene);
    groupItems = CimdrawGroup::expandItemsWithInternalConnectLines(groupItems);
    if (groupItems.count() <= 1)
        return;
    QGraphicsItemGroup* obj = scene->createGroup(groupItems);
    stack->push(new CimdrawGroupCommand(scene, obj, groupItems));
}

void CimdrawSceneEditController::ungroup(const QList<QGraphicsItem*>& selection, CimdrawScene* scene, QUndoStack* stack) const
{
    if (!scene || !stack || selection.count() != 1)
        return;
    CimdrawGroup* group = qgraphicsitem_cast<CimdrawGroup*>(selection.first());
    if (group)
        stack->push(new CimdrawUngroupCommand(group, scene));
}
