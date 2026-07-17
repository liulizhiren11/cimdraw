#include "CimdrawPowerSvgCompItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

void CimdrawPowerSvgCompItem::drawSymbol(QPainter* painter,
                                    const QRectF& rect,
                                    CimdrawWiringRunState runState,
                                    bool alarmPulse,
                                    int flowSign)
{
    Q_UNUSED(flowSign);
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    painter->drawLine(QLineF(rect.left(), centerY, rect.right(), centerY));
    const QRectF box(rect.center().x() - qMin(rect.width() * 0.32, 28.0),
                     centerY - qMin(rect.height() * 0.32, 20.0),
                     qMin(rect.width() * 0.64, 56.0),
                     qMin(rect.height() * 0.55, 36.0));
    painter->drawRoundedRect(box, 2.0, 2.0);

    const qreal xLeft = box.left() + 4;
    const qreal xRight = box.right() - 4;
    QPolygonF wave;
    for (int i = 0; i <= 7; ++i)
    {
        const qreal t = qreal(i) / 7.0;
        const qreal x = xLeft + (xRight - xLeft) * t;
        const qreal y = box.center().y() + ((i % 2) == 0 ? -3.0 : 3.0);
        wave << QPointF(x, y);
    }
    const QColor waveColor = cimGraphicUsesAlarmAccent(runState) ? stroke : CimdrawWiringDrawPalette::svgWave();
    painter->setPen(QPen(waveColor, pen.width()));
    painter->drawPolyline(wave);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("SVG"));
}

CimdrawPowerSvgCompItem::CimdrawPowerSvgCompItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::SvgComp, parent)
{
}

CimdrawPowerSvgCompItem::CimdrawPowerSvgCompItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::SvgComp, pos, parent)
{
}

QString CimdrawPowerSvgCompItem::className()
{
    return QStringLiteral("CimdrawPowerSvgCompItem");
}

QString CimdrawPowerSvgCompItem::shapeName() const
{
    return QStringLiteral("wsymsvgcomp");
}

CimdrawDrawTypeId CimdrawPowerSvgCompItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_SVG_COMP;
}

CimdrawWiringItemBase* CimdrawPowerSvgCompItem::cloneForDuplicate() const
{
    return new CimdrawPowerSvgCompItem(rect());
}

void CimdrawPowerSvgCompItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, visualState.flowSign);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerSvgCompItem, wsymsvgcomp)
