#include "LzSameHeightCommand.h"
#include "LzScene.h"
#include "LzLayoutCommandUtils.h"
#include "item/LzItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class LzSameHeightCommandPrivate
{
public:
    LzSameHeightCommandPrivate(const QList<QGraphicsItem*>& items, LzScene* scene)
        : scene(scene)
    {
        for (QGraphicsItem* item : items)
        {
            auto* shape = qgraphicsitem_cast<TmpShape*>(item);
            if (!shape || lzIsHandle(item))
                continue;
            if (scene && item->scene() != scene)
                continue;
            if (shape->getWidth() <= 0 || shape->getHeight() <= 0)
                continue;

            this->items.append(item);
            originalSizes.insert(item, QSizeF(shape->getWidth(), shape->getHeight()));
            if (!hasTarget)
            {
                targetHeight = shape->getHeight();
                hasTarget = true;
            }
        }
    }

    LzScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    qreal targetHeight = 0;
    bool hasTarget = false;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
};

namespace {

bool nearlyEqual(qreal a, qreal b)
{
    return qAbs(a - b) <= 0.01;
}

void refreshItem(QGraphicsItem* item)
{
    if (auto* lzItem = dynamic_cast<LzItem*>(item))
        lzItem->refreshConnectedLines();
    if (item)
        item->update();
}

} // namespace

LzSameHeightCommand::LzSameHeightCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzSameHeightCommandPrivate(items, scene))
{
}

bool LzSameHeightCommand::validateChange()
{
    Q_D(LzSameHeightCommand);
    if (!d->hasTarget || d->items.size() <= 1)
        return false;

    for (QGraphicsItem* item : d->items)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (shape && !nearlyEqual(shape->getHeight(), d->targetHeight))
            return true;
    }
    return false;
}

void LzSameHeightCommand::undo()
{
    Q_D(LzSameHeightCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto it = d->originalSizes.cbegin(); it != d->originalSizes.cend(); ++it)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(it.key()))
        {
            shape->setSize(it.value());
            refreshItem(it.key());
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}

void LzSameHeightCommand::redo()
{
    Q_D(LzSameHeightCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            shape->setHeight(d->targetHeight);
            refreshItem(item);
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
