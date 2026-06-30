#include "LzHyperLink.h"
#include "LzObjectFactory.h"
#include <QPainter>

class LzHyperLinkPrivate
{
    Q_DECLARE_PUBLIC(LzHyperLink)
public:
    LzHyperLinkPrivate(LzHyperLink* link)
        :q_ptr(link)
    {

    }
    QString hyperLink;
    QString displayText;
    QString imageFile;
    QPixmap pixmap;
    HYPER_LINK_TYPE linkType;
    LzHyperLink* q_ptr;
};

LzHyperLink::LzHyperLink(QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzHyperLinkPrivate(this))
{
    Q_D(LzHyperLink);
    setUseFont(false);
}

LzHyperLink::LzHyperLink(const QRectF& pos,QGraphicsItem* parent)
    :LzItem(parent),d_ptr(new LzHyperLinkPrivate(this))
{
    Q_D(LzHyperLink);
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath LzHyperLink::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF LzHyperLink::boundingRect() const
{
    return itemPosition;
}

void LzHyperLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const LzHyperLink);
    prepare(painter);
   // painter->drawArc(itemPosition, d_ptr->startAngle,d_ptr->sweepAngle*16); // 使用初始矩形进行绘制
}

void LzHyperLink::updateCoordinate()
{
    QPointF pt1, pt2, delta;
    pt1 = mapToScene(transformOriginPoint());
    pt2 = mapToScene(itemPosition.center());
    delta = pt1 - pt2;
    prepareGeometryChange();
    if (!parentItem())
    {
        itemPosition = QRectF(-itemWidth / 2, -itemHeight / 2, itemWidth, itemHeight);
        itemWidth = itemPosition.width();
        itemHeight = itemPosition.height();
        setTransform(transform().translate(delta.x(), delta.y()));
        setTransformOriginPoint(itemPosition.center());
        moveBy(-delta.x(), -delta.y());
        setTransform(transform().translate(-delta.x(), -delta.y()));
        updateHandles();
    }
    itemInitialRect = itemPosition;
}

bool LzHyperLink::saveXml(QDomElement *g)
{
    Q_D(const LzHyperLink);
    g->setAttribute("shape", LZ_HYPER_LINK);

    LzItem::saveXml(g);
    return true;
}

bool LzHyperLink::loadXml(QDomElement *g)
{
    Q_D(LzHyperLink);
    LzItem::loadXml(g);

    updateCoordinate();
    return true;
}

QString LzHyperLink::className() const
{
    return "LzHyperLink";
}

QString LzHyperLink::shapeName() const
{
    return "hyperlink";
}

REGISTER_OBJECT_CREATOR(LzHyperLink, hyperlink)
