#include <QPainter>
#include <QRectF>

#include "Item/CimdrawLine.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawLineTool.h"

class CimdrawLineToolPrivate
{
public:
    CimdrawLineToolPrivate(CimdrawLineTool* tool)
        :q_ptr(tool)
    {

    }
    CimdrawLine* line = nullptr;
    QPointF downPoint;
    QPointF lastPoint;
    CimdrawLineTool* q_ptr;
};

CimdrawLineTool::CimdrawLineTool(QObject* parent) 
    : CimdrawTool(parent),d_ptr(new CimdrawLineToolPrivate(this))
{
    name = tr("line");
    createMode = CREATE_MODE_PRESS;
}

CimdrawLineTool::~CimdrawLineTool()
{

}

CimdrawDrawTypeId CimdrawLineTool::getDrawType() const
{
    return CIMDRAW_LINE;
}

QGraphicsItem* CimdrawLineTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    d_ptr->line = new CimdrawLine(*pos);
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

QIcon CimdrawLineTool::getIcon(const QSize& size, const QVariant& para) const
{
    Q_UNUSED(para);
    QPixmap map(size);
    map.fill(Qt::transparent);

    QPainter p(&map);
    const qreal m = 4;
    CimdrawLine::paintToolboxIcon(&p, QRectF(m, m, size.width() - 2 * m, size.height() - 2 * m));

    return QIcon(map);
}

void CimdrawLineTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    d_ptr->downPoint = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();
    if (evt->button() == Qt::RightButton)
    {
        if(d_ptr->line)
        {
            d_ptr->line->updateCoordinate();
            CimdrawView* view = scene->getView();
            CimdrawAddCommand* command = new CimdrawAddCommand(d_ptr->line,scene,list);
            view->getStack()->push(command);
            emit view->editChanged();
            d_ptr->line = nullptr;
            scene->setPaintState(false);
        }
        currentSelectMode = MODE_NONE;
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
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
        CimdrawView* view = scene->getView();
        CimdrawAddCommand* command = new CimdrawAddCommand(d_ptr->line,scene,list);
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

void CimdrawLineTool::onMouseMove(CimdrawScene* scene, QGraphicsSceneMouseEvent *evt)
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
