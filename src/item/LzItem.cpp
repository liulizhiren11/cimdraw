#include <QPainter>
#include "LzItem.h"
#include "LzConnectLine.h"
#include "LzScene.h"

LzItem::LzItem(QGraphicsItem* parent)
    :TmpBase<QGraphicsItem>(parent)
{

}

LzItem::LzItem(const QRectF& pos, QGraphicsItem* parent)
    :TmpBase<QGraphicsItem>(parent)
{

}

QVariant LzItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(parentItem());
        if (!g)
        {
            setState(value.toBool() ? HANDLE_ACTIVE : HANDLE_OFF);
            setOpacity(value.toBool() ? 0.88 : 1.0);
            // 选中/框选仅更新手柄与半透明，不重算连线路径（路径在移动/缩放/改端口时 refresh）
        }
        else
        {
            setSelected(false);
            return QVariant::fromValue<bool>(false);
        }
    }
    if(change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemScenePositionHasChanged)
    {
        QGraphicsItem *g = dynamic_cast<QGraphicsItem*>(parentItem());
        if (g)
            return QGraphicsItem::itemChange(change, value);
        if (!g)
        {
            LzScene* scene_ = dynamic_cast<LzScene*>(scene());
            if(!scene_)
                return QGraphicsItem::itemChange(change, value);
            if (scene_->isInteractiveTransformActive())
                return QGraphicsItem::itemChange(change, value);
            if (scene_->isConnectLinePathUpdatePending())
                return QGraphicsItem::itemChange(change, value);
            if(scene_->getSelections().count()==1)
            {
                if(scene_->getSelections().contains(this))
                {
                    emit scene_->itemPropertyChanged();
                }
            }
            refreshConnectedLines();
        }
        else
        {
            refreshConnectedLines();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void LzItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

}

void LzItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{

}

bool LzItem::saveXml(QDomElement *g)
{
    writeXmlObjectId(g);
    g->setAttribute(QStringLiteral("topologyNodeId"), lzObjectId());
    g->setAttribute("type", shapeName());
    g->setAttribute("version", "1.0.0");

    QDomDocument doc = g->toDocument();

    QDomElement posElm = doc.createElement("position");
    g->appendChild(posElm);
    posElm.setAttribute("x", pos().x());
    posElm.setAttribute("y", pos().y());
    posElm.setAttribute("w", itemWidth);
    posElm.setAttribute("h", itemHeight);
    posElm.setAttribute("rotate", rotation());

    QDomElement penElm = doc.createElement("pen");
    g->appendChild(penElm);
    penElm.setAttribute("valid", usePen ? 1 : 0);
    penElm.setAttribute("style", itemPenStyle);
    penElm.setAttribute("width", itemPenWidth);
    penElm.setAttribute("color", itemPenColor.name());

    QDomElement brushElm = doc.createElement("brush");
    g->appendChild(brushElm);
    brushElm.setAttribute("valid", useBrush ? 1 : 0);
    brushElm.setAttribute("style", itemBrushStyle);
    brushElm.setAttribute("color", itemBrushColor.name());

    QDomElement fontElm = doc.createElement("font");
    g->appendChild(fontElm);
    fontElm.setAttribute("valid", useFont ? 1:0);
    fontElm.setAttribute("family", itemFont.family());
    fontElm.setAttribute("weight", itemFont.weight());
    fontElm.setAttribute("italic", itemFont.italic());
    fontElm.setAttribute("pointsize", itemFont.pointSize());
    fontElm.setAttribute("underline", itemFont.underline());
    fontElm.setAttribute("bold", itemFont.bold());
    return true;
}

bool LzItem::loadXml(QDomElement *g)
{
    readXmlObjectId(*g);
    if (g->hasAttribute(QStringLiteral("topologyNodeId"))) {
        const QString tid = g->attribute(QStringLiteral("topologyNodeId"));
        if (!tid.isEmpty() && lzObjectId().isEmpty())
            setLzObjectId(tid);
    }
    if (!g->hasAttribute("version"))
    {
        return false;
    }
    QDomElement posElm = g->firstChildElement("position");
    if (!posElm.isNull())
    {
        setPos(posElm.attribute("x").toDouble(), posElm.attribute("y").toDouble());
        itemWidth=posElm.attribute("w").toDouble();
        itemHeight=posElm.attribute("h").toDouble();
        itemDegree = posElm.attribute("rotate").toInt();
        setRotation(posElm.attribute("rotate").toInt());
    }

    QDomElement penElm = g->firstChildElement("pen");
    if (!penElm.isNull())
    {
        usePen = penElm.attribute("valid").toUInt();
        itemPenStyle = penElm.attribute("style").toInt();
        itemPenWidth = penElm.attribute("width").toInt();
        itemPenColor = QColor(penElm.attribute("color"));
    }

    QDomElement brushElm = g->firstChildElement("brush");
    if (!brushElm.isNull())
    {
        useBrush = brushElm.attribute("valid").toInt();
        itemBrushStyle = brushElm.attribute("style").toInt();
        itemBrushColor = QColor(brushElm.attribute("color"));
    }

    QDomElement fontElm = g->firstChildElement("font");
    if (!fontElm.isNull())
    {
        itemFont.setFamily(fontElm.attribute("family"));
        itemFont.setItalic(fontElm.attribute("italic").toInt());
        itemFont.setPointSize(fontElm.attribute("pointsize").toInt());
        itemFont.setUnderline(fontElm.attribute("underline").toInt());
        itemFont.setBold(fontElm.attribute("bold").toInt());
    }
    return true;
}

qreal LzItem::itemLeft()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal leftEdge = std::min({topLeft.x(), topRight.x(), bottomLeft.x(), bottomRight.x()});
    return leftEdge;
}

qreal LzItem::itemTop()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal topEdge = std::min({topLeft.y(), topRight.y(), bottomLeft.y(), bottomRight.y()});
    return topEdge;
}

qreal LzItem::itemBottom()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal bottomEdge = std::max({topLeft.y(), topRight.y(), bottomLeft.y(), bottomRight.y()});
    return bottomEdge;
}

qreal LzItem::itemRight()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal rightEdge = std::max({topLeft.x(), topRight.x(), bottomLeft.x(), bottomRight.x()});
    return rightEdge;
}

qreal LzItem::centerX()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal centerX = (topLeft.x() + topRight.x()) / 2;
    return centerX;
}

qreal LzItem::centerY()
{
    QRectF boundingRect = this->boundingRect();

    QPointF topLeft = mapToScene(boundingRect.topLeft());
    QPointF topRight = mapToScene(boundingRect.topRight());
    QPointF bottomLeft = mapToScene(boundingRect.bottomLeft());
    QPointF bottomRight = mapToScene(boundingRect.bottomRight());

    qreal centerY = (topLeft.y() + topRight.y()) / 2;
    return centerY;
}

void LzItem::setWidth(qreal width)
{
    itemWidth = width;
    updateCoordinate();
}

void LzItem::setHeight(qreal height)
{
    itemHeight = height;
    updateCoordinate();
}

void LzItem::setSize(QSizeF size)
{
    itemWidth = size.width();
    itemHeight = size.height();
    updateCoordinate();
}

void LzItem::updateConnect(const QPointF& delta)
{
    for (auto item : connectStruct.connects)
    {
        TmpShape* connect = qgraphicsitem_cast<TmpShape*>(item);
        if (!connect)
            continue;
        connect->updatePosition(this, delta);
    }
}

void LzItem::refreshConnectedLines()
{
    QList<LzConnectLine*> lines;
    lines.reserve(connectStruct.connects.size());
    for (QGraphicsItem* item : connectStruct.connects)
    {
        if (auto* line = qgraphicsitem_cast<LzConnectLine*>(item))
            lines.append(line);
    }
    if (lines.isEmpty())
        return;

    if (LzScene* sc = dynamic_cast<LzScene*>(scene()))
    {
        Q_UNUSED(sc);
        for (LzConnectLine* line : lines)
        {
            if (!line)
                continue;
            line->cancelDeferredPathRecompute();
            line->finalizePathAfterItemDrag();
        }
        return;
    }

    for (LzConnectLine* line : lines)
        line->refreshEndpointsFromAttachedItems();
}
