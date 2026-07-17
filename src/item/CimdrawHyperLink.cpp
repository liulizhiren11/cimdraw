#include "CimdrawHyperLink.h"
#include "CimdrawObjectFactory.h"
#include <QPainter>

class CimdrawHyperLinkPrivate
{
    Q_DECLARE_PUBLIC(CimdrawHyperLink)
public:
    CimdrawHyperLinkPrivate(CimdrawHyperLink* link)
        :q_ptr(link)
    {

    }
    QString hyperLink;
    QString displayText;
    QString imageFile;
    QPixmap pixmap;
    HYPER_LINK_TYPE linkType;
    CimdrawHyperLink* q_ptr;
};

CimdrawHyperLink::CimdrawHyperLink(QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawHyperLinkPrivate(this))
{
    Q_D(CimdrawHyperLink);
    setUseFont(false);
}

CimdrawHyperLink::CimdrawHyperLink(const QRectF& pos,QGraphicsItem* parent)
    :CimdrawItem(parent),d_ptr(new CimdrawHyperLinkPrivate(this))
{
    Q_D(CimdrawHyperLink);
    setUseFont(false);
    itemWidth = pos.width();
    itemHeight = pos.height();
    itemPosition = QRectF(0,0, itemWidth,itemHeight);
    itemInitialRect = itemPosition;
    updateCoordinate();
}

QPainterPath CimdrawHyperLink::shape() const
{
    QPainterPath path;
    path.addRect(itemPosition);
    return path;
}

QRectF CimdrawHyperLink::boundingRect() const
{
    return itemPosition;
}

void CimdrawHyperLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_D(const CimdrawHyperLink);
    prepare(painter);
   // painter->drawArc(itemPosition, d_ptr->startAngle,d_ptr->sweepAngle*16); // 使用初始矩形进行绘制
}

void CimdrawHyperLink::updateCoordinate()
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

bool CimdrawHyperLink::saveXml(QDomElement *g)
{
    Q_D(const CimdrawHyperLink);
    g->setAttribute("shape", CIMDRAW_HYPER_LINK);

    CimdrawItem::saveXml(g);
    return true;
}

bool CimdrawHyperLink::loadXml(QDomElement *g)
{
    Q_D(CimdrawHyperLink);
    CimdrawItem::loadXml(g);

    updateCoordinate();
    return true;
}

QString CimdrawHyperLink::className() const
{
    return "CimdrawHyperLink";
}

QString CimdrawHyperLink::shapeName() const
{
    return "hyperlink";
}

REGISTER_OBJECT_CREATOR(CimdrawHyperLink, hyperlink)
