#include "CimdrawPowerMotorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void CimdrawPowerMotorItem::drawSymbol(QPainter* painter,
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

    const qreal radius = qMin(rect.width(), rect.height()) * 0.38;
    const QPointF center = rect.center();
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius, center.y()));
    painter->drawLine(QLineF(center.x() + radius, center.y(), rect.right(), center.y()));
    painter->drawEllipse(center, radius, radius);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(10.0, radius * 1.1, 22.0));
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QStringLiteral("M"));
}

CimdrawPowerMotorItem::CimdrawPowerMotorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Motor, parent)
{
}

CimdrawPowerMotorItem::CimdrawPowerMotorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Motor, pos, parent)
{
}

QString CimdrawPowerMotorItem::className()
{
    return QStringLiteral("CimdrawPowerMotorItem");
}

QString CimdrawPowerMotorItem::shapeName() const
{
    return QStringLiteral("sldmotor");
}

CimdrawDrawTypeId CimdrawPowerMotorItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_MOTOR;
}

CimdrawWiringItemBase* CimdrawPowerMotorItem::cloneForDuplicate() const
{
    return new CimdrawPowerMotorItem(rect());
}

void CimdrawPowerMotorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerMotorItem, sldmotor)
