#include "CimdrawHorizontalDistributionCommand.h"
#include "CimdrawScene.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/TmpBase.h"

class CimdrawHorizontalDistributionCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawHorizontalDistributionCommand)
public:
    CimdrawHorizontalDistributionCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawHorizontalDistributionCommand* command)
        :q_ptr(command),items(items),scene(scene)
    {

    }
    CimdrawHorizontalDistributionCommand* q_ptr;
    QList<QGraphicsItem*> items;
    CimdrawScene* scene;
    QMap<QGraphicsItem*,QPointF> originalPositions;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
};


CimdrawHorizontalDistributionCommand::CimdrawHorizontalDistributionCommand(QList<QGraphicsItem*> items,CimdrawScene* scene, QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new CimdrawHorizontalDistributionCommandPrivate(items,scene,this))
{

}

bool CimdrawHorizontalDistributionCommand::validateChange()
{
    Q_D(CimdrawHorizontalDistributionCommand);
    if(d->items.size() < 3)
        return false;

    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal totalWidth = 0;

    for(auto item : d->items)
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        minX = qMin(minX, sceneBounds.left());
        maxX = qMax(maxX, sceneBounds.right());
        totalWidth += sceneBounds.width();
    }

    qreal availableSpace = maxX - minX;
    qreal spacing = 0;
    if (d->items.size() > 1)
    {
        spacing = (availableSpace - totalWidth) / (d->items.size() - 1);
    }

    auto sortedItems = d->items;
    std::sort(sortedItems.begin(), sortedItems.end(), 
        [](QGraphicsItem* a, QGraphicsItem* b) 
        {
            return a->sceneBoundingRect().left() < b->sceneBoundingRect().left();
        });

    qreal currentX = minX;
    for (auto item : sortedItems) 
    {
        QRectF sceneBounds = item->sceneBoundingRect(); 
        if (!qFuzzyCompare(currentX, sceneBounds.left())) 
            return true;  
        currentX += sceneBounds.width() + spacing;
    }
    
    return false;
}

void CimdrawHorizontalDistributionCommand::undo()
{
    Q_D(CimdrawHorizontalDistributionCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for(auto item : d->items)
    {
        if(d->originalPositions.contains(item))
        {
            item->setPos(d->originalPositions[item]);
        }
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->originalPositions.clear();
}

void CimdrawHorizontalDistributionCommand::redo()
{
    Q_D(CimdrawHorizontalDistributionCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    d->originalPositions.clear();
    for(auto item : d->items)
    {
        d->originalPositions[item] = item->pos();
    }
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::min();
    qreal totalWidth = 0;

    for(auto item : d->items)
    {
        QRectF sceneBounds = item->sceneBoundingRect();
    }

    for (auto item : d->items) 
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        minX = qMin(minX, sceneBounds.left());
        maxX = qMax(maxX, sceneBounds.right());
        totalWidth += sceneBounds.width();
    }

    qreal availableSpace = maxX - minX;
    qreal spacing = 0;
    if (d->items.size() > 1)
    {
        spacing = (availableSpace - totalWidth) / (d->items.size() - 1);
    }

    std::sort(d->items.begin(), d->items.end(), 
        [](QGraphicsItem* a, QGraphicsItem* b) 
        {
            return a->sceneBoundingRect().left() < b->sceneBoundingRect().left();
        });

    qreal currentX = minX;
    for (auto item : d->items) 
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        
        // 计算需要移动的差值
        qreal deltaX = currentX - sceneBounds.left();
        
        // 将场景坐标转换为图元的局部坐标系
        QPointF newPos = item->pos() + QPointF(deltaX, 0);
        
        // 如果图元有父项，需要将场景坐标转换为父项坐标系
        if (item->parentItem()) 
        {
            QPointF scenePos = QPointF(currentX, sceneBounds.y());
            newPos = item->parentItem()->mapFromScene(scenePos);
        }
        
        item->setPos(newPos);
        currentX += sceneBounds.width() + spacing;
    }
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
