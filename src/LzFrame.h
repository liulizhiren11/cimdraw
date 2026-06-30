#ifndef LZFRAMW_H
#define LZFRAMW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStatusBar>
#include <QDockWidget>
#include <QListWidget>
#include <QScopedPointer>
#include <QGraphicsItem>
#include <QDateTime>
#include <QApplication>

class LzView;
class LzScene;
class CimModelBrowserDock;
class CimModel;
class LzCenterWidget;
class LzFramePrivate;
class LzItemController;
class LzToolBoxManager;
class QComboBox;

class LzFrame :public QMainWindow
{
    Q_OBJECT
public:
    LzFrame(QString filePath = QString(),QWidget* parent=nullptr);
    ~LzFrame();

    //初始化框架
    void initFrameTools();
    //初始化文件工具（具体装配逻辑已下沉到 builder）
    void installFileActions();
    //初始化编辑菜单（具体装配逻辑已下沉到 builder）
    void installEditToolActions();
    //初始化视图菜单（具体装配逻辑已下沉到 builder）
    void installViewToolActions();
    //初始化工具菜单（具体装配逻辑已下沉到 builder）
    void installToolActions();
    //初始化对象对齐栏（具体装配逻辑已下沉到 builder）
    void initAlignAction();
    //窗口菜单 帮助菜单（具体装配逻辑已下沉到 builder）
    void initOtherMenu();
    //初始化画布相关（具体装配逻辑已下沉到 builder）
    void initDocumentSizeToolBar();
    /** 接线图符号标准（国标/欧标/美标）工具栏，具体装配逻辑已下沉到 builder */
    void initWiringSymbolStandardToolBar();
    //初始化状态栏（具体装配逻辑已下沉到 builder）
    void initStatusBar();
    //初始化图元列表（具体装配逻辑已下沉到 builder）
    void initDrawItemsDock();
    //初始化图元属性控件（具体装配逻辑已下沉到 builder）
    void initPropertyDock();
    //打开文档
    bool openFile();
    //打开文档
    bool openFile(const QString& fileNamePath);
    //其他快捷键
    void initShortCut();

    /** 当前标签页对应的绘图场景；无文档时返回 nullptr（供接线数据 TCP 桥等使用） */
    LzScene* activeScene();
    /** 主窗口兼容入口；具体导入执行已下沉到独立控制模块，后续仅保留 UI 协调职责。 */
    bool importCimFromPath(const QString& path);

protected:
public slots:
    void onUpdateRedoUndo();
    void onUpdateFileActions();
    void about();
    LzView* newPowerScene();
    void onEditUndo();
    void onEditRedo();
    void onEditGroup();
    void onEditUnGroup();
    void onDrawToolChanged(QListWidgetItem *item);
    void onUpdatePostion(const QPointF&);
    void onChooseBackgroundColor();
    void onChooseBackgroundImage();
    void onLeftAlign();
    void onRightAlign();
    void onTopAlign();
    void onBottomAlign();
    void onVcenterAlign();
    void onHcenterAlign();
    void onAutoRow();
    void onAutoCol();
    void onSameWidth();
    void onSameHeight();
    void onSameSize();
    void onBringFont();
    void onBringBack();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void setTapWidgetName(const QString& widget_name);
    void onTabWidgetChanged(int index);
    void onCurrentObjectChanged(QList<QGraphicsItem*> items);
    void onDeleteItem();
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void editCut();
    void editCopy();
    void editPaste();
    void onBestfitView();
    void onNormalView();
    void onShortcutLeft();
    void onShortcutRight();
    void onShortcutUp();
    void onShortcutDown();
    void onCloseTab();
    void onWiringSymbolStandardUiChanged(int index);
    void onWiringSymbolStandardChanged();
    void refreshAllWiringItemPaint();
    void onShowTopologySummary();
    void onHighlightSelectedTopology();
    void onClearTopologyHighlights();
    void onGenerateTopologyGraph();
    void onGeneratePowerTopologyGraph();
    void onImportCimCgmes();
    void onCimBrowserObjectActivated(const QString& mrid);
private:
    void generateTopologyGraphForMode(int mode);
    Q_DECLARE_PRIVATE(LzFrame);
    QScopedPointer<LzFramePrivate> d_ptr;
};
#endif
