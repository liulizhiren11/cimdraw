#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

#include "Tool/LzTool.h"
#include "LzListWidgetItem.h"
#include "LzListWidget.h"

LzListWidget::LzListWidget(QWidget* parent) : QListWidget(parent)
{

}

void LzListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return QListWidget::mousePressEvent(event);
    }

    QListWidgetItem* item = this->itemAt(event->pos());
    LzListWidgetItem* pDrawItem = dynamic_cast<LzListWidgetItem*>(item);
    if (pDrawItem != nullptr)
    {
        if(pDrawItem->getTool()->getCreateMode() != CREATE_MODE_DRAG)
        {
            return QListWidget::mousePressEvent(event);
        }
        QMimeData* mimeData = new QMimeData();
        mimeData->setText(pDrawItem->getTool()->getName());

        const QSize dragSize = QSize(50, 50);
        QIcon icon = pDrawItem->getTool()->getIcon(dragSize);
        QPixmap dragPixmap = icon.pixmap(dragSize);

        QDrag* drag = new QDrag(this);
        drag->setPixmap(dragPixmap);
        drag->setMimeData(mimeData);
        drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
        drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
}

