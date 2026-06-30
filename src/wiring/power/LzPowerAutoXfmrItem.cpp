#include "LzPowerAutoXfmrItem.h"

#include "Item/LzConnectPoint.h"
#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerAutoXfmrItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal cy = rect.center().y();
    const qreal rad = qMin(rect.height(), rect.width() * 0.38) * 0.38;
    const QPointF c(rect.center().x(), cy);
    painter->drawLine(QLineF(rect.left(), cy, c.x() - rad, cy));
    painter->drawLine(QLineF(c.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c, rad, rad);
    QPen dash(stroke, 1, Qt::DashLine);
    painter->setPen(dash);
    painter->drawEllipse(c, rad * 0.52, rad * 0.52);
    painter->setPen(pen);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("自耦"));
}

LzPowerAutoXfmrItem::LzPowerAutoXfmrItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::AutoXfmr, parent)
{
}

LzPowerAutoXfmrItem::LzPowerAutoXfmrItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::AutoXfmr, pos, parent)
{
}

QString LzPowerAutoXfmrItem::className()
{
    return QStringLiteral("LzPowerAutoXfmrItem");
}

QString LzPowerAutoXfmrItem::shapeName() const
{
    return QStringLiteral("wsymautoxfmr");
}

LzDrawTypeId LzPowerAutoXfmrItem::drawTypeForXml() const
{
    return LZ_WSYM_AUTO_XFMR;
}

LzWiringItemBase* LzPowerAutoXfmrItem::cloneForDuplicate() const
{
    return new LzPowerAutoXfmrItem(rect());
}

void LzPowerAutoXfmrItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerAutoXfmrItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzPowerAutoXfmrItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
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
    point->bindToItem(const_cast<LzPowerAutoXfmrItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerAutoXfmrItem, wsymautoxfmr)
