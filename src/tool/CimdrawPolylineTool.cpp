#include <QPainter>
#include <QRectF>

#include "Item/CimdrawPolyline.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawPolylineTool.h"

CimdrawPolylineTool::CimdrawPolylineTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("polyline");
    polyline = nullptr;
    createMode = CREATE_MODE_PRESS;
}

CimdrawPolylineTool::~CimdrawPolylineTool()
{

}

CimdrawDrawTypeId CimdrawPolylineTool::getDrawType() const
{
    return CIMDRAW_POLYLINE;
}

QGraphicsItem* CimdrawPolylineTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    polyline = new CimdrawPolyline(*pos);
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

QIcon CimdrawPolylineTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawPolyline::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}


void CimdrawPolylineTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(polyline)
        {
            polyline->updateCoordinate();
            CimdrawView* view = scene->getView();
            CimdrawAddCommand* command = new CimdrawAddCommand(polyline,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            polyline = nullptr;
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

void CimdrawPolylineTool::onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
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
