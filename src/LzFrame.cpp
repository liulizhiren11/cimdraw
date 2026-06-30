#include <QFileDialog>
#include <QFileInfo>
#include <QProperty>
#include <QMetaProperty>
#include <QColorDialog>
#include <QShortcut>
#include <QCoreApplication>
#include <QComboBox>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QTabWidget>
#include <QMessageBox>
#include <QStringList>
#include <QFrame>
#include <QHash>
#include <QStyle>

#include "LzFrame.h"
#include "LzAppConfig.h"
#include "LzDataSourceManager.h"
#include "FrameActionBuilder.h"
#include "FrameActionStateController.h"
#include "FrameCanvasSlotWorkbenchController.h"
#include "FrameCimSlotWorkbenchController.h"
#include "FrameCimWorkbenchController.h"
#include "FrameDataSourceCoordinator.h"
#include "FrameDockBuilder.h"
#include "FrameDocumentSlotWorkbenchController.h"
#include "FrameEditorSlotWorkbenchController.h"
#include "FrameSceneLifecycleCoordinator.h"
#include "FrameShortcutBuilder.h"
#include "FrameTopologySlotWorkbenchController.h"
#include "FrameToolSelectionSlotWorkbenchController.h"
#include "FrameWindowStateSlotWorkbenchController.h"
#include "FrameWorkbenchUiController.h"
#include "FrameWorkbenchStyler.h"
#include "FrameWorkspaceBuilder.h"
#include "wiring/base/LzWiringSymbolStyle.h"
#include "wiring/power/LzPowerBusbarSectionItem.h"
#include "item/LzTopologyNodeItem.h"
#include "item/LzConnectLine.h"
#include "Item/TmpBase.h"
#include "LzView.h"
#include "LzScene.h"
#include "LzItemConfig.h"
#include "Tool/LzTool.h"
#include "LzCenterWidget.h"
#include "LzItemController.h"
#include "LzImageManager.h"
#include "LzToolBoxManager.h"
#include "LzListWidgetItem.h"
#include "cim/model/CimModel.h"
#include "cim/ui/CimModelBrowserDock.h"
#include "topology/ui/FrameTopologyController.h"


class LzFramePrivate
{
    Q_DECLARE_PUBLIC(LzFrame)
public:
    LzFramePrivate(LzFrame* frame)
        :q_ptr(frame),controledObject(nullptr)
    {

    }
    LzCenterWidget* centralWidget;
    //属性插件
    QTabWidget* pAttrTabWidget = nullptr;
    //状态栏显示错误信息
    QLineEdit* xLineEdit = nullptr;
    //状态栏显示数据库连接信息
    QLineEdit* yLineEdit = nullptr;
    //状态栏显示当前用户信息
    QLineEdit* errLineEdit = nullptr;
    QAction* actionEditUndo = nullptr;
    QAction* actionEditRedo = nullptr;
    QAction* actionFileSave = nullptr;
    QAction* actionFileSaveAs = nullptr;
    QObject* controledObject;
    LzItemController* control = nullptr;
    LzToolBoxManager* toolBox = nullptr;
    QPointF distance = QPointF(10,10);
    LzFrame* q_ptr;

    LzDataSourceManager* dataSourceManager = nullptr;
    QString appConfigPath;

    QPushButton* bgColorBtn;    // 保存当前view背景颜色按钮指针，以便在切换tab时更新颜色
    QComboBox* wiringSymbolStandardCombo = nullptr;
    CimModelBrowserDock* cimBrowserDock = nullptr;
    CimModel lastImportedCimModel;
    QHash<QString, QGraphicsItem*> cimSceneShapeByMrid;
};

LzFrame::LzFrame(QString filePath,QWidget* parent)
    : QMainWindow(parent),d_ptr(new LzFramePrivate(this))
{
    d_ptr->centralWidget = new LzCenterWidget;
    d_ptr->centralWidget->setObjectName(QStringLiteral("editorWorkbench"));
    this->setCentralWidget(d_ptr->centralWidget);

    QTabWidget* tabWiget = d_ptr->centralWidget->getTabWidget();
    connect(tabWiget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetChanged(int)));
    connect(d_ptr->centralWidget, &LzCenterWidget::requestNewScene, this, &LzFrame::newPowerScene);
    connect(d_ptr->centralWidget, &LzCenterWidget::currentTabClosed, this, &LzFrame::onCloseTab);
    setWindowTitle(QString::fromUtf8("电力工程"));
    this->setMinimumSize(1280, 800);
    initFrameTools();
    FrameWorkbenchStyler workbenchStyler;
    workbenchStyler.apply(this);

    connect(&LzWiringSymbolStyle::instance(), &LzWiringSymbolStyle::standardChanged,
            this, &LzFrame::onWiringSymbolStandardChanged);

    d_ptr->dataSourceManager = new LzDataSourceManager(this);
    d_ptr->appConfigPath = QCoreApplication::applicationDirPath() + QStringLiteral("/lz_data.json");
    FrameDataSourceCoordinator dataSourceCoordinator;
    dataSourceCoordinator.initializeManager(d_ptr->dataSourceManager,
                                            d_ptr->appConfigPath,
                                            std::bind(&LzFrame::activeScene, this));

    this->showMaximized();
}

LzFrame::~LzFrame()
{

}

LzScene* LzFrame::activeScene()
{
    Q_D(LzFrame);
    FrameTopologySlotWorkbenchController controller;
    return controller.currentScene(d->centralWidget);
}

void LzFrame::initFrameTools()
{
    //初始化文件工具
    installFileActions();
    //初始化编辑菜单
    installEditToolActions();
    //初始化视图菜单
    installViewToolActions();
    //初始化工具菜单
    installToolActions();
    //初始化对象对齐栏
    initAlignAction();
    //窗口菜单 帮助菜单
    initOtherMenu();
    //初始化画布相关
    initDocumentSizeToolBar();
    initWiringSymbolStandardToolBar();
    //初始化状态栏
    initStatusBar();
    //初始化图元列表
    initDrawItemsDock();
    //初始化图元属性控件
    initPropertyDock();
    //初始化其他快捷键
    initShortCut();
    //更新undo/redo状态
    //onUpdateRedoUndo();
}

void LzFrame::installFileActions()
{
    FrameActionBuilder builder;
    const FrameActionBuilder::FileActionsResult result = builder.installFileActions(this, this);
    d_ptr->actionFileSave = result.actionFileSave;
    d_ptr->actionFileSaveAs = result.actionFileSaveAs;
}

void LzFrame::installEditToolActions()
{
    FrameActionBuilder builder;
    const FrameActionBuilder::EditActionsResult result = builder.installEditActions(this, this);
    d_ptr->actionEditUndo = result.actionEditUndo;
    d_ptr->actionEditRedo = result.actionEditRedo;
    onUpdateRedoUndo();
}

void LzFrame::installViewToolActions()
{
    FrameActionBuilder builder;
    builder.installViewActions(this, this);
}

void LzFrame::installToolActions()
{
    FrameActionBuilder builder;
    builder.installTopologyActions(this, this);
}

void LzFrame::initAlignAction()
{
    FrameActionBuilder builder;
    builder.installAlignActions(this, this);
}

void LzFrame::initOtherMenu()
{
    FrameActionBuilder builder;
    builder.installHelpMenu(this, this);
}

void LzFrame::initDocumentSizeToolBar()
{
    Q_D(LzFrame);
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::CanvasToolbarResult result = builder.createCanvasToolbar(this);
    d_ptr->bgColorBtn = result.backgroundColorButton;

    connect(d_ptr->bgColorBtn, &QPushButton::clicked, this, &LzFrame::onChooseBackgroundColor);
    connect(result.backgroundImageButton, &QPushButton::clicked, this, &LzFrame::onChooseBackgroundImage);
}

void LzFrame::initWiringSymbolStandardToolBar()
{
    Q_D(LzFrame);
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::WiringToolbarResult result = builder.createWiringToolbar(this);
    d->wiringSymbolStandardCombo = result.standardCombo;

    const LzWiringSymbolStandard current = LzWiringSymbolStyle::instance().standard();
    for (int i = 0; i < d->wiringSymbolStandardCombo->count(); ++i)
    {
        if (d->wiringSymbolStandardCombo->itemData(i).toInt() == static_cast<int>(current))
        {
            d->wiringSymbolStandardCombo->setCurrentIndex(i);
            break;
        }
    }

    connect(d->wiringSymbolStandardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &LzFrame::onWiringSymbolStandardUiChanged);
}

void LzFrame::onWiringSymbolStandardUiChanged(int index)
{
    Q_D(LzFrame);
    Q_UNUSED(index);
    FrameCanvasSlotWorkbenchController controller;
    controller.applySelectedWiringStandard(d->wiringSymbolStandardCombo, d->appConfigPath);
}

void LzFrame::onWiringSymbolStandardChanged()
{
    Q_D(LzFrame);
    QSignalBlocker blocker(d->wiringSymbolStandardCombo);
    FrameCanvasSlotWorkbenchController controller;
    controller.applyWiringStandardChange(d->wiringSymbolStandardCombo,
                                         d->toolBox,
                                         d->centralWidget,
                                         LzWiringSymbolStyle::instance().standard());
}

void LzFrame::refreshAllWiringItemPaint()
{
    Q_D(LzFrame);
    FrameCanvasSlotWorkbenchController controller;
    controller.refreshAllWiringItemPaint(d->centralWidget);
}

void LzFrame::initStatusBar()
{
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::StatusBarResult result = builder.createStatusBar(this);
    d_ptr->xLineEdit = result.xLineEdit;
    d_ptr->yLineEdit = result.yLineEdit;
    d_ptr->errLineEdit = result.messageLineEdit;
}

void LzFrame::initDrawItemsDock()
{
    FrameDockBuilder builder;
    const FrameDockBuilder::DrawItemsDockResult result = builder.createDrawItemsDock(this, this);
    d_ptr->toolBox = result.toolBox;
}

void LzFrame::initPropertyDock()
{
    FrameDockBuilder builder;
    const FrameDockBuilder::PropertyDockResult result = builder.createPropertyDock(this, this);
    d_ptr->pAttrTabWidget = result.tabWidget;
    d_ptr->control = result.controller;
}

bool LzFrame::openFile()
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.openFile(this, [this, &lifecycle]()
    {
        return lifecycle.makeOpenSceneContext(this);
    });
}

bool LzFrame::openFile(const QString& fileNamePath)
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.openFile(fileNamePath, [this, &lifecycle]()
    {
        return lifecycle.makeOpenSceneContext(this);
    });
}

void LzFrame::initShortCut()
{
    FrameShortcutBuilder builder;
    builder.installCommonShortcuts(this, this);
}

void LzFrame::onUpdateRedoUndo()
{
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncActionStates(d_ptr->centralWidget,
                                d_ptr->actionEditUndo,
                                d_ptr->actionEditRedo,
                                d_ptr->actionFileSave,
                                d_ptr->actionFileSaveAs);
}

void LzFrame::onUpdateFileActions()
{
    onUpdateRedoUndo();
}

void LzFrame::onShowTopologySummary()
{
    Q_D(LzFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::ShowSelectedSummary);
}

void LzFrame::onHighlightSelectedTopology()
{
    Q_D(LzFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::HighlightSelected);
}

void LzFrame::onClearTopologyHighlights()
{
    Q_D(LzFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::ClearHighlights);
}

void LzFrame::onGenerateTopologyGraph()
{
    generateTopologyGraphForMode(static_cast<int>(FrameTopologyController::GeneratedMode::Combined));
}

void LzFrame::onGeneratePowerTopologyGraph()
{
    generateTopologyGraphForMode(static_cast<int>(FrameTopologyController::GeneratedMode::PowerSystem));
}

void LzFrame::generateTopologyGraphForMode(int modeValue)
{
    Q_D(LzFrame);
    FrameTopologySlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    controller.dispatchGenerateGraph(
        this,
        d->centralWidget,
        [this, &lifecycle](LzScene* sourceScene)
        {
            return lifecycle.makeTopologyContext(this, sourceScene);
        },
        static_cast<FrameTopologyController::GeneratedMode>(modeValue));
}

void LzFrame::about()
{
    QMessageBox::about(this, "lzDraw", tr("lzDraw V1.0.0\nCopyright ©2011-%1 Inc. All Rights Reserved.")
                       .arg(QDateTime::currentDateTime().date().year()));
}

LzView* LzFrame::newPowerScene()
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.createPowerScene([this, &lifecycle]()
    {
        return lifecycle.makeNewSceneContext(this, d_ptr->centralWidget);
    });
}

void LzFrame::onEditUndo()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Undo);
}

void LzFrame::onEditRedo()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Redo);
}

void LzFrame::onEditGroup()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Group);
}

void LzFrame::onEditUnGroup()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Ungroup);
}

void LzFrame::onDrawToolChanged(QListWidgetItem *item)
{
    FrameToolSelectionSlotWorkbenchController controller;
    controller.activateDrawTool(item, d_ptr->centralWidget);
}

void LzFrame::onUpdatePostion(const QPointF& point)
{
    FrameCanvasSlotWorkbenchController controller;
    controller.syncPointerPosition(d_ptr->xLineEdit, d_ptr->yLineEdit, point);
}

void LzFrame::onChooseBackgroundColor()
{
    FrameCanvasSlotWorkbenchController controller;
    controller.chooseAndApplyBackgroundColor(this, d_ptr->centralWidget, d_ptr->bgColorBtn);
}

void LzFrame::onChooseBackgroundImage()
{
    FrameCanvasSlotWorkbenchController controller;
    controller.chooseAndApplyBackgroundImage(this, d_ptr->centralWidget);
}

void LzFrame::onLeftAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignLeft);
}

void LzFrame::onRightAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignRight);
}

void LzFrame::onTopAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignTop);
}

void LzFrame::onBottomAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignBottom);
}

void LzFrame::onVcenterAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignVCenter);
}
void LzFrame::onHcenterAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignHCenter);
}

void LzFrame::onAutoRow()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AutoRow);
}

void LzFrame::onAutoCol()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AutoCol);
}

void LzFrame::onSameWidth()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameWidth);
}

void LzFrame::onSameHeight()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameHeight);
}

void LzFrame::onSameSize()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameSize);
}

void LzFrame::onBringFont()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BringFront);
}

void LzFrame::onBringBack()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BringBack);
}

void LzFrame::onFileOpen()
{
    openFile();
}

void LzFrame::onFileSave()
{
    FrameDocumentSlotWorkbenchController controller;
    controller.saveCurrentViewAndSyncUi(d_ptr->centralWidget,
                                        d_ptr->actionEditUndo,
                                        d_ptr->actionEditRedo,
                                        d_ptr->actionFileSave,
                                        d_ptr->actionFileSaveAs);
}

void LzFrame::onFileSaveAs()
{
    FrameDocumentSlotWorkbenchController controller;
    controller.saveCurrentViewAsAndSyncUi(this,
                                          d_ptr->centralWidget,
                                          d_ptr->actionEditUndo,
                                          d_ptr->actionEditRedo,
                                          d_ptr->actionFileSave,
                                          d_ptr->actionFileSaveAs);
}

void LzFrame::setTapWidgetName(const QString& widget_name)
{
    FrameWindowStateSlotWorkbenchController controller;
    controller.renameCurrentTab(d_ptr->centralWidget, widget_name);
}

void LzFrame::onTabWidgetChanged(int index)
{
    Q_D(LzFrame);
    Q_UNUSED(index);
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncCurrentTabState(d->centralWidget,
                                   d->bgColorBtn,
                                   d->cimBrowserDock,
                                   d->actionEditUndo,
                                   d->actionEditRedo,
                                   d->actionFileSave,
                                   d->actionFileSaveAs);
}

void LzFrame::onCurrentObjectChanged(QList<QGraphicsItem*> items)
{
    Q_D(LzFrame);
    FrameWorkbenchUiController controller;
    d->controledObject = controller.handleCurrentObjectChanged(d->control, items, d->cimBrowserDock);
}

void LzFrame::onImportCimCgmes()
{
    FrameCimSlotWorkbenchController controller;
    const QString selectedPath = controller.chooseImportPath(this);
    if (selectedPath.trimmed().isEmpty())
        return;
    importCimFromPath(selectedPath);
}

bool LzFrame::importCimFromPath(const QString& path)
{
    FrameCimSlotWorkbenchController controller;
    FrameCimWorkbenchController::ImportState state =
        controller.makeImportState(d_ptr->cimBrowserDock,
                                   d_ptr->lastImportedCimModel,
                                   d_ptr->cimSceneShapeByMrid);

    FrameSceneLifecycleCoordinator lifecycle;
    const FrameCimWorkbenchController::ImportContext context =
        lifecycle.makeCimImportContext(this, d_ptr->errLineEdit);
    const bool success = controller.importFromPath(path, context, &state);
    controller.storeImportState(state,
                                d_ptr->cimBrowserDock,
                                d_ptr->lastImportedCimModel,
                                d_ptr->cimSceneShapeByMrid);
    return success;
}

void LzFrame::onCimBrowserObjectActivated(const QString& mrid)
{
    FrameCimSlotWorkbenchController controller;
    const FrameCimWorkbenchController::ImportState state =
        controller.makeActivationState(d_ptr->cimSceneShapeByMrid);
    controller.activateObjectByMrid(mrid, activeScene(), state);
}

void LzFrame::onDeleteItem()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::RemoveSelection);
}

void LzFrame::onZoomIn()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomIn);
}

void LzFrame::onZoomOut()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomOut);
}

void LzFrame::onZoomReset()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomReset);
}

void LzFrame::editCut()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Cut);
}

void LzFrame::editCopy()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Copy);
}

void LzFrame::editPaste()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Paste);
}

void LzFrame::onBestfitView()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BestFit);
}

void LzFrame::onNormalView()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::NormalView);
}

void LzFrame::onShortcutLeft()
{
    Q_D(LzFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Left);
}

void LzFrame::onShortcutRight()
{
    Q_D(LzFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Right);
}

void LzFrame::onShortcutUp()
{
    Q_D(LzFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Up);
}

void LzFrame::onShortcutDown()
{
    Q_D(LzFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Down);
}

void LzFrame::onCloseTab()
{
    Q_D(LzFrame);
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncCurrentTabState(d->centralWidget,
                                   d->bgColorBtn,
                                   d->cimBrowserDock,
                                   d->actionEditUndo,
                                   d->actionEditRedo,
                                   d->actionFileSave,
                                   d->actionFileSaveAs);
}
