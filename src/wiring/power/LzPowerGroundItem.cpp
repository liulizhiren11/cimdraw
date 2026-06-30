#include "LzPowerGroundItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerGroundItem::drawSymbol(QPainter* painter,
                                   const QRectF& rect,
                                   LzWiringRunState runState,
                                   bool alarmPulse,
                                   LzWiringSymbolStandard symStd)
{
    Q_UNUSED(symStd);
    if (!painter)
        return;

    const qreal centerX = rect.center().x();
    const qreal h = rect.height();
    const qreal yTop = rect.top() + h * 0.06;
    const qreal y1 = rect.top() + h * 0.34;
    const qreal y2 = rect.top() + h * 0.47;
    const qreal y3 = rect.top() + h * 0.60;
    const qreal w1 = qMin(rect.width() * 0.72, 28.0);
    const qreal w2 = qMin(rect.width() * 0.50, 20.0);
    const qreal w3 = qMin(rect.width() * 0.30, 12.0);
    painter->setPen(QPen(LzWiringDrawPalette::conductorStroke(runState), 2));
    painter->drawLine(QLineF(centerX, yTop, centerX, y1));
    painter->setPen(QPen(LzWiringDrawPalette::earthGround(), 2));
    painter->drawLine(QLineF(centerX - w1 * 0.5, y1, centerX + w1 * 0.5, y1));
    painter->drawLine(QLineF(centerX - w2 * 0.5, y2, centerX + w2 * 0.5, y2));
    painter->drawLine(QLineF(centerX - w3 * 0.5, y3, centerX + w3 * 0.5, y3));
    if (alarmPulse)
    {
        painter->setPen(QPen(LzWiringDrawPalette::conductorStroke(LzWiringRunState::Alarm), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(rect.adjusted(4, 4, -4, -4));
    }
}

LzPowerGroundItem::LzPowerGroundItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Ground, parent)
{
}

LzPowerGroundItem::LzPowerGroundItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Ground, pos, parent)
{
}

QString LzPowerGroundItem::className()
{
    return QStringLiteral("LzPowerGroundItem");
}

QString LzPowerGroundItem::shapeName() const
{
    return QStringLiteral("sldground");
}

LzDrawTypeId LzPowerGroundItem::drawTypeForXml() const
{
    return LZ_SLD_GROUND;
}

LzWiringItemBase* LzPowerGroundItem::cloneForDuplicate() const
{
    return new LzPowerGroundItem(rect());
}

void LzPowerGroundItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(LzPowerGroundItem, sldground)
