#include "LzPowerCurrentTransformerTool.h"

#include "LzPowerCurrentTransformerItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerCurrentTransformerTool::LzPowerCurrentTransformerTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("电流互感器 CT");
    createMode = CREATE_MODE_DRAG;
}

LzPowerCurrentTransformerTool::~LzPowerCurrentTransformerTool() = default;

LzDrawTypeId LzPowerCurrentTransformerTool::getDrawType() const
{
    return LZ_WSYM_CURRENT_TRANSFORMER;
}

QGraphicsItem* LzPowerCurrentTransformerTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerCurrentTransformerItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerCurrentTransformerTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerCurrentTransformerItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerCurrentTransformerItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false);
    return QIcon(pm);
}

void LzPowerCurrentTransformerTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
    QRectF rc(local, LzPowerCurrentTransformerItem::defaultSize());
    QGraphicsItem* object = createObject(scene, &rc);
    lzSldPushAdd(scene, object, evt);
}
