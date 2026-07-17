#include "CimdrawPowerGridItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

namespace {

void drawGridArrowHead(QPainter* painter, const QPointF& tip, const QPointF& dirIn, qreal len)
{
    const QPointF unit = dirIn / qMax(1e-6, QLineF(QPointF(), dirIn).length());
    const QPointF normal(-unit.y(), unit.x());
    QPolygonF triangle;
    triangle << tip << tip - unit * len + normal * (len * 0.45) << tip - unit * len - normal * (len * 0.45);
    painter->drawPolygon(triangle);
}

void drawGridBase(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse, int flowSign)
{
    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const bool leftToRight = cimGraphicFlowLeftToRightFromSign(flowSign);
    constexpr qreal inset = 8.0;
    constexpr qreal gap = 14.0;
    if (leftToRight)
    {
        painter->drawLine(QLineF(rect.left() + inset, centerY, rect.right() - inset - gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawGridArrowHead(painter, QPointF(rect.right() - 2, centerY), QPointF(1, 0), 11);
    }
    else
    {
        painter->drawLine(QLineF(rect.right() - inset, centerY, rect.left() + inset + gap, centerY));
        painter->setBrush(stroke);
        painter->setPen(QPen(stroke, 1));
        drawGridArrowHead(painter, QPointF(rect.left() + 2, centerY), QPointF(-1, 0), 11);
    }
}

} // namespace

void CimdrawPowerGridItem::drawSymbol(QPainter* painter,
                                 const QRectF& rect,
                                 CimdrawWiringRunState runState,
                                 bool alarmPulse,
                                 int flowSign)
{
    if (!painter)
        return;

    drawGridBase(painter, rect, runState, alarmPulse, flowSign);

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(7.0, rect.height() * 0.26, 11.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(4, 0, -4, -4), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("系统"));
}

CimdrawPowerGridItem::CimdrawPowerGridItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Grid, parent)
{
}

CimdrawPowerGridItem::CimdrawPowerGridItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Grid, pos, parent)
{
}

QString CimdrawPowerGridItem::className()
{
    return QStringLiteral("CimdrawPowerGridItem");
}

QString CimdrawPowerGridItem::shapeName() const
{
    return QStringLiteral("wsymgrid");
}

CimdrawDrawTypeId CimdrawPowerGridItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_GRID;
}

CimdrawWiringItemBase* CimdrawPowerGridItem::cloneForDuplicate() const
{
    return new CimdrawPowerGridItem(rect());
}

void CimdrawPowerGridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, visualState.flowSign);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerGridItem, wsymgrid)
