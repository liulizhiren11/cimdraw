#include "CimdrawPowerReactorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerReactorItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    const QColor coil = CimdrawWiringDrawPalette::reactorCoil(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal coilW = qMin(rect.width() * 0.48, 52.0);
    const qreal amp = qMin(rect.height() * 0.32, 12.0);
    const qreal x0 = rect.center().x() - coilW * 0.5;
    painter->drawLine(QLineF(rect.left(), centerY, x0, centerY));
    painter->setPen(QPen(coil, pen.width()));

    const int arcs = 4;
    for (int i = 0; i < arcs; ++i)
    {
        const QRectF arcRect(x0 + i * (coilW / qreal(arcs)), centerY - amp, coilW / qreal(arcs), amp * 2);
        painter->drawArc(arcRect, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(x0 + coilW, centerY, rect.right(), centerY));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.26, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("L"));
}

CimdrawPowerReactorItem::CimdrawPowerReactorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Reactor, parent)
{
}

CimdrawPowerReactorItem::CimdrawPowerReactorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Reactor, pos, parent)
{
}

QString CimdrawPowerReactorItem::className()
{
    return QStringLiteral("CimdrawPowerReactorItem");
}

QString CimdrawPowerReactorItem::shapeName() const
{
    return QStringLiteral("wsymreactor");
}

CimdrawDrawTypeId CimdrawPowerReactorItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_REACTOR;
}

CimdrawWiringItemBase* CimdrawPowerReactorItem::cloneForDuplicate() const
{
    return new CimdrawPowerReactorItem(rect());
}

void CimdrawPowerReactorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerReactorItem, wsymreactor)
