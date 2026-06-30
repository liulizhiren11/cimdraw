#include "LzPowerArresterItem.h"

#include "wiring/base/LzWiringDrawPalette.h"
#include "Item/LzObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void LzPowerArresterItem::drawSymbol(QPainter* painter,
                                     const QRectF& rect,
                                     LzWiringRunState runState,
                                     bool alarmPulse,
                                     LzWiringSymbolStandard symStd)
{
    Q_UNUSED(symStd);
    if (!painter)
        return;

    const QColor stroke = LzWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, runState == LzWiringRunState::Alarm && alarmPulse ? 3 : 2);
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

LzPowerArresterItem::LzPowerArresterItem(QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Arrester, parent)
{
}

LzPowerArresterItem::LzPowerArresterItem(const QRectF& pos, QGraphicsItem* parent)
    : LzWiringItemBase(LzWiringItemKind::Arrester, pos, parent)
{
}

QString LzPowerArresterItem::className()
{
    return QStringLiteral("LzPowerArresterItem");
}

QString LzPowerArresterItem::shapeName() const
{
    return QStringLiteral("wsymarrester");
}

LzDrawTypeId LzPowerArresterItem::drawTypeForXml() const
{
    return LZ_WSYM_ARRESTER;
}

LzWiringItemBase* LzPowerArresterItem::cloneForDuplicate() const
{
    return new LzPowerArresterItem(rect());
}

void LzPowerArresterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    prepare(painter);
    const bool alarmPulse = (wiringRunState() == LzWiringRunState::Alarm)
        && ((QDateTime::currentMSecsSinceEpoch() / 400) % 2 == 0);
    drawSymbol(painter, rect(), wiringRunState(), alarmPulse, wiringSymbolStandard());
}

REGISTER_OBJECT_CREATOR(LzPowerArresterItem, wsymarrester)
