#include "LzPowerGridTool.h"
#include "LzPowerGridItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerGridTool::LzPowerGridTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("系统进线");
    createMode = CREATE_MODE_DRAG;
}

LzPowerGridTool::~LzPowerGridTool() = default;

LzDrawTypeId LzPowerGridTool::getDrawType() const
{
    return LZ_WSYM_GRID;
}

QGraphicsItem* LzPowerGridTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerGridItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerGridTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerGridItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerGridItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false, 1);
    return QIcon(pm);
}


void LzPowerGridTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, LzPowerGridItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    lzSldPushAdd(scene, pObj, evt);
}

