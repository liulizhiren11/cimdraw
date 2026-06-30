#include "LzPowerEssItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerEssItem::drawSymbol(QPainter* painter,
                                const QRectF& rect,
                                LzWiringRunState runState,
                                bool alarmPulse,
                                int flowSign)
{
    Q_UNUSED(flowSign);
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    painter->drawLine(QLineF(rect.left(), centerY, rect.right(), centerY));
    const QRectF box(rect.center().x() - qMin(rect.width() * 0.36, 32.0),
                     centerY - qMin(rect.height() * 0.32, 22.0),
                     qMin(rect.width() * 0.72, 64.0),
                     qMin(rect.height() * 0.5, 36.0));
    painter->drawRoundedRect(box, 2.0, 2.0);

    const qreal middleX = box.center().x();
    painter->drawLine(QLineF(middleX, box.top() + 2, middleX, box.bottom() - 2));
    const QPointF plus((box.left() + middleX) * 0.5, box.center().y());
    const QPointF minus((middleX + box.right()) * 0.5, box.center().y());
    constexpr qreal halfSize = 5.0;
    painter->setPen(QPen(LzWiringDrawPalette::essPlus(), 2));
    painter->drawLine(QLineF(plus.x() - halfSize, plus.y(), plus.x() + halfSize, plus.y()));
    painter->drawLine(QLineF(plus.x(), plus.y() - halfSize, plus.x(), plus.y() + halfSize));
    painter->setPen(QPen(LzWiringDrawPalette::essMinus(), 2));
    painter->drawLine(QLineF(minus.x() - halfSize, minus.y(), minus.x() + halfSize, minus.y()));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("储能"));
}

LzPowerEssItem::LzPowerEssItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Ess, parent)
{
}

LzPowerEssItem::LzPowerEssItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Ess, pos, parent)
{
}

QString LzPowerEssItem::className()
{
    return QStringLiteral("LzPowerEssItem");
}

QString LzPowerEssItem::shapeName() const
{
    return QStringLiteral("wsymess");
}

LzDrawTypeId LzPowerEssItem::drawTypeForXml() const
{
    return LZ_WSYM_ESS;
}

LzWiringItemBase* LzPowerEssItem::cloneForDuplicate() const
{
    return new LzPowerEssItem(rect());
}

void LzPowerEssItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, flowSign());
}

REGISTER_OBJECT_CREATOR(LzPowerEssItem, wsymess)
