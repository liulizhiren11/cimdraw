#include "CimdrawPowerCircuitBreakerTool.h"
#include "CimdrawPowerCircuitBreakerItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerCircuitBreakerTool::CimdrawPowerCircuitBreakerTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("断路器 QF");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerCircuitBreakerTool::~CimdrawPowerCircuitBreakerTool() = default;

CimdrawDrawTypeId CimdrawPowerCircuitBreakerTool::getDrawType() const
{
    return CIMDRAW_SLD_CIRCUIT_BREAKER;
}

QGraphicsItem* CimdrawPowerCircuitBreakerTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerCircuitBreakerItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerCircuitBreakerTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerCircuitBreakerItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerCircuitBreakerItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false, false);
    return QIcon(pm);
}


void CimdrawPowerCircuitBreakerTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, CimdrawPowerCircuitBreakerItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, pObj, evt);
}

