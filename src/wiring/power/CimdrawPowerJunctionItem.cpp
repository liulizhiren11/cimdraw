#include "CimdrawPowerJunctionItem.h"

#include "Item/CimdrawConnectPoint.h"
#include "Item/CimdrawObjectFactory.h"
#include "wiring/base/CimdrawWiringDrawPalette.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerJunctionItem::drawSymbol(QPainter* painter,
                                     const QRectF& rect,
                                     CimdrawWiringRunState runState,
                                     bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    const qreal penWidth = cimGraphicAlarmPenWidth(runState, alarmPulse);
    painter->setPen(QPen(stroke, penWidth, Qt::SolidLine, Qt::RoundCap));
    painter->setBrush(stroke);

    const QPointF center = rect.center();
    const qreal radius = qBound<qreal>(2.5, qMin(rect.width(), rect.height()) * 0.12, 4.5);
    const qreal lead = qMax<qreal>(radius + 2.0, qMin(rect.width(), rect.height()) * 0.34);

    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - lead, center.y()));
    painter->drawLine(QLineF(center.x() + lead, center.y(), rect.right(), center.y()));
    painter->drawLine(QLineF(center.x(), rect.top(), center.x(), center.y() - lead));
    painter->drawLine(QLineF(center.x(), center.y() + lead, center.x(), rect.bottom()));
    painter->drawEllipse(center, radius, radius);
}

CimdrawPowerJunctionItem::CimdrawPowerJunctionItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Junction, parent)
{
}

CimdrawPowerJunctionItem::CimdrawPowerJunctionItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Junction, pos, parent)
{
}

QString CimdrawPowerJunctionItem::className()
{
    return QStringLiteral("CimdrawPowerJunctionItem");
}

QString CimdrawPowerJunctionItem::shapeName() const
{
    return QStringLiteral("wsymjunction");
}

CimdrawDrawTypeId CimdrawPowerJunctionItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_JUNCTION;
}

CimdrawWiringItemBase* CimdrawPowerJunctionItem::cloneForDuplicate() const
{
    return new CimdrawPowerJunctionItem(rect());
}

void CimdrawPowerJunctionItem::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

int CimdrawPowerJunctionItem::defaultWiringConnectPointCount() const
{
    return 4;
}

void CimdrawPowerJunctionItem::configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const
{
    if (!point)
        return;

    static const CONNECT_DIRECTION kDirections[] = {
        TOP_DIRECTION,
        RIGHT_DIRECTION,
        BOTTOM_DIRECTION,
        LEFT_DIRECTION,
    };
    static const QPointF kOffsets[] = {
        QPointF(0.0, -0.5),
        QPointF(0.5, 0.0),
        QPointF(0.0, 0.5),
        QPointF(-0.5, 0.0),
    };

    const int safeIndex = qBound(0, index, 3);
    point->setDir(safeIndex);
    point->setDirection(kDirections[safeIndex]);
    point->setConnectionLeadOut(0.0);
    point->bindToItem(const_cast<CimdrawPowerJunctionItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerJunctionItem, wsymjunction)
