#include "LzPowerTransformer3wItem.h"

#include "Item/LzConnectPoint.h"
#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerTransformer3wItem::drawSymbol(QPainter* painter,
                                          const QRectF& rect,
                                          LzWiringRunState runState,
                                          bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal usable = rect.width() - qMax(16.0, rect.width() * 0.08);
    const qreal radius = qBound(5.0, qMin(rect.height() * 0.32, usable / 6.8), 18.0);
    const qreal step = radius * 2.15;
    const QPointF c1(rect.center().x() - step, centerY);
    const QPointF c2(rect.center().x(), centerY);
    const QPointF c3(rect.center().x() + step, centerY);
    painter->drawLine(QLineF(rect.left(), centerY, c1.x() - radius, centerY));
    painter->drawLine(QLineF(c3.x() + radius, centerY, rect.right(), centerY));
    painter->drawEllipse(c1, radius, radius);
    painter->drawEllipse(c2, radius, radius);
    painter->drawEllipse(c3, radius, radius);
    painter->drawLine(QLineF(c1.x() + radius * 0.85, c1.y(), c2.x() - radius * 0.85, c2.y()));
    painter->drawLine(QLineF(c2.x() + radius * 0.85, c2.y(), c3.x() - radius * 0.85, c3.y()));
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.16, 9.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("三绕组"));
}

LzPowerTransformer3wItem::LzPowerTransformer3wItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Transformer3W, parent)
{
}

LzPowerTransformer3wItem::LzPowerTransformer3wItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Transformer3W, pos, parent)
{
}

QString LzPowerTransformer3wItem::className()
{
    return QStringLiteral("LzPowerTransformer3wItem");
}

QString LzPowerTransformer3wItem::shapeName() const
{
    return QStringLiteral("wsymtransformer3w");
}

LzDrawTypeId LzPowerTransformer3wItem::drawTypeForXml() const
{
    return LZ_WSYM_TRANSFORMER_3W;
}

LzWiringItemBase* LzPowerTransformer3wItem::cloneForDuplicate() const
{
    return new LzPowerTransformer3wItem(rect());
}

void LzPowerTransformer3wItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerTransformer3wItem::defaultWiringConnectPointCount() const
{
    return 3;
}

void LzPowerTransformer3wItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
{
    if (!point)
        return;

    static const CONNECT_DIRECTION kDirections[] = {
        LEFT_DIRECTION,
        RIGHT_DIRECTION,
        BOTTOM_DIRECTION,
    };
    static const QPointF kOffsets[] = {
        QPointF(-0.5, 0.0),
        QPointF(0.5, 0.0),
        QPointF(0.0, 0.5),
    };

    const int safeIndex = qBound(0, index, 2);
    point->setDir(safeIndex);
    point->setDirection(kDirections[safeIndex]);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<LzPowerTransformer3wItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerTransformer3wItem, wsymtransformer3w)
