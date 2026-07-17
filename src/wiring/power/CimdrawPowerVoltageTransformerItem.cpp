#include "CimdrawPowerVoltageTransformerItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerVoltageTransformerItem::drawSymbol(QPainter* painter,
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

    const QPointF center = rect.center();
    const qreal radius = qMin(rect.width(), rect.height()) * 0.26;
    const qreal gap = qMax<qreal>(6.0, radius * 0.45);
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius - gap * 0.5, center.y()));
    painter->drawLine(QLineF(center.x() + radius + gap * 0.5, center.y(), rect.right(), center.y()));
    painter->drawEllipse(QPointF(center.x() - gap * 0.5, center.y()), radius, radius);
    painter->drawEllipse(QPointF(center.x() + gap * 0.5, center.y()), radius, radius);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.2, rect.height() * 0.16, 9.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(0, 0, 0, 1), Qt::AlignCenter, QStringLiteral("VT"));
}

CimdrawPowerVoltageTransformerItem::CimdrawPowerVoltageTransformerItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::VoltageTransformer, parent)
{
}

CimdrawPowerVoltageTransformerItem::CimdrawPowerVoltageTransformerItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::VoltageTransformer, pos, parent)
{
}

QString CimdrawPowerVoltageTransformerItem::className()
{
    return QStringLiteral("CimdrawPowerVoltageTransformerItem");
}

QString CimdrawPowerVoltageTransformerItem::shapeName() const
{
    return QStringLiteral("wsymvoltagetransformer");
}

CimdrawDrawTypeId CimdrawPowerVoltageTransformerItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_VOLTAGE_TRANSFORMER;
}

CimdrawWiringItemBase* CimdrawPowerVoltageTransformerItem::cloneForDuplicate() const
{
    return new CimdrawPowerVoltageTransformerItem(rect());
}

void CimdrawPowerVoltageTransformerItem::paint(QPainter* painter,
                                          const QStyleOptionGraphicsItem* option,
                                          QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerVoltageTransformerItem, wsymvoltagetransformer)
