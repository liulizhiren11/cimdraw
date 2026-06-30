#include <QPainter>
#include <QRectF>

#include "Item/LzLine.h"
#include "LzScene.h"
#include "LzView.h"
#include "Command/LzAddCommand.h"
#include "LzLineTool.h"

class LzLineToolPrivate
{
public:
    LzLineToolPrivate(LzLineTool* tool)
        :q_ptr(tool)
    {

    }
    LzLine* line = nullptr;
    QPointF downPoint;
    QPointF lastPoint;
    LzLineTool* q_ptr;
};

LzLineTool::LzLineTool(QObject* parent) 
    : LzTool(parent),d_ptr(new LzLineToolPrivate(this))
{
    name = tr("line");
    createMode = CREATE_MODE_PRESS;
}

LzLineTool::~LzLineTool()
{

}

LzDrawTypeId LzLineTool::getDrawType() const
{
    return LZ_LINE;
}

QGraphicsItem* LzLineTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    d_ptr->line = new LzLine(*pos);
    d_ptr->line->setDrawTool(this);
    if (d_ptr->line)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                d_ptr->line->setSelected(true);
            }
        }
    }
    return d_ptr->line;
}

QIcon LzLineTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    LzLine::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void LzLineTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    d_ptr->downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(d_ptr->line)
        {
            d_ptr->line->updateCoordinate();
            LzView* view = scene->getView();
            LzAddCommand* command = new LzAddCommand(d_ptr->line,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            d_ptr->line = nullptr;
            scene->setPaintState(false);
        }
        currentSelectMode = MODE_NONE;
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->clearSelection();
        return;
    }
    if (!d_ptr->line)
    {
        QRectF rc(d_ptr->downPoint, QSizeF(0, 0));
        createObject(scene, &rc);
        d_ptr->line->setPos(d_ptr->downPoint);

        scene->addItem(d_ptr->line);
        d_ptr->line->addPoint(d_ptr->downPoint);
        scene->addSelection(d_ptr->line);
        currentSelectMode = MODE_SIZE;
        currentDragHandle = d_ptr->line->handleCount();
        scene->setPaintState(true);
        scene->getView()->setCursor(Qt::ArrowCursor);
    }
    else
    {
        d_ptr->line->updateCoordinate();
        LzView* view = scene->getView();
        LzAddCommand* command = new LzAddCommand(d_ptr->line,scene,list);
        view->getStack()->push(command);
        emit view->editChanged();
        d_ptr->line = nullptr;
        scene->setPaintState(false);
        currentSelectMode = MODE_NONE;
    }
    if (d_ptr->line)
    {
        d_ptr->line->addPoint(d_ptr->downPoint);
        currentDragHandle = d_ptr->line->handleCount();
    }
}

void LzLineTool::onMouseMove(LzScene* scene, QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton)
    {
        return;
    }
    if (d_ptr->line)
    {
        d_ptr->lastPoint = evt->scenePos();
        if (currentDragHandle != HANDLE_NONE && currentSelectMode == MODE_SIZE)
        {
            if (evt->modifiers() & Qt::ControlModifier)
            {
                qreal dx = qAbs(d_ptr->lastPoint.x() - d_ptr->downPoint.x());
                qreal dy = qAbs(d_ptr->lastPoint.y() - d_ptr->downPoint.y());
                if (dx >= dy)
                {
                    d_ptr->lastPoint.setY(d_ptr->downPoint.y());
                }
                else
                {
                    d_ptr->lastPoint.setX(d_ptr->downPoint.x());
                }
            }
            d_ptr->line->control(currentDragHandle, d_ptr->lastPoint);
        }
    }
}
