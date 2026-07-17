#include "CimdrawPowerGeneratorItem.h"

#include "wiring/base/CimdrawWiringDrawPalette.h"
#include "Item/CimdrawObjectFactory.h"

#include <QDateTime>
#include <QPainter>

void CimdrawPowerGeneratorItem::drawSymbol(QPainter* painter,
                                      const QRectF& rect,
                                      CimdrawWiringRunState runState,
                                      bool alarmPulse)
{
    if (!painter)
        return;

    const QColor stroke = CimdrawWiringDrawPalette::conductorStroke(runState);
    QPen pen(stroke, cimGraphicAlarmPenWidth(runState, alarmPulse));
    painter->setPen(pen);
    painter->setBrush(CimdrawWiringDrawPalette::generatorFill());
    const qreal radius = qMin(rect.width(), rect.height()) * 0.38;
    const QPointF center = rect.center();
    painter->drawLine(QLineF(rect.left(), center.y(), center.x() - radius, center.y()));
    painter->drawLine(QLineF(center.x() + radius, center.y(), rect.right(), center.y()));
    painter->drawEllipse(center, radius, radius);
    painter->setBrush(Qt::NoBrush);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qBound(10.0, radius * 1.1, 22.0));
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QStringLiteral("G"));
}

CimdrawPowerGeneratorItem::CimdrawPowerGeneratorItem(QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Generator, parent)
{
}

CimdrawPowerGeneratorItem::CimdrawPowerGeneratorItem(const QRectF& pos, QGraphicsItem* parent)
    : CimdrawWiringItemBase(CimdrawWiringItemKind::Generator, pos, parent)
{
}

QString CimdrawPowerGeneratorItem::className()
{
    return QStringLiteral("CimdrawPowerGeneratorItem");
}

QString CimdrawPowerGeneratorItem::shapeName() const
{
    return QStringLiteral("generator");
}

CimdrawDrawTypeId CimdrawPowerGeneratorItem::drawTypeForXml() const
{
    return CIMDRAW_SLD_GENERATOR;
}

CimdrawWiringItemBase* CimdrawPowerGeneratorItem::cloneForDuplicate() const
{
    return new CimdrawPowerGeneratorItem(rect());
}

void CimdrawPowerGeneratorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    const CimdrawWiringVisualState visualState = prepareGraphicRenderStateForPaint(painter);
    drawSymbol(painter, rect(), visualState.runState, visualState.alarmPulse);
}

REGISTER_OBJECT_CREATOR(CimdrawPowerGeneratorItem, generator)
