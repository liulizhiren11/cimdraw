#include "LzPowerSvgCompItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>
#include <QPolygonF>

void LzPowerSvgCompItem::drawSymbol(QPainter* painter,
                                    const QRectF& rect,
                                    LzWiringRunState runState,
                                    bool alarmPulse,
                                    int flowSign)
{
    Q_UNUSED(flowSign);
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
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
    const QColor waveColor = (runState == LzWiringRunState::Alarm) ? stroke : LzWiringDrawPalette::svgWave();
    painter->setPen(QPen(waveColor, pen.width()));
    painter->drawPolyline(wave);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("SVG"));
}

LzPowerSvgCompItem::LzPowerSvgCompItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::SvgComp, parent)
{
}

LzPowerSvgCompItem::LzPowerSvgCompItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::SvgComp, pos, parent)
{
}

QString LzPowerSvgCompItem::className()
{
    return QStringLiteral("LzPowerSvgCompItem");
}

QString LzPowerSvgCompItem::shapeName() const
{
    return QStringLiteral("wsymsvgcomp");
}

LzDrawTypeId LzPowerSvgCompItem::drawTypeForXml() const
{
    return LZ_WSYM_SVG_COMP;
}

LzWiringItemBase* LzPowerSvgCompItem::cloneForDuplicate() const
{
    return new LzPowerSvgCompItem(rect());
}

void LzPowerSvgCompItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, flowSign());
}

REGISTER_OBJECT_CREATOR(LzPowerSvgCompItem, wsymsvgcomp)
