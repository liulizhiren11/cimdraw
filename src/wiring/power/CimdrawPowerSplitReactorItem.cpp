#include "CimdrawPowerSplitReactorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void CimdrawPowerSplitReactorItem::drawSymbol(QPainter* painter,
                                         const QRectF& rect,
                                         CimdrawWiringRunState runState,
                                         bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    const QColor coil = CimdrawWiringDrawPalette::reactorCoil(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    constexpr qreal midGap = 10.0;
    const qreal available = rect.width() - qMax(20.0, rect.width() * 0.06) - midGap;
    const qreal coilWidth = available * 0.5;
    const qreal amplitude = qMin(rect.height() * 0.30, 11.0);
    const qreal leftCoilStart = rect.center().x() - midGap * 0.5 - coilWidth;
    const qreal rightCoilStart = rect.center().x() + midGap * 0.5;

    painter->drawLine(QLineF(rect.left(), centerY, leftCoilStart, centerY));
    painter->setPen(QPen(coil, pen.width()));
    for (int i = 0; i < 3; ++i)
    {
        const QRectF arc(leftCoilStart + i * (coilWidth / 3.0), centerY - amplitude, coilWidth / 3.0, amplitude * 2);
        painter->drawArc(arc, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(leftCoilStart + coilWidth, centerY, rightCoilStart, centerY));
    painter->setPen(QPen(coil, pen.width()));
    for (int i = 0; i < 3; ++i)
    {
        const QRectF arc(rightCoilStart + i * (coilWidth / 3.0), centerY - amplitude, coilWidth / 3.0, amplitude * 2);
        painter->drawArc(arc, 0, 180 * 16);
    }

    painter->setPen(pen);
    painter->drawLine(QLineF(rightCoilStart + coilWidth, centerY, rect.right(), centerY));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.22, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("分裂电抗"));
}

CimdrawPowerSplitReactorItem::CimdrawPowerSplitReactorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::SplitReactor, parent)
{
}

CimdrawPowerSplitReactorItem::CimdrawPowerSplitReactorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::SplitReactor, pos, parent)
{
}

QString CimdrawPowerSplitReactorItem::className()
{
    return QStringLiteral("CimdrawPowerSplitReactorItem");
}

QString CimdrawPowerSplitReactorItem::shapeName() const
{
    return QStringLiteral("wsymsplitreactor");
}

CimdrawDrawTypeId CimdrawPowerSplitReactorItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_SPLIT_REACTOR;
}

CimdrawWiringItemBase* CimdrawPowerSplitReactorItem::cloneForDuplicate() const
{
    return new CimdrawPowerSplitReactorItem(rect());
}

void CimdrawPowerSplitReactorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerSplitReactorItem, wsymsplitreactor)
