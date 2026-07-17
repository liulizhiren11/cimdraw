#include "CimFrameCoordinator.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>

#include "CimdrawScene.h"
#include "CimdrawView.h"
#include "FrameCimWorkbenchController.h"
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

CimFrameCoordinator::ImportPresentation CimFrameCoordinator::buildImportPresentation(const CimImportResult& result)
{
    ImportPresentation presentation;
    const CimImportSummary summary = result.summary();

    if (!result.success)
    {
        if (summary.objectCount > 0)
        {
            presentation.statusMessage =
                QObject::tr("CIM 导入失败：%1 个对象，含 %2 个错误")
                    .arg(summary.objectCount)
                    .arg(summary.errorCount);
        }
        else
        {
            presentation.statusMessage =
                QObject::tr("CIM 导入失败：%1 个错误").arg(summary.errorCount);
        }
    }
    else if (summary.errorCount > 0)
    {
        presentation.statusMessage =
            QObject::tr("CIM 导入部分成功：%1 个对象，含 %2 个错误")
                .arg(summary.objectCount)
                .arg(summary.errorCount);
    }
    else if (summary.warningCount > 0)
    {
        presentation.statusMessage =
            QObject::tr("CIM 导入成功：%1 个对象，含 %2 个告警")
                .arg(summary.objectCount)
                .arg(summary.warningCount);
    }
    else
    {
        presentation.statusMessage =
            QObject::tr("CIM 导入成功：%1 个对象").arg(summary.objectCount);
    }

    QStringList details;
    if (!summary.errors.isEmpty())
    {
        details << QObject::tr("导入错误：");
        for (const QString& error : summary.errors)
            details << QStringLiteral(" - %1").arg(error);
    }
    if (!summary.warnings.isEmpty())
    {
        if (!details.isEmpty())
            details << QString();
        details << QObject::tr("导入告警：");
        for (const QString& warning : summary.warnings)
            details << QStringLiteral(" - %1").arg(warning);
    }
    if (!summary.logs.isEmpty())
    {
        if (!details.isEmpty())
            details << QString();
        details << QObject::tr("导入日志：");
        for (const QString& log : summary.logs)
            details << QStringLiteral(" - %1").arg(log);
    }
    if (details.isEmpty() && !result.success)
        details << summary.diagnosticText();
    presentation.detailText = details.join(QLatin1Char('\n'));
    return presentation;
}

bool CimFrameCoordinator::applyImportResult(const CimImportResult& result,
                                            const ImportExecutionContext& context) const
{
    if (!context.host || !context.messageLineEdit)
        return false;

    const ImportPresentation presentation = buildImportPresentation(result);

    if (!result.success)
    {
        CimBrowserBridge bridge;
        bridge.updateDock(context.browserDock,
                          result,
                          FrameCimWorkbenchController::ImportState::buildGraphicVisualSummaryForQueryState(
                              result.model));
        context.messageLineEdit->setText(presentation.statusMessage);
        QMessageBox::warning(context.host, QObject::tr("导入 CIM/CGMES"), presentation.detailText);
        return false;
    }

    if (!context.createTargetView || !context.populateScene)
        return false;

    CimdrawView* view = context.createTargetView();
    if (!view)
        return false;

    if (context.renameCurrentTab)
        context.renameCurrentTab(QObject::tr("CIM 导入场景"));

    CimdrawScene* scene = view->getScene();
    context.populateScene(scene, result.model);
    view->fitSceneToView(scene->getContentsRect());
    view->setModified(true);

    if (context.afterSceneCreated)
        context.afterSceneCreated();

    if (!result.errors.isEmpty())
    {
        context.messageLineEdit->setText(presentation.statusMessage);
        QMessageBox::warning(context.host, QObject::tr("导入 CIM/CGMES"), presentation.detailText);
    }
    else if (!result.warnings.isEmpty())
    {
        context.messageLineEdit->setText(presentation.statusMessage);
    }
    else
    {
        context.messageLineEdit->setText(presentation.statusMessage);
    }
    return true;
}
