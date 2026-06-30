#include <QPainter>
#include <QRectF>

#include "Item/LzText.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzTextTool.h"

LzTextTool::LzTextTool(QObject* parent) : LzTool(parent)
{
    name = tr("text");
    createMode = CREATE_MODE_DRAG;
}

LzTextTool::~LzTextTool()
{

}

LzDrawTypeId LzTextTool::getDrawType() const
{
    return LZ_TEXT;
}

QGraphicsItem* LzTextTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzText* item = new LzText(*pos);
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

QIcon LzTextTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzText::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzTextTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
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

