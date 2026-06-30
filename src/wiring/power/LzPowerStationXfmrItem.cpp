#include "LzPowerStationXfmrItem.h"

#include "Item/LzConnectPoint.h"
#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerStationXfmrItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal rad = qMin(rect.height(), rect.width() * 0.42) * 0.30;
    const QPointF c1(rect.left() + rect.width() * 0.28, rect.center().y());
    const QPointF c2(rect.right() - rect.width() * 0.28, rect.center().y());
    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, c1.x() - rad, cy));
    painter->drawLine(QLineF(c2.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c1, rad, rad);
    painter->drawEllipse(c2, rad, rad);
    painter->drawLine(QLineF(c1.x() + rad * 0.85, c1.y(), c2.x() - rad * 0.85, c2.y()));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.2, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("站用变"));
}

LzPowerStationXfmrItem::LzPowerStationXfmrItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::StationXfmr, parent)
{
}

LzPowerStationXfmrItem::LzPowerStationXfmrItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::StationXfmr, pos, parent)
{
}

QString LzPowerStationXfmrItem::className()
{
    return QStringLiteral("LzPowerStationXfmrItem");
}

QString LzPowerStationXfmrItem::shapeName() const
{
    return QStringLiteral("wsymstationxfmr");
}

LzDrawTypeId LzPowerStationXfmrItem::drawTypeForXml() const
{
    return LZ_WSYM_STATION_XFMR;
}

LzWiringItemBase* LzPowerStationXfmrItem::cloneForDuplicate() const
{
    return new LzPowerStationXfmrItem(rect());
}

void LzPowerStationXfmrItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerStationXfmrItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzPowerStationXfmrItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
{
    if (!point)
        return;

    static const CONNECT_DIRECTION kDirections[] = {
        LEFT_DIRECTION,
        RIGHT_DIRECTION,
    };
    static const QPointF kOffsets[] = {
        QPointF(-0.5, 0.0),
        QPointF(0.5, 0.0),
    };

    const int safeIndex = qBound(0, index, 1);
    point->setDir(safeIndex);
    point->setDirection(kDirections[safeIndex]);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<LzPowerStationXfmrItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerStationXfmrItem, wsymstationxfmr)
