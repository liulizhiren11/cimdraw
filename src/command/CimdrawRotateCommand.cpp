#include "CimdrawRotateCommand.h"

#include <QMap>
#include <QtMath>

#include "Command/CimdrawLayoutCommandUtils.h"
#include "Item/CimdrawHandle.h"
#include "item/CimdrawItem.h"

namespace {

void refreshRotatedItem(QGraphicsItem* item)
{
    if (auto* cimdrawItem = dynamic_cast<CimdrawItem*>(item))
        cimdrawItem->refreshConnectedLines();
    if (item)
        item->update();
}

}

class CimdrawRotateCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawRotateCommand)
public:
    CimdrawRotateCommandPrivate(const QList<QGraphicsItem*>& items,
                           CimdrawScene* scene,
                           qreal angleDegrees,
                           CimdrawRotateCommand* command)
        : scene(scene)
        , angleDegrees(angleDegrees)
        , q_ptr(command)
    {
        for (QGraphicsItem* item : items)
        {
            if (!item || cimdrawIsHandle(item))
                continue;
            if (scene && item->scene() != scene)
                continue;
            this->items.append(item);
            originalAngles.insert(item, item->rotation());
        }
    }

    CimdrawScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, qreal> originalAngles;
    qreal angleDegrees = 0.0;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    bool appliedOnce = false;
    CimdrawRotateCommand* q_ptr = nullptr;
};

CimdrawRotateCommand::CimdrawRotateCommand(const QList<QGraphicsItem*>& items,
                                 CimdrawScene* scene,
                                 qreal angleDegrees,
                                 QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new CimdrawRotateCommandPrivate(items, scene, angleDegrees, this))
{
}

CimdrawRotateCommand::~CimdrawRotateCommand() = default;

bool CimdrawRotateCommand::validateChange()
{
    Q_D(CimdrawRotateCommand);
    return !d->items.isEmpty() && !qFuzzyIsNull(d->angleDegrees);
}

void CimdrawRotateCommand::undo()
{
    Q_D(CimdrawRotateCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto it = d->originalAngles.cbegin(); it != d->originalAngles.cend(); ++it)
    {
        if (it.key())
        {
            it.key()->setRotation(it.value());
            refreshRotatedItem(it.key());
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->appliedOnce = false;
}

void CimdrawRotateCommand::redo()
{
    Q_D(CimdrawRotateCommand);
    if (d->items.isEmpty() || d->appliedOnce)
        return;

    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (!item)
            continue;
        item->setRotation(item->rotation() + d->angleDegrees);
        refreshRotatedItem(item);
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->appliedOnce = true;
}
