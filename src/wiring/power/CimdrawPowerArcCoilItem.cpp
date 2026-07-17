#include "CimdrawPowerArcCoilItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerArcCoilItem::drawSymbol(QPainter* painter,
                                    const QRectF& rect,
                                    CimdrawWiringRunState runState,
                                    bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal centerX = rect.center().x();
    const qreal coilW = qMin(rect.width() * 0.42, 48.0);
    const qreal amp = qMin(rect.height() * 0.28, 11.0);
    const qreal x0 = centerX - coilW * 0.5;
    painter->drawLine(QLineF(rect.left(), centerY, x0, centerY));
    const int arcs = 4;
    for (int i = 0; i < arcs; ++i)
    {
        const QRectF ar(x0 + i * (coilW / qreal(arcs)), centerY - amp, coilW / qreal(arcs), amp * 2);
        painter->drawArc(ar, 0, 180 * 16);
    }
    painter->drawLine(QLineF(x0 + coilW, centerY, rect.right(), centerY));
    const qreal groundY = rect.bottom() - qMax(3.0, rect.height() * 0.08);
    painter->drawLine(QLineF(centerX, centerY + amp, centerX, groundY));
    const qreal groundW = qMin(rect.width() * 0.22, 14.0);
    painter->drawLine(QLineF(centerX - groundW * 0.5, groundY, centerX + groundW * 0.5, groundY));
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.2, 9.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("消弧"));
}

CimdrawPowerArcCoilItem::CimdrawPowerArcCoilItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::ArcCoil, parent)
{
}

CimdrawPowerArcCoilItem::CimdrawPowerArcCoilItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::ArcCoil, pos, parent)
{
}

QString CimdrawPowerArcCoilItem::className()
{
    return QStringLiteral("CimdrawPowerArcCoilItem");
}

QString CimdrawPowerArcCoilItem::shapeName() const
{
    return QStringLiteral("wsymarccoil");
}

CimdrawDrawTypeId CimdrawPowerArcCoilItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_ARC_COIL;
}

CimdrawWiringItemBase* CimdrawPowerArcCoilItem::cloneForDuplicate() const
{
    return new CimdrawPowerArcCoilItem(rect());
}

void CimdrawPowerArcCoilItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerArcCoilItem, wsymarccoil)
