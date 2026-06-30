#include "LzPowerFeederTool.h"
#include "LzPowerFeederItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerFeederTool::LzPowerFeederTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("潮流方向");
    createMode = CREATE_MODE_DRAG;
}

LzPowerFeederTool::~LzPowerFeederTool() = default;

LzDrawTypeId LzPowerFeederTool::getDrawType() const
{
    return LZ_SLD_POWER_FEEDER;
}

QGraphicsItem* LzPowerFeederTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerFeederItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerFeederTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerFeederItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerFeederItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false, 1);
    return QIcon(pm);
}


void LzPowerFeederTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, LzPowerFeederItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    lzSldPushAdd(scene, pObj, evt);
}

