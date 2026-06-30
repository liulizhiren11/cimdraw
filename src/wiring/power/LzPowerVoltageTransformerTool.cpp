#include "LzPowerVoltageTransformerTool.h"

#include "LzPowerVoltageTransformerItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerVoltageTransformerTool::LzPowerVoltageTransformerTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("电压互感器 PT/VT");
    createMode = CREATE_MODE_DRAG;
}

LzPowerVoltageTransformerTool::~LzPowerVoltageTransformerTool() = default;

LzDrawTypeId LzPowerVoltageTransformerTool::getDrawType() const
{
    return LZ_WSYM_VOLTAGE_TRANSFORMER;
}

QGraphicsItem* LzPowerVoltageTransformerTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerVoltageTransformerItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerVoltageTransformerTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerVoltageTransformerItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerVoltageTransformerItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false);
    return QIcon(pm);
}

void LzPowerVoltageTransformerTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
    QRectF rc(local, LzPowerVoltageTransformerItem::defaultSize());
    QGraphicsItem* object = createObject(scene, &rc);
    lzSldPushAdd(scene, object, evt);
}
