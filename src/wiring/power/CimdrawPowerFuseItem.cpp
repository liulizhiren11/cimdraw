#include "CimdrawPowerFuseItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QLineF>

void CimdrawPowerFuseItem::drawSymbol(QPainter* painter,
                                 const QRectF& rect,
                                 CimdrawWiringRunState runState,
                                 bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    const qreal centerY = rect.center().y();
    const qreal bodyW = qMin(rect.width() * 0.34, 28.0);
    const qreal bodyH = qMin(rect.height() * 0.55, 18.0);
    const QRectF body(rect.center().x() - bodyW * 0.5, centerY - bodyH * 0.5, bodyW, bodyH);
    painter->drawLine(QLineF(rect.left(), centerY, body.left(), centerY));
    painter->drawRect(body);
    painter->drawLine(QLineF(body.right(), centerY, rect.right(), centerY));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(6.2, rect.height() * 0.28, 10.0));
    painter->setFont(font);
    painter->drawText(body, Qt::AlignCenter, QStringLiteral("F"));
}

CimdrawPowerFuseItem::CimdrawPowerFuseItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Fuse, parent)
{
}

CimdrawPowerFuseItem::CimdrawPowerFuseItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Fuse, pos, parent)
{
}

QString CimdrawPowerFuseItem::className()
{
    return QStringLiteral("CimdrawPowerFuseItem");
}

QString CimdrawPowerFuseItem::shapeName() const
{
    return QStringLiteral("wsymfuse");
}

CimdrawDrawTypeId CimdrawPowerFuseItem::drawTypeForXml() const
{
    return CIMDRAW_WSYM_FUSE;
}

CimdrawWiringItemBase* CimdrawPowerFuseItem::cloneForDuplicate() const
{
    return new CimdrawPowerFuseItem(rect());
}

void CimdrawPowerFuseItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* option,
                            QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerFuseItem, wsymfuse)
