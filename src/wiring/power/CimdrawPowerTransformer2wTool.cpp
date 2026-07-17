#include "CimdrawPowerTransformer2wTool.h"
#include "CimdrawPowerTransformer2wItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerTransformer2wTool::CimdrawPowerTransformer2wTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("双绕组变压器");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerTransformer2wTool::~CimdrawPowerTransformer2wTool() = default;

CimdrawDrawTypeId CimdrawPowerTransformer2wTool::getDrawType() const
{
    return CIMDRAW_SLD_TRANSFORMER_2W;
}

QGraphicsItem* CimdrawPowerTransformer2wTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerTransformer2wItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerTransformer2wTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerTransformer2wItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerTransformer2wItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false);
    return QIcon(pm);
}


void CimdrawPowerTransformer2wTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, CimdrawPowerTransformer2wItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, pObj, evt);
}

