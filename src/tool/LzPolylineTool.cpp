#include <QPainter>
#include <QRectF>

#include "Item/LzPolyline.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzPolylineTool.h"

LzPolylineTool::LzPolylineTool(QObject* parent) : LzTool(parent)
{
    name = tr("polyline");
    polyline = nullptr;
    createMode = CREATE_MODE_PRESS;
}

LzPolylineTool::~LzPolylineTool()
{

}

LzDrawTypeId LzPolylineTool::getDrawType() const
{
    return LZ_POLYLINE;
}

QGraphicsItem* LzPolylineTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    polyline = new LzPolyline(*pos);
    polyline->setDrawTool(this);
    if (polyline)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                polyline->setSelected(true);
            }
        }
    }
    return polyline;
}

QIcon LzPolylineTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzPolyline::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}


void LzPolylineTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(polyline)
        {
            polyline->updateCoordinate();
            LzView* view = scene->getView();
            LzAddCommand* command = new LzAddCommand(polyline,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            polyline = nullptr;
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
    if (!polyline)
    {
        QRectF rc(downPoint, QSizeF(0, 0));
        createObject(scene, &rc);
        polyline->setPos(downPoint);

        polyline->addPoint(downPoint);
        scene->addItem(polyline);
        scene->addSelection(polyline);
        scene->setPaintState(true);
        scene->getView()->setCursor(Qt::ArrowCursor);
    }
    polyline->addPoint(downPoint);
    currentSelectMode = MODE_SIZE;
    currentDragHandle = polyline->handleCount();
    scene->setPaintState(true);
}

void LzPolylineTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        return;
    }
    if (polyline)
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
            polyline->control(currentDragHandle, lastPoint);
        }
    }
}
