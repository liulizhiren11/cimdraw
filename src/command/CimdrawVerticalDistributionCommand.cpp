#include "CimdrawVerticalDistributionCommand.h"
#include "CimdrawScene.h"
#include "CimdrawLayoutCommandUtils.h"
#include "item/TmpBase.h"

class CimdrawVerticalDistributionCommandPrivate
{
    Q_DECLARE_PUBLIC(CimdrawVerticalDistributionCommand)
public:
    CimdrawVerticalDistributionCommandPrivate(QList<QGraphicsItem*> items,CimdrawScene* scene,CimdrawVerticalDistributionCommand* command)
        :items(items),scene(scene),q_ptr(command)
    {

    }
    QList<QGraphicsItem*> items;
    QMap<QGraphicsItem*, QPointF> originalPositions;
    CimdrawScene* scene;
    QVector<CimdrawLayoutConnectLineSnapshot> lineSnapshots;
    bool m_hasChange = false;
    CimdrawVerticalDistributionCommand* q_ptr;
};

CimdrawVerticalDistributionCommand::CimdrawVerticalDistributionCommand(QList<QGraphicsItem*> items,CimdrawScene* scene,QUndoCommand* parent)
    :QUndoCommand(parent),d_ptr(new CimdrawVerticalDistributionCommandPrivate(items,scene,this))
{

}

bool CimdrawVerticalDistributionCommand::validateChange()
{
    Q_D(CimdrawVerticalDistributionCommand);
    
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

void CimdrawVerticalDistributionCommand::undo()
{   
    Q_D(CimdrawVerticalDistributionCommand);
    cimdrawBeginLayoutCommand(d->scene);
    for (auto item : d->items) 
    {
        if (d->originalPositions.contains(item))
        {
            item->setPos(d->originalPositions[item]);
        }
    } 
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
    d->originalPositions.clear();
}

void CimdrawVerticalDistributionCommand::redo()
{
    Q_D(CimdrawVerticalDistributionCommand);
    d->lineSnapshots = cimdrawCaptureLayoutConnectLineSnapshots(d->items);
    cimdrawBeginLayoutCommand(d->scene);
    
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
    cimdrawRestoreLayoutConnectLineSnapshots(d->lineSnapshots);
    cimdrawEndLayoutCommand(d->scene);
}
