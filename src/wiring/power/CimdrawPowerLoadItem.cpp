#include "CimdrawPowerLoadItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void CimdrawPowerLoadItem::drawSymbol(QPainter* painter,
                                 const QRectF& rect,
                                 CimdrawWiringRunState runState,
                                 bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    const qreal centerY = rect.center().y();
    const qreal x0 = rect.left();
    const qreal x1 = rect.right();
    const qreal height = qMin(rect.height() * 0.35, 18.0);
    QPolygonF zigzag;
    zigzag << QPointF(x0, centerY)
           << QPointF(x0 + (x1 - x0) * 0.25, centerY - height)
           << QPointF(x0 + (x1 - x0) * 0.5, centerY + height)
           << QPointF(x0 + (x1 - x0) * 0.75, centerY - height)
           << QPointF(x1, centerY);
    painter->drawPolyline(zigzag);

    QFont font = painter->font();
    font.setPointSizeF(qBound(7.0, rect.height() * 0.22, 11.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("Load"));
}

CimdrawPowerLoadItem::CimdrawPowerLoadItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Load, parent)
{
}

CimdrawPowerLoadItem::CimdrawPowerLoadItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Load, pos, parent)
{
}

QString CimdrawPowerLoadItem::className()
{
    return QStringLiteral("CimdrawPowerLoadItem");
}

QString CimdrawPowerLoadItem::shapeName() const
{
    return QStringLiteral("sldload");
}

CimdrawDrawTypeId CimdrawPowerLoadItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_LOAD;
}

CimdrawWiringItemBase* CimdrawPowerLoadItem::cloneForDuplicate() const
{
    return new CimdrawPowerLoadItem(rect());
}

void CimdrawPowerLoadItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerLoadItem, sldload)
