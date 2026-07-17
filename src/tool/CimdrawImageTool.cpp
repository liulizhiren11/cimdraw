#include <QApplication>
#include <QPainter>
#include <QDebug>

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "Item/CimdrawImage.h"
#include "CimdrawListWidgetItem.h"
#include "Command/CimdrawAddCommand.h"
#include "CimdrawImageTool.h"

CimdrawImageTool::CimdrawImageTool(QObject* parent) : CimdrawTool(parent)
{
    name = tr("image");
    createMode = CREATE_MODE_DRAG;
}

CimdrawImageTool::~CimdrawImageTool()
{

}

CimdrawDrawTypeId CimdrawImageTool::getDrawType() const
{
    return CIMDRAW_IMAGE;
}

QGraphicsItem* CimdrawImageTool::createObject(CimdrawScene* scene, QRectF* pos, bool addToScene)
{
    CimdrawImage* item = new CimdrawImage(imagePath,*pos);
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

QIcon CimdrawImageTool::getIcon(const QSize& size, const QVariant& para) const
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

void CimdrawImageTool::onMousePress(CimdrawScene* scene, QGraphicsSceneMouseEvent* evt)
{
    if (evt->button() == Qt::RightButton)
    {
        CimdrawToolManager::getInstance()->changeTool(CIMDRAW_SELECTION);
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
        CimdrawView* view = scene->getView();
        view->setModified(true);
        QUndoCommand* command = new CimdrawAddCommand(pObj,scene,list);
        view->getStack()->push(command);
        emit view->editChanged();
    }
}

void CimdrawImageTool::setFilePath(const QString& path)
{
    imagePath = path;
}

QString CimdrawImageTool::filePath() const
{
    return imagePath;
}

//QList<CimdrawListWidgetItem*> CimdrawImageTool::createDrawItemList(const QSize& size)
//{
//    QList<CimdrawListWidgetItem*> list;

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

//        CimdrawListWidgetItem* item = new CimdrawListWidgetItem(size, this, enrtries.at(i).fileName());
//        item->setText(displayName);
//        list.append(item);
//    }

//    return list;
//}




