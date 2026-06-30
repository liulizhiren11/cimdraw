#include <QPainter>
#include <QPen>

#include "Item/LzTopologyNodeItem.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzTopologyNodeTool.h"

LzTopologyNodeTool::LzTopologyNodeTool(QObject* parent)
    : LzTool(parent)
{
    name = tr("topologyNode");
    createMode = CREATE_MODE_DRAG;
}

LzTopologyNodeTool::~LzTopologyNodeTool() = default;

LzDrawTypeId LzTopologyNodeTool::getDrawType() const
{
    return LZ_TOPOLOGY_NODE;
}

QGraphicsItem* LzTopologyNodeTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzTopologyNodeItem* item = new LzTopologyNodeItem(*pos);
    item->setDrawTool(this);
    if (item && addToScene && scene)
    {
        scene->cleanSelection();
        scene->addItem(item);
    }
    return item;
}

QIcon LzTopologyNodeTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    p.setRenderHint(QPainter::Antialiasing);
    const qreal m = 4;
    LzTopologyNodeItem::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzTopologyNodeTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
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
        LzView* view = scene->getView();
        view->setModified(true);
        scene->addSelection(pObj);
        QUndoCommand* command = new LzAddCommand(pObj, scene, list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}
