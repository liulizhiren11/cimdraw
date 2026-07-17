#include "CimdrawPowerPvInfeedItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>
#include <QtMath>

namespace {

void drawPvArrowHead(QPainter* painter, const QPointF& tip, const QPointF& dirIn, qreal len)
{
    const QPointF unit = dirIn / qMax(1e-6, QLineF(QPointF(), dirIn).length());
    const QPointF normal(-unit.y(), unit.x());
    QPolygonF triangle;
    triangle << tip << tip - unit * len + normal * (len * 0.45) << tip - unit * len - normal * (len * 0.45);
    painter->drawPolygon(triangle);
}

void drawPvBase(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse, int flowSign)
{
    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const bool leftToRight = cimGraphicFlowLeftToRightFromSign(flowSign);
    constexpr qreal inset = 8.0;
    constexpr qreal gap = 14.0;
    if (leftToRight)
    {
        painter->drawLine(QLineF(rect.left() + inset, centerY, rect.right() - inset - gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawPvArrowHead(painter, QPointF(rect.right() - 2, centerY), QPointF(1, 0), 11);
    }
    else
    {
        painter->drawLine(QLineF(rect.right() - inset, centerY, rect.left() + inset + gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawPvArrowHead(painter, QPointF(rect.left() + 2, centerY), QPointF(-1, 0), 11);
    }
}

} // namespace

void CimdrawPowerPvInfeedItem::drawSymbol(QPainter* painter,
                                     const QRectF& rect,
                                     CimdrawWiringRunState runState,
                                     bool alarmPulse,
                                     int flowSign)
{
    if (!painter)
        return;

    drawPvBase(painter, rect, runState, alarmPulse, flowSign);

    const qreal centerY = rect.center().y();
    const bool leftToRight = cimGraphicFlowLeftToRightFromSign(flowSign);
    const QPointF sunCenter(leftToRight ? rect.left() + qMin(rect.width() * 0.12, 22.0)
                                        : rect.right() - qMin(rect.width() * 0.12, 22.0),
                            centerY - qMin(rect.height() * 0.22, 12.0));
    const qreal sunRadius = qMin(rect.height(), rect.width()) * 0.08;
    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    const QColor sunColor = cimGraphicUsesAlarmAccent(runState) ? stroke : CimdrawWiringDrawPalette::solarAccent();
    painter->setPen(QPen(sunColor, 1));
    painter->setBrush(sunColor);
    painter->drawEllipse(sunCenter, sunRadius, sunRadius);
    for (int i = 0; i < 8; ++i)
    {
        const qreal angle = qDegreesToRadians(45.0 * i);
        const QPointF delta(qCos(angle), qSin(angle));
        painter->drawLine(QLineF(sunCenter + delta * (sunRadius + 2), sunCenter + delta * (sunRadius + 6)));
    }

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.26, 11.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(4, 0, -4, -4), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("光伏"));
}

CimdrawPowerPvInfeedItem::CimdrawPowerPvInfeedItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::PvInfeed, parent)
{
}

CimdrawPowerPvInfeedItem::CimdrawPowerPvInfeedItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::PvInfeed, pos, parent)
{
}

QString CimdrawPowerPvInfeedItem::className()
{
    return QStringLiteral("CimdrawPowerPvInfeedItem");
}

QString CimdrawPowerPvInfeedItem::shapeName() const
{
    return QStringLiteral("wsympvinfeed");
}

CimdrawDrawTypeId CimdrawPowerPvInfeedItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_PV_INFEED;
}

CimdrawWiringItemBase* CimdrawPowerPvInfeedItem::cloneForDuplicate() const
{
    return new CimdrawPowerPvInfeedItem(rect());
}

void CimdrawPowerPvInfeedItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, visualState.flowSign);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerPvInfeedItem, wsympvinfeed)
