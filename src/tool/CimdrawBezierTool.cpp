#include <QPainter>
#include <QRectF>

#include "Item/CimdrawBezier.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawBezierTool.h"

CimdrawBezierTool::CimdrawBezierTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("bezier");
    bezier = nullptr;
    createMode = CREATE_MODE_PRESS;
}

CimdrawBezierTool::~CimdrawBezierTool()
{

}

CimdrawDrawTypeId CimdrawBezierTool::getDrawType() const
{
    return CIMDRAW_BEZIER;
}


QGraphicsItem* CimdrawBezierTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    bezier = new CimdrawBezier(*pos);
    bezier->setDrawTool(this);
    if (bezier)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                bezier->setSelected(true);
            }
        }
    }
    return bezier;
}

QIcon CimdrawBezierTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawBezier::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawBezierTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(bezier)
        {
            bezier->updateCoordinate();
            CimdrawView* view = scene->getView();
            CimdrawAddCommand* command = new CimdrawAddCommand(bezier,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            bezier = nullptr;
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
    if (!bezier)
    {
        QRectF rc(downPoint, QSizeF(0, 0));
        createObject(scene, &rc);
        bezier->setPos(downPoint);

        bezier->addPoint(downPoint);
        scene->addItem(bezier);
        scene->addSelection(bezier);
        scene->setPaintState(true);
        scene->getView()->setCursor(Qt::ArrowCursor);
    }
    bezier->addPoint(downPoint);
    currentSelectMode = MODE_SIZE;
    currentDragHandle = bezier->handleCount();
    scene->setPaintState(true);
}

void CimdrawBezierTool::onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton)
    {
        return;
    }
    if (bezier)
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
            bezier->control(currentDragHandle, lastPoint);
        }
    }
}


