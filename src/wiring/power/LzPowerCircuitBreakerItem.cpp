#include "LzPowerCircuitBreakerItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerCircuitBreakerItem::drawSymbol(QPainter* painter,
                                           const QRectF& rect,
                                           LzWiringRunState runState,
                                           bool alarmPulse,
                                           bool breakerOpen)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const int baseW = runState == LzWiringRunState::Alarm && alarmPulse ? 4 : 2;
    QPen solidPen(stroke, baseW);
    solidPen.setCapStyle(Qt::FlatCap);
    painter->setBrush(Qt::NoBrush);
    const qreal gap = breakerOpen ? qMax(14.0, rect.width() * 0.26) : qMax(5.0, rect.width() * 0.11);
    const qreal mid = rect.center().x();
    const qreal y1 = rect.top() + rect.height() * 0.22;
    const qreal y2 = rect.bottom() - rect.height() * 0.22;
    painter->setPen(solidPen);
    painter->drawLine(QLineF(rect.left(), y1, rect.left(), y2));
    painter->drawLine(QLineF(rect.right(), y1, rect.right(), y2));
    QPen innerPen = solidPen;
    if (breakerOpen)
    {
        innerPen.setStyle(Qt::DashLine);
        innerPen.setDashPattern({6, 4});
    }
    painter->setPen(innerPen);
    painter->drawLine(QLineF(rect.left(), y1, mid - gap / 2, y1));
    painter->drawLine(QLineF(mid + gap / 2, y1, rect.right(), y1));
    painter->drawLine(QLineF(rect.left(), y2, mid - gap / 2, y2));
    painter->drawLine(QLineF(mid + gap / 2, y2, rect.right(), y2));
    painter->setPen(solidPen);
    painter->drawLine(QLineF(mid - gap / 2, y1, mid - gap / 2, y2));
    painter->drawLine(QLineF(mid + gap / 2, y1, mid + gap / 2, y2));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.28, 12.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("QF"));
}

LzPowerCircuitBreakerItem::LzPowerCircuitBreakerItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::CircuitBreaker, parent)
{
}

LzPowerCircuitBreakerItem::LzPowerCircuitBreakerItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::CircuitBreaker, pos, parent)
{
}

QString LzPowerCircuitBreakerItem::className()
{
    return QStringLiteral("LzPowerCircuitBreakerItem");
}

QString LzPowerCircuitBreakerItem::shapeName() const
{
    return QStringLiteral("circuitbreaker");
}

LzDrawTypeId LzPowerCircuitBreakerItem::drawTypeForXml() const
{
    return LZ_SLD_CIRCUIT_BREAKER;
}

LzWiringItemBase* LzPowerCircuitBreakerItem::cloneForDuplicate() const
{
    return new LzPowerCircuitBreakerItem(rect());
}

void LzPowerCircuitBreakerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, breakerVisualOpen());
}

REGISTER_OBJECT_CREATOR(LzPowerCircuitBreakerItem, circuitbreaker)

int LzPowerCircuitBreakerItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzPowerCircuitBreakerItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
{
    if (!point)
        return;

    const qreal y = index <= 0 ? -0.28 : 0.28;
    point->setConnectionLeadOut(0.0);
    point->setDirection(index <= 0 ? TOP_DIRECTION : BOTTOM_DIRECTION);
    point->bindToItem(const_cast<LzPowerCircuitBreakerItem*>(this), QPointF(0.0, y));
}

QPainterPath LzPowerCircuitBreakerItem::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    const QRectF r = rect().normalized();
    if (!r.isValid())
        return path;

    const qreal pad = 2.0;
    const qreal y1 = r.top() + r.height() * 0.22;
    const qreal y2 = r.bottom() - r.height() * 0.22;
    const qreal barW = qMax(4.0, r.width() * 0.14);
    const qreal mid = r.center().x();
    const qreal gap = qMax(5.0, r.width() * 0.11);

    path.addRect(QRectF(r.left(), y1, barW, y2 - y1));
    path.addRect(QRectF(r.right() - barW, y1, barW, y2 - y1));
    path.addRect(QRectF(r.left(), y1 - pad, mid - gap * 0.5 - r.left(), pad * 2));
    path.addRect(QRectF(mid + gap * 0.5, y1 - pad, r.right() - mid - gap * 0.5, pad * 2));
    path.addRect(QRectF(r.left(), y2 - pad, mid - gap * 0.5 - r.left(), pad * 2));
    path.addRect(QRectF(mid + gap * 0.5, y2 - pad, r.right() - mid - gap * 0.5, pad * 2));
    path.addRect(QRectF(mid - gap * 0.5, y1, pad * 2, y2 - y1));
    path.addRect(QRectF(mid + gap * 0.5 - pad * 2, y1, pad * 2, y2 - y1));
    return path;
}
