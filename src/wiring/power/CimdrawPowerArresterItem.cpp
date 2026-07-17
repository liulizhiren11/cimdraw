#include "CimdrawPowerArresterItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerArresterItem::drawSymbol(QPainter* painter,
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

    const qreal centerX = rect.center().x();
    const qreal centerY = rect.center().y();
    painter->drawLine(QLineF(rect.left(), centerY, rect.right(), centerY));
    const qreal stem = qMin(rect.height() * 0.38, 28.0);
    const qreal tipY = centerY + stem;
    painter->drawLine(QLineF(centerX, centerY, centerX, tipY));
    const qreal gap = qMin(rect.width() * 0.22, 14.0);
    painter->drawLine(QLineF(centerX - gap * 0.5, tipY - gap * 0.45, centerX, tipY));
    painter->drawLine(QLineF(centerX + gap * 0.5, tipY - gap * 0.45, centerX, tipY));
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.0, rect.height() * 0.2, 9.0));
    painter->setFont(font);
    painter->setPen(QPen(stroke, 1));
    painter->drawText(rect.adjusted(2, 0, -2, -2), Qt::AlignHCenter | Qt::AlignBottom, QStringLiteral("MOA"));
}

CimdrawPowerArresterItem::CimdrawPowerArresterItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Arrester, parent)
{
}

CimdrawPowerArresterItem::CimdrawPowerArresterItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Arrester, pos, parent)
{
}

QString CimdrawPowerArresterItem::className()
{
    return QStringLiteral("CimdrawPowerArresterItem");
}

QString CimdrawPowerArresterItem::shapeName() const
{
    return QStringLiteral("wsymarrester");
}

CimdrawDrawTypeId CimdrawPowerArresterItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_ARRESTER;
}

CimdrawWiringItemBase* CimdrawPowerArresterItem::cloneForDuplicate() const
{
    return new CimdrawPowerArresterItem(rect());
}

void CimdrawPowerArresterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(CimdrawPowerArresterItem, wsymarrester)
