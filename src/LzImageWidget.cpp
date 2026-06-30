#include "LzItemConfig.h"
#include "LzViewConfig.h"
#include "LzImageWidget.h"
#include "LzViewConfig.h"
#include <QFileDialog>
#include <QApplication>
#include <QMenu>
#include <QDir>

LzImageWidget::LzImageWidget(QWidget* parent)
{
    setViewMode(QListView::IconMode);
    setIconSize(QSize(64, 64));
    setSpacing(0);
    setResizeMode(QListWidget::Adjust);
    setMovement(QListWidget::Static);
}


void LzImageWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu(this);

    QAction* addAction = contextMenu.addAction("添加图片");
    QAction* deleteAction = contextMenu.addAction("删除图片");

    QPoint globalPos = QCursor::pos();
    QPoint viewportPos = viewport()->mapFromGlobal(globalPos);
    QListWidgetItem* itemAtPos = itemAt(viewportPos);

    QAction* selectedAction = contextMenu.exec(QCursor::pos());

    // 处理菜单项的选择
    if (selectedAction == addAction)
    {
        addImage();
    }
    else if (selectedAction == deleteAction)
    {
        deleteSelectedItem(itemAtPos);
    }
}

void LzImageWidget::addImage()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
                this,
                tr("选择图片文件"),
                QString(),
                tr("Images (*.png *.jpg *.bmp)")
                );

    // Check if any files were selected
    if (fileNames.isEmpty())
    {
        return;
    }

    QString targetDir = QString("%1/../../%2").arg(qApp->applicationDirPath()).arg(FILE_IMAGE);

    // Ensure the target directory exists, create it if not
    QDir dir(targetDir);
    if (!dir.exists())
    {
        if (!dir.mkpath(targetDir))
        {
            QMessageBox::warning(this, tr("错误"), tr("无法创建目标目录。"));
            return;
        }
    }

    QList<QString> list;
    // Process each selected file
    for (const QString& fileName : fileNames)
    {
        QFile file(fileName);
        QString baseName = QFileInfo(fileName).fileName();
        QString targetFilePath = targetDir + QDir::separator() + baseName;
        if (file.copy(targetFilePath))
        {
            list.append(baseName);
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法复制文件: ") + fileName);
        }
    }
    emit addImageItem(list);
}

void LzImageWidget::deleteSelectedItem(QListWidgetItem* item)
{
    if (item)
    {
        QString itemName = item->text();
        takeItem(row(item));
        delete item;
        QString targetDir = QString("%1/../%2")
                .arg(QApplication::applicationDirPath())
                .arg(FILE_IMAGE);
        // Ensure the target directory exists
        QDir dir(targetDir);
        if (!dir.exists())
        {
            QMessageBox::warning(this, tr("删除错误"), tr("目标目录不存在: ") + targetDir);
            return;
        }

        QStringList files = dir.entryList(QDir::Files);

        for (const QString& fileName : files)
        {
            if (fileName.startsWith(itemName))
            {
                QString filePath = targetDir + QDir::separator() + fileName;

                QFile file(filePath);

                if (!file.remove())
                {
                    QMessageBox::warning(this, tr("删除错误"), tr("无法删除文件: ") + filePath);
                }
            }
        }
    }
}
