#include "CimdrawPowerGroundItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerGroundItem::drawSymbol(QPainter* painter,
                                   const QRectF& rect,
                                   CimdrawWiringRunState runState,
                                   bool alarmPulse,
                                   CimdrawWiringSymbolStandard symStd)
{
    Q_UNUSED(symStd);
    if (!painter)
        return;

    const qreal centerX = rect.center().x();
    const qreal h = rect.height();
    const qreal yTop = rect.top() + h * 0.06;
    const qreal y1 = rect.top() + h * 0.34;
    const qreal y2 = rect.top() + h * 0.47;
    const qreal y3 = rect.top() + h * 0.60;
    const qreal w1 = qMin(rect.width() * 0.72, 28.0);
    const qreal w2 = qMin(rect.width() * 0.50, 20.0);
    const qreal w3 = qMin(rect.width() * 0.30, 12.0);
    painter->setPen(QPen(CimdrawWiringDrawPalette::conductorStroke(runState), 2));
    painter->drawLine(QLineF(centerX, yTop, centerX, y1));
    painter->setPen(QPen(CimdrawWiringDrawPalette::earthGround(), 2));
    painter->drawLine(QLineF(centerX - w1 * 0.5, y1, centerX + w1 * 0.5, y1));
    painter->drawLine(QLineF(centerX - w2 * 0.5, y2, centerX + w2 * 0.5, y2));
    painter->drawLine(QLineF(centerX - w3 * 0.5, y3, centerX + w3 * 0.5, y3));
    if (cimGraphicAlarmActive(runState, alarmPulse))
    {
        painter->setPen(QPen(CimdrawWiringDrawPalette::conductorStroke(CimdrawWiringRunState::Alarm), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(rect.adjusted(4, 4, -4, -4));
    }
}

CimdrawPowerGroundItem::CimdrawPowerGroundItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Ground, parent)
{
}

CimdrawPowerGroundItem::CimdrawPowerGroundItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Ground, pos, parent)
{
}

QString CimdrawPowerGroundItem::className()
{
    return QStringLiteral("CimdrawPowerGroundItem");
}

QString CimdrawPowerGroundItem::shapeName() const
{
    return QStringLiteral("sldground");
}

CimdrawDrawTypeId CimdrawPowerGroundItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_GROUND;
}

CimdrawWiringItemBase* CimdrawPowerGroundItem::cloneForDuplicate() const
{
    return new CimdrawPowerGroundItem(rect());
}

void CimdrawPowerGroundItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(CimdrawPowerGroundItem, sldground)
