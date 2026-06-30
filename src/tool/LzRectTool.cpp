#include <QPainter>
#include <QRectF>

#include "Item/LzRect.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzRectTool.h"

LzRectTool::LzRectTool(QObject* parent) : LzTool(parent)
{
    name = tr("rect");
    createMode = CREATE_MODE_DRAG;
}

LzRectTool::~LzRectTool()
{

}

LzDrawTypeId LzRectTool::getDrawType() const
{
    return LZ_RECT;
}

QGraphicsItem* LzRectTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzRect* item = new LzRect(*pos);
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

QIcon LzRectTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzRect::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzRectTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->cleanSelection();
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
