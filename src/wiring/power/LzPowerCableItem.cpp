#include "LzPowerCableItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

void LzPowerCableItem::drawSymbol(QPainter* painter,
                                  const QRectF& rect,
                                  LzWiringRunState runState,
                                  bool alarmPulse,
                                  LzWiringSymbolStandard symStd)
{
    Q_UNUSED(symStd);
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal leftX = rect.left();
    const qreal rightX = rect.right();
    const qreal width = rightX - leftX;
    const qreal x0 = leftX + width * 0.22;
    const qreal x1 = rightX - width * 0.22;
    const qreal amp = qMin(rect.height() * 0.22, 8.0);
    painter->drawLine(QLineF(leftX, centerY, x0, centerY));
    QPolygonF zig;
    const int count = 6;
    zig << QPointF(x0, centerY);
    for (int i = 1; i < count; ++i)
    {
        const qreal t = qreal(i) / qreal(count);
        const qreal x = x0 + (x1 - x0) * t;
        const qreal y = centerY + ((i % 2) == 1 ? -amp : amp);
        zig << QPointF(x, y);
    }
    zig << QPointF(x1, centerY);
    QPen zigPen(LzWiringDrawPalette::cableZigAccent(runState), pen.width());
    zigPen.setCapStyle(pen.capStyle());
    painter->setPen(zigPen);
    painter->drawPolyline(zig);
    painter->setPen(pen);
    painter->drawLine(QLineF(x1, centerY, rightX, centerY));
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.26, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("电缆"));
}

LzPowerCableItem::LzPowerCableItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Cable, parent)
{
}

LzPowerCableItem::LzPowerCableItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Cable, pos, parent)
{
}

QString LzPowerCableItem::className()
{
    return QStringLiteral("LzPowerCableItem");
}

QString LzPowerCableItem::shapeName() const
{
    return QStringLiteral("wsymcable");
}

LzDrawTypeId LzPowerCableItem::drawTypeForXml() const
{
    return LZ_WSYM_CABLE;
}

LzWiringItemBase* LzPowerCableItem::cloneForDuplicate() const
{
    return new LzPowerCableItem(rect());
}

void LzPowerCableItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(LzPowerCableItem, wsymcable)
