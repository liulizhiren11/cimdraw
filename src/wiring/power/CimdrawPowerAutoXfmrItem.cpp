#include "CimdrawPowerAutoXfmrItem.h"

#include "Item/CimdrawConnectPoint.h"
#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerAutoXfmrItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal cy = rect.center().y();
    const qreal rad = qMin(rect.height(), rect.width() * 0.38) * 0.38;
    const QPointF c(rect.center().x(), cy);
    painter->drawLine(QLineF(rect.left(), cy, c.x() - rad, cy));
    painter->drawLine(QLineF(c.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c, rad, rad);
    QPen dash(stroke, 1, Qt::DashLine);
    painter->setPen(dash);
    painter->drawEllipse(c, rad * 0.52, rad * 0.52);
    painter->setPen(pen);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("自耦"));
}

CimdrawPowerAutoXfmrItem::CimdrawPowerAutoXfmrItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::AutoXfmr, parent)
{
}

CimdrawPowerAutoXfmrItem::CimdrawPowerAutoXfmrItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::AutoXfmr, pos, parent)
{
}

QString CimdrawPowerAutoXfmrItem::className()
{
    return QStringLiteral("CimdrawPowerAutoXfmrItem");
}

QString CimdrawPowerAutoXfmrItem::shapeName() const
{
    return QStringLiteral("wsymautoxfmr");
}

CimdrawDrawTypeId CimdrawPowerAutoXfmrItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_AUTO_XFMR;
}

CimdrawWiringItemBase* CimdrawPowerAutoXfmrItem::cloneForDuplicate() const
{
    return new CimdrawPowerAutoXfmrItem(rect());
}

void CimdrawPowerAutoXfmrItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

int CimdrawPowerAutoXfmrItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void CimdrawPowerAutoXfmrItem::configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const
{
    if (!point)
        return;

    static const CONNECT_DIRECTION kDirections[] = {
        LEFT_DIRECTION,
        RIGHT_DIRECTION,
    };
    static const QPointF kOffsets[] = {
        QPointF(-0.5, 0.0),
        QPointF(0.5, 0.0),
    };

    const int safeIndex = qBound(0, index, 1);
    point->setDir(safeIndex);
    point->setDirection(kDirections[safeIndex]);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<CimdrawPowerAutoXfmrItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerAutoXfmrItem, wsymautoxfmr)
