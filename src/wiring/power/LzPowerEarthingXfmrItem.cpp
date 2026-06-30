#include "LzPowerEarthingXfmrItem.h"

#include "Item/LzConnectPoint.h"
#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

void LzPowerEarthingXfmrItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal rad = qMin(rect.height() * 0.36, rect.width() * 0.22);
    const QPointF c1(rect.left() + rect.width() * 0.30, rect.center().y());
    const QPointF c2(rect.right() - rect.width() * 0.30, rect.center().y());
    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, c1.x() - rad, cy));
    painter->drawLine(QLineF(c2.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c1, rad, rad);
    painter->drawEllipse(c2, rad, rad);
    painter->drawLine(QLineF(c1.x() + rad * 0.85, c1.y(), c2.x() - rad * 0.85, c2.y()));
    QPolygonF zig;
    const qreal zr = rad * 0.55;
    zig << QPointF(c1.x() - zr * 0.4, c1.y() + zr * 0.15)
        << QPointF(c1.x() - zr * 0.1, c1.y() - zr * 0.35)
        << QPointF(c1.x() + zr * 0.2, c1.y() + zr * 0.2)
        << QPointF(c1.x() + zr * 0.45, c1.y() - zr * 0.25);
    painter->drawPolyline(zig);
    const qreal yG = rect.bottom() - qMax(6.0, rect.height() * 0.12);
    const qreal gw = qMin(rect.width() * 0.22, 18.0);
    const qreal gx = rect.center().x();
    painter->drawLine(QLineF(gx, cy + rad + 2, gx, yG - 10));
    painter->drawLine(QLineF(gx - gw * 0.5, yG - 10, gx + gw * 0.5, yG - 10));
    painter->drawLine(QLineF(gx - gw * 0.35, yG - 6, gx + gw * 0.35, yG - 6));
    painter->drawLine(QLineF(gx - gw * 0.2, yG - 2, gx + gw * 0.2, yG - 2));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.17, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 2, -2, -2), Qt::AlignHCenter | Qt::AlignTop, QStringLiteral("接地变"));
}

LzPowerEarthingXfmrItem::LzPowerEarthingXfmrItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::EarthingXfmr, parent)
{
}

LzPowerEarthingXfmrItem::LzPowerEarthingXfmrItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::EarthingXfmr, pos, parent)
{
}

QString LzPowerEarthingXfmrItem::className()
{
    return QStringLiteral("LzPowerEarthingXfmrItem");
}

QString LzPowerEarthingXfmrItem::shapeName() const
{
    return QStringLiteral("wsymearthingxfmr");
}

LzDrawTypeId LzPowerEarthingXfmrItem::drawTypeForXml() const
{
    return LZ_WSYM_EARTHING_XFMR;
}

LzWiringItemBase* LzPowerEarthingXfmrItem::cloneForDuplicate() const
{
    return new LzPowerEarthingXfmrItem(rect());
}

void LzPowerEarthingXfmrItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerEarthingXfmrItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzPowerEarthingXfmrItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
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
    point->bindToItem(const_cast<LzPowerEarthingXfmrItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerEarthingXfmrItem, wsymearthingxfmr)
