#include "CimdrawPowerStationXfmrItem.h"

#include "Item/CimdrawConnectPoint.h"
#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerStationXfmrItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal rad = qMin(rect.height(), rect.width() * 0.42) * 0.30;
    const QPointF c1(rect.left() + rect.width() * 0.28, rect.center().y());
    const QPointF c2(rect.right() - rect.width() * 0.28, rect.center().y());
    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, c1.x() - rad, cy));
    painter->drawLine(QLineF(c2.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c1, rad, rad);
    painter->drawEllipse(c2, rad, rad);
    painter->drawLine(QLineF(c1.x() + rad * 0.85, c1.y(), c2.x() - rad * 0.85, c2.y()));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.2, 10.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("站用变"));
}

CimdrawPowerStationXfmrItem::CimdrawPowerStationXfmrItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::StationXfmr, parent)
{
}

CimdrawPowerStationXfmrItem::CimdrawPowerStationXfmrItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::StationXfmr, pos, parent)
{
}

QString CimdrawPowerStationXfmrItem::className()
{
    return QStringLiteral("CimdrawPowerStationXfmrItem");
}

QString CimdrawPowerStationXfmrItem::shapeName() const
{
    return QStringLiteral("wsymstationxfmr");
}

CimdrawDrawTypeId CimdrawPowerStationXfmrItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_STATION_XFMR;
}

CimdrawWiringItemBase* CimdrawPowerStationXfmrItem::cloneForDuplicate() const
{
    return new CimdrawPowerStationXfmrItem(rect());
}

void CimdrawPowerStationXfmrItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

int CimdrawPowerStationXfmrItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void CimdrawPowerStationXfmrItem::configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const
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
    point->bindToItem(const_cast<CimdrawPowerStationXfmrItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerStationXfmrItem, wsymstationxfmr)
