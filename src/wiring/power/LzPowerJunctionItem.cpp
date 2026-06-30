#include "LzPowerJunctionItem.h"

#include "Item/LzConnectPoint.h"
#include "Item/LzObjectFactory.h"
#include "wiring/base/LzWiringDrawPalette.h"

#include <QDateTime>
#include <QLineF>

void LzPowerJunctionItem::drawSymbol(QPainter* painter,
                                     const QRectF& rect,
                                     LzWiringRunState runState,
                                     bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const qreal penWidth = runState == LzWiringRunState::Alarm && alarmPulse ? 3.0 : 2.0;
    painter->setPen(QPen(stroke, penWidth, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(stroke);

    const QPointF center = rect.center();
    const qreal radius = qBound<qreal>(2.5, qMin(rect.width(), rect.height()) * 0.12, 4.5);
    const qreal lead = qMax<qreal>(radius + 2.0, qMin(rect.width(), rect.height()) * 0.34);

    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - lead, center.y()));
    painter->drawLine(QLineF(center.x() + lead, center.y(), rect.right(), center.y()));
    painter->drawLine(QLineF(center.x(), rect.top(), center.x(), center.y() - lead));
    painter->drawLine(QLineF(center.x(), center.y() + lead, center.x(), rect.bottom()));
    painter->drawEllipse(center, radius, radius);
}

LzPowerJunctionItem::LzPowerJunctionItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Junction, parent)
{
}

LzPowerJunctionItem::LzPowerJunctionItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Junction, pos, parent)
{
}

QString LzPowerJunctionItem::className()
{
    return QStringLiteral("LzPowerJunctionItem");
}

QString LzPowerJunctionItem::shapeName() const
{
    return QStringLiteral("wsymjunction");
}

LzDrawTypeId LzPowerJunctionItem::drawTypeForXml() const
{
    return LZ_WSYM_JUNCTION;
}

LzWiringItemBase* LzPowerJunctionItem::cloneForDuplicate() const
{
    return new LzPowerJunctionItem(rect());
}

void LzPowerJunctionItem::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerJunctionItem::defaultWiringConnectPointCount() const
{
    return 4;
}

void LzPowerJunctionItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
{
    if (!point)
        return;

    static const CONNECT_DIRECTION kDirections[] = {
        TOP_DIRECTION,
        RIGHT_DIRECTION,
        BOTTOM_DIRECTION,
        LEFT_DIRECTION,
    };
    static const QPointF kOffsets[] = {
        QPointF(0.0, -0.5),
        QPointF(0.5, 0.0),
        QPointF(0.0, 0.5),
        QPointF(-0.5, 0.0),
    };

    const int safeIndex = qBound(0, index, 3);
    point->setDir(safeIndex);
    point->setDirection(kDirections[safeIndex]);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<LzPowerJunctionItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerJunctionItem, wsymjunction)
