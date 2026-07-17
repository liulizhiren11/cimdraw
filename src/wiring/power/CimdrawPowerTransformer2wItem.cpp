#include "CimdrawPowerTransformer2wItem.h"

#include "Item/CimdrawConnectPoint.h"
#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerTransformer2wItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    const qreal rad = qMin(rect.height(), rect.width() * 0.42) * 0.38;
    const QPointF c1(rect.left() + rect.width() * 0.28, rect.center().y());
    const QPointF c2(rect.right() - rect.width() * 0.28, rect.center().y());
    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, c1.x() - rad, cy));
    painter->drawLine(QLineF(c2.x() + rad, cy, rect.right(), cy));
    painter->drawEllipse(c1, rad, rad);
    painter->drawEllipse(c2, rad, rad);
    painter->drawLine(QLineF(c1.x() + rad * 0.85, c1.y(), c2.x() - rad * 0.85, c2.y()));
    QFont font = painter->font();
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->drawText(QRectF(rect.left(), rect.top(), rect.width() * 0.45, rect.height() * 0.35), Qt::AlignCenter, QStringLiteral("HV"));
    painter->drawText(QRectF(rect.right() - rect.width() * 0.45, rect.top(), rect.width() * 0.45, rect.height() * 0.35), Qt::AlignCenter, QStringLiteral("LV"));
}

CimdrawPowerTransformer2wItem::CimdrawPowerTransformer2wItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Transformer2W, parent)
{
}

CimdrawPowerTransformer2wItem::CimdrawPowerTransformer2wItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Transformer2W, pos, parent)
{
}

QString CimdrawPowerTransformer2wItem::className()
{
    return QStringLiteral("CimdrawPowerTransformer2wItem");
}

QString CimdrawPowerTransformer2wItem::shapeName() const
{
    return QStringLiteral("transformer2w");
}

CimdrawDrawTypeId CimdrawPowerTransformer2wItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_TRANSFORMER_2W;
}

CimdrawWiringItemBase* CimdrawPowerTransformer2wItem::cloneForDuplicate() const
{
    return new CimdrawPowerTransformer2wItem(rect());
}

void CimdrawPowerTransformer2wItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

int CimdrawPowerTransformer2wItem::defaultWiringConnectPointCount() const
{
    return 2;
}

void CimdrawPowerTransformer2wItem::configureDefaultWiringConnectPoint(CimdrawConnectPoint* point, int index) const
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
    point->bindToItem(const_cast<CimdrawPowerTransformer2wItem*>(this), kOffsets[safeIndex]);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerTransformer2wItem, transformer2w)
