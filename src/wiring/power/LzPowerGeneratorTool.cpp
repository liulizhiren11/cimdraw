#include "LzPowerGeneratorTool.h"
#include "LzPowerGeneratorItem.h"
#include "LzWiringToolUtils.h"
#include "LzScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

LzPowerGeneratorTool::LzPowerGeneratorTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("发电机 G");
    createMode = CREATE_MODE_DRAG;
}

LzPowerGeneratorTool::~LzPowerGeneratorTool() = default;

LzDrawTypeId LzPowerGeneratorTool::getDrawType() const
{
    return LZ_SLD_GENERATOR;
}

QGraphicsItem* LzPowerGeneratorTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new LzPowerGeneratorItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzPowerGeneratorTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = LzPowerGeneratorItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    LzPowerGeneratorItem::drawSymbol(&painter, iconRect, LzWiringRunState::Normal, false);
    return QIcon(pm);
}


void LzPowerGeneratorTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, LzPowerGeneratorItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    lzSldPushAdd(scene, pObj, evt);
}

