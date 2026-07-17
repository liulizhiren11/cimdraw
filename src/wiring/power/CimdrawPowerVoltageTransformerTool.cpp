#include "CimdrawPowerVoltageTransformerTool.h"

#include "CimdrawPowerVoltageTransformerItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerVoltageTransformerTool::CimdrawPowerVoltageTransformerTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("电压互感器 PT/VT");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerVoltageTransformerTool::~CimdrawPowerVoltageTransformerTool() = default;

CimdrawDrawTypeId CimdrawPowerVoltageTransformerTool::getDrawType() const
{
    return CIMDRAW_WSYM_VOLTAGE_TRANSFORMER;
}

QGraphicsItem* CimdrawPowerVoltageTransformerTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerVoltageTransformerItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerVoltageTransformerTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerVoltageTransformerItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerVoltageTransformerItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false);
    return QIcon(pm);
}

void CimdrawPowerVoltageTransformerTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
    QRectF rc(local, CimdrawPowerVoltageTransformerItem::defaultSize());
    QGraphicsItem* object = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, object, evt);
}
