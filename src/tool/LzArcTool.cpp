#include <QPainter>
#include <QRectF>

#include "Item/LzArc.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzArcTool.h"

LzArcTool::LzArcTool(QObject* parent) : LzTool(parent)
{
    name = tr("arc");
    createMode = CREATE_MODE_DRAG;
}

LzArcTool::~LzArcTool()
{

}

LzDrawTypeId LzArcTool::getDrawType() const
{
    return LZ_ARC;
}

QGraphicsItem* LzArcTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzArc* item = new LzArc(*pos);
    item->setDrawTool(this);
    if (item)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                scene->addItem(item);
            }
        }
    }
    return item;
}

QIcon LzArcTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzArc::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzArcTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
     if (evt->button() == Qt::RightButton)
     {
         LzToolManager::getInstance()->changeTool(LZ_SELECTION);
         scene->clearSelection();
         return;
     }

     QPointF local = evt->scenePos();
     QList<QGraphicsItem*> list = scene->getSelections();

     QRectF rc(local, QSizeF(100, 100));
     QGraphicsItem* pObj = createObject(scene, &rc);
     if (pObj)
     {
         pObj->setPos(local);
         LzView* view = scene->getView();
         view->setModified(true);
         scene->addSelection(pObj);
         QUndoCommand* command = new LzAddCommand(pObj,scene,list);
         view->getStack()->push(command);
         emit view->editChanged();
     }
}













