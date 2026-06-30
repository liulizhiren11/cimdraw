#include "LzPasteCommand.h"
#include "LzScene.h"
#include "LzView.h"
#include "item/LzConnectLine.h"
#include "item/LzGroup.h"
#include "item/LzItem.h"
#include "item/TmpBase.h"
#include <algorithm>

namespace {

bool isPasteManagedItem(QGraphicsItem* item)
{
    return dynamic_cast<LzGroup*>(item)
        || dynamic_cast<TmpShape*>(item)
        || qgraphicsitem_cast<LzConnectLine*>(item);
}

bool isTopLevelPastedSelectionTarget(QGraphicsItem* item, const QList<QGraphicsItem*>& pastedItems)
{
    if (!item || !pastedItems.contains(item))
        return false;
    QGraphicsItem* parent = item->parentItem();
    while (parent)
    {
        if (pastedItems.contains(parent))
            return false;
        parent = parent->parentItem();
    }
    return true;
}

void refreshPastedConnections(const QList<QGraphicsItem*>& items)
{
    for (QGraphicsItem* item : items)
    {
        if (!item || lzIsHandle(item))
            continue;
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
        {
            line->cancelDeferredPathRecompute();
            line->registerEndpointAttachments();
            continue;
        }
        if (auto* group = dynamic_cast<LzGroup*>(item))
        {
            for (QGraphicsItem* child : group->childItems())
            {
                if (auto* line = qgraphicsitem_cast<LzConnectLine*>(child))
                    line->cancelDeferredPathRecompute();
            }
            LzGroup::registerConnectLinesAmongItems(group->childItems());
            continue;
        }
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            line->registerEndpointAttachments();
        else if (auto* shape = dynamic_cast<LzItem*>(item))
        {
            for (QGraphicsItem* conn : shape->connectedItems())
            {
                if (auto* cl = qgraphicsitem_cast<LzConnectLine*>(conn))
                    cl->cancelDeferredPathRecompute();
            }
        }
    }
}

QList<QGraphicsItem*> orderedPasteItemsForAdd(const QList<QGraphicsItem*>& items)
{
    QList<QGraphicsItem*> ordered = items;
    std::stable_sort(ordered.begin(), ordered.end(), [](QGraphicsItem* lhs, QGraphicsItem* rhs) {
        const bool lhsIsLine = lhs && lhs->type() == LzConnectLine::Type;
        const bool rhsIsLine = rhs && rhs->type() == LzConnectLine::Type;
        if (lhsIsLine != rhsIsLine)
            return !lhsIsLine;
        return false;
    });
    return ordered;
}

QList<QGraphicsItem*> orderedPasteItemsForRemove(const QList<QGraphicsItem*>& items)
{
    QList<QGraphicsItem*> ordered = items;
    std::stable_sort(ordered.begin(), ordered.end(), [](QGraphicsItem* lhs, QGraphicsItem* rhs) {
        const bool lhsIsLine = lhs && lhs->type() == LzConnectLine::Type;
        const bool rhsIsLine = rhs && rhs->type() == LzConnectLine::Type;
        if (lhsIsLine != rhsIsLine)
            return lhsIsLine;
        return false;
    });
    return ordered;
}

}

class LzPasteCommandPrivate
{
public:
    LzPasteCommandPrivate() = default;

    LzScene* scene = nullptr;
    QList<QGraphicsItem*> pasted;
    QList<QGraphicsItem*> previousSelection;
};

LzPasteCommand::LzPasteCommand(LzScene* scene, QList<QGraphicsItem*> pastedItems, QList<QGraphicsItem*> previousSelection,
                               QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzPasteCommandPrivate())
{
    Q_D(LzPasteCommand);
    d->scene = scene;
    d->pasted = std::move(pastedItems);
    d->previousSelection = std::move(previousSelection);
    setText(QStringLiteral("粘贴"));
}

LzPasteCommand::~LzPasteCommand()
{
    Q_D(LzPasteCommand);
    if (!d->scene)
        return;
    const QList<QGraphicsItem*> ordered = orderedPasteItemsForRemove(d->pasted);
    for (QGraphicsItem* i : ordered) {
        if (!i)
            continue;
        if (i->scene() == d->scene)
            d->scene->removeSceneItem(i);
        delete i;
    }
    d->pasted.clear();
}

void LzPasteCommand::redo()
{
    Q_D(LzPasteCommand);
    if (!d->scene || d->pasted.isEmpty())
        return;

    d->scene->cleanSelection();
    const QList<QGraphicsItem*> ordered = orderedPasteItemsForAdd(d->pasted);
    for (QGraphicsItem* item : ordered)
    {
        if (isPasteManagedItem(item))
        {
            if (!lzIsHandle(item))
            {
                if (!item->scene())
                    d->scene->addItem(item);
                if (isTopLevelPastedSelectionTarget(item, d->pasted))
                    d->scene->addSelection(item);
            }
        }
    }
    QList<QGraphicsItem*> list = d->pasted;
    refreshPastedConnections(d->pasted);
    emit d->scene->currentObjectChanged(list);
    d->scene->rebuildTopologyIndex();

    if (LzView* view = d->scene->getView())
    {
        view->setModified(true);
        emit view->editChanged();
    }
}

void LzPasteCommand::undo()
{
    Q_D(LzPasteCommand);
    if (!d->scene || d->pasted.isEmpty())
        return;

    const QList<QGraphicsItem*> ordered = orderedPasteItemsForRemove(d->pasted);
    for (QGraphicsItem* item : ordered)
    {
        if (isPasteManagedItem(item))
        {
            if (!lzIsHandle(item))
            {
                d->scene->removeSelection(item);
                d->scene->removeSceneItem(item);
            }
        }
    }
    d->scene->setSelections(d->previousSelection);
    emit d->scene->currentObjectChanged(d->previousSelection);
    d->scene->rebuildTopologyIndex();

    if (LzView* view = d->scene->getView())
        emit view->editChanged();
}
