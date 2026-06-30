#include "LzPowerCapacitorItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

namespace {

void drawPassiveLabels(QPainter* painter,
                       const QRectF& rect,
                       const QString& valueText,
                       const QString& referenceDesignator,
                       const QString& fallbackBottomLabel)
{
    if (!painter)
        return;

    const QString value = valueText.trimmed();
    const QString refdes = referenceDesignator.trimmed();
    const QString bottom = refdes.isEmpty() ? fallbackBottomLabel : refdes;

    QFont font = painter->font();
    if (!value.isEmpty())
    {
        font.setBold(false);
        font.setPointSizeF(qBound(5.8, rect.height() * 0.18, 9.0));
        painter->setFont(font);
        painter->drawText(QRectF(rect.left() + 2.0, rect.top(), rect.width() - 4.0, rect.height() * 0.18),
                          Qt::AlignHCenter | Qt::AlignTop,
                          value);
    }

    if (!bottom.isEmpty())
    {
        font.setBold(refdes.isEmpty());
        font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
        painter->setFont(font);
        painter->drawText(QRectF(rect.left() + 2.0,
                                 rect.bottom() - rect.height() * 0.20,
                                 rect.width() - 4.0,
                                 rect.height() * 0.18),
                          Qt::AlignHCenter | Qt::AlignTop,
                          bottom);
    }
}

} // namespace

void LzPowerCapacitorItem::drawSymbol(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const QColor plate = LzWiringDrawPalette::capacitorPlate(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, rect.right(), cy));
    const qreal d = qMax(5.0, rect.width() * 0.11);
    const qreal hh = qMin(rect.height() * 0.4, 22.0);
    painter->setPen(QPen(plate, pen.width()));
    painter->drawLine(QLineF(rect.center().x() - d, cy - hh * 0.5, rect.center().x() - d, cy + hh * 0.5));
    painter->drawLine(QLineF(rect.center().x() + d, cy - hh * 0.5, rect.center().x() + d, cy + hh * 0.5));
    painter->setPen(QPen(stroke, 1));
}

LzPowerCapacitorItem::LzPowerCapacitorItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Capacitor, parent)
{
}

LzPowerCapacitorItem::LzPowerCapacitorItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Capacitor, pos, parent)
{
    finalizeWiringConstruction();
}

QString LzPowerCapacitorItem::className()
{
    return QStringLiteral("LzPowerCapacitorItem");
}

QString LzPowerCapacitorItem::shapeName() const
{
    return QStringLiteral("wsymcapacitor");
}

LzDrawTypeId LzPowerCapacitorItem::drawTypeForXml() const
{
    return LZ_WSYM_CAPACITOR;
}

LzWiringItemBase* LzPowerCapacitorItem::cloneForDuplicate() const
{
    return new LzPowerCapacitorItem(rect());
}

QString LzPowerCapacitorItem::displayName() const
{
    if (!displayName_.isEmpty())
        return displayName_;
    return QStringLiteral("电容");
}

void LzPowerCapacitorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
    drawPassiveLabels(painter, rect(), valueText_, referenceDesignator_, QStringLiteral("C"));
}

REGISTER_OBJECT_CREATOR(LzPowerCapacitorItem, wsymcapacitor)
