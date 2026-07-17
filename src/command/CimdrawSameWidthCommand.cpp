#include "CimdrawSameWidthCommand.h"
#include "CimdrawScene.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/CimdrawItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class CimdrawSameWidthCommandPrivate
{
public:
    CimdrawSameWidthCommandPrivate(const QList<QGraphicsItem*>& items, CimdrawScene* scene, CimdrawSameWidthCommand* command)
        : scene(scene)
        , q_ptr(command)
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
                targetWidth = shape->getWidth();
                hasTarget = true;
            }
        }
    }

    CimdrawScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    qreal targetWidth = 0;
    bool hasTarget = false;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    CimdrawSameWidthCommand* q_ptr = nullptr;
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

CimdrawSameWidthCommand::CimdrawSameWidthCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new CimdrawSameWidthCommandPrivate(items, scene, this))
{
}

bool CimdrawSameWidthCommand::validateChange()
{
    Q_D(CimdrawSameWidthCommand);
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

void CimdrawSameWidthCommand::undo()
{
    Q_D(CimdrawSameWidthCommand);
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

void CimdrawSameWidthCommand::redo()
{
    Q_D(CimdrawSameWidthCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
    {
        if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
        {
            shape->setWidth(d->targetWidth);
            refreshItem(item);
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
