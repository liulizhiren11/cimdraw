#include "LzSceneEditController.h"

#include <QApplication>
#include <QClipboard>
#include <QGraphicsScene>
#include <QMimeData>
#include <QUndoCommand>
#include <QUndoStack>
#include <algorithm>

#include "Command/LzBottomAliginCommand.h"
#include "Command/LzCutCommand.h"
#include "Command/LzDeleteCommand.h"
#include "Command/LzGroupCommand.h"
#include "Command/LzHorizontallAlignCommand.h"
#include "Command/LzHorizontalDistributionCommand.h"
#include "Command/LzLeftAlignCommand.h"
#include "Command/LzPasteCommand.h"
#include "Command/LzRightAlignCommand.h"
#include "Command/LzRotateCommand.h"
#include "Command/LzSameHeightCommand.h"
#include "Command/LzSameSizeCommand.h"
#include "Command/LzSameWidthCommand.h"
#include "Command/LzShortcutDownCommand.h"
#include "Command/LzShortcutLeftCommand.h"
#include "Command/LzShortcutRightCommand.h"
#include "Command/LzShortcutUpCommand.h"
#include "Command/LzTopAlignCommand.h"
#include "Command/LzUngroupCommand.h"
#include "Command/LzVerticalDistributionCommand.h"
#include "Command/LzVerticallAlignCommand.h"
#include "Item/LzGroup.h"
#include "Item/LzHandle.h"
#include "LzMimeData.h"
#include "LzScene.h"
#include "LzView.h"

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

void markSceneModified(LzScene* scene)
{
    if (!scene)
        return;
    if (LzView* view = scene->getView())
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
        if (!item || item->scene() != scene || lzIsHandle(item))
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
        if (!item || item->scene() != scene || lzIsHandle(item))
            continue;
        if (item->parentItem())
            continue;
        result.append(item);
    }
    return result;
}

} // namespace

void LzSceneEditController::alignLeft(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzLeftAlignCommand(selection, scene), stack);
}

void LzSceneEditController::alignRight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzRightAlignCommand(selection, scene), stack);
}

void LzSceneEditController::alignTop(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzTopAlignCommand(selection, scene), stack);
}

void LzSceneEditController::alignBottom(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzBottomAlignCommand(selection, scene), stack);
}

void LzSceneEditController::alignVCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzVerticallAlignCommand(selection, scene), stack);
}

void LzSceneEditController::alignHCenter(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() > 1)
        pushValidated(new LzHorizontallAlignCommand(selection, scene), stack);
}

void LzSceneEditController::autoCol(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() >= 3)
        pushValidated(new LzVerticalDistributionCommand(selection, scene), stack);
}

void LzSceneEditController::autoRow(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (selection.count() >= 3)
        pushValidated(new LzHorizontalDistributionCommand(selection, scene), stack);
}

void LzSceneEditController::sameWidth(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    pushValidated(new LzSameWidthCommand(selection, scene), stack);
    markSceneModified(scene);
}

void LzSceneEditController::sameHeight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    pushValidated(new LzSameHeightCommand(selection, scene), stack);
    markSceneModified(scene);
}

void LzSceneEditController::sameSize(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    pushValidated(new LzSameSizeCommand(selection, scene), stack);
    markSceneModified(scene);
}

void LzSceneEditController::shortcutLeft(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzShortcutLeftCommand(selection, scene, delta));
}

void LzSceneEditController::shortcutRight(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzShortcutRightCommand(selection, scene, delta));
}

void LzSceneEditController::shortcutUp(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzShortcutUpCommand(selection, scene, delta));
}

void LzSceneEditController::shortcutDown(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack, const QPointF& delta) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzShortcutDownCommand(selection, scene, delta));
}

void LzSceneEditController::cut(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzCutCommand(selection, scene));
}

void LzSceneEditController::copy(const QList<QGraphicsItem*>& selection) const
{
    if (!selection.isEmpty())
        QApplication::clipboard()->setMimeData(new LzMimeData(selection));
}

void LzSceneEditController::paste(LzScene* scene, QUndoStack* stack) const
{
    if (!scene)
        return;
    const QMimeData* md = QApplication::clipboard()->mimeData();
    if (!md)
        return;
    const auto* lz = dynamic_cast<const LzMimeData*>(md);
    if (!lz || lz->items().isEmpty())
        return;

    LzView* view = scene->getView();
    if (!view)
        return;

    const QList<QGraphicsItem*> prev = scene->getSelections();
    scene->setInteractiveTransformActive(true);
    const QList<QGraphicsItem*> batch = lz->instantiatePasteBatch(QPointF(20, 20));
    scene->setInteractiveTransformActive(false);
    if (batch.isEmpty())
        return;

    if (stack)
        stack->push(new LzPasteCommand(scene, batch, prev));
}

void LzSceneEditController::remove(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (!selection.isEmpty() && stack)
        stack->push(new LzDeleteCommand(selection, scene));
}

void LzSceneEditController::levelUp(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const
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

void LzSceneEditController::levelDown(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const
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

void LzSceneEditController::toTop(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty() || !graphicsScene)
        return;
    qreal maxZValue = 0.0;
    for (QGraphicsItem* item : graphicsScene->items())
    {
        if (!item || lzIsHandle(item))
            continue;
        maxZValue = qMax(maxZValue, item->zValue());
    }
    for (int i = 0; i < selected.size(); ++i)
        selected.at(i)->setZValue(maxZValue + 1.0 + i);
    if (scene)
        scene->update();
    markSceneModified(scene);
}

void LzSceneEditController::toBottom(const QList<QGraphicsItem*>& selection, LzScene* scene, const QGraphicsScene* graphicsScene) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, graphicsScene);
    if (selected.isEmpty() || !graphicsScene)
        return;
    qreal minZValue = 0.0;
    bool initialized = false;
    for (QGraphicsItem* item : graphicsScene->items())
    {
        if (!item || lzIsHandle(item))
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

void LzSceneEditController::rotate(const QList<QGraphicsItem*>& selection,
                                   LzScene* scene,
                                   QUndoStack* stack,
                                   qreal angleDegrees) const
{
    QList<QGraphicsItem*> selected = movableSelectionItems(selection, scene);
    if (selected.isEmpty() || !stack || qFuzzyIsNull(angleDegrees))
        return;

    auto* command = new LzRotateCommand(selected, scene, angleDegrees);
    if (!command->validateChange())
    {
        delete command;
        return;
    }

    stack->push(command);
    markSceneModified(scene);
}

void LzSceneEditController::group(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (!scene || !stack)
        return;
    QList<QGraphicsItem*> groupItems = groupableSelectionItems(selection, scene);
    groupItems = LzGroup::expandItemsWithInternalConnectLines(groupItems);
    if (groupItems.count() <= 1)
        return;
    QGraphicsItemGroup* obj = scene->createGroup(groupItems);
    stack->push(new LzGroupCommand(scene, obj, groupItems));
}

void LzSceneEditController::ungroup(const QList<QGraphicsItem*>& selection, LzScene* scene, QUndoStack* stack) const
{
    if (!scene || !stack || selection.count() != 1)
        return;
    LzGroup* group = qgraphicsitem_cast<LzGroup*>(selection.first());
    if (group)
        stack->push(new LzUngroupCommand(group, scene));
}
