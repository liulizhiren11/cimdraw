#include "CimdrawImageManager.h"

#include <QListWidget>

#include "CimdrawImageWidget.h"
#include "CimdrawListWidgetItem.h"
#include "Tool/CimdrawImageTool.h"

CimdrawImageManager::CimdrawImageManager()
    : d_ptr(new CimdrawImageManagerPrivate(this))
{
    Q_D(CimdrawImageManager);
    d->widget = new CimdrawImageWidget;
    d->widget->setViewMode(QListView::IconMode);
    d->widget->setIconSize(QSize(64, 64));
    d->widget->setSpacing(0);
    d->widget->setResizeMode(QListWidget::Adjust);
    d->widget->setMovement(QListWidget::Static);

    d->tool = dynamic_cast<CimdrawImageTool*>(CimdrawToolManager::getInstance()->changeTool(CIMDRAW_IMAGE));
    CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
    connect(d->widget, &CimdrawImageWidget::addImageItem, this, &CimdrawImageManager::onAddImageItem);
}

CimdrawImageManager::~CimdrawImageManager()
{
    Q_D(CimdrawImageManager);
    delete d->widget;
    d->widget = nullptr;
}

void CimdrawImageManager::onAddImageItem(const QList<QString>& fileName)
{
    Q_D(CimdrawImageManager);
    disconnect(d->widget, &CimdrawImageWidget::addImageItem, this, &CimdrawImageManager::onAddImageItem);
    for (const auto& iter : fileName)
    {
        auto* item = new CimdrawListWidgetItem(QSize(64, 64), d->tool, iter);
        item->setText(iter);
        d->widget->addItem(item);
    }
    connect(d->widget, &CimdrawImageWidget::addImageItem, this, &CimdrawImageManager::onAddImageItem);
}

QListWidget* CimdrawImageManager::getWidget()
{
    Q_D(const CimdrawImageManager);
    return d->widget;
}
