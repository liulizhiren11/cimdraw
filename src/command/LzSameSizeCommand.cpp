#include "LzSameSizeCommand.h"
#include "LzScene.h"
#include "LzLayoutCommandUtils.h"
#include "item/LzItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class LzSameSizeCommandPrivate
{
public:
    LzSameSizeCommandPrivate(const QList<QGraphicsItem*>& items, LzScene* scene, LzSameSizeCommand* command)
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
            const QSizeF size(shape->getWidth(), shape->getHeight());
            originalSizes.insert(item, size);
            if (!hasTarget)
            {
                targetSize = size;
                hasTarget = true;
            }
        }
    }

    LzScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    QSizeF targetSize;
    bool hasTarget = false;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    LzSameSizeCommand* q_ptr = nullptr;
};

namespace {

bool nearlyEqual(qreal a, qreal b)
{
    return qAbs(a - b) <= 0.01;
}

void setShapeSize(QGraphicsItem* item, const QSizeF& size)
{
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    if (!shape)
        return;
    shape->setSize(size);
    if (auto* lzItem = dynamic_cast<LzItem*>(item))
        lzItem->refreshConnectedLines();
    item->update();
}

} // namespace

LzSameSizeCommand::LzSameSizeCommand(const QList<QGraphicsItem*>& items, LzScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzSameSizeCommandPrivate(items, scene, this))
{
}

bool LzSameSizeCommand::validateChange()
{
    Q_D(LzSameSizeCommand);
    if (!d->hasTarget || d->items.size() <= 1)
        return false;

    for (QGraphicsItem* item : d->items)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape)
            continue;
        if (!nearlyEqual(shape->getWidth(), d->targetSize.width())
            || !nearlyEqual(shape->getHeight(), d->targetSize.height()))
            return true;
    }
    return false;
}

void LzSameSizeCommand::undo()
{
    Q_D(LzSameSizeCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto it = d->originalSizes.cbegin(); it != d->originalSizes.cend(); ++it)
        setShapeSize(it.key(), it.value());
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}

void LzSameSizeCommand::redo()
{
    Q_D(LzSameSizeCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
        setShapeSize(item, d->targetSize);
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
