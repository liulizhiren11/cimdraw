#include "CimdrawSceneViewportController.h"

#include <QEvent>

#include "Item/CimdrawHandle.h"
#include "CimdrawScene.h"

namespace {

QRectF normalizedSceneRectForFit(const QRectF& rect)
{
    if (rect.isNull() || rect.isEmpty())
        return QRectF();

    QRectF normalized = rect.normalized();
    constexpr qreal padding = 24.0;
    normalized.adjust(-padding, -padding, padding, padding);

    if (normalized.width() < 1.0)
        normalized.setWidth(1.0);
    if (normalized.height() < 1.0)
        normalized.setHeight(1.0);
    return normalized;
}

}

QRectF CimdrawSceneViewportController::contentsRect(CimdrawScene* scene) const
{
    if (!scene)
        return QRectF();

    QRectF bounds;
    bool hasContent = false;
    const QList<QGraphicsItem*> all = scene->items();
    for (QGraphicsItem* item : all)
    {
        if (!item || !item->isVisible() || cimdrawIsHandle(item))
            continue;
        const QRectF itemBounds = item->sceneBoundingRect();
        if (itemBounds.isNull() || itemBounds.isEmpty())
            continue;
        bounds = hasContent ? bounds.united(itemBounds) : itemBounds;
        hasContent = true;
    }

    if (!hasContent)
        bounds = scene->sceneRect();

    return normalizedSceneRectForFit(bounds);
}

bool CimdrawSceneViewportController::dispatchCompatMouseEvent(CimdrawScene* scene,
                                                         QGraphicsSceneMouseEvent* event) const
{
    if (!scene || !event)
        return false;

    switch (event->type())
    {
    case QEvent::GraphicsSceneMousePress:
        scene->mousePressEvent(event);
        return true;
    case QEvent::GraphicsSceneMouseMove:
        scene->mouseMoveEvent(event);
        return true;
    case QEvent::GraphicsSceneMouseRelease:
        scene->mouseReleaseEvent(event);
        return true;
    case QEvent::GraphicsSceneMouseDoubleClick:
        scene->mouseDoubleClickEvent(event);
        return true;
    default:
        return false;
    }
}
