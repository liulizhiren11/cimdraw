#include "LzPowerGeneratorItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void LzPowerGeneratorItem::drawSymbol(QPainter* painter,
                                      const QRectF& rect,
                                      LzWiringRunState runState,
                                      bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    painter->setPen(pen);
    painter->setBrush(LzWiringDrawPalette::generatorFill());
    const qreal radius = qMin(rect.width(), rect.height()) * 0.38;
    const QPointF center = rect.center();
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius, center.y()));
    painter->drawLine(QLineF(center.x() + radius, center.y(), rect.right(), center.y()));
    painter->drawEllipse(center, radius, radius);
    painter->setBrush(Qt::NoBrush);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(10.0, radius * 1.1, 22.0));
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QStringLiteral("G"));
}

LzPowerGeneratorItem::LzPowerGeneratorItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Generator, parent)
{
}

LzPowerGeneratorItem::LzPowerGeneratorItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Generator, pos, parent)
{
}

QString LzPowerGeneratorItem::className()
{
    return QStringLiteral("LzPowerGeneratorItem");
}

QString LzPowerGeneratorItem::shapeName() const
{
    return QStringLiteral("generator");
}

LzDrawTypeId LzPowerGeneratorItem::drawTypeForXml() const
{
    return LZ_SLD_GENERATOR;
}

LzWiringItemBase* LzPowerGeneratorItem::cloneForDuplicate() const
{
    return new LzPowerGeneratorItem(rect());
}

void LzPowerGeneratorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse);
}

REGISTER_OBJECT_CREATOR(LzPowerGeneratorItem, generator)
