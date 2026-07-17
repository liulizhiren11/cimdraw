#include "CimdrawPowerDisconnectorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QtMath>

void CimdrawPowerDisconnectorItem::drawSymbol(QPainter* painter,
                                         const QRectF& rect,
                                         CimdrawWiringRunState runState,
                                         bool alarmPulse,
                                         bool switchClosed)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal yBus = rect.center().y();
    const QPointF pivot(rect.center().x(), yBus);
    const qreal halfGap = qMax(14.0, rect.width() * 0.09);
    const QPointF contactLeft(pivot.x() - halfGap, yBus);
    const QPointF contactRight(pivot.x() + halfGap, yBus);
    painter->drawLine(QLineF(rect.left(), yBus, contactLeft.x(), yBus));
    painter->drawLine(QLineF(contactRight.x(), yBus, rect.right(), yBus));
    if (switchClosed)
    {
        QPen bladePen(stroke, qMax(2.5, pen.widthF() + 0.5));
        bladePen.setCapStyle(Qt::FlatCap);
        painter->setPen(bladePen);
        painter->drawLine(QLineF(contactLeft, contactRight));
        painter->setPen(pen);
    }
    else
    {
        const qreal bladeLen = halfGap * 1.45;
        const qreal a = qDegreesToRadians(50.0);
        const QPointF bladeTip(pivot.x() + bladeLen * qCos(a), pivot.y() - bladeLen * qSin(a));
        QPen bladePen(stroke, qMax(2.0, pen.widthF()));
        bladePen.setCapStyle(Qt::RoundCap);
        painter->setPen(bladePen);
        painter->drawLine(QLineF(pivot, bladeTip));
        painter->setPen(pen);
    }
    painter->drawEllipse(contactLeft, 2.5, 2.5);
    painter->drawEllipse(contactRight, 2.5, 2.5);
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.22, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("QS"));
}

CimdrawPowerDisconnectorItem::CimdrawPowerDisconnectorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Disconnector, parent)
{
}

CimdrawPowerDisconnectorItem::CimdrawPowerDisconnectorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Disconnector, pos, parent)
{
}

QString CimdrawPowerDisconnectorItem::className()
{
    return QStringLiteral("CimdrawPowerDisconnectorItem");
}

QString CimdrawPowerDisconnectorItem::shapeName() const
{
    return QStringLiteral("disconnector");
}

CimdrawDrawTypeId CimdrawPowerDisconnectorItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_DISCONNECTOR;
}

CimdrawWiringItemBase* CimdrawPowerDisconnectorItem::cloneForDuplicate() const
{
    return new CimdrawPowerDisconnectorItem(rect());
}

void CimdrawPowerDisconnectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, cimGraphicRenderStateSwitchClosed(visualState));
}

REGISTER_OBJECT_CREATOR(CimdrawPowerDisconnectorItem, disconnector)

QPainterPath CimdrawPowerDisconnectorItem::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    const QRectF r = rect().normalized();
    const qreal yBus = r.center().y();
    const qreal halfGap = qMax(14.0, r.width() * 0.09);
    path.addRect(QRectF(r.left(), yBus - 6.0, r.width() * 0.35, 12.0));
    path.addRect(QRectF(r.right() - r.width() * 0.35, yBus - 6.0, r.width() * 0.35, 12.0));
    path.addEllipse(r.center(), halfGap, 8.0);
    return path;
}
