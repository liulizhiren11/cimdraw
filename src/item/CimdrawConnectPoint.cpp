#include "CimdrawConnectPoint.h"
#include "CimdrawConnectConfig.h"
#include "Tmpbase.h"

// draw.io 风格：连接点半径；直接点击热区较小，避免挡住图元选中（吸附仍用 CimdrawConnectPointSnapRadiusPx）
static constexpr qreal ConnectPointRadius = 5.0;
static constexpr qreal ConnectPointHoverRadius = 6.0;
static constexpr qreal ConnectPointHitRadius = 8.0;

class CimdrawConnectPointPrivate
{
    Q_DECLARE_PUBLIC(CimdrawConnectPoint)
public:
    CimdrawConnectPointPrivate(CimdrawConnectPoint* point)
        :q_ptr(point)
    {

    }
    QPointF relativePos = QPointF(); // 记录相对于某个主图元的位置
    QGraphicsItem* targetItem = nullptr; // 指向绑定的主图元
    CONNECT_DIRECTION direction = CONNECT_DIRECTION::NONE_DIRECTION;
    int dir = -1;
    QString portTag;
    qreal offset = 12.0;      // 连接点相对父图元中心的偏移（用于计算圆心）
    bool hovered = false;
    CimdrawConnectPoint* q_ptr;
};

CimdrawConnectPoint::CimdrawConnectPoint(QGraphicsItem* parent)
    :QGraphicsItem(parent),d_ptr(new CimdrawConnectPointPrivate(this))
{
    setAcceptHoverEvents(true);
    setZValue(100.0);
}

CimdrawConnectPoint::~CimdrawConnectPoint()
{
    
}

int CimdrawConnectPoint::dir() const
{
    return d_ptr->dir;
}

void CimdrawConnectPoint::setDir(int d)
{
    d_ptr->dir = d;
}

CONNECT_DIRECTION CimdrawConnectPoint::getDirection() const
{
    return d_ptr->direction;
}

void CimdrawConnectPoint::setDirection(CONNECT_DIRECTION dir)
{
    d_ptr->direction = dir;
}

QString CimdrawConnectPoint::portTag() const
{
    return d_ptr->portTag;
}

void CimdrawConnectPoint::setPortTag(const QString& tag)
{
    d_ptr->portTag = tag.trimmed();
}

void CimdrawConnectPoint::setConnectionLeadOut(qreal pixels)
{
    d_ptr->offset = qMax(0.0, pixels);
}

qreal CimdrawConnectPoint::connectionLeadOut() const
{
    return d_ptr->offset;
}

void CimdrawConnectPoint::bindToItem(QGraphicsItem* target, const QPointF& offset)
{
    d_ptr->targetItem = target;
    d_ptr->relativePos = offset;
}

QPointF CimdrawConnectPoint::getRelativePos() const
{
    return d_ptr->relativePos;
}

void CimdrawConnectPoint::move(qreal x, qreal y)
{
    setPos(x, y);
}

void CimdrawConnectPoint::setState(HANDLE_STATE state)
{
    Q_UNUSED(state);
}

void CimdrawConnectPoint::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    QGraphicsItem::hoverEnterEvent(e);
    d_ptr->hovered = true;
    update();
}

void CimdrawConnectPoint::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    QGraphicsItem::hoverLeaveEvent(e);
    d_ptr->hovered = false;
    update();
}

// 根据方向计算连接点圆心（draw.io 风格：点在形状边缘）
static QPointF connectPointCenter(const CimdrawConnectPointPrivate* d)
{
    QPointF center(0, 0);
    switch (d->direction)
    {
    case TOP_DIRECTION:
        center.setY(-d->offset);
        break;
    case RIGHT_DIRECTION:
        center.setX(d->offset);
        break;
    case BOTTOM_DIRECTION:
        center.setY(d->offset);
        break;
    case LEFT_DIRECTION:
        center.setX(-d->offset);
        break;
    default:
        break;
    }
    return center;
}

QPointF CimdrawConnectPoint::connectionCenterLocal() const
{
    return connectPointCenter(d_ptr.get());
}

QPointF CimdrawConnectPoint::connectionCenterInScene() const
{
    return mapToScene(connectionCenterLocal());
}

QPainterPath CimdrawConnectPoint::shape() const
{
    QPointF center = connectPointCenter(d_ptr.get());
    QPainterPath path;
    path.addEllipse(center, ConnectPointHitRadius, ConnectPointHitRadius);
    return path;
}

QRectF CimdrawConnectPoint::boundingRect() const
{
    QPointF c = connectPointCenter(d_ptr.get());
    qreal r = ConnectPointHoverRadius + 1.0;  // 留边
    return QRectF(c.x() - r, c.y() - r, r * 2, r * 2);
}

void CimdrawConnectPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QPointF center = connectPointCenter(d_ptr.get());
    qreal radius = d_ptr->hovered ? ConnectPointHoverRadius : ConnectPointRadius;

    // draw.io 风格：白底 + 蓝色描边
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0xFF, 0xFF, 0xFF));
    painter->drawEllipse(center, radius, radius);

    QPen pen(QColor(0x1a, 0x73, 0xe8), d_ptr->hovered ? 1.5 : 1.0);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(center, radius, radius);
}
