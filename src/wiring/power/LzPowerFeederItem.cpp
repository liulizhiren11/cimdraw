#include "LzPowerFeederItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QPainter>

namespace {

void drawFeederArrowHead(QPainter* painter, const QPointF& tip, const QPointF& dirIn, qreal len)
{
    const qreal length = qMax(1e-6, QLineF(QPointF(), dirIn).length());
    const QPointF unit = dirIn / length;
    const QPointF normal(-unit.y(), unit.x());
    QPolygonF triangle;
    triangle << tip << tip - unit * len + normal * (len * 0.45) << tip - unit * len - normal * (len * 0.45);
    painter->drawPolygon(triangle);
}

} // namespace

void LzPowerFeederItem::drawSymbol(QPainter* painter,
                                   const QRectF& rect,
                                   LzWiringRunState runState,
                                   bool alarmPulse,
                                   int flowSign)
{
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    pen.setStyle(Qt::DashLine);
    pen.setDashOffset(qreal(QDateTime::currentMSecsSinceEpoch() % 2000) * 0.05);
    painter->setPen(pen);

    const qreal centerY = rect.center().y();
    const bool leftToRight = flowSign >= 0;
    const qreal xStart = leftToRight ? rect.left() : rect.right();
    const qreal xEnd = leftToRight ? (rect.right() - 10) : (rect.left() + 10);
    painter->drawLine(QLineF(xStart, centerY, xEnd, centerY));

    painter->setBrush(stroke);
    painter->setPen(QPen(stroke, 1));
    drawFeederArrowHead(painter,
                        QPointF(leftToRight ? rect.right() : rect.left(), centerY),
                        QPointF(leftToRight ? 1.0 : -1.0, 0.0),
                        12);
    drawFeederArrowHead(painter,
                        QPointF(leftToRight ? rect.left() : rect.right(), centerY),
                        QPointF(leftToRight ? -1.0 : 1.0, 0.0),
                        6);

    QFont font = painter->font();
    font.setPointSizeF(qBound(7.0, rect.height() * 0.32, 11.0));
    painter->setFont(font);
    painter->setPen(stroke);
    const QString label = leftToRight ? QCoreApplication::translate("LzWiringItemBase", "潮流 ->")
                                      : QCoreApplication::translate("LzWiringItemBase", "<- 潮流");
    painter->drawText(rect.adjusted(4, 0, -4, -4), Qt::AlignLeft | Qt::AlignBottom, label);
}

LzPowerFeederItem::LzPowerFeederItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::PowerFeeder, parent)
{
}

LzPowerFeederItem::LzPowerFeederItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::PowerFeeder, pos, parent)
{
}

QString LzPowerFeederItem::className()
{
    return QStringLiteral("LzPowerFeederItem");
}

QString LzPowerFeederItem::shapeName() const
{
    return QStringLiteral("powerfeeder");
}

LzDrawTypeId LzPowerFeederItem::drawTypeForXml() const
{
    return LZ_SLD_POWER_FEEDER;
}

LzWiringItemBase* LzPowerFeederItem::cloneForDuplicate() const
{
    return new LzPowerFeederItem(rect());
}

void LzPowerFeederItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, flowSign());
}

REGISTER_OBJECT_CREATOR(LzPowerFeederItem, powerfeeder)
