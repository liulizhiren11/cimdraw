#include "CimdrawPowerEarthSwitchItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QtMath>

void CimdrawPowerEarthSwitchItem::drawSymbol(QPainter* painter,
                                        const QRectF& rect,
                                        CimdrawWiringRunState runState,
                                        bool alarmPulse,
                                        CimdrawWiringSymbolStandard symStd,
                                        bool switchClosed)
{
    Q_UNUSED(symStd);

    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal cy = rect.center().y();
    const qreal cx = rect.center().x();
    const qreal pivotX = cx - qMin(rect.width() * 0.18, 18.0);
    const QPointF pivot(pivotX, cy);
    painter->drawLine(QLineF(rect.left(), cy, pivot.x() - 1, cy));
    painter->drawLine(QLineF(cx + qMin(rect.width() * 0.12, 14.0), cy, rect.right(), cy));
    const qreal yStem = cy + qMin(rect.height() * 0.38, 32.0);
    const QPointF gTop(cx, yStem);
    const qreal yBot = rect.bottom() - qMax(3.0, rect.height() * 0.06);
    const qreal gw = qMin(rect.width() * 0.28, 22.0);
    if (switchClosed)
    {
        painter->drawLine(QLineF(pivot, gTop));
        painter->drawLine(QLineF(gTop, QPointF(cx, yBot)));
    }
    else
    {
        const qreal bladeLen = qMin(rect.width(), rect.height()) * 0.38;
        constexpr qreal openDeg = 52.0;
        const qreal a = qDegreesToRadians(openDeg);
        const QPointF bladeTip(pivot.x() + bladeLen * qCos(a), pivot.y() - bladeLen * qSin(a));
        painter->drawLine(QLineF(pivot, bladeTip));
    }
    painter->drawLine(QLineF(cx - gw * 0.5, yBot - 2, cx + gw * 0.5, yBot - 2));
    painter->drawLine(QLineF(cx - gw * 0.35, yBot - 6, cx + gw * 0.35, yBot - 6));
    painter->drawLine(QLineF(cx - gw * 0.2, yBot - 10, cx + gw * 0.2, yBot - 10));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("接地刀"));
}

CimdrawPowerEarthSwitchItem::CimdrawPowerEarthSwitchItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::EarthSwitch, parent)
{
}

CimdrawPowerEarthSwitchItem::CimdrawPowerEarthSwitchItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::EarthSwitch, pos, parent)
{
}

QString CimdrawPowerEarthSwitchItem::className()
{
    return QStringLiteral("CimdrawPowerEarthSwitchItem");
}

QString CimdrawPowerEarthSwitchItem::shapeName() const
{
    return QStringLiteral("wsyearthswitch");
}

CimdrawDrawTypeId CimdrawPowerEarthSwitchItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_EARTH_SWITCH;
}

CimdrawWiringItemBase* CimdrawPowerEarthSwitchItem::cloneForDuplicate() const
{
    return new CimdrawPowerEarthSwitchItem(rect());
}

void CimdrawPowerEarthSwitchItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, wiringSymbolStandard(), cimGraphicRenderStateSwitchClosed(visualState));
}

REGISTER_OBJECT_CREATOR(CimdrawPowerEarthSwitchItem, wsyearthswitch)
