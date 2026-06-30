#include <QPainter>
#include <QRectF>

#include "Item/LzPolygon.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzPolygonTool.h"

LzPolygonTool::LzPolygonTool(QObject* parent) : LzTool(parent)
{
    name = tr("polygon");
    polygon = nullptr;
    createMode = CREATE_MODE_PRESS;
}

LzPolygonTool::~LzPolygonTool()
{

}

LzDrawTypeId LzPolygonTool::getDrawType() const
{
    return LZ_POLYGON;
}

QGraphicsItem* LzPolygonTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    polygon = new LzPolygon(*pos);
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

QIcon LzPolygonTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzPolygon::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzPolygonTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(polygon)
        {
            polygon->updateCoordinate();
            LzView* view = scene->getView();
            LzAddCommand* command = new LzAddCommand(polygon,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            polygon = nullptr;
            scene->setPaintState(false);
        }
        else
        {
           currentSelectMode = MODE_NONE;
           LzToolManager::getInstance()->changeTool(LZ_SELECTION);
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

void LzPolygonTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt)
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

