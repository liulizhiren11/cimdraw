#include "LzPowerTransformer2wItem.h"

#include "Item/LzConnectPoint.h"
#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerTransformer2wItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal rad = qMin(rect.height(), rect.width() * 0.42) * 0.38;
    const QPointF c1(rect.left() + rect.width() * 0.28, rect.center().y());
    const QPointF c2(rect.right() - rect.width() * 0.28, rect.center().y());
    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, c1.x() - rad, cy));
    painter->drawLine(QLineF(c2.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c1, rad, rad);
    painter->drawEllipse(c2, rad, rad);
    painter->drawLine(QLineF(c1.x() + rad * 0.85, c1.y(), c2.x() - rad * 0.85, c2.y()));
    QFont font = painter->font();
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->drawText(QRectF(rect.left(), rect.top(), rect.width() * 0.45, rect.height() * 0.35), Qt::AlignCenter, QStringLiteral("HV"));
    painter->drawText(QRectF(rect.right() - rect.width() * 0.45, rect.top(), rect.width() * 0.45, rect.height() * 0.35), Qt::AlignCenter, QStringLiteral("LV"));
}

LzPowerTransformer2wItem::LzPowerTransformer2wItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Transformer2W, parent)
{
}

LzPowerTransformer2wItem::LzPowerTransformer2wItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Transformer2W, pos, parent)
{
}

QString LzPowerTransformer2wItem::className()
{
    return QStringLiteral("LzPowerTransformer2wItem");
}

QString LzPowerTransformer2wItem::shapeName() const
{
    return QStringLiteral("transformer2w");
}

LzDrawTypeId LzPowerTransformer2wItem::drawTypeForXml() const
{
    return LZ_SLD_TRANSFORMER_2W;
}

LzWiringItemBase* LzPowerTransformer2wItem::cloneForDuplicate() const
{
    return new LzPowerTransformer2wItem(rect());
}

void LzPowerTransformer2wItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

int LzPowerTransformer2wItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void LzPowerTransformer2wItem::configureDefaultWiringConnectPoint(LzConnectPoint* point, int index) const
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
    point->bindToItem(const_cast<LzPowerTransformer2wItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(LzPowerTransformer2wItem, transformer2w)
