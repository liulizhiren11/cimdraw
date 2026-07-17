#include "CimdrawPowerCapacitorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

namespace {

void drawPassiveLabels(QPainter* painter,
                       const QRectF& rect,
                       const QString& valueText,
                       const QString& referenceDesignator,
                       const QString& fallbackBottomLabel)
{
    if (!painter)
        return;

    const QString value = valueText.trimmed();
    const QString refdes = referenceDesignator.trimmed();
    const QString bottom = refdes.isEmpty() ? fallbackBottomLabel : refdes;

    QFont font = painter->font();
    if (!value.isEmpty())
    {
        font.setBold(false);
        font.setPointSizeF(qBound(5.8, rect.height() * 0.18, 9.0));
        painter->setFont(font);
        painter->drawText(QRectF(rect.left() + 2.0, rect.top(), rect.width() - 4.0, rect.height() * 0.18),
                          Qt::AlignHCenter | Qt::AlignTop,
                          value);
    }

    if (!bottom.isEmpty())
    {
        font.setBold(refdes.isEmpty());
        font.setPointSizeF(qBound(6.0, rect.height() * 0.18, 9.0));
        painter->setFont(font);
        painter->drawText(QRectF(rect.left() + 2.0,
                                 rect.bottom() - rect.height() * 0.20,
                                 rect.width() - 4.0,
                                 rect.height() * 0.18),
                          Qt::AlignHCenter | Qt::AlignTop,
                          bottom);
    }
}

} // namespace

void CimdrawPowerCapacitorItem::drawSymbol(QPainter* painter, const QRectF& rect, CimdrawWiringRunState runState, bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    const QColor plate = CimdrawWiringDrawPalette::capacitorPlate(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal cy = rect.center().y();
    painter->drawLine(QLineF(rect.left(), cy, rect.right(), cy));
    const qreal d = qMax(5.0, rect.width() * 0.11);
    const qreal hh = qMin(rect.height() * 0.4, 22.0);
    painter->setPen(QPen(plate, pen.width()));
    painter->drawLine(QLineF(rect.center().x() - d, cy - hh * 0.5, rect.center().x() - d, cy + hh * 0.5));
    painter->drawLine(QLineF(rect.center().x() + d, cy - hh * 0.5, rect.center().x() + d, cy + hh * 0.5));
    painter->setPen(QPen(stroke, 1));
}

CimdrawPowerCapacitorItem::CimdrawPowerCapacitorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Capacitor, parent)
{
}

CimdrawPowerCapacitorItem::CimdrawPowerCapacitorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Capacitor, pos, parent)
{
    finalizeWiringConstruction();
}

QString CimdrawPowerCapacitorItem::className()
{
    return QStringLiteral("CimdrawPowerCapacitorItem");
}

QString CimdrawPowerCapacitorItem::shapeName() const
{
    return QStringLiteral("wsymcapacitor");
}

CimdrawDrawTypeId CimdrawPowerCapacitorItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_CAPACITOR;
}

CimdrawWiringItemBase* CimdrawPowerCapacitorItem::cloneForDuplicate() const
{
    return new CimdrawPowerCapacitorItem(rect());
}

QString CimdrawPowerCapacitorItem::displayName() const
{
    if (!displayName_.isEmpty())
        return displayName_;
    return QStringLiteral("电容");
}

void CimdrawPowerCapacitorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
    drawPassiveLabels(painter, rect(), valueText_, referenceDesignator_, QStringLiteral("C"));
}

REGISTER_OBJECT_CREATOR(CimdrawPowerCapacitorItem, wsymcapacitor)
