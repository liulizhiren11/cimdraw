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

#include "CimdrawFrame.h"
#include "CimdrawAppConfig.h"
#include "CimdrawDataSourceManager.h"
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
#include "wiring/base/CimdrawWiringSymbolStyle.h"
#include "wiring/power/CimdrawPowerBusbarSectionItem.h"
#include "item/CimdrawTopologyNodeItem.h"
#include "item/CimdrawConnectLine.h"
#include "Item/TmpBase.h"
#include "CimdrawView.h"
#include "CimdrawScene.h"
#include "CimdrawItemConfig.h"
#include "Tool/CimdrawTool.h"
#include "CimdrawCenterWidget.h"
#include "CimdrawToolBoxManager.h"
#include "CimdrawItemController.h"
#include "CimdrawImageManager.h"
#include "CimdrawListWidgetItem.h"
#include "cim/model/CimModel.h"
#include "cim/ui/CimModelBrowserDock.h"
#include "topology/ui/FrameTopologyController.h"


class CimdrawFramePrivate
{
    Q_DECLARE_PUBLIC(CimdrawFrame)
public:
    CimdrawFramePrivate(CimdrawFrame* frame)
        :q_ptr(frame),controledObject(nullptr)
    {

    }
    CimdrawCenterWidget* centralWidget;
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
    CimdrawItemController* control = nullptr;
    CimdrawToolBoxManager* toolBox = nullptr;
    QPointF distance = QPointF(10,10);
    CimdrawFrame* q_ptr;

    CimdrawDataSourceManager* dataSourceManager = nullptr;
    QString appConfigPath;

    QPushButton* bgColorBtn;    // 保存当前view背景颜色按钮指针，以便在切换tab时更新颜色
    QComboBox* wiringSymbolStandardCombo = nullptr;
    FrameCimWorkbenchController::ImportState cimImportState;
};

CimdrawFrame::CimdrawFrame(QString filePath,QWidget* parent)
    : QMainWindow(parent),d_ptr(new CimdrawFramePrivate(this))
{
    d_ptr->centralWidget = new CimdrawCenterWidget;
    d_ptr->centralWidget->setObjectName(QStringLiteral("editorWorkbench"));
    this->setCentralWidget(d_ptr->centralWidget);

    QTabWidget* tabWiget = d_ptr->centralWidget->getTabWidget();
    connect(tabWiget, SIGNAL(currentChanged(int)), this, SLOT(onTabWidgetChanged(int)));
    connect(d_ptr->centralWidget, &CimdrawCenterWidget::requestNewScene, this, &CimdrawFrame::newPowerScene);
    connect(d_ptr->centralWidget, &CimdrawCenterWidget::currentTabClosed, this, &CimdrawFrame::onCloseTab);
    setWindowTitle(QString::fromUtf8("CIMDraw"));
    this->setMinimumSize(1280, 800);
    initFrameTools();
    FrameWorkbenchStyler workbenchStyler;
    workbenchStyler.apply(this);

    connect(&CimdrawWiringSymbolStyle::instance(), &CimdrawWiringSymbolStyle::standardChanged,
            this, &CimdrawFrame::onWiringSymbolStandardChanged);

    d_ptr->dataSourceManager = new CimdrawDataSourceManager(this);
    d_ptr->appConfigPath = QCoreApplication::applicationDirPath() + QStringLiteral("/cimdraw_data.json");
    FrameDataSourceCoordinator dataSourceCoordinator;
    dataSourceCoordinator.initializeManager(d_ptr->dataSourceManager,
                                            d_ptr->appConfigPath,
                                            std::bind(&CimdrawFrame::activeScene, this));

    this->showMaximized();
}

CimdrawFrame::~CimdrawFrame()
{

}

CimdrawScene* CimdrawFrame::activeScene()
{
    Q_D(CimdrawFrame);
    FrameTopologySlotWorkbenchController controller;
    return controller.currentScene(d->centralWidget);
}

void CimdrawFrame::initFrameTools()
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

void CimdrawFrame::installFileActions()
{
    FrameActionBuilder builder;
    const FrameActionBuilder::FileActionsResult result = builder.installFileActions(this, this);
    d_ptr->actionFileSave = result.actionFileSave;
    d_ptr->actionFileSaveAs = result.actionFileSaveAs;
}

void CimdrawFrame::installEditToolActions()
{
    FrameActionBuilder builder;
    const FrameActionBuilder::EditActionsResult result = builder.installEditActions(this, this);
    d_ptr->actionEditUndo = result.actionEditUndo;
    d_ptr->actionEditRedo = result.actionEditRedo;
    onUpdateRedoUndo();
}

void CimdrawFrame::installViewToolActions()
{
    FrameActionBuilder builder;
    builder.installViewActions(this, this);
}

void CimdrawFrame::installToolActions()
{
    FrameActionBuilder builder;
    builder.installTopologyActions(this, this);
}

void CimdrawFrame::initAlignAction()
{
    FrameActionBuilder builder;
    builder.installAlignActions(this, this);
}

void CimdrawFrame::initOtherMenu()
{
    FrameActionBuilder builder;
    builder.installHelpMenu(this, this);
}

void CimdrawFrame::initDocumentSizeToolBar()
{
    Q_D(CimdrawFrame);
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::CanvasToolbarResult result = builder.createCanvasToolbar(this);
    d_ptr->bgColorBtn = result.backgroundColorButton;

    connect(d_ptr->bgColorBtn, &QPushButton::clicked, this, &CimdrawFrame::onChooseBackgroundColor);
    connect(result.backgroundImageButton, &QPushButton::clicked, this, &CimdrawFrame::onChooseBackgroundImage);
}

void CimdrawFrame::initWiringSymbolStandardToolBar()
{
    Q_D(CimdrawFrame);
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::WiringToolbarResult result = builder.createWiringToolbar(this);
    d->wiringSymbolStandardCombo = result.standardCombo;

    const CimdrawWiringSymbolStandard current = CimdrawWiringSymbolStyle::instance().standard();
    for (int i = 0; i < d->wiringSymbolStandardCombo->count(); ++i)
    {
        if (d->wiringSymbolStandardCombo->itemData(i).toInt() == static_cast<int>(current))
        {
            d->wiringSymbolStandardCombo->setCurrentIndex(i);
            break;
        }
    }

    connect(d->wiringSymbolStandardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &CimdrawFrame::onWiringSymbolStandardUiChanged);
}

void CimdrawFrame::onWiringSymbolStandardUiChanged(int index)
{
    Q_D(CimdrawFrame);
    Q_UNUSED(index);
    FrameCanvasSlotWorkbenchController controller;
    controller.applySelectedWiringStandard(d->wiringSymbolStandardCombo, d->appConfigPath);
}

void CimdrawFrame::onWiringSymbolStandardChanged()
{
    Q_D(CimdrawFrame);
    QSignalBlocker blocker(d->wiringSymbolStandardCombo);
    FrameCanvasSlotWorkbenchController controller;
    controller.applyWiringStandardChange(d->wiringSymbolStandardCombo,
                                         d->toolBox,
                                         d->centralWidget,
                                         CimdrawWiringSymbolStyle::instance().standard());
}

void CimdrawFrame::refreshAllWiringItemPaint()
{
    Q_D(CimdrawFrame);
    FrameCanvasSlotWorkbenchController controller;
    controller.refreshAllWiringItemPaint(d->centralWidget);
}

void CimdrawFrame::initStatusBar()
{
    FrameWorkspaceBuilder builder;
    const FrameWorkspaceBuilder::StatusBarResult result = builder.createStatusBar(this);
    d_ptr->xLineEdit = result.xLineEdit;
    d_ptr->yLineEdit = result.yLineEdit;
    d_ptr->errLineEdit = result.messageLineEdit;
}

void CimdrawFrame::initDrawItemsDock()
{
    FrameDockBuilder builder;
    const FrameDockBuilder::DrawItemsDockResult result = builder.createDrawItemsDock(this, this);
    d_ptr->toolBox = result.toolBox;
}

void CimdrawFrame::initPropertyDock()
{
    FrameDockBuilder builder;
    const FrameDockBuilder::PropertyDockResult result = builder.createPropertyDock(this, this);
    d_ptr->pAttrTabWidget = result.tabWidget;
    d_ptr->control = result.controller;
}

bool CimdrawFrame::openFile()
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.openFile(this, [this, &lifecycle]()
    {
        return lifecycle.makeOpenSceneContext(this);
    });
}

bool CimdrawFrame::openFile(const QString& fileNamePath)
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.openFile(fileNamePath, [this, &lifecycle]()
    {
        return lifecycle.makeOpenSceneContext(this);
    });
}

void CimdrawFrame::initShortCut()
{
    FrameShortcutBuilder builder;
    builder.installCommonShortcuts(this, this);
}

void CimdrawFrame::onUpdateRedoUndo()
{
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncActionStates(d_ptr->centralWidget,
                                d_ptr->actionEditUndo,
                                d_ptr->actionEditRedo,
                                d_ptr->actionFileSave,
                                d_ptr->actionFileSaveAs);
}

void CimdrawFrame::onUpdateFileActions()
{
    onUpdateRedoUndo();
}

void CimdrawFrame::onShowTopologySummary()
{
    Q_D(CimdrawFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::ShowSelectedSummary);
}

void CimdrawFrame::onHighlightSelectedTopology()
{
    Q_D(CimdrawFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::HighlightSelected);
}

void CimdrawFrame::onClearTopologyHighlights()
{
    Q_D(CimdrawFrame);
    FrameTopologySlotWorkbenchController controller;
    controller.dispatch(this,
                        d->centralWidget,
                        FrameTopologySlotWorkbenchController::Action::ClearHighlights);
}

void CimdrawFrame::onGenerateTopologyGraph()
{
    generateTopologyGraphForMode(static_cast<int>(FrameTopologyController::GeneratedMode::Combined));
}

void CimdrawFrame::onGeneratePowerTopologyGraph()
{
    generateTopologyGraphForMode(static_cast<int>(FrameTopologyController::GeneratedMode::PowerSystem));
}

void CimdrawFrame::generateTopologyGraphForMode(int modeValue)
{
    Q_D(CimdrawFrame);
    FrameTopologySlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    controller.dispatchGenerateGraph(
        this,
        d->centralWidget,
        [this, &lifecycle](CimdrawScene* sourceScene)
        {
            return lifecycle.makeTopologyContext(this, sourceScene);
        },
        static_cast<FrameTopologyController::GeneratedMode>(modeValue));
}

void CimdrawFrame::about()
{
    QMessageBox::about(this, "CIMDraw", tr("CIMDraw V1.0.0\nCopyright ©2011-%1 Inc. All Rights Reserved.")
                       .arg(QDateTime::currentDateTime().date().year()));
}

CimdrawView* CimdrawFrame::newPowerScene()
{
    FrameDocumentSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    return controller.createPowerScene([this, &lifecycle]()
    {
        return lifecycle.makeNewSceneContext(this, d_ptr->centralWidget);
    });
}

void CimdrawFrame::onEditUndo()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Undo);
}

void CimdrawFrame::onEditRedo()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Redo);
}

void CimdrawFrame::onEditGroup()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Group);
}

void CimdrawFrame::onEditUnGroup()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Ungroup);
}

void CimdrawFrame::onDrawToolChanged(QListWidgetItem *item)
{
    FrameToolSelectionSlotWorkbenchController controller;
    controller.activateDrawTool(item, d_ptr->centralWidget);
}

void CimdrawFrame::onUpdatePostion(const QPointF& point)
{
    FrameCanvasSlotWorkbenchController controller;
    controller.syncPointerPosition(d_ptr->xLineEdit, d_ptr->yLineEdit, point);
}

void CimdrawFrame::onChooseBackgroundColor()
{
    FrameCanvasSlotWorkbenchController controller;
    controller.chooseAndApplyBackgroundColor(this, d_ptr->centralWidget, d_ptr->bgColorBtn);
}

void CimdrawFrame::onChooseBackgroundImage()
{
    FrameCanvasSlotWorkbenchController controller;
    controller.chooseAndApplyBackgroundImage(this, d_ptr->centralWidget);
}

void CimdrawFrame::onLeftAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignLeft);
}

void CimdrawFrame::onRightAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignRight);
}

void CimdrawFrame::onTopAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignTop);
}

void CimdrawFrame::onBottomAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignBottom);
}

void CimdrawFrame::onVcenterAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignVCenter);
}
void CimdrawFrame::onHcenterAlign()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AlignHCenter);
}

void CimdrawFrame::onAutoRow()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AutoRow);
}

void CimdrawFrame::onAutoCol()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::AutoCol);
}

void CimdrawFrame::onSameWidth()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameWidth);
}

void CimdrawFrame::onSameHeight()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameHeight);
}

void CimdrawFrame::onSameSize()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::SameSize);
}

void CimdrawFrame::onBringFont()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BringFront);
}

void CimdrawFrame::onBringBack()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BringBack);
}

void CimdrawFrame::onFileOpen()
{
    openFile();
}

void CimdrawFrame::onFileSave()
{
    FrameDocumentSlotWorkbenchController controller;
    controller.saveCurrentViewAndSyncUi(d_ptr->centralWidget,
                                        d_ptr->actionEditUndo,
                                        d_ptr->actionEditRedo,
                                        d_ptr->actionFileSave,
                                        d_ptr->actionFileSaveAs);
}

void CimdrawFrame::onFileSaveAs()
{
    FrameDocumentSlotWorkbenchController controller;
    controller.saveCurrentViewAsAndSyncUi(this,
                                          d_ptr->centralWidget,
                                          d_ptr->actionEditUndo,
                                          d_ptr->actionEditRedo,
                                          d_ptr->actionFileSave,
                                          d_ptr->actionFileSaveAs);
}

void CimdrawFrame::setTapWidgetName(const QString& widget_name)
{
    FrameWindowStateSlotWorkbenchController controller;
    controller.renameCurrentTab(d_ptr->centralWidget, widget_name);
}

void CimdrawFrame::onTabWidgetChanged(int index)
{
    Q_D(CimdrawFrame);
    Q_UNUSED(index);
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncCurrentTabState(d->centralWidget,
                                   d->bgColorBtn,
                                   &d->cimImportState,
                                   d->actionEditUndo,
                                   d->actionEditRedo,
                                   d->actionFileSave,
                                   d->actionFileSaveAs);
}

void CimdrawFrame::onCurrentObjectChanged(QList<QGraphicsItem*> items)
{
    Q_D(CimdrawFrame);
    FrameWorkbenchUiController controller;
    d->controledObject = controller.handleCurrentObjectChanged(d->control,
                                                               items,
                                                               &d->cimImportState);
}

void CimdrawFrame::onImportCimCgmes()
{
    FrameCimSlotWorkbenchController controller;
    const QString selectedPath = controller.chooseImportPath(this);
    if (selectedPath.trimmed().isEmpty())
        return;
    importCimFromPath(selectedPath);
}

bool CimdrawFrame::importCimFromPath(const QString& path)
{
    FrameCimSlotWorkbenchController controller;
    FrameSceneLifecycleCoordinator lifecycle;
    const FrameCimWorkbenchController::ImportContext context =
        lifecycle.makeCimImportContext(this, d_ptr->errLineEdit);
    return controller.importFromPath(path, context, &d_ptr->cimImportState);
}

void CimdrawFrame::onCimBrowserObjectActivated(const QString& mrid)
{
    FrameCimSlotWorkbenchController controller;
    controller.activateObjectByMrid(mrid, activeScene(), d_ptr->cimImportState);
}

void CimdrawFrame::onDeleteItem()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::RemoveSelection);
}

void CimdrawFrame::onZoomIn()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomIn);
}

void CimdrawFrame::onZoomOut()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomOut);
}

void CimdrawFrame::onZoomReset()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::ZoomReset);
}

void CimdrawFrame::editCut()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Cut);
}

void CimdrawFrame::editCopy()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Copy);
}

void CimdrawFrame::editPaste()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::Paste);
}

void CimdrawFrame::onBestfitView()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::BestFit);
}

void CimdrawFrame::onNormalView()
{
    FrameEditorSlotWorkbenchController controller;
    controller.dispatch(d_ptr->centralWidget, FrameEditorSlotWorkbenchController::Action::NormalView);
}

void CimdrawFrame::onShortcutLeft()
{
    Q_D(CimdrawFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Left);
}

void CimdrawFrame::onShortcutRight()
{
    Q_D(CimdrawFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Right);
}

void CimdrawFrame::onShortcutUp()
{
    Q_D(CimdrawFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Up);
}

void CimdrawFrame::onShortcutDown()
{
    Q_D(CimdrawFrame);
    FrameEditorSlotWorkbenchController controller;
    controller.dispatchShortcut(d->centralWidget,
                                d->distance,
                                FrameEditorSlotWorkbenchController::ShortcutDirection::Down);
}

void CimdrawFrame::onCloseTab()
{
    Q_D(CimdrawFrame);
    FrameWindowStateSlotWorkbenchController controller;
    controller.syncCurrentTabState(d->centralWidget,
                                   d->bgColorBtn,
                                   &d->cimImportState,
                                   d->actionEditUndo,
                                   d->actionEditRedo,
                                   d->actionFileSave,
                                   d->actionFileSaveAs);
}

