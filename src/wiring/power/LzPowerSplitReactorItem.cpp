#include "LzPowerSplitReactorItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void LzPowerSplitReactorItem::drawSymbol(QPainter* painter,
                                         const QRectF& rect,
                                         LzWiringRunState runState,
                                         bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const QColor coil = LzWiringDrawPalette::reactorCoil(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    constexpr qreal midGap = 10.0;
    const qreal available = rect.width() - qMax(20.0, rect.width() * 0.06) - midGap;
    const qreal coilWidth = available * 0.5;
    const qreal amplitude = qMin(rect.height() * 0.30, 11.0);
    const qreal leftCoilStart = rect.center().x() - midGap * 0.5 - coilWidth;
    const qreal rightCoilStart = rect.center().x() + midGap * 0.5;

    painter->drawLine(QLineF(rect.left(), centerY, leftCoilStart, centerY));
    painter->setPen(QPen(coil, pen.width()));
    for (int i = 0; i < 3; ++i)
    {
        const QRectF arc(leftCoilStart + i * (coilWidth / 3.0), centerY - amplitude, coilWidth / 3.0, amplitude * 2);
        painter->drawArc(arc, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(leftCoilStart + coilWidth, centerY, rightCoilStart, centerY));
    painter->setPen(QPen(coil, pen.width()));
    for (int i = 0; i < 3; ++i)
    {
        const QRectF arc(rightCoilStart + i * (coilWidth / 3.0), centerY - amplitude, coilWidth / 3.0, amplitude * 2);
        painter->drawArc(arc, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(rightCoilStart + coilWidth, centerY, rect.right(), centerY));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.22, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("分裂电抗"));
}

LzPowerSplitReactorItem::LzPowerSplitReactorItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::SplitReactor, parent)
{
}

LzPowerSplitReactorItem::LzPowerSplitReactorItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::SplitReactor, pos, parent)
{
}

QString LzPowerSplitReactorItem::className()
{
    return QStringLiteral("LzPowerSplitReactorItem");
}

QString LzPowerSplitReactorItem::shapeName() const
{
    return QStringLiteral("wsymsplitreactor");
}

LzDrawTypeId LzPowerSplitReactorItem::drawTypeForXml() const
{
    return LZ_WSYM_SPLIT_REACTOR;
}

LzWiringItemBase* LzPowerSplitReactorItem::cloneForDuplicate() const
{
    return new LzPowerSplitReactorItem(rect());
}

void LzPowerSplitReactorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

REGISTER_OBJECT_CREATOR(LzPowerSplitReactorItem, wsymsplitreactor)
