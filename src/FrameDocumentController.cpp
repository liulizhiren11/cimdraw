#include "FrameDocumentController.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>

#include "CimdrawCenterWidget.h"
#include "CimdrawItemConfig.h"
#include "CimdrawScene.h"
#include "CimdrawView.h"

CimdrawView* FrameDocumentController::createPowerScene(const NewSceneContext& context) const
{
    if (!context.centerWidget)
        return nullptr;

    auto* scene = new CimdrawScene;
    auto* view = new CimdrawView;
    view->setPowerScene(scene);
    scene->setView(view);
    scene->setSceneRect(view->rect());

    context.centerWidget->newTab(view);
    if (context.afterCreate)
        context.afterCreate(view, scene);
    if (context.renameCurrentTab)
        context.renameCurrentTab(QObject::tr("未命名 CIMDraw"));
    return view;
}

CimdrawView* FrameDocumentController::openScene(const QString& fileName, const OpenSceneContext& context) const
{
    if (fileName.isEmpty() || !context.createEmptyScene)
        return nullptr;

    CimdrawView* view = context.createEmptyScene();
    if (!view)
        return nullptr;

    QString errorMsg;
    CimdrawScene* scene = view->getScene();
    if (!scene)
        return nullptr;

    if (scene->load(fileName, &errorMsg))
        view->update();
    return view;
}

bool FrameDocumentController::chooseAndOpenScene(QWidget* host, const OpenSceneContext& context) const
{
    const QString filePathName = QString("%1/../../%2").arg(qApp->applicationDirPath()).arg(FILE_PATH);
    const QString openFileName = QFileDialog::getOpenFileName(host,
                                                              QObject::tr("打开文件"),
                                                              filePathName,
                                                              QObject::tr("Cimdraw 场景 (*.xml *.pdw)"),
                                                              nullptr);
    return finalizeOpenedScene(openScene(openFileName, context), openFileName,
                               context.renameCurrentTab, context.afterOpen);
}

bool FrameDocumentController::finalizeOpenedScene(CimdrawView* view,
                                                  const QString& fileNamePath,
                                                  const std::function<void(const QString&)>& renameCurrentTab,
                                                  const std::function<void()>& afterOpen) const
{
    if (fileNamePath.isEmpty() || !view)
        return false;

    view->setFileName(fileNamePath);
    view->setModified(false);
    if (renameCurrentTab)
        renameCurrentTab(QFileInfo(fileNamePath).fileName());
    if (afterOpen)
        afterOpen();
    return true;
}
