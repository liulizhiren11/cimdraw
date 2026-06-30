#include "LzHorizontalDistributionCommand.h"
#include "LzScene.h"
#include "LzLayoutCommandUtils.h"
#include "item/TmpBase.h"

class LzHorizontalDistributionCommandPrivate
{
    Q_DECLARE_PUBLIC(LzHorizontalDistributionCommand)
public:
    LzHorizontalDistributionCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzHorizontalDistributionCommand* command)
        :q_ptr(command),items(items),scene(scene)
    {

    }
    LzHorizontalDistributionCommand* q_ptr;
    QList<QGraphicsItem*> items;
    LzScene* scene;
    QMap<QGraphicsItem*,QPointF> originalPositions;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
};


LzHorizontalDistributionCommand::LzHorizontalDistributionCommand(QList<QGraphicsItem*> items,LzScene* scene, QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new LzHorizontalDistributionCommandPrivate(items,scene,this))
{

}

bool LzHorizontalDistributionCommand::validateChange()
{
    Q_D(LzHorizontalDistributionCommand);
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

void LzHorizontalDistributionCommand::undo()
{
    Q_D(LzHorizontalDistributionCommand);
    lzBeginLayoutCommand(d->scene);
    for(auto item : d->items)
    {
        if(d->originalPositions.contains(item))
        {
            item->setPos(d->originalPositions[item]);
        }
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->originalPositions.clear();
}

void LzHorizontalDistributionCommand::redo()
{
    Q_D(LzHorizontalDistributionCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
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
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
