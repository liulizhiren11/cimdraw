#include "CimdrawPowerCableItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

void CimdrawPowerCableItem::drawSymbol(QPainter* painter,
                                  const QRectF& rect,
                                  CimdrawWiringRunState runState,
                                  bool alarmPulse,
                                  CimdrawWiringSymbolStandard symStd)
{
    Q_UNUSED(symStd);
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal leftX = rect.left();
    const qreal rightX = rect.right();
    const qreal width = rightX - leftX;
    const qreal x0 = leftX + width * 0.22;
    const qreal x1 = rightX - width * 0.22;
    const qreal amp = qMin(rect.height() * 0.22, 8.0);
    painter->drawLine(QLineF(leftX, centerY, x0, centerY));
    QPolygonF zig;
    const int count = 6;
    zig << QPointF(x0, centerY);
    for (int i = 1; i < count; ++i)
    {
        const qreal t = qreal(i) / qreal(count);
        const qreal x = x0 + (x1 - x0) * t;
        const qreal y = centerY + ((i % 2) == 1 ? -amp : amp);
        zig << QPointF(x, y);
    }
    zig << QPointF(x1, centerY);
    QPen zigPen(CimdrawWiringDrawPalette::cableZigAccent(runState), pen.width());
    zigPen.setCapStyle(pen.capStyle());
    painter->setPen(zigPen);
    painter->drawPolyline(zig);
    painter->setPen(pen);
    painter->drawLine(QLineF(x1, centerY, rightX, centerY));
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.26, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("电缆"));
}

CimdrawPowerCableItem::CimdrawPowerCableItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Cable, parent)
{
}

CimdrawPowerCableItem::CimdrawPowerCableItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Cable, pos, parent)
{
}

QString CimdrawPowerCableItem::className()
{
    return QStringLiteral("CimdrawPowerCableItem");
}

QString CimdrawPowerCableItem::shapeName() const
{
    return QStringLiteral("wsymcable");
}

CimdrawDrawTypeId CimdrawPowerCableItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_CABLE;
}

CimdrawWiringItemBase* CimdrawPowerCableItem::cloneForDuplicate() const
{
    return new CimdrawPowerCableItem(rect());
}

void CimdrawPowerCableItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(CimdrawPowerCableItem, wsymcable)
