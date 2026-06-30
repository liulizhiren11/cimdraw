#include "FrameActionBuilder.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

#include "LzFrame.h"

namespace {

QAction* createAction(QObject* parent,
                      const QString& text,
                      const QString& iconPath = QString(),
                      const QKeySequence& shortcut = QKeySequence(),
                      const QString& statusTip = QString())
{
    QAction* action = iconPath.isEmpty()
        ? new QAction(text, parent)
        : new QAction(QIcon(iconPath), text, parent);
    if (!shortcut.isEmpty())
        action->setShortcut(shortcut);
    if (!statusTip.isEmpty())
        action->setStatusTip(statusTip);
    return action;
}

} // namespace

FrameActionBuilder::FileActionsResult FrameActionBuilder::installFileActions(QMainWindow* host, LzFrame* frame) const
{
    FileActionsResult result;
    if (!host || !frame)
        return result;

    QMenu* fileMenu = host->menuBar()->addMenu(QObject::tr("文件"));
    QToolBar* fileBar = host->addToolBar(QObject::tr("File"));

    QAction* fileNewAction = createAction(host,
        QObject::tr("新建电力工程"),
        QStringLiteral(":/image/file_new.png"),
        QKeySequence(QObject::tr("Ctrl+N")),
        QObject::tr("新建电力工程"));
    QObject::connect(fileNewAction, &QAction::triggered, frame, &LzFrame::newPowerScene);

    QAction* fileNewPowerAction = createAction(host,
        QObject::tr("新建电力工程"),
        QString(),
        QKeySequence(),
        QObject::tr("新建电力工程"));
    QObject::connect(fileNewPowerAction, &QAction::triggered, frame, &LzFrame::newPowerScene);

    QAction* fileOpenAction = createAction(host,
        QObject::tr("打开"),
        QStringLiteral(":/image/file_open.png"),
        QKeySequence(QObject::tr("Ctrl+O")),
        QObject::tr("打开"));
    QObject::connect(fileOpenAction, &QAction::triggered, frame, &LzFrame::onFileOpen);

    QAction* importCimAction = createAction(host,
        QObject::tr("导入 CIM/CGMES"),
        QString(),
        QKeySequence(),
        QObject::tr("导入 CIM/CGMES 数据目录"));
    QObject::connect(importCimAction, &QAction::triggered, frame, &LzFrame::onImportCimCgmes);

    result.actionFileSave = createAction(host,
        QObject::tr("保存"),
        QStringLiteral(":/image/file_save.png"),
        QKeySequence(QObject::tr("Ctrl+S")),
        QObject::tr("保存"));
    QObject::connect(result.actionFileSave, &QAction::triggered, frame, &LzFrame::onFileSave);

    result.actionFileSaveAs = createAction(host,
        QObject::tr("另存为"),
        QString(),
        QKeySequence(QObject::tr("Ctrl+Shift+S")),
        QObject::tr("另存为"));
    QObject::connect(result.actionFileSaveAs, &QAction::triggered, frame, &LzFrame::onFileSaveAs);

    QAction* exitAction = createAction(host,
        QObject::tr("退出"),
        QStringLiteral(":/image/file_exit.png"),
        QKeySequence(QObject::tr("Esc")),
        QObject::tr("退出"));
    QObject::connect(exitAction, &QAction::triggered, frame, &QWidget::close);

    fileMenu->addAction(fileNewAction);
    fileMenu->addAction(fileNewPowerAction);
    fileMenu->addAction(fileOpenAction);
    fileMenu->addAction(importCimAction);
    fileMenu->addAction(result.actionFileSave);
    fileMenu->addAction(result.actionFileSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    fileBar->addAction(fileNewAction);
    fileBar->addAction(fileNewPowerAction);
    fileBar->addAction(fileOpenAction);
    fileBar->addAction(importCimAction);
    fileBar->addAction(result.actionFileSave);
    return result;
}

FrameActionBuilder::EditActionsResult FrameActionBuilder::installEditActions(QMainWindow* host, LzFrame* frame) const
{
    EditActionsResult result;
    if (!host || !frame)
        return result;

    QMenu* editMenu = host->menuBar()->addMenu(QObject::tr("编辑(&E)"));
    QToolBar* editToolBar = host->addToolBar(QObject::tr("Edit Tools"));

    QAction* actionEditCut = createAction(host,
        QObject::tr("剪切"),
        QStringLiteral(":/image/edit_cut.png"),
        QKeySequence(QObject::tr("Ctrl+X")),
        QObject::tr("剪切"));
    QObject::connect(actionEditCut, &QAction::triggered, frame, &LzFrame::editCut);

    QAction* actionEditCopy = createAction(host,
        QObject::tr("复制"),
        QStringLiteral(":/image/edit_copy.png"),
        QKeySequence(QObject::tr("Ctrl+C")),
        QObject::tr("复制"));
    QObject::connect(actionEditCopy, &QAction::triggered, frame, &LzFrame::editCopy);

    QAction* actionEditPaste = createAction(host,
        QObject::tr("粘贴"),
        QStringLiteral(":/image/edit_paste.png"),
        QKeySequence(QObject::tr("Ctrl+V")),
        QObject::tr("粘贴"));
    QObject::connect(actionEditPaste, &QAction::triggered, frame, &LzFrame::editPaste);

    result.actionEditUndo = createAction(host,
        QObject::tr("撤销"),
        QStringLiteral(":/image/edit_undo.png"),
        QKeySequence::Undo,
        QObject::tr("撤销"));
    QObject::connect(result.actionEditUndo, &QAction::triggered, frame, &LzFrame::onEditUndo);

    QAction* actionDelete = createAction(host,
        QObject::tr("删除"),
        QStringLiteral(":/image/edit_delete.png"),
        QKeySequence(QObject::tr("Delete")));
    QObject::connect(actionDelete, &QAction::triggered, frame, &LzFrame::onDeleteItem);

    result.actionEditRedo = createAction(host,
        QObject::tr("还原"),
        QStringLiteral(":/image/edit_redo.png"),
        QKeySequence::Redo,
        QObject::tr("还原"));
    QObject::connect(result.actionEditRedo, &QAction::triggered, frame, &LzFrame::onEditRedo);

    QAction* actionGroup = createAction(host,
        QObject::tr("分组"),
        QStringLiteral(":/image/group.png"),
        QKeySequence(),
        QObject::tr("分组"));
    QObject::connect(actionGroup, &QAction::triggered, frame, &LzFrame::onEditGroup);

    QAction* actionUnGroup = createAction(host,
        QObject::tr("取消分组"),
        QStringLiteral(":/image/ungroup.png"),
        QKeySequence(),
        QObject::tr("取消分组"));
    QObject::connect(actionUnGroup, &QAction::triggered, frame, &LzFrame::onEditUnGroup);

    editMenu->addAction(actionEditCut);
    editMenu->addAction(actionEditCopy);
    editMenu->addAction(actionEditPaste);
    editMenu->addAction(actionDelete);
    editMenu->addAction(actionGroup);
    editMenu->addAction(actionUnGroup);
    editMenu->addSeparator();
    editMenu->addAction(result.actionEditUndo);
    editMenu->addAction(result.actionEditRedo);

    editToolBar->addAction(actionEditCut);
    editToolBar->addAction(actionEditCopy);
    editToolBar->addAction(actionEditPaste);
    editToolBar->addAction(actionGroup);
    editToolBar->addAction(actionUnGroup);
    editToolBar->addSeparator();
    editToolBar->addAction(result.actionEditUndo);
    editToolBar->addAction(result.actionEditRedo);

    result.actionEditUndo->setEnabled(false);
    result.actionEditRedo->setEnabled(false);
    return result;
}

void FrameActionBuilder::installViewActions(QMainWindow* host, LzFrame* frame) const
{
    if (!host || !frame)
        return;

    QMenu* viewToolMenu = host->menuBar()->addMenu(QObject::tr("视图"));
    QToolBar* viewToolBar = host->addToolBar(QObject::tr("View Tools"));

    QAction* normalViewAction = createAction(host,
        QObject::tr("实际大小"),
        QStringLiteral(":/image/view_normal.png"),
        QKeySequence(QObject::tr("Shift+N")),
        QObject::tr("实际大小"));
    QObject::connect(normalViewAction, &QAction::triggered, frame, &LzFrame::onNormalView);

    QAction* bestfitViewAction = createAction(host,
        QObject::tr("适合大小"),
        QStringLiteral(":/image/view_bestfit.png"),
        QKeySequence(QObject::tr("Shift+F")),
        QObject::tr("适合大小"));
    QObject::connect(bestfitViewAction, &QAction::triggered, frame, &LzFrame::onBestfitView);

    QAction* zoomInViewAction = createAction(host,
        QObject::tr("放大"),
        QStringLiteral(":/image/view_zoomin.png"),
        QKeySequence(QObject::tr("Shift+I")),
        QObject::tr("放大"));
    QObject::connect(zoomInViewAction, &QAction::triggered, frame, &LzFrame::onZoomIn);

    QAction* zoomOutViewAction = createAction(host,
        QObject::tr("缩小"),
        QStringLiteral(":/image/view_zoomout.png"),
        QKeySequence(QObject::tr("Shift+O")),
        QObject::tr("缩小"));
    QObject::connect(zoomOutViewAction, &QAction::triggered, frame, &LzFrame::onZoomOut);

    QAction* resetZoomAction = createAction(host,
        QObject::tr("重置缩放"),
        QStringLiteral(":/image/view_zoom_reset.png"),
        QKeySequence(QObject::tr("Shift+R")),
        QObject::tr("重置缩放"));
    QObject::connect(resetZoomAction, &QAction::triggered, frame, &LzFrame::onZoomReset);

    viewToolMenu->addAction(normalViewAction);
    viewToolMenu->addAction(bestfitViewAction);
    viewToolMenu->addAction(zoomInViewAction);
    viewToolMenu->addAction(zoomOutViewAction);
    viewToolMenu->addAction(resetZoomAction);

    viewToolBar->addAction(normalViewAction);
    viewToolBar->addAction(bestfitViewAction);
    viewToolBar->addAction(zoomInViewAction);
    viewToolBar->addAction(zoomOutViewAction);
    viewToolBar->addAction(resetZoomAction);
}

void FrameActionBuilder::installTopologyActions(QMainWindow* host, LzFrame* frame) const
{
    if (!host || !frame)
        return;

    QMenu* topologyMenu = host->menuBar()->addMenu(QObject::tr("拓扑"));
    QMenu* powerMenu = host->menuBar()->addMenu(QObject::tr("电力分析"));

    QAction* generateAction = createAction(host, QObject::tr("生成综合关系拓扑图"));
    QObject::connect(generateAction, &QAction::triggered, frame, &LzFrame::onGenerateTopologyGraph);

    QAction* generatePowerAction = createAction(host, QObject::tr("生成电力拓扑图"));
    QObject::connect(generatePowerAction, &QAction::triggered, frame, &LzFrame::onGeneratePowerTopologyGraph);

    QAction* summaryAction = createAction(host, QObject::tr("当前选中图元拓扑摘要"));
    QObject::connect(summaryAction, &QAction::triggered, frame, &LzFrame::onShowTopologySummary);

    QAction* highlightAction = createAction(host, QObject::tr("高亮当前选中图元相关拓扑"));
    QObject::connect(highlightAction, &QAction::triggered, frame, &LzFrame::onHighlightSelectedTopology);

    QAction* clearHighlightAction = createAction(host, QObject::tr("清除拓扑高亮"));
    QObject::connect(clearHighlightAction, &QAction::triggered, frame, &LzFrame::onClearTopologyHighlights);

    topologyMenu->addAction(generateAction);
    topologyMenu->addSeparator();
    topologyMenu->addAction(summaryAction);
    topologyMenu->addAction(highlightAction);
    topologyMenu->addSeparator();
    topologyMenu->addAction(clearHighlightAction);

    powerMenu->addAction(generatePowerAction);
}

void FrameActionBuilder::installAlignActions(QMainWindow* host, LzFrame* frame) const
{
    if (!host || !frame)
        return;

    QMenu* alignMenu = host->menuBar()->addMenu(QObject::tr("对齐"));
    QToolBar* alignBar = host->addToolBar(QObject::tr("Align tool"));

    QAction* leftAlignAction = createAction(host, QObject::tr("左对齐"), QStringLiteral(":/image/align_left.png"));
    QObject::connect(leftAlignAction, &QAction::triggered, frame, &LzFrame::onLeftAlign);

    QAction* rightAlignAction = createAction(host, QObject::tr("右对齐"), QStringLiteral(":/image/align_right.png"));
    QObject::connect(rightAlignAction, &QAction::triggered, frame, &LzFrame::onRightAlign);

    QAction* topAlignAction = createAction(host, QObject::tr("上对齐"), QStringLiteral(":/image/align_up.png"));
    QObject::connect(topAlignAction, &QAction::triggered, frame, &LzFrame::onTopAlign);

    QAction* bottomAlignAction = createAction(host, QObject::tr("下对齐"), QStringLiteral(":/image/align_bottom.png"));
    QObject::connect(bottomAlignAction, &QAction::triggered, frame, &LzFrame::onBottomAlign);

    QAction* vCenterAlignAction = createAction(host, QObject::tr("垂直对齐"), QStringLiteral(":/image/align_vcenter.png"));
    QObject::connect(vCenterAlignAction, &QAction::triggered, frame, &LzFrame::onVcenterAlign);

    QAction* hCenterAlignAction = createAction(host, QObject::tr("水平对齐"), QStringLiteral(":/image/align_hcenter.png"));
    QObject::connect(hCenterAlignAction, &QAction::triggered, frame, &LzFrame::onHcenterAlign);

    QAction* autoRowAction = createAction(host, QObject::tr("横向分布"), QStringLiteral(":/image/rowDistribution.png"));
    QObject::connect(autoRowAction, &QAction::triggered, frame, &LzFrame::onAutoRow);

    QAction* autoColAction = createAction(host, QObject::tr("纵向分布"), QStringLiteral(":/image/colDistribution.png"));
    QObject::connect(autoColAction, &QAction::triggered, frame, &LzFrame::onAutoCol);

    QAction* sameWidthAction = createAction(host, QObject::tr("等宽"), QStringLiteral(":/image/same_width.png"));
    QObject::connect(sameWidthAction, &QAction::triggered, frame, &LzFrame::onSameWidth);

    QAction* sameHeightAction = createAction(host, QObject::tr("等高"), QStringLiteral(":/image/same_width.png"));
    QObject::connect(sameHeightAction, &QAction::triggered, frame, &LzFrame::onSameHeight);

    QAction* sameSizeAction = createAction(host, QObject::tr("等大小"), QStringLiteral(":/image/same_width.png"));
    QObject::connect(sameSizeAction, &QAction::triggered, frame, &LzFrame::onSameSize);

    QAction* bringFrontAction = createAction(host, QObject::tr("置于顶层"), QStringLiteral(":/image/bring_front.png"));
    QObject::connect(bringFrontAction, &QAction::triggered, frame, &LzFrame::onBringFont);

    QAction* bringBackAction = createAction(host, QObject::tr("置于底层"), QStringLiteral(":/image/send_back.png"));
    QObject::connect(bringBackAction, &QAction::triggered, frame, &LzFrame::onBringBack);

    alignMenu->addAction(leftAlignAction);
    alignMenu->addAction(rightAlignAction);
    alignMenu->addAction(topAlignAction);
    alignMenu->addAction(bottomAlignAction);
    alignMenu->addAction(vCenterAlignAction);
    alignMenu->addAction(hCenterAlignAction);
    alignMenu->addAction(autoRowAction);
    alignMenu->addAction(autoColAction);
    alignMenu->addAction(bringFrontAction);
    alignMenu->addAction(bringBackAction);

    alignBar->addAction(leftAlignAction);
    alignBar->addAction(rightAlignAction);
    alignBar->addAction(topAlignAction);
    alignBar->addAction(bottomAlignAction);
    alignBar->addAction(vCenterAlignAction);
    alignBar->addAction(hCenterAlignAction);
    alignBar->addAction(autoRowAction);
    alignBar->addAction(autoColAction);
    alignBar->addAction(bringFrontAction);
    alignBar->addAction(bringBackAction);

    Q_UNUSED(sameWidthAction);
    Q_UNUSED(sameHeightAction);
    Q_UNUSED(sameSizeAction);
}

void FrameActionBuilder::installHelpMenu(QMainWindow* host, LzFrame* frame) const
{
    if (!host || !frame)
        return;

    QMenu* aboutMenu = host->menuBar()->addMenu(QObject::tr("帮助(&H)"));

    QAction* aboutAction = createAction(host, QObject::tr("关于(&A)"), QStringLiteral(":/image/about.png"));
    QObject::connect(aboutAction, &QAction::triggered, frame, &LzFrame::about);
    aboutMenu->addAction(aboutAction);

    QAction* aboutQtAction = createAction(host, QObject::tr("关于Qt(&Q)"));
    QObject::connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    aboutMenu->addAction(aboutQtAction);
}
