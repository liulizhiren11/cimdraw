#include "FrameFileController.h"

#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "LzScene.h"
#include "LzView.h"
#include "LzViewConfig.h"

FrameFileController::SaveResult FrameFileController::saveCurrentView(LzView* view) const
{
    SaveResult result;
    if (!view || !view->isModified())
    {
        qWarning() << "save failed";
        return result;
    }

    LzScene* scene = view->getScene();
    if (!scene)
        return result;

    QString tabName;
    switch (scene->getSceneType())
    {
    case FILE_TYPE::LZ_DRAW:
        if (!view->save(tabName))
            return result;
        result.success = true;
        result.changed = true;
        result.tabName = tabName;
        return result;
    default:
        qWarning() << "save type is error";
        return result;
    }
}

FrameFileController::SaveResult FrameFileController::saveCurrentViewAs(QWidget* host,
                                                                       LzView* view,
                                                                       LzScene* scene) const
{
    SaveResult result;
    if (!host || !view || !scene)
        return result;

    const QString filePathName = QString("%1/../../%2").arg(qApp->applicationDirPath()).arg(FILE_PATH);
    QString saveFileName = QFileDialog::getSaveFileName(host,
                                                        QObject::tr("另存为"),
                                                        filePathName,
                                                        QObject::tr("Lz 场景 (*.xml *.pdw)"),
                                                        nullptr);
    if (saveFileName.isEmpty())
        return result;

    QFileInfo info(saveFileName);
    if (info.suffix().isEmpty())
        saveFileName += QStringLiteral(".xml");

    if (!scene->save(saveFileName))
    {
        QMessageBox::warning(host, QObject::tr("另存为"), QObject::tr("文件保存失败。"));
        return result;
    }

    view->setFileName(saveFileName);
    view->setModified(false);
    result.success = true;
    result.changed = true;
    result.tabName = QFileInfo(saveFileName).fileName();
    return result;
}
