#include <QPainter>
#include <QRectF>

#include "Item/CimdrawArc.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawArcTool.h"

CimdrawArcTool::CimdrawArcTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("arc");
    createMode = CREATE_MODE_DRAG;
}

CimdrawArcTool::~CimdrawArcTool()
{

}

CimdrawDrawTypeId CimdrawArcTool::getDrawType() const
{
    return CIMDRAW_ARC;
}

QGraphicsItem* CimdrawArcTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawArc* item = new CimdrawArc(*pos);
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

QIcon CimdrawArcTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawArc::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawArcTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
     if (evt->button() == Qt::RightButton)
     {
         CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
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
         CimdrawView* view = scene->getView();
         view->setModified(true);
         scene->addSelection(pObj);
         QUndoCommand* command = new CimdrawAddCommand(pObj,scene,list);
         view->getStack()->push(command);
         emit view->editChanged();
     }
}













