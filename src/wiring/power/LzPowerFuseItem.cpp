#include "LzPowerFuseItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerFuseItem::drawSymbol(QPainter* painter,
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
    const qreal bodyW = qMin(rect.width() * 0.34, 28.0);
    const qreal bodyH = qMin(rect.height() * 0.55, 18.0);
    const QRectF body(rect.center().x() - bodyW * 0.5, centerY - bodyH * 0.5, bodyW, bodyH);
    painter->drawLine(QLineF(rect.left(), centerY, body.left(), centerY));
    painter->drawRect(body);
    painter->drawLine(QLineF(body.right(), centerY, rect.right(), centerY));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.2, rect.height() * 0.28, 10.0));
    painter->setFont(font);
    painter->drawText(body, Qt::AlignCenter, QStringLiteral("F"));
}

LzPowerFuseItem::LzPowerFuseItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Fuse, parent)
{
}

LzPowerFuseItem::LzPowerFuseItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Fuse, pos, parent)
{
}

QString LzPowerFuseItem::className()
{
    return QStringLiteral("LzPowerFuseItem");
}

QString LzPowerFuseItem::shapeName() const
{
    return QStringLiteral("wsymfuse");
}

LzDrawTypeId LzPowerFuseItem::drawTypeForXml() const
{
    return LZ_WSYM_FUSE;
}

LzWiringItemBase* LzPowerFuseItem::cloneForDuplicate() const
{
    return new LzPowerFuseItem(rect());
}

void LzPowerFuseItem::paint(QPainter* painter,
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

REGISTER_OBJECT_CREATOR(LzPowerFuseItem, wsymfuse)
