#include <QPainter>
#include <QRectF>

#include "Item/CimdrawRect.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawRectTool.h"

CimdrawRectTool::CimdrawRectTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("rect");
    createMode = CREATE_MODE_DRAG;
}

CimdrawRectTool::~CimdrawRectTool()
{

}

CimdrawDrawTypeId CimdrawRectTool::getDrawType() const
{
    return CIMDRAW_RECT;
}

QGraphicsItem* CimdrawRectTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawRect* item = new CimdrawRect(*pos);
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

QIcon CimdrawRectTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawRect::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawRectTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
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
        CimdrawView* view = scene->getView();
        view->setModified(true);
        scene->addSelection(pObj);
        QUndoCommand* command = new CimdrawAddCommand(pObj,scene,list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}
