#include <QPainter>
#include <QPen>

#include "Item/CimdrawTopologyNodeItem.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawTopologyNodeTool.h"

CimdrawTopologyNodeTool::CimdrawTopologyNodeTool(QObject* parent)
    : CimdrawTool(parent)
{
    name = tr("topologyNode");
    createMode = CREATE_MODE_DRAG;
}

CimdrawTopologyNodeTool::~CimdrawTopologyNodeTool() = default;

CimdrawDrawTypeId CimdrawTopologyNodeTool::getDrawType() const
{
    return CIMDRAW_TOPOLOGY_NODE;
}

QGraphicsItem* CimdrawTopologyNodeTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawTopologyNodeItem* item = new CimdrawTopologyNodeItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon CimdrawTopologyNodeTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal m = 4;
    CimdrawTopologyNodeItem::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawTopologyNodeTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
        scene->cleanSelection();
        return;
    }

    const QPointF local = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();

    QRectF rc(local, QSizeF(100, 100));
    QGraphicsItem* pObj = createObject(scene, &rc);
    if (pObj)
    {
        pObj->setPos(local);
        CimdrawView* view = scene->getView();
        view->setModified(true);
        scene->addSelection(pObj);
        QUndoCommand* command = new CimdrawAddCommand(pObj, scene, list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}
