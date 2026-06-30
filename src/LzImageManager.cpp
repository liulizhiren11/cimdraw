#include "LzImageManager.h"
#include "LzListWidgetItem.h"
#include "LzImageWidget.h"
#include "Tool/LzImageTool.h"
#include <QListWidget>

LzImageManager::LzImageManager()
    :d_ptr(new LzImageManagerPrivate(this))
{
    Q_D(LzImageManager);
    d->widget = new LzImageWidget;
    d->widget->setViewMode(QListView::IconMode);
    d->widget->setIconSize(QSize(64, 64));
    d->widget->setSpacing(0);
    d->widget->setResizeMode(QListWidget::Adjust);
    d->widget->setMovement(QListWidget::Static);

    d->tool = dynamic_cast<LzImageTool*>(LzToolManager::getInstance()->changeTool(LZ_IMAGE));
//    QList<LzListWidgetItem*> items = d->tool->createDrawItemList(QSize(64, 64));
//    for (int i = 0; i < items.count(); ++i)
//    {
//        d->widget->addItem(items.at(i));
//    }
    LzToolManager::getInstance()->changeTool(LZ_SELECTION);
    connect(d->widget, &LzImageWidget::addImageItem, this, &LzImageManager::onAddImageItem);
}

LzImageManager::~LzImageManager()
{
    Q_D(LzImageManager);
    delete d->widget;
    d->widget = nullptr;
}

void LzImageManager::onAddImageItem(const QList<QString>& fileName)
{
    Q_D(LzImageManager);
    disconnect(d->widget, &LzImageWidget::addImageItem, this, &LzImageManager::onAddImageItem);
    for (auto iter : fileName)
    {
        LzListWidgetItem* item = new LzListWidgetItem(QSize(64, 64), d->tool, iter);
        item->setText(iter);
        d->widget->addItem(item);
    }
    connect(d->widget, &LzImageWidget::addImageItem, this, &LzImageManager::onAddImageItem);
}

QListWidget* LzImageManager::getWidget()
{
    Q_D(const LzImageManager);
    return d->widget;
}
