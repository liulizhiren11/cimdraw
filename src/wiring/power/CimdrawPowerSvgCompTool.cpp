#include "CimdrawPowerSvgCompTool.h"
#include "CimdrawPowerSvgCompItem.h"
#include "CimdrawWiringToolUtils.h"
#include "CimdrawScene.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CimdrawPowerSvgCompTool::CimdrawPowerSvgCompTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("静止无功 SVG");
    createMode = CREATE_MODE_DRAG;
}

CimdrawPowerSvgCompTool::~CimdrawPowerSvgCompTool() = default;

CimdrawDrawTypeId CimdrawPowerSvgCompTool::getDrawType() const
{
    return CIMDRAW_WSYM_SVG_COMP;
}

QGraphicsItem* CimdrawPowerSvgCompTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    auto* item = new CimdrawPowerSvgCompItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawPowerSvgCompTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter painter(&pm);
    painter.setRenderHint(QPainter::Antialiasing, true);
    const qreal margin = 4.0;
    const QRectF avail(margin, margin, qMax(1.0, size.width() - margin * 2), qMax(1.0, size.height() - margin * 2));
    const QSizeF def = CimdrawPowerSvgCompItem::defaultSize();
    const qreal scale = qMin(avail.width() / def.width(), avail.height() / def.height());
    const QRectF iconRect(avail.center().x() - def.width() * scale * 0.5,
                          avail.center().y() - def.height() * scale * 0.5,
                          def.width() * scale,
                          def.height() * scale);
    CimdrawPowerSvgCompItem::drawSymbol(&painter, iconRect, CimdrawWiringRunState::Normal, false, 1);
    return QIcon(pm);
}


void CimdrawPowerSvgCompTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }
    const QPointF local = evt->scenePos();
        QRectF rc(local, CimdrawPowerSvgCompItem::defaultSize());
    QGraphicsItem* pObj = createObject(scene, &rc);
    cimdrawSldPushAdd(scene, pObj, evt);
}

