#include "LzPowerDisconnectorItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QtMath>

void LzPowerDisconnectorItem::drawSymbol(QPainter* painter,
                                         const QRectF& rect,
                                         LzWiringRunState runState,
                                         bool alarmPulse,
                                         bool switchClosed)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal yBus = rect.center().y();
    const QPointF pivot(rect.center().x(), yBus);
    const qreal halfGap = qMax(14.0, rect.width() * 0.09);
    const QPointF contactLeft(pivot.x() - halfGap, yBus);
    const QPointF contactRight(pivot.x() + halfGap, yBus);
    painter->drawLine(QLineF(rect.left(), yBus, contactLeft.x(), yBus));
    painter->drawLine(QLineF(contactRight.x(), yBus, rect.right(), yBus));
    if (switchClosed)
    {
        QPen bladePen(stroke, qMax(2.5, pen.widthF() + 0.5));
        bladePen.setCapStyle(Qt::FlatCap);
        painter->setPen(bladePen);
        painter->drawLine(QLineF(contactLeft, contactRight));
        painter->setPen(pen);
    }
    else
    {
        const qreal bladeLen = halfGap * 1.45;
        const qreal a = qDegreesToRadians(50.0);
        const QPointF bladeTip(pivot.x() + bladeLen * qCos(a), pivot.y() - bladeLen * qSin(a));
        QPen bladePen(stroke, qMax(2.0, pen.widthF()));
        bladePen.setCapStyle(Qt::RoundCap);
        painter->setPen(bladePen);
        painter->drawLine(QLineF(pivot, bladeTip));
        painter->setPen(pen);
    }
    painter->drawEllipse(contactLeft, 2.5, 2.5);
    painter->drawEllipse(contactRight, 2.5, 2.5);
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.22, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("QS"));
}

LzPowerDisconnectorItem::LzPowerDisconnectorItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Disconnector, parent)
{
}

LzPowerDisconnectorItem::LzPowerDisconnectorItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Disconnector, pos, parent)
{
}

QString LzPowerDisconnectorItem::className()
{
    return QStringLiteral("LzPowerDisconnectorItem");
}

QString LzPowerDisconnectorItem::shapeName() const
{
    return QStringLiteral("disconnector");
}

LzDrawTypeId LzPowerDisconnectorItem::drawTypeForXml() const
{
    return LZ_SLD_DISCONNECTOR;
}

LzWiringItemBase* LzPowerDisconnectorItem::cloneForDuplicate() const
{
    return new LzPowerDisconnectorItem(rect());
}

void LzPowerDisconnectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, switchPosition() != 0);
}

REGISTER_OBJECT_CREATOR(LzPowerDisconnectorItem, disconnector)

QPainterPath LzPowerDisconnectorItem::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    const QRectF r = rect().normalized();
    const qreal yBus = r.center().y();
    const qreal halfGap = qMax(14.0, r.width() * 0.09);
    path.addRect(QRectF(r.left(), yBus - 6.0, r.width() * 0.35, 12.0));
    path.addRect(QRectF(r.right() - r.width() * 0.35, yBus - 6.0, r.width() * 0.35, 12.0));
    path.addEllipse(r.center(), halfGap, 8.0);
    return path;
}
