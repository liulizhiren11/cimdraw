#include "LzRotateCommand.h"

#include <QMap>
#include <QtMath>

#include "Command/LzLayoutCommandUtils.h"
#include "Item/LzHandle.h"
#include "item/LzItem.h"

namespace {

void refreshRotatedItem(QGraphicsItem* item)
{
    if (auto* lzItem = dynamic_cast<LzItem*>(item))
        lzItem->refreshConnectedLines();
    if (item)
        item->update();
}

}

class LzRotateCommandPrivate
{
    Q_DECLARE_PUBLIC(LzRotateCommand)
public:
    LzRotateCommandPrivate(const QList<QGraphicsItem*>& items,
                           LzScene* scene,
                           qreal angleDegrees,
                           LzRotateCommand* command)
        : scene(scene)
        , angleDegrees(angleDegrees)
        , q_ptr(command)
    {
        for (QGraphicsItem* item : items)
        {
            if (!item || lzIsHandle(item))
                continue;
            if (scene && item->scene() != scene)
                continue;
            this->items.append(item);
            originalAngles.insert(item, item->rotation());
        }
    }

    LzScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, qreal> originalAngles;
    qreal angleDegrees = 0.0;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    bool appliedOnce = false;
    LzRotateCommand* q_ptr = nullptr;
};

LzRotateCommand::LzRotateCommand(const QList<QGraphicsItem*>& items,
                                 LzScene* scene,
                                 qreal angleDegrees,
                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new LzRotateCommandPrivate(items, scene, angleDegrees, this))
{
}

LzRotateCommand::~LzRotateCommand() = default;

bool LzRotateCommand::validateChange()
{
    Q_D(LzRotateCommand);
    return !d->items.isEmpty() && !qFuzzyIsNull(d->angleDegrees);
}

void LzRotateCommand::undo()
{
    Q_D(LzRotateCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto it = d->originalAngles.cbegin(); it != d->originalAngles.cend(); ++it)
    {
        if (it.key())
        {
            it.key()->setRotation(it.value());
            refreshRotatedItem(it.key());
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->appliedOnce = false;
}

void LzRotateCommand::redo()
{
    Q_D(LzRotateCommand);
    if (d->items.isEmpty() || d->appliedOnce)
        return;

    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (!item)
            continue;
        item->setRotation(item->rotation() + d->angleDegrees);
        refreshRotatedItem(item);
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->appliedOnce = true;
}
