#include "CimdrawPowerFuseTool.h"

#include "CimdrawPowerFuseItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerFuseTool::CimdrawPowerFuseTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("熔断器 Fuse");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerFuseTool::~CimdrawPowerFuseTool() = default;

CimdrawDrawTypeId CimdrawPowerFuseTool::getDrawType() const
{
    return CIMDRAW_WSYM_FUSE;
}

QGraphicsItem* CimdrawPowerFuseTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerFuseItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerFuseTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerFuseItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerFuseItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false);
    return QIcon(pm);
}

void CimdrawPowerFuseTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
    QRectF rc(local, CimdrawPowerFuseItem::defaultSize());
    QGraphicsItem* object = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, object, evt);
}
