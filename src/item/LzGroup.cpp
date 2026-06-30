#include "LzGroup.h"
#include "LzItem.h"
#include "LzConnectLine.h"
#include "LzObjectFactory.h"

#include <QSet>

namespace {

bool isGroupHandle(const QGraphicsItem* item)
{
    return item && item->type() == LzHandle::Type;
}

void collectLinesFromItem(QGraphicsItem* item, QSet<LzConnectLine*>& lines)
{
    if (!item)
        return;
    if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
        lines.insert(line);
    if (auto* shape = qgraphicsitem_cast<TmpShape*>(item))
    {
        for (QGraphicsItem* connected : shape->connectedItems())
        {
            if (auto* cl = qgraphicsitem_cast<LzConnectLine*>(connected))
                lines.insert(cl);
        }
    }
}

} // namespace

void LzGroup::collectConnectLineSnapshots(const QList<QGraphicsItem*>& items,
                                          QVector<LzConnectLinePathSnapshot>& out)
{
    QSet<LzConnectLine*> lines;
    for (QGraphicsItem* item : items)
        collectLinesFromItem(item, lines);

    out.clear();
    out.reserve(lines.size());
    for (LzConnectLine* line : lines)
    {
        if (!line || line->pathInSceneCoords().size() < 2)
            continue;
        LzConnectLinePathSnapshot snap;
        snap.line = line;
        snap.scenePoints = line->pathInSceneCoords();
        snap.routingMode = line->pathRoutingMode();
        out.append(snap);
    }
}

void LzGroup::restoreConnectLineSnapshots(const QVector<LzConnectLinePathSnapshot>& snapshots)
{
    for (const LzConnectLinePathSnapshot& snap : snapshots)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        snap.line->restorePathSnapshot(snap.scenePoints, snap.routingMode);
    }
}

void LzGroup::restoreConnectLineSnapshotsExact(const QVector<LzConnectLinePathSnapshot>& snapshots)
{
    for (const LzConnectLinePathSnapshot& snap : snapshots)
    {
        if (!snap.line || snap.scenePoints.size() < 2)
            continue;
        snap.line->applyScenePathExact(snap.scenePoints, snap.routingMode);
    }
}

void LzGroup::reattachConnectLinesAmongItems(const QList<QGraphicsItem*>& items)
{
    for (QGraphicsItem* item : items)
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            line->reattachToEndpointShapes(false);
    }
}

void LzGroup::registerConnectLinesAmongItems(const QList<QGraphicsItem*>& items)
{
    for (QGraphicsItem* item : items)
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            line->registerEndpointAttachments();
    }
}

void LzGroup::collectConnectLineSnapshotsInGroup(const LzGroup* group,
                                               QVector<LzConnectLinePathSnapshot>& out)
{
    out.clear();
    if (!group)
        return;
    for (QGraphicsItem* child : group->childItems())
    {
        if (isGroupHandle(child))
            continue;
        auto* line = qgraphicsitem_cast<LzConnectLine*>(child);
        if (!line || line->pathInSceneCoords().size() < 2)
            continue;
        LzConnectLinePathSnapshot snap;
        snap.line = line;
        snap.scenePoints = line->pathInSceneCoords();
        snap.routingMode = line->pathRoutingMode();
        out.append(snap);
    }
}

QList<QGraphicsItem*> LzGroup::expandItemsWithInternalConnectLines(const QList<QGraphicsItem*>& items)
{
    QSet<QGraphicsItem*> itemSet;
    QList<QGraphicsItem*> expanded = items;
    for (QGraphicsItem* item : items)
    {
        if (item)
            itemSet.insert(item);
    }

    for (QGraphicsItem* item : items)
    {
        auto* shape = qgraphicsitem_cast<TmpShape*>(item);
        if (!shape || qgraphicsitem_cast<LzConnectLine*>(item))
            continue;
        for (QGraphicsItem* connected : shape->connectedItems())
        {
            auto* line = qgraphicsitem_cast<LzConnectLine*>(connected);
            if (!line || itemSet.contains(line))
                continue;
            QGraphicsItem* start = line->getStartItem();
            QGraphicsItem* end = line->getEndItem();
            if (!start || !end || !itemSet.contains(start) || !itemSet.contains(end))
                continue;
            itemSet.insert(line);
            expanded.append(line);
        }
    }
    return expanded;
}

void LzGroup::refreshConnectionsForItemTree(QGraphicsItem* item, bool preservePathShape)
{
    if (!item)
        return;

    if (auto* grp = dynamic_cast<LzGroup*>(item))
        grp->syncItemBoundsFromChildren();

    if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
    {
        line->cancelDeferredPathRecompute();
        return;
    }
    if (auto* shape = dynamic_cast<LzItem*>(item))
    {
        const QVector<QGraphicsItem*> connected = shape->connectedItems();
        for (QGraphicsItem* connectedItem : connected)
        {
            if (auto* line = qgraphicsitem_cast<LzConnectLine*>(connectedItem))
            {
                if (preservePathShape)
                    line->preservePathAfterHierarchyChange();
                else
                {
                    if (line->pathRoutingMode() == ConnectorPathRoutingMode::Manual)
                        line->translateManualPathWithAttachedItems();
                    else
                        line->refreshEndpointsFromAttachedItems();
                }
            }
        }
    }

    const auto children = item->childItems();
    for (QGraphicsItem* child : children)
        refreshConnectionsForItemTree(child, preservePathShape);
}

LzGroup::LzGroup(QGraphicsItem* parent)
    :TmpBase<QGraphicsItemGroup>(parent)
{
    //setHandlesChildEvents(true);
}

LzGroup::~LzGroup()
{

}

QRectF LzGroup::boundingRect() const
{
    return itemPosition;
}

void LzGroup::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

}

bool LzGroup::saveXml(QDomElement* g)
{
    if (!g)
        return false;

    writeXmlObjectId(g);
    g->setAttribute(QStringLiteral("topologyNodeId"), lzObjectId());
    g->setAttribute("type", "group");
    g->setAttribute("x", pos().x());
    g->setAttribute("y", pos().y());

    foreach(QGraphicsItem* child, childItems()) {
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(child)) {
            QDomElement childElement = g->ownerDocument().createElement("item");
            if (shape->saveXml(&childElement)) {
                g->appendChild(childElement);
            }
        }
    }
    return true;
}

bool LzGroup::loadXml(QDomElement* g)
{
    if (!g)
        return false;

    readXmlObjectId(*g);

    if (g->hasAttribute(QStringLiteral("topologyNodeId"))) {
        const QString tid = g->attribute(QStringLiteral("topologyNodeId"));
        if (!tid.isEmpty() && lzObjectId().isEmpty())
            setLzObjectId(tid);
    }

    setPos(g->attribute(QStringLiteral("x")).toDouble(), g->attribute(QStringLiteral("y")).toDouble());

    QDomElement childItem = g->firstChildElement(QStringLiteral("item"));
    while (!childItem.isNull()) {
        QString type = childItem.attribute(QStringLiteral("type"));
        if (type.isEmpty())
            type = childItem.attribute(QStringLiteral("shape"));

        QGraphicsItem* child = LzObjectFactory::instance()->createObject(type);
        if (!child) {
            childItem = childItem.nextSiblingElement(QStringLiteral("item"));
            continue;
        }
        if (auto* li = dynamic_cast<LzItem*>(child)) {
            if (!li->loadXml(&childItem)) {
                delete li;
                return false;
            }
            addToGroup(li);
        } else if (auto* nested = dynamic_cast<LzGroup*>(child)) {
            if (!nested->loadXml(&childItem)) {
                delete nested;
                return false;
            }
            addToGroup(nested);
        } else {
            delete child;
        }
        childItem = childItem.nextSiblingElement(QStringLiteral("item"));
    }
    return true;
}


void LzGroup::addToGroup(QGraphicsItem* item)
{
    if (!item || item == this)
        return;
    QGraphicsItemGroup::addToGroup(item);
}

void LzGroup::removeFromGroup(QGraphicsItem* item)
{
    if (!item || item == this)
        return;
    QGraphicsItemGroup::removeFromGroup(item);
}

void LzGroup::control(int dir, const QPointF & delta)
{
    updateHandles();
}

void LzGroup::stretch(int handle, double sx, double sy, const QPointF & origin)
{
    QTransform trans;
	switch (handle) 
	{
	case RIGHT:
	case LEFT:
		sy = 1;
		break;
	case TOP:
	case BOTTOM:
		sx = 1;
		break;
	default:
		break;
	}
	for (auto item : childItems())
	{
		TmpShape* ab = qgraphicsitem_cast<TmpShape*>(item);
		if (ab && !isGroupHandle(ab))
		{
			ab->stretch(handle, sx, sy, ab->mapFromParent(origin));
		}
	}
	trans.translate(origin.x(), origin.y());
	trans.scale(sx, sy);
	trans.translate(-origin.x(), -origin.y());
	prepareGeometryChange();
	itemPosition = trans.mapRect(itemInitialRect);
	itemWidth = itemPosition.width();
	itemHeight = itemPosition.height();
    refreshConnectionsForItemTree(this, false);
	updateHandles();
}

void LzGroup::copyItemFrameFrom(const LzGroup* source)
{
    if (!source || source == this)
        return;
    prepareGeometryChange();
    itemPosition = source->itemPosition;
    itemInitialRect = source->itemInitialRect;
    itemWidth = source->itemWidth;
    itemHeight = source->itemHeight;
    updateHandles();
}

void LzGroup::syncItemBoundsFromChildren()
{
    QRectF bounds;
    for (QGraphicsItem* child : childItems())
    {
        if (isGroupHandle(child))
            continue;
        const QRectF r = mapRectFromItem(child, child->boundingRect());
        if (!r.isValid())
            continue;
        bounds = bounds.isNull() ? r : bounds.united(r);
    }
    if (bounds.isNull())
        return;
    prepareGeometryChange();
    itemPosition = bounds;
    itemWidth = bounds.width();
    itemHeight = bounds.height();
    itemInitialRect = itemPosition;
    updateHandles();
}

QGraphicsItem* LzGroup::duplicate()
{
    auto* g = new LzGroup();
    g->setPos(pos());
    g->setTransform(transform());
    g->setTransformOriginPoint(transformOriginPoint());
    g->setRotation(rotation());
    g->setScale(scale());
    g->setZValue(zValue());

    QVector<LzConnectLinePathSnapshot> lineSnapshots;
    lineSnapshots.reserve(8);

    for (QGraphicsItem* child : childItems())
    {
        if (isGroupHandle(child))
            continue;
        if (auto* nested = dynamic_cast<LzGroup*>(child))
        {
            if (QGraphicsItem* dup = nested->duplicate())
                g->addToGroup(dup);
            continue;
        }
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(child))
        {
            LzConnectLinePathSnapshot snap;
            snap.scenePoints = line->pathInSceneCoords();
            snap.routingMode = line->pathRoutingMode();
            if (QGraphicsItem* dup = line->duplicate())
            {
                g->addToGroup(dup);
                snap.line = qgraphicsitem_cast<LzConnectLine*>(dup);
                if (snap.line && snap.scenePoints.size() >= 2)
                    lineSnapshots.append(snap);
            }
            continue;
        }
        if (auto* li = dynamic_cast<LzItem*>(child))
        {
            if (!qgraphicsitem_cast<LzConnectLine*>(li))
            {
                const QPointF scenePos = child->scenePos();
                if (QGraphicsItem* dup = li->duplicate())
                {
                    g->addToGroup(dup);
                    dup->setPos(g->mapFromScene(scenePos));
                }
            }
            continue;
        }
        if (TmpShape* shape = qgraphicsitem_cast<TmpShape*>(child))
        {
            const QPointF scenePos = child->scenePos();
            if (QGraphicsItem* dup = shape->duplicate())
            {
                g->addToGroup(dup);
                dup->setPos(g->mapFromScene(scenePos));
            }
        }
    }
    restoreConnectLineSnapshots(lineSnapshots);
    g->copyItemFrameFrom(this);
    g->setLzObjectId(QString());
    return g;
}

void LzGroup::setPreserveLinePathsDuringGeometryUpdate(bool preserve)
{
    preserveLinePathsDuringGeometryUpdate_ = preserve;
}

bool LzGroup::preserveLinePathsDuringGeometryUpdate() const
{
    return preserveLinePathsDuringGeometryUpdate_;
}

void LzGroup::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    if (itemPosition.isNull())
    {
        itemPosition = QGraphicsItemGroup::boundingRect();
    }

	pt1 = mapToScene(transformOriginPoint()); 
	pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    itemInitialRect = itemPosition;
	itemWidth = itemPosition.width();
    itemHeight = itemPosition.height();
	setTransform(transform().translate(delta.x(), delta.y()));
	setTransformOriginPoint(itemPosition.center());
    moveBy(-delta.x(), -delta.y());

    updateHandles();
}

QVariant LzGroup::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged) 
	{
		QGraphicsItemGroup *g = qgraphicsitem_cast<QGraphicsItemGroup*>(parentItem());
		if (!g)
		{
			setState(value.toBool() ? HANDLE_ACTIVE : HANDLE_OFF);
		}
		else 
		{
			setSelected(false); 
			return QVariant::fromValue<bool>(false);
		}
	}
    else if (change == QGraphicsItem::ItemPositionHasChanged
             || change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        syncItemBoundsFromChildren();
        refreshConnectionsForItemTree(this, preserveLinePathsDuringGeometryUpdate_);
    }
	return QGraphicsItemGroup::itemChange(change, value);
}

void LzGroup::setWidth(qreal width)
{
    itemWidth = width;
    updateCoordinate();
    refreshConnectionsForItemTree(this, false);
}

void LzGroup::setHeight(qreal height)
{
    itemHeight = height;
    updateCoordinate();
    refreshConnectionsForItemTree(this, false);
}

void LzGroup::setSize(QSizeF size)
{
    itemWidth = size.width();
    itemHeight = size.height();
    updateCoordinate();
    refreshConnectionsForItemTree(this, false);
}

qreal LzGroup::itemLeft()
{
    return mapRectToScene(itemPosition).topLeft().x();
}	

qreal LzGroup::itemTop()
{
    return mapRectToScene(itemPosition).topLeft().y();
}

qreal LzGroup::itemBottom()
{
    return mapRectToScene(itemPosition).bottomRight().y();
}

qreal LzGroup::itemRight()
{
    return mapRectToScene(itemPosition).bottomRight().x();
}

qreal LzGroup::centerX()
{
    return mapRectToScene(itemPosition).center().x();
}

qreal LzGroup::centerY()
{
    return mapRectToScene(itemPosition).center().y();
}

REGISTER_OBJECT_CREATOR(LzGroup, group)
