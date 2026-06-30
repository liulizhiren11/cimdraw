#include "CimFrameCoordinator.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "LzScene.h"
#include "LzView.h"
#include "cim/ui/CimBrowserBridge.h"

CimFrameCoordinator::ImportDialogResult CimFrameCoordinator::chooseImportPath(QWidget* host,
                                                                              const QString& defaultPath) const
{
    ImportDialogResult result;
    if (!host)
        return result;

    QMessageBox chooser(host);
    chooser.setWindowTitle(QObject::tr("导入 CIM/CGMES"));
    chooser.setText(QObject::tr("请选择导入方式。"));
    chooser.setInformativeText(QObject::tr("目录导入适合整套 CGMES 包；文件导入会自动定位所选 xml/rdf 文件所在目录。"));
    QPushButton* chooseDirectoryButton = chooser.addButton(QObject::tr("选择目录"), QMessageBox::AcceptRole);
    QPushButton* chooseFileButton = chooser.addButton(QObject::tr("选择文件"), QMessageBox::ActionRole);
    chooser.addButton(QMessageBox::Cancel);
    chooser.exec();

    if (chooser.clickedButton() == chooseDirectoryButton)
    {
        result.selectedPath = QFileDialog::getExistingDirectory(host,
                                                                QObject::tr("选择 CIM/CGMES 数据目录"),
                                                                defaultPath,
                                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    else if (chooser.clickedButton() == chooseFileButton)
    {
        result.selectedPath = QFileDialog::getOpenFileName(host,
                                                           QObject::tr("选择 CIM/CGMES 文件"),
                                                           defaultPath,
                                                           QObject::tr("CIM/CGMES 文件 (*.xml *.rdf);;所有文件 (*)"));
    }

    result.accepted = !result.selectedPath.trimmed().isEmpty();
    return result;
}

bool CimFrameCoordinator::applyImportResult(const CimImportResult& result,
                                            const ImportExecutionContext& context) const
{
    if (!context.host || !context.messageLineEdit)
        return false;

    CimBrowserBridge bridge;
    bridge.updateDock(context.browserDock, result);

    if (!result.success)
    {
        context.messageLineEdit->setText(QObject::tr("CIM 导入失败"));
        QMessageBox::warning(context.host, QObject::tr("导入 CIM/CGMES"), result.errors.join(QLatin1Char('\n')));
        return false;
    }

    if (!context.createTargetView || !context.populateScene)
        return false;

    LzView* view = context.createTargetView();
    if (!view)
        return false;

    if (context.renameCurrentTab)
        context.renameCurrentTab(QObject::tr("CIM 导入场景"));

    LzScene* scene = view->getScene();
    context.populateScene(scene, result.model);
    view->fitSceneToView(scene->getContentsRect());
    view->setModified(true);

    if (context.afterSceneCreated)
        context.afterSceneCreated();

    if (!result.errors.isEmpty())
    {
        context.messageLineEdit->setText(QObject::tr("CIM 导入部分成功：%1 个对象，含 %2 个错误")
                                             .arg(result.model.objectCount())
                                             .arg(result.errors.size()));
        QMessageBox::warning(context.host, QObject::tr("导入 CIM/CGMES"), result.errors.join(QLatin1Char('\n')));
    }
    else if (!result.warnings.isEmpty())
    {
        context.messageLineEdit->setText(QObject::tr("CIM 导入成功：%1 个对象，含 %2 个告警")
                                             .arg(result.model.objectCount())
                                             .arg(result.warnings.size()));
    }
    else
    {
        context.messageLineEdit->setText(QObject::tr("CIM 导入成功：%1 个对象").arg(result.model.objectCount()));
    }
    return true;
}
