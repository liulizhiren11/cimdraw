#include <QApplication>
#include <QPainter>
#include <QDebug>

#include "LzScene.h"
#include "LzView.h"
#include "Item/LzImage.h"
#include "LzListWidgetItem.h"
#include "Command/LzAddCommand.h"
#include "LzImageTool.h"

LzImageTool::LzImageTool(QObject* parent) : LzTool(parent)
{
    name = tr("image");
    createMode = CREATE_MODE_DRAG;
}

LzImageTool::~LzImageTool()
{

}

LzDrawTypeId LzImageTool::getDrawType() const
{
    return LZ_IMAGE;
}

QGraphicsItem* LzImageTool::createObject(LzScene* scene, QRectF* pos, bool addToScene)
{
    LzImage* item = new LzImage(imagePath,*pos);
    item->setDrawTool(this);
    if (item)
    {
        if (addToScene)
        {
            if (scene)
            {
                scene->cleanSelection();
                scene->addItem(item);
                scene->addSelection(item);
            }
        }
    }
    return item;
}

QIcon LzImageTool::getIcon(const QSize& size, const QVariant& para) const
{
    QString fileName = para.toString();
    QString filePath = QString("%1/../../%2/%3").arg(QApplication::applicationDirPath()).arg(FILE_IMAGE).arg(fileName);
    if (!QFile::exists(filePath))
        return QIcon();

    QPixmap pixmap(filePath);

    qreal factFile = static_cast<qreal>(pixmap.size().width() / pixmap.size().height());
    qreal factNeed = static_cast<qreal>(size.width() / size.height());

    if (factFile > factNeed)
    {
        pixmap = pixmap.scaledToWidth(size.width());
    }
    else
    {
        pixmap = pixmap.scaledToHeight(size.height());
    }

    QPixmap newPix(size);
    newPix.fill(Qt::transparent);
    QPainter p(&newPix);

    if (factFile > factNeed)
    {
        p.drawPixmap(0, size.height() / 2 - pixmap.size().height() / 2, pixmap);
    }
    else
    {
        p.drawPixmap(size.width() / 2 - pixmap.size().width() / 2, 0, pixmap);
    }
    return QIcon(newPix);
}

void LzImageTool::onMousePress(LzScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        LzToolManager::getInstance()->changeTool(LZ_SELECTION);
        scene->clearSelection();
        return;
    }

    QPointF local = evt->scenePos();
    QList<QGraphicsItem*> list = scene->getSelections();

    QRectF rc(local, QSizeF(100, 100));
    QGraphicsItem* pObj = createObject(scene, &rc);
    if (pObj)
    {
        pObj->setPos(local);
        LzView* view = scene->getView();
        view->setModified(true);
        QUndoCommand* command = new LzAddCommand(pObj,scene,list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}

void LzImageTool::setFilePath(const QString& path)
{
    imagePath = path;
}

QString LzImageTool::filePath() const
{
    return imagePath;
}

//QList<LzListWidgetItem*> LzImageTool::createDrawItemList(const QSize& size)
//{
//    QList<LzListWidgetItem*> list;

//    QString filepath = QString("%1/../../%2").arg(QApplication::applicationDirPath()).arg(FILE_IMAGE);
//    if (filepath.isEmpty())
//    {
//        return list;
//    }

//    QDir d(filepath);
//    if (!d.exists())
//    {
//        return list;
//    }

//    d.setFilter(QDir::Files | QDir::NoDotAndDotDot);

//    QFileInfoList enrtries = d.entryInfoList();
//    if (enrtries.count() <= 0)
//    {
//        return list;
//    }

//    int n = enrtries.count();
//    for (int i = 0; i < n; i++)
//    {
//        QString fileNameWithoutSuffix = enrtries.at(i).baseName();

//        // 如果文件名过长，截取前10个字符并添加省略号
//        QString displayName = (fileNameWithoutSuffix.length() > 10) ? fileNameWithoutSuffix.left(10) + "..." : fileNameWithoutSuffix;

//        LzListWidgetItem* item = new LzListWidgetItem(size, this, enrtries.at(i).fileName());
//        item->setText(displayName);
//        list.append(item);
//    }

//    return list;
//}




