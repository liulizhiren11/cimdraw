#include "CimdrawSameHeightCommand.h"
#include "CimdrawScene.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/CimdrawItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class CimdrawSameHeightCommandPrivate
{
public:
    CimdrawSameHeightCommandPrivate(const QList<QGraphicsItem*>& items, CimdrawScene* scene)
        : scene(scene)
    {
        for (QGraphicsItem* item : items)
        {
            auto* shape = qgraphicsitem_cast<TmpShape*>(item);
            if (!shape || cimdrawIsHandle(item))
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

    CimdrawScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    qreal targetHeight = 0;
    bool hasTarget = false;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
};

namespace {

bool nearlyEqual(qreal a, qreal b)
{
    return qAbs(a - b) <= 0.01;
}

void refreshItem(QGraphicsItem* item)
{
    if (auto* cimdrawItem = dynamic_cast<CimdrawItem*>(item))
        cimdrawItem->refreshConnectedLines();
    if (item)
        item->update();
}

} // namespace

CimdrawSameHeightCommand::CimdrawSameHeightCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new CimdrawSameHeightCommandPrivate(items, scene))
{
}

bool CimdrawSameHeightCommand::validateChange()
{
    Q_D(CimdrawSameHeightCommand);
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

void CimdrawSameHeightCommand::undo()
{
    Q_D(CimdrawSameHeightCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto it = d->originalSizes.cbegin(); it != d->originalSizes.cend(); ++it)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(it.key()))
        {
            shape->setSize(it.value());
            refreshItem(it.key());
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}

void CimdrawSameHeightCommand::redo()
{
    Q_D(CimdrawSameHeightCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            shape->setHeight(d->targetHeight);
            refreshItem(item);
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
