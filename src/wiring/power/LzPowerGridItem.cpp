#include "LzPowerGridItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

namespace {

void drawGridArrowHead(QPainter* painter, const QPointF& tip, const QPointF& dirIn, qreal len)
{
    const QPointF unit = dirIn / qMax(1e-6, QLineF(QPointF(), dirIn).length());
    const QPointF normal(-unit.y(), unit.x());
    QPolygonF triangle;
    triangle << tip << tip - unit * len + normal * (len * 0.45) << tip - unit * len - normal * (len * 0.45);
    painter->drawPolygon(triangle);
}

void drawGridBase(QPainter* painter, const QRectF& rect, LzWiringRunState runState, bool alarmPulse, int flowSign)
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
        drawGridArrowHead(painter, QPointF(rect.right() - 2, centerY), QPointF(1, 0), 11);
    }
    else
    {
        painter->drawLine(QLineF(rect.right() - inset, centerY, rect.left() + inset + gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawGridArrowHead(painter, QPointF(rect.left() + 2, centerY), QPointF(-1, 0), 11);
    }
}

} // namespace

void LzPowerGridItem::drawSymbol(QPainter* painter,
                                 const QRectF& rect,
                                 LzWiringRunState runState,
                                 bool alarmPulse,
                                 int flowSign)
{
    if (!painter)
        return;

    drawGridBase(painter, rect, runState, alarmPulse, flowSign);

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.26, 11.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(4, 0, -4, -4), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("系统"));
}

LzPowerGridItem::LzPowerGridItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Grid, parent)
{
}

LzPowerGridItem::LzPowerGridItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Grid, pos, parent)
{
}

QString LzPowerGridItem::className()
{
    return QStringLiteral("LzPowerGridItem");
}

QString LzPowerGridItem::shapeName() const
{
    return QStringLiteral("wsymgrid");
}

LzDrawTypeId LzPowerGridItem::drawTypeForXml() const
{
    return LZ_WSYM_GRID;
}

LzWiringItemBase* LzPowerGridItem::cloneForDuplicate() const
{
    return new LzPowerGridItem(rect());
}

void LzPowerGridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, flowSign());
}

REGISTER_OBJECT_CREATOR(LzPowerGridItem, wsymgrid)
