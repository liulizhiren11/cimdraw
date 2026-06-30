#include "LzPowerReactorItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerReactorItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const QColor coil = LzWiringDrawPalette::reactorCoil(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal coilW = qMin(rect.width() * 0.48, 52.0);
    const qreal amp = qMin(rect.height() * 0.32, 12.0);
    const qreal x0 = rect.center().x() - coilW * 0.5;
    painter->drawLine(QLineF(rect.left(), centerY, x0, centerY));
    painter->setPen(QPen(coil, pen.width()));

    const int arcs = 4;
    for (int i = 0; i < arcs; ++i)
    {
        const QRectF arcRect(x0 + i * (coilW / qreal(arcs)), centerY - amp, coilW / qreal(arcs), amp * 2);
        painter->drawArc(arcRect, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(x0 + coilW, centerY, rect.right(), centerY));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.26, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("L"));
}

LzPowerReactorItem::LzPowerReactorItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Reactor, parent)
{
}

LzPowerReactorItem::LzPowerReactorItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Reactor, pos, parent)
{
}

QString LzPowerReactorItem::className()
{
    return QStringLiteral("LzPowerReactorItem");
}

QString LzPowerReactorItem::shapeName() const
{
    return QStringLiteral("wsymreactor");
}

LzDrawTypeId LzPowerReactorItem::drawTypeForXml() const
{
    return LZ_WSYM_REACTOR;
}

LzWiringItemBase* LzPowerReactorItem::cloneForDuplicate() const
{
    return new LzPowerReactorItem(rect());
}

void LzPowerReactorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

REGISTER_OBJECT_CREATOR(LzPowerReactorItem, wsymreactor)
