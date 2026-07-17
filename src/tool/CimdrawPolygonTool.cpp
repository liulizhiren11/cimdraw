#include <QPainter>
#include <QRectF>

#include "Item/CimdrawPolygon.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawPolygonTool.h"

CimdrawPolygonTool::CimdrawPolygonTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("polygon");
    polygon = nullptr;
    createMode = CREATE_MODE_PRESS;
}

CimdrawPolygonTool::~CimdrawPolygonTool()
{

}

CimdrawDrawTypeId CimdrawPolygonTool::getDrawType() const
{
    return CIMDRAW_POLYGON;
}

QGraphicsItem* CimdrawPolygonTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    polygon = new CimdrawPolygon(*pos);
    polygon->setDrawTool(this);
    if (polygon)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                polygon->setSelected(true);
            }
        }
    }
    return polygon;
}

QIcon CimdrawPolygonTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawPolygon::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawPolygonTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(polygon)
        {
            polygon->updateCoordinate();
            CimdrawView* view = scene->getView();
            CimdrawAddCommand* command = new CimdrawAddCommand(polygon,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            polygon = nullptr;
            scene->setPaintState(false);
        }
        else
        {
           currentSelectMode = MODE_NONE;
           CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
           scene->clearSelection();
        }
        return;
    }
    if (!polygon)
    {
        QRectF rc(downPoint, QSizeF(0, 0));
        createObject(scene, &rc);
        polygon->setPos(downPoint);

        polygon->addPoint(downPoint);
        scene->addItem(polygon);
        scene->addSelection(polygon);
        scene->setPaintState(true);
        scene->getView()->setCursor(Qt::ArrowCursor);
    }
    polygon->addPoint(downPoint);
    currentSelectMode = MODE_SIZE;
    currentDragHandle = polygon->handleCount();
    scene->setPaintState(true);
}

void CimdrawPolygonTool::onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        return;
    }
    if (polygon)
    {
        lastPoint = evt->scenePos();
        if (currentDragHandle != HANDLE_NONE && currentSelectMode == MODE_SIZE)
        {
            if (evt->modifiers() & Qt::ControlModifier)
            {
                qreal dx = qAbs(lastPoint.x() - lastPoint.x());
                qreal dy = qAbs(lastPoint.y() - lastPoint.y());
                if (dx >= dy)
                {
                    lastPoint.setY(lastPoint.y());
                }
                else
                {
                    lastPoint.setX(lastPoint.x());
                }
            }
            polygon->control(currentDragHandle, lastPoint);
        }
    }
}

