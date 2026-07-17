#include "CimdrawSameSizeCommand.h"
#include "CimdrawScene.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/CimdrawItem.h"
#include "item/TmpBase.h"

#include <QMap>
#include <QSizeF>
#include <QtMath>

class CimdrawSameSizeCommandPrivate
{
public:
    CimdrawSameSizeCommandPrivate(const QList<QGraphicsItem*>& items, CimdrawScene* scene, CimdrawSameSizeCommand* command)
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
            const QSizeF size(shape->getWidth(), shape->getHeight());
            originalSizes.insert(item, size);
            if (!hasTarget)
            {
                targetSize = size;
                hasTarget = true;
            }
        }
    }

    CimdrawScene* scene = nullptr;
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QSizeF> originalSizes;
    QSizeF targetSize;
    bool hasTarget = false;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    CimdrawSameSizeCommand* q_ptr = nullptr;
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
    if (auto* cimdrawItem = dynamic_cast<CimdrawItem*>(item))
        cimdrawItem->refreshConnectedLines();
    item->update();
}

} // namespace

CimdrawSameSizeCommand::CimdrawSameSizeCommand(const QList<QGraphicsItem*>& items, CimdrawScene* scene, QUndoCommand* parent)
    : QUndoCommand(parent)
    , d_ptr(new CimdrawSameSizeCommandPrivate(items, scene, this))
{
}

bool CimdrawSameSizeCommand::validateChange()
{
    Q_D(CimdrawSameSizeCommand);
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

void CimdrawSameSizeCommand::undo()
{
    Q_D(CimdrawSameSizeCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto it = d->originalSizes.cbegin(); it != d->originalSizes.cend(); ++it)
        setShapeSize(it.key(), it.value());
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}

void CimdrawSameSizeCommand::redo()
{
    Q_D(CimdrawSameSizeCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    for (QGraphicsItem* item : d->items)
        setShapeSize(item, d->targetSize);
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
