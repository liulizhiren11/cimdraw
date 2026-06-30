#include <QPainter>
#include <QRectF>

#include "Item/LzBezier.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzBezierTool.h"

LzBezierTool::LzBezierTool(QObject* parent) : LzTool(parent)
{
    name = tr("bezier");
    bezier = nullptr;
    createMode = CREATE_MODE_PRESS;
}

LzBezierTool::~LzBezierTool()
{

}

LzDrawTypeId LzBezierTool::getDrawType() const
{
    return LZ_BEZIER;
}


QGraphicsItem* LzBezierTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    bezier = new LzBezier(*pos);
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

QIcon LzBezierTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);

    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzBezier::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzBezierTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(bezier)
        {
            bezier->updateCoordinate();
            LzView* view = scene->getView();
            LzAddCommand* command = new LzAddCommand(bezier,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            bezier = nullptr;
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

void LzBezierTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent *evt)
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


