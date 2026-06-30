#include "LzVerticalDistributionCommand.h"
#include "LzScene.h"
#include "LzLayoutCommandUtils.h"
#include "item/TmpBase.h"

class LzVerticalDistributionCommandPrivate
{
    Q_DECLARE_PUBLIC(LzVerticalDistributionCommand)
public:
    LzVerticalDistributionCommandPrivate(QList<QGraphicsItem*> items,LzScene* scene,LzVerticalDistributionCommand* command)
        :items(items),scene(scene),q_ptr(command)
    {

    }
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QPointF> originalPositions;
    LzScene* scene;
    QVector<LzLayoutConnectLineSnapshot> lineSnapshots;
    bool m_hasChange = false;
    LzVerticalDistributionCommand* q_ptr;
};

LzVerticalDistributionCommand::LzVerticalDistributionCommand(QList<QGraphicsItem*> items,LzScene* scene,QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new LzVerticalDistributionCommandPrivate(items,scene,this))
{

}

bool LzVerticalDistributionCommand::validateChange()
{
    Q_D(LzVerticalDistributionCommand);
    
    if (d->items.size() < 3)
        return false;
    
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();
    qreal totalHeight = 0;

    for (auto item : d->items)
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        minY = qMin(minY, sceneBounds.top());
        maxY = qMax(maxY, sceneBounds.bottom());
        totalHeight += sceneBounds.height();
    }

    qreal availableSpace = maxY - minY;
    qreal spacing = 0;
    if (d->items.size() > 1)
    {
        spacing = (availableSpace - totalHeight) / (d->items.size() - 1);
    }

    auto sortedItems = d->items;
    std::sort(sortedItems.begin(), sortedItems.end(),
        [](QGraphicsItem* a, QGraphicsItem* b)
        {
            return a->sceneBoundingRect().top() < b->sceneBoundingRect().top();
        });
    
    qreal currentY = minY;
    for (auto item : sortedItems)
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        if (!qFuzzyCompare(currentY, sceneBounds.top()))
            return true;
        currentY += sceneBounds.height() + spacing;
    }
    
    return false;
}

void LzVerticalDistributionCommand::undo()
{   
    Q_D(LzVerticalDistributionCommand);
    lzBeginLayoutCommand(d->scene);
    for (auto item : d->items) 
    {
        if (d->originalPositions.contains(item))
        {
            item->setPos(d->originalPositions[item]);
        }
    } 
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
    d->originalPositions.clear();
}

void LzVerticalDistributionCommand::redo()
{
    Q_D(LzVerticalDistributionCommand);
    d->lineSnapshots = lzCaptureLayoutConnectLineSnapshots(d->items);
    lzBeginLayoutCommand(d->scene);
    
    d->originalPositions.clear();
    for (auto item : d->items) 
    {
        d->originalPositions[item] = item->pos();
    }
    
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::min();
    qreal totalHeight = 0;
    
    for (auto item : d->items) 
    {
          QRectF sceneBounds = item->sceneBoundingRect();
    }
    
    for (auto item : d->items)
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        minY = qMin(minY, sceneBounds.top());
        maxY = qMax(maxY, sceneBounds.bottom());
        totalHeight += sceneBounds.width();
    }

    qreal availableSpace = maxY - minY;
    qreal spacing = 0;
    if (d->items.size() > 1)
    {
        spacing = (availableSpace - totalHeight) / (d->items.size() - 1);
    }
    
    std::sort(d->items.begin(), d->items.end(), 
        [](QGraphicsItem* a, QGraphicsItem* b) 
        {
            return a->sceneBoundingRect().top() < b->sceneBoundingRect().top();
        });
    
    qreal currentY = minY;
    for (auto item : d->items) 
    {
        QRectF sceneBounds = item->sceneBoundingRect();
        
        qreal deltaY = currentY - sceneBounds.top();
        
        QPointF newPos = item->pos() + QPointF(0, deltaY);
        
        if (item->parentItem()) 
        {
            QPointF scenePos = QPointF(sceneBounds.x(), currentY);
            newPos = item->parentItem()->mapFromScene(scenePos);
        }
        
        item->setPos(newPos);
        currentY += sceneBounds.height() + spacing;
    }
    lzRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    lzEndLayoutCommand(d->scene);
}
