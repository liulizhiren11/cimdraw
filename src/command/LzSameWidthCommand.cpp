#include "LzSameWidthCommand.h"
#include "LzScene.h"
#include "LzLayoutCommandUtils.h"
#include "item/LzItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class LzSameWidthCommandPrivate
{
public:
    LzSameWidthCommandPrivate(const QList<QGraphicsItem*>& items, LzScene* scene, LzSameWidthCommand* command)
        : scene(scene)
        , q_ptr(command)
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
                targetWidth = shape->getWidth();
                hasTarget = true;
            }
        }
    }

    LzScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    qreal targetWidth = 0;
    bool hasTarget = false;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    LzSameWidthCommand* q_ptr = nullptr;
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

LzSameWidthCommand::LzSameWidthCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzSameWidthCommandPrivate(items, scene, this))
{
}

bool LzSameWidthCommand::validateChange()
{
    Q_D(LzSameWidthCommand);
    if (!d->hasTarget || d->items.size() <= 1)
        return false;

    for (QGraphicsItem* item : d->items)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (shape && !nearlyEqual(shape->getWidth(), d->targetWidth))
            return true;
    }
    return false;
}

void LzSameWidthCommand::undo()
{
    Q_D(LzSameWidthCommand);
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

void LzSameWidthCommand::redo()
{
    Q_D(LzSameWidthCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            shape->setWidth(d->targetWidth);
            refreshItem(item);
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
