#include "LzPowerPvInfeedTool.h"
#include "LzPowerPvInfeedItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerPvInfeedTool::LzPowerPvInfeedTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("光伏进线");
    createMode = CREATE_MODE_DRAG;
}

LzPowerPvInfeedTool::~LzPowerPvInfeedTool() = default;

LzDrawTypeId LzPowerPvInfeedTool::getDrawType() const
{
    return LZ_WSYM_PV_INFEED;
}

QGraphicsItem* LzPowerPvInfeedTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerPvInfeedItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerPvInfeedTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerPvInfeedItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerPvInfeedItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false, 1);
    return QIcon(pm);
}


void LzPowerPvInfeedTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, LzPowerPvInfeedItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    lzSldPushAdd(scene, pObj, evt);
}

