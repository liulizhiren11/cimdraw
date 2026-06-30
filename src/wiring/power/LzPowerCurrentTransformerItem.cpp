#include "LzPowerCurrentTransformerItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerCurrentTransformerItem::drawSymbol(QPainter* painter,
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
    const qreal radius = qMin(rect.width(), rect.height()) * 0.28;
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius, center.y()));
    painter->drawLine(QLineF(center.x() + radius, center.y(), rect.right(), center.y()));
    painter->drawEllipse(center, radius, radius);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(0, 0, 0, 1), Qt::AlignCenter, QStringLiteral("CT"));
}

LzPowerCurrentTransformerItem::LzPowerCurrentTransformerItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::CurrentTransformer, parent)
{
}

LzPowerCurrentTransformerItem::LzPowerCurrentTransformerItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::CurrentTransformer, pos, parent)
{
}

QString LzPowerCurrentTransformerItem::className()
{
    return QStringLiteral("LzPowerCurrentTransformerItem");
}

QString LzPowerCurrentTransformerItem::shapeName() const
{
    return QStringLiteral("wsymcurrenttransformer");
}

LzDrawTypeId LzPowerCurrentTransformerItem::drawTypeForXml() const
{
    return LZ_WSYM_CURRENT_TRANSFORMER;
}

LzWiringItemBase* LzPowerCurrentTransformerItem::cloneForDuplicate() const
{
    return new LzPowerCurrentTransformerItem(rect());
}

void LzPowerCurrentTransformerItem::paint(QPainter* painter,
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

REGISTER_OBJECT_CREATOR(LzPowerCurrentTransformerItem, wsymcurrenttransformer)
