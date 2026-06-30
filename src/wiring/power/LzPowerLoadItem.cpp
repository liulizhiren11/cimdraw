#include "LzPowerLoadItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void LzPowerLoadItem::drawSymbol(QPainter* painter,
                                 const QRectF& rect,
                                 LzWiringRunState runState,
                                 bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    const qreal centerY = rect.center().y();
    const qreal x0 = rect.left();
    const qreal x1 = rect.right();
    const qreal height = qMin(rect.height() * 0.35, 18.0);
    QPolygonF zigzag;
    zigzag << QPointF(x0, centerY)
           << QPointF(x0 + (x1 - x0) * 0.25, centerY - height)
           << QPointF(x0 + (x1 - x0) * 0.5, centerY + height)
           << QPointF(x0 + (x1 - x0) * 0.75, centerY - height)
           << QPointF(x1, centerY);
    painter->drawPolyline(zigzag);

    QFont font = painter->font();
    font.setPointSizeF(qBound(7.0, rect.height() * 0.22, 11.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("Load"));
}

LzPowerLoadItem::LzPowerLoadItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Load, parent)
{
}

LzPowerLoadItem::LzPowerLoadItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Load, pos, parent)
{
}

QString LzPowerLoadItem::className()
{
    return QStringLiteral("LzPowerLoadItem");
}

QString LzPowerLoadItem::shapeName() const
{
    return QStringLiteral("sldload");
}

LzDrawTypeId LzPowerLoadItem::drawTypeForXml() const
{
    return LZ_SLD_LOAD;
}

LzWiringItemBase* LzPowerLoadItem::cloneForDuplicate() const
{
    return new LzPowerLoadItem(rect());
}

void LzPowerLoadItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

REGISTER_OBJECT_CREATOR(LzPowerLoadItem, sldload)
