#include "LzPowerVoltageTransformerItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerVoltageTransformerItem::drawSymbol(QPainter* painter,
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

    const QPointF center = rect.center();
    const qreal radius = qMin(rect.width(), rect.height()) * 0.26;
    const qreal gap = qMax<qreal>(6.0, radius * 0.45);
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius - gap * 0.5, center.y()));
    painter->drawLine(QLineF(center.x() + radius + gap * 0.5, center.y(), rect.right(), center.y()));
    painter->drawEllipse(QPointF(center.x() - gap * 0.5, center.y()), radius, radius);
    painter->drawEllipse(QPointF(center.x() + gap * 0.5, center.y()), radius, radius);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.2, rect.height() * 0.16, 9.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(0, 0, 0, 1), Qt::AlignCenter, QStringLiteral("VT"));
}

LzPowerVoltageTransformerItem::LzPowerVoltageTransformerItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::VoltageTransformer, parent)
{
}

LzPowerVoltageTransformerItem::LzPowerVoltageTransformerItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::VoltageTransformer, pos, parent)
{
}

QString LzPowerVoltageTransformerItem::className()
{
    return QStringLiteral("LzPowerVoltageTransformerItem");
}

QString LzPowerVoltageTransformerItem::shapeName() const
{
    return QStringLiteral("wsymvoltagetransformer");
}

LzDrawTypeId LzPowerVoltageTransformerItem::drawTypeForXml() const
{
    return LZ_WSYM_VOLTAGE_TRANSFORMER;
}

LzWiringItemBase* LzPowerVoltageTransformerItem::cloneForDuplicate() const
{
    return new LzPowerVoltageTransformerItem(rect());
}

void LzPowerVoltageTransformerItem::paint(QPainter* painter,
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

REGISTER_OBJECT_CREATOR(LzPowerVoltageTransformerItem, wsymvoltagetransformer)
