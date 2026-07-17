#include "CimdrawPowerCurrentTransformerItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerCurrentTransformerItem::drawSymbol(QPainter* painter,
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
    const qreal radius = qMin(rect.width(), rect.height()) * 0.28;
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius, center.y()));
    painter->drawLine(QLineF(center.x() + radius, center.y(), rect.right(), center.y()));
    painter->drawEllipse(center, radius, radius);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.18, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(0, 0, 0, 1), Qt::AlignCenter, QStringLiteral("CT"));
}

CimdrawPowerCurrentTransformerItem::CimdrawPowerCurrentTransformerItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::CurrentTransformer, parent)
{
}

CimdrawPowerCurrentTransformerItem::CimdrawPowerCurrentTransformerItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::CurrentTransformer, pos, parent)
{
}

QString CimdrawPowerCurrentTransformerItem::className()
{
    return QStringLiteral("CimdrawPowerCurrentTransformerItem");
}

QString CimdrawPowerCurrentTransformerItem::shapeName() const
{
    return QStringLiteral("wsymcurrenttransformer");
}

CimdrawDrawTypeId CimdrawPowerCurrentTransformerItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_CURRENT_TRANSFORMER;
}

CimdrawWiringItemBase* CimdrawPowerCurrentTransformerItem::cloneForDuplicate() const
{
    return new CimdrawPowerCurrentTransformerItem(rect());
}

void CimdrawPowerCurrentTransformerItem::paint(QPainter* painter,
                                          const QStyleOptionGraphicsItem* option,
                                          QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerCurrentTransformerItem, wsymcurrenttransformer)
