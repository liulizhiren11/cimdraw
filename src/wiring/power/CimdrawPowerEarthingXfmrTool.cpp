#include "CimdrawPowerEarthingXfmrTool.h"
#include "CimdrawPowerEarthingXfmrItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerEarthingXfmrTool::CimdrawPowerEarthingXfmrTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("接地变");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerEarthingXfmrTool::~CimdrawPowerEarthingXfmrTool() = default;

CimdrawDrawTypeId CimdrawPowerEarthingXfmrTool::getDrawType() const
{
    return CIMDRAW_WSYM_EARTHING_XFMR;
}

QGraphicsItem* CimdrawPowerEarthingXfmrTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerEarthingXfmrItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerEarthingXfmrTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerEarthingXfmrItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerEarthingXfmrItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false);
    return QIcon(pm);
}


void CimdrawPowerEarthingXfmrTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, CimdrawPowerEarthingXfmrItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, pObj, evt);
}

