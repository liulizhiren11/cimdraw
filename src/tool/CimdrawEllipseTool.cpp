#include <QPainter>
#include <QRectF>

#include "Item/CimdrawEllipse.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawEllipseTool.h"

CimdrawEllipseTool::CimdrawEllipseTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("ellipse");
    createMode = CREATE_MODE_DRAG;
}

CimdrawEllipseTool::~CimdrawEllipseTool()
{

}

CimdrawDrawTypeId CimdrawEllipseTool::getDrawType() const
{
    return CIMDRAW_ELLIPSE;
}

QGraphicsItem* CimdrawEllipseTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawEllipse* item = new CimdrawEllipse(*pos);
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

QIcon CimdrawEllipseTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawEllipse::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawEllipseTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
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



