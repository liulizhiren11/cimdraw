#include "CimdrawPowerLoadBreakItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>

void CimdrawPowerLoadBreakItem::drawSymbol(QPainter* painter,
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
    const qreal bw = qMin(rect.width() * 0.28, 36.0);
    const qreal bh = qMin(rect.height() * 0.5, 28.0);
    const QRectF box(cx - bw * 0.5, cy - bh * 0.5, bw, bh);
    painter->drawLine(QLineF(rect.left(), cy, box.left(), cy));
    painter->drawLine(QLineF(box.right(), cy, rect.right(), cy));
    painter->drawRect(box);
    if (switchClosed)
    {
        painter->drawLine(QLineF(box.left() + 2, cy, box.right() - 2, cy));
    }
    else
    {
        const qreal gap = qMax(4.0, bw * 0.12);
        painter->drawLine(QLineF(box.left() + 2, cy, cx - gap * 0.5, cy));
        painter->drawLine(QLineF(cx + gap * 0.5, cy, box.right() - 2, cy));
    }
    painter->setPen(QPen(stroke, 1));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.22, 10.0));
    painter->setFont(font);
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("Fz"));
}

CimdrawPowerLoadBreakItem::CimdrawPowerLoadBreakItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::LoadBreak, parent)
{
}

CimdrawPowerLoadBreakItem::CimdrawPowerLoadBreakItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::LoadBreak, pos, parent)
{
}

QString CimdrawPowerLoadBreakItem::className()
{
    return QStringLiteral("CimdrawPowerLoadBreakItem");
}

QString CimdrawPowerLoadBreakItem::shapeName() const
{
    return QStringLiteral("wsymloadbreak");
}

CimdrawDrawTypeId CimdrawPowerLoadBreakItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_LOAD_BREAK;
}

CimdrawWiringItemBase* CimdrawPowerLoadBreakItem::cloneForDuplicate() const
{
    return new CimdrawPowerLoadBreakItem(rect());
}

void CimdrawPowerLoadBreakItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, wiringSymbolStandard(), cimGraphicRenderStateSwitchClosed(visualState, true));
}

REGISTER_OBJECT_CREATOR(CimdrawPowerLoadBreakItem, wsymloadbreak)

QPainterPath CimdrawPowerLoadBreakItem::defaultRoutingObstaclePath() const
{
    QPainterPath path;
    const QRectF r = rect().normalized();
    const qreal cy = r.center().y();
    const qreal cx = r.center().x();
    const qreal bw = qMin(r.width() * 0.28, 36.0);
    const qreal bh = qMin(r.height() * 0.5, 28.0);
    const QRectF box(cx - bw * 0.5, cy - bh * 0.5, bw, bh);
    path.addRect(QRectF(r.left(), cy - 2.0, box.left() - r.left(), 4.0));
    path.addRect(QRectF(box.right(), cy - 2.0, r.right() - box.right(), 4.0));
    path.addRect(box);
    return path;
}
