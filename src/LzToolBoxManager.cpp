#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStyle>
#include <QToolBox>
#include <QVBoxLayout>
#include <initializer_list>

#include "LzToolBoxManager.h"
#include "Tool/LzTool.h"
#include "Tool/LzRectTool.h"
#include "Tool/LzSelectTool.h"
#include "Tool/LzArcTool.h"
#include "Tool/LzBraceCalloutTool.h"
#include "Tool/LzRoundRectTool.h"
#include "Tool/LzEllipseTool.h"
#include "Tool/LzConnectLineTool.h"
#include "Tool/LzLineTool.h"
#include "Tool/LzPolylineTool.h"
#include "Tool/LzPolygonTool.h"
#include "Tool/LzBezierTool.h"
#include "Tool/LzTextTool.h"
#include "LzImageManager.h"
#include "LzListWidgetItem.h"
#include "LzListWidget.h"
#include "LzFrame.h"

namespace {

void lzConnectToolListIfFrame(QObject* sender, LzFrame* frame)
{
    if (!sender || !frame)
        return;
    QObject::connect(sender, SIGNAL(itemClicked(QListWidgetItem*)), frame, SLOT(onDrawToolChanged(QListWidgetItem*)));
}

void lzSetupToolListWidget(QListWidget* listWidget)
{
    if (!listWidget)
        return;
    listWidget->setObjectName(QStringLiteral("toolPaletteList"));
    listWidget->setFrameShape(QFrame::NoFrame);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(64, 64));
    listWidget->setGridSize(QSize(108, 124));
    listWidget->setSpacing(10);
    listWidget->setResizeMode(QListWidget::Adjust);
    listWidget->setMovement(QListWidget::Static);
    listWidget->setUniformItemSizes(true);
    listWidget->setWordWrap(true);
    listWidget->setTextElideMode(Qt::ElideNone);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setFocusPolicy(Qt::NoFocus);
}

void lzAppendToolItems(QListWidget* listWidget, std::initializer_list<LzDrawTypeId> drawTypes)
{
    if (!listWidget)
        return;
    for (LzDrawTypeId drawType : drawTypes)
        listWidget->addItem(new LzListWidgetItem(QSize(64, 64), LzToolManager::getInstance()->changeTool(drawType)));
}

}

LzToolBoxManager::LzToolBoxManager(LzFrame* frame)
    : QObject(frame)
    , frame(frame)
{
    rootWidget_ = new QWidget;
    rootWidget_->setObjectName(QStringLiteral("leftToolSidebar"));

    auto* rootLayout = new QHBoxLayout(rootWidget_);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    activityBar_ = new QListWidget(rootWidget_);
    activityBar_->setObjectName(QStringLiteral("leftToolActivityBar"));
    activityBar_->setViewMode(QListView::IconMode);
    activityBar_->setFlow(QListView::TopToBottom);
    activityBar_->setMovement(QListView::Static);
    activityBar_->setResizeMode(QListWidget::Adjust);
    activityBar_->setWrapping(false);
    activityBar_->setSpacing(4);
    activityBar_->setFrameShape(QFrame::NoFrame);
    activityBar_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    activityBar_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    activityBar_->setSelectionMode(QAbstractItemView::SingleSelection);
    activityBar_->setFocusPolicy(Qt::NoFocus);
    activityBar_->setIconSize(QSize(20, 20));
    activityBar_->setGridSize(QSize(48, 48));
    activityBar_->setFixedWidth(56);

    detailHost_ = new QFrame(rootWidget_);
    detailHost_->setObjectName(QStringLiteral("leftToolDetailHost"));
    auto* detailLayout = new QVBoxLayout(detailHost_);
    detailLayout->setContentsMargins(0, 0, 0, 0);
    detailLayout->setSpacing(0);

    panelStack_ = new QStackedWidget(detailHost_);
    panelStack_->setObjectName(QStringLiteral("leftToolPanelStack"));
    detailLayout->addWidget(panelStack_);

    rootLayout->addWidget(activityBar_, 0);
    rootLayout->addWidget(detailHost_, 1);

    rootWidget_->setStyleSheet(QStringLiteral(
        "#leftToolSidebar { background: #f3f6fb; }"
        "#leftToolActivityBar { background: #e9eef5; border: none; border-right: 1px solid #d7dde8; }"
        "#leftToolActivityBar::item { margin: 4px 6px; padding: 8px; border-radius: 8px; }"
        "#leftToolActivityBar::item:selected { background: #d8e6fb; }"
        "#leftToolActivityBar::item:hover:!selected { background: #dde5ef; }"
        "#leftToolDetailHost { background: #ffffff; border-left: 1px solid #d7dde8; }"
        "QLabel#leftToolPageTitle { font-weight: 600; font-size: 13px; color: #1f2937; letter-spacing: 0.3px; }"
        "QListWidget#toolPaletteList { background: #ffffff; color: #1f2937; border: none; padding-top: 4px; }"
        "QListWidget#toolPaletteList::item { border: 1px solid transparent; border-radius: 8px; padding: 8px 6px; margin: 3px; }"
        "QListWidget#toolPaletteList::item:selected { background: #e8f1ff; border-color: #c8d9f2; color: #0f172a; }"
        "QListWidget#toolPaletteList::item:hover:!selected { background: #f3f6fb; }"));

    connect(activityBar_, &QListWidget::currentRowChanged, this, [this](int row) {
        if (!panelStack_ || row < 0 || row >= panelStack_->count())
            return;
        panelStack_->setCurrentIndex(row);
    });

    addSidebarPage(QStringLiteral("power"), QString::fromUtf8("电力接线图元"),
                   qApp->style()->standardIcon(QStyle::SP_DriveNetIcon),
                   createPowerWiringToolListWidget());
    addSidebarPage(QStringLiteral("base"), QString::fromUtf8("基础图元"),
                   qApp->style()->standardIcon(QStyle::SP_FileDialogContentsView),
                   createBaseToolListWidget());
    addSidebarPage(QStringLiteral("image"), QString::fromUtf8("图像图元"),
                   qApp->style()->standardIcon(QStyle::SP_FileIcon),
                   createImageToolListWidget());
}

LzToolBoxManager::~LzToolBoxManager()
{
}

QWidget* LzToolBoxManager::getWidget()
{
    return rootWidget_;
}

QToolBox* LzToolBoxManager::getToolBox()
{
    auto* compatToolBox = new QToolBox;
    for (const SidebarPage& page : pages_)
    {
        if (!page.toolList)
            continue;
        compatToolBox->addItem(page.toolList, page.title);
    }
    return compatToolBox;
}

QListWidget* LzToolBoxManager::activityBar() const
{
    return activityBar_;
}

QListWidget* LzToolBoxManager::toolList(const QString& pageId) const
{
    for (const SidebarPage& page : pages_)
    {
        if (page.id == pageId)
            return page.toolList;
    }
    return nullptr;
}

QStringList LzToolBoxManager::pageIds() const
{
    QStringList ids;
    ids.reserve(pages_.size());
    for (const SidebarPage& page : pages_)
        ids.push_back(page.id);
    return ids;
}

QString LzToolBoxManager::pageTitle(const QString& pageId) const
{
    for (const SidebarPage& page : pages_)
    {
        if (page.id == pageId)
            return page.title;
    }
    return QString();
}

void LzToolBoxManager::addSidebarPage(const QString& pageId, const QString& title, const QIcon& icon, QWidget* content)
{
    if (pageId.trimmed().isEmpty() || title.trimmed().isEmpty() || !content || !activityBar_ || !panelStack_)
        return;
    for (const SidebarPage& page : pages_)
    {
        if (page.id == pageId)
            return;
    }

    QWidget* pageWidget = createSidebarPage(title, content, pageId);
    panelStack_->addWidget(pageWidget);

    auto* item = new QListWidgetItem(icon, QString(), activityBar_);
    item->setToolTip(title);
    item->setStatusTip(title);
    item->setData(Qt::UserRole, pageId);
    item->setSizeHint(QSize(40, 40));

    SidebarPage page;
    page.id = pageId;
    page.title = title;
    page.pageWidget = pageWidget;
    page.contentWidget = content;
    page.toolList = qobject_cast<QListWidget*>(content);
    pages_.push_back(page);

    if (activityBar_->count() == 1)
        activityBar_->setCurrentRow(0);
}

QListWidget* LzToolBoxManager::createPowerWiringToolListWidget()
{
    LzListWidget* listWidget = new LzListWidget;
    lzSetupToolListWidget(listWidget);
    lzAppendToolItems(listWidget, {
        LZ_SLD_BUSBAR_SECTION,
        LZ_SLD_CIRCUIT_BREAKER,
        LZ_SLD_DISCONNECTOR,
        LZ_SLD_TRANSFORMER_2W,
        LZ_SLD_POWER_FEEDER,
        LZ_SLD_GENERATOR,
        LZ_SLD_MOTOR,
        LZ_SLD_LOAD,
        LZ_SLD_GROUND,
        LZ_WSYM_CAPACITOR,
        LZ_WSYM_REACTOR,
        LZ_WSYM_ARRESTER,
        LZ_WSYM_ARC_COIL,
        LZ_WSYM_CABLE,
        LZ_WSYM_LOAD_BREAK,
        LZ_WSYM_EARTH_SWITCH,
        LZ_WSYM_GRID,
        LZ_WSYM_SVG_COMP,
        LZ_WSYM_PV_INFEED,
        LZ_WSYM_ESS,
        LZ_WSYM_WIND_INFEED,
        LZ_WSYM_STATION_XFMR,
        LZ_WSYM_TRANSFORMER_3W,
        LZ_WSYM_EARTHING_XFMR,
        LZ_WSYM_AUTO_XFMR,
        LZ_WSYM_SPLIT_REACTOR,
        LZ_WSYM_CURRENT_TRANSFORMER,
        LZ_WSYM_VOLTAGE_TRANSFORMER,
        LZ_WSYM_FUSE,
        LZ_WSYM_JUNCTION
    });

    lzConnectToolListIfFrame(listWidget, frame);
    powerWiringToolList_ = listWidget;
    return listWidget;
}

void LzToolBoxManager::refreshWiringToolIcons()
{
    const QSize iconSize(64, 64);
    for (const SidebarPage& page : pages_)
    {
        QListWidget* listWidget = page.toolList;
        if (!listWidget)
            continue;
        for (int i = 0; i < listWidget->count(); ++i)
        {
            auto* lwItem = dynamic_cast<LzListWidgetItem*>(listWidget->item(i));
            if (!lwItem)
                continue;
            LzTool* tool = lwItem->getTool();
            if (tool)
                lwItem->setIcon(tool->getIcon(iconSize, lwItem->getData()));
        }
    }
}

QListWidget* LzToolBoxManager::createBaseToolListWidget()
{
    LzListWidget* listWidget = new LzListWidget;
    lzSetupToolListWidget(listWidget);
    lzAppendToolItems(listWidget, {
        LZ_RECT,
        LZ_ARC,
        LZ_ROUNDRECT,
        LZ_ELLIPSE,
        LZ_TOPOLOGY_NODE,
        LZ_CONNECT_LINE,
        LZ_LINE,
        LZ_POLYLINE,
        LZ_POLYGON,
        LZ_BEZIER,
        LZ_BRACE_CALLOUT,
        LZ_TEXT
    });

    lzConnectToolListIfFrame(listWidget, frame);
    return listWidget;
}

QListWidget* LzToolBoxManager::createImageToolListWidget()
{
    imageManager = new LzImageManager;
    lzSetupToolListWidget(imageManager->getWidget());
    lzConnectToolListIfFrame(imageManager->getWidget(), frame);
    return imageManager->getWidget();
}

QWidget* LzToolBoxManager::createSidebarPage(const QString& title, QWidget* content, const QString& pageId) const
{
    auto* page = new QWidget;
    page->setObjectName(QStringLiteral("leftToolPage_%1").arg(pageId));

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 10, 12, 12);
    layout->setSpacing(6);

    auto* titleLabel = new QLabel(title, page);
    titleLabel->setObjectName(QStringLiteral("leftToolPageTitle"));
    layout->addWidget(titleLabel, 0);

    auto* divider = new QFrame(page);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet(QStringLiteral("color: #e5e7eb; background: #e5e7eb; min-height: 1px; max-height: 1px;"));
    layout->addWidget(divider, 0);
    layout->addWidget(content, 1);

    return page;
}
