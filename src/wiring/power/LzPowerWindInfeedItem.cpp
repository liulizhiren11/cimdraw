#include "LzPowerWindInfeedItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>
#include <QtMath>

namespace {

void drawWindArrowHead(QPainter* painter, const QPointF& tip, const QPointF& dirIn, qreal len)
{
    const QPointF unit = dirIn / qMax(1e-6, QLineF(QPointF(), dirIn).length());
    const QPointF normal(-unit.y(), unit.x());
    QPolygonF triangle;
    triangle << tip << tip - unit * len + normal * (len * 0.45) << tip - unit * len - normal * (len * 0.45);
    painter->drawPolygon(triangle);
}

void drawWindBase(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign)
{
    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const bool leftToRight = flowSign >= 0;
    constexpr qreal inset = 8.0;
    constexpr qreal gap = 14.0;
    if (leftToRight)
    {
        painter->drawLine(QLineF(rect.left() + inset, centerY, rect.right() - inset - gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawWindArrowHead(painter, QPointF(rect.right() - 2, centerY), QPointF(1, 0), 11);
    }
    else
    {
        painter->drawLine(QLineF(rect.right() - inset, centerY, rect.left() + inset + gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawWindArrowHead(painter, QPointF(rect.left() + 2, centerY), QPointF(-1, 0), 11);
    }
}

} // namespace

void LzPowerWindInfeedItem::drawSymbol(QPainter* painter,
                                       const QRectF& rect,
                                       LzWiringRunState runState,
                                       bool alarmPulse,
                                       int flowSign)
{
    if (!painter)
        return;

    drawWindBase(painter, rect, runState, alarmPulse, flowSign);

    const qreal centerY = rect.center().y();
    const bool leftToRight = flowSign >= 0;
    const QPointF hub(leftToRight ? rect.left() + qMin(rect.width() * 0.12, 22.0)
                                  : rect.right() - qMin(rect.width() * 0.12, 22.0),
                      centerY - qMin(rect.height() * 0.2, 11.0));
    const qreal blade = qMin(rect.height(), rect.width()) * 0.14;
    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    const QColor wind = (runState == LzWiringRunState::Alarm) ? stroke : LzWiringDrawPalette::windAccent();
    painter->setPen(QPen(wind, 2, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 3; ++i)
    {
        const qreal angle = qDegreesToRadians(-90.0 + 120.0 * i);
        const QPointF delta(qCos(angle), qSin(angle));
        painter->drawLine(QLineF(hub, hub + delta * blade));
    }
    painter->setBrush(wind);
    painter->setPen(QPen(wind, 1));
    painter->drawEllipse(hub, 2.2, 2.2);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.26, 11.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(4, 0, -4, -4), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("风电"));
}

LzPowerWindInfeedItem::LzPowerWindInfeedItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::WindInfeed, parent)
{
}

LzPowerWindInfeedItem::LzPowerWindInfeedItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::WindInfeed, pos, parent)
{
}

QString LzPowerWindInfeedItem::className()
{
    return QStringLiteral("LzPowerWindInfeedItem");
}

QString LzPowerWindInfeedItem::shapeName() const
{
    return QStringLiteral("wsymwindinfeed");
}

LzDrawTypeId LzPowerWindInfeedItem::drawTypeForXml() const
{
    return LZ_WSYM_WIND_INFEED;
}

LzWiringItemBase* LzPowerWindInfeedItem::cloneForDuplicate() const
{
    return new LzPowerWindInfeedItem(rect());
}

void LzPowerWindInfeedItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, flowSign());
}

REGISTER_OBJECT_CREATOR(LzPowerWindInfeedItem, wsymwindinfeed)
