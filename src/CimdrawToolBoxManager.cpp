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

#include "CimdrawToolBoxManager.h"
#include "Tool/CimdrawTool.h"
#include "Tool/CimdrawRectTool.h"
#include "Tool/CimdrawSelectTool.h"
#include "Tool/CimdrawArcTool.h"
#include "Tool/CimdrawBraceCalloutTool.h"
#include "Tool/CimdrawRoundRectTool.h"
#include "Tool/CimdrawEllipseTool.h"
#include "Tool/CimdrawConnectLineTool.h"
#include "Tool/CimdrawLineTool.h"
#include "Tool/CimdrawPolylineTool.h"
#include "Tool/CimdrawPolygonTool.h"
#include "Tool/CimdrawBezierTool.h"
#include "Tool/CimdrawTextTool.h"
#include "CimdrawImageManager.h"
#include "CimdrawListWidgetItem.h"
#include "CimdrawListWidget.h"
#include "CimdrawFrame.h"

namespace {

void cimdrawConnectToolListIfFrame(QObject* sender, CimdrawFrame* frame)
{
    if (!sender || !frame)
        return;
    QObject::connect(sender, SIGNAL(itemClicked(QListWidgetItem*)), frame, SLOT(onDrawToolChanged(QListWidgetItem*)));
}

void cimdrawSetupToolListWidget(QListWidget* listWidget)
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

void cimdrawAppendToolItems(QListWidget* listWidget, std::initializer_list<CimdrawDrawTypeId> drawTypes)
{
    if (!listWidget)
        return;
    for (CimdrawDrawTypeId drawType : drawTypes)
        listWidget->addItem(new CimdrawListWidgetItem(QSize(64, 64), CimdrawToolManager::getInstance()->changeTool(drawType)));
}

}

CimdrawToolBoxManager::CimdrawToolBoxManager(CimdrawFrame* frame)
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

CimdrawToolBoxManager::~CimdrawToolBoxManager()
{
}

QWidget* CimdrawToolBoxManager::getWidget()
{
    return rootWidget_;
}

QToolBox* CimdrawToolBoxManager::getToolBox()
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

QListWidget* CimdrawToolBoxManager::activityBar() const
{
    return activityBar_;
}

QListWidget* CimdrawToolBoxManager::toolList(const QString& pageId) const
{
    for (const SidebarPage& page : pages_)
    {
        if (page.id == pageId)
            return page.toolList;
    }
    return nullptr;
}

QStringList CimdrawToolBoxManager::pageIds() const
{
    QStringList ids;
    ids.reserve(pages_.size());
    for (const SidebarPage& page : pages_)
        ids.push_back(page.id);
    return ids;
}

QString CimdrawToolBoxManager::pageTitle(const QString& pageId) const
{
    for (const SidebarPage& page : pages_)
    {
        if (page.id == pageId)
            return page.title;
    }
    return QString();
}

void CimdrawToolBoxManager::addSidebarPage(const QString& pageId, const QString& title, const QIcon& icon, QWidget* content)
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

QListWidget* CimdrawToolBoxManager::createPowerWiringToolListWidget()
{
    CimdrawListWidget* listWidget = new CimdrawListWidget;
    cimdrawSetupToolListWidget(listWidget);
    cimdrawAppendToolItems(listWidget, {
        CIMDRAW_SLD_BUSBAR_SECTION,
        CIMDRAW_SLD_CIRCUIT_BREAKER,
        CIMDRAW_SLD_DISCONNECTOR,
        CIMDRAW_SLD_TRANSFORMER_2W,
        CIMDRAW_SLD_GENERATOR,
        CIMDRAW_SLD_MOTOR,
        CIMDRAW_SLD_LOAD,
        CIMDRAW_SLD_GROUND,
        CIMDRAW_WSYM_CAPACITOR,
        CIMDRAW_WSYM_REACTOR,
        CIMDRAW_WSYM_ARRESTER,
        CIMDRAW_WSYM_ARC_COIL,
        CIMDRAW_WSYM_CABLE,
        CIMDRAW_WSYM_LOAD_BREAK,
        CIMDRAW_WSYM_EARTH_SWITCH,
        CIMDRAW_WSYM_GRID,
        CIMDRAW_WSYM_SVG_COMP,
        CIMDRAW_WSYM_PV_INFEED,
        CIMDRAW_WSYM_ESS,
        CIMDRAW_WSYM_WIND_INFEED,
        CIMDRAW_WSYM_STATION_XFMR,
        CIMDRAW_WSYM_TRANSFORMER_3W,
        CIMDRAW_WSYM_EARTHING_XFMR,
        CIMDRAW_WSYM_AUTO_XFMR,
        CIMDRAW_WSYM_SPLIT_REACTOR,
        CIMDRAW_WSYM_CURRENT_TRANSFORMER,
        CIMDRAW_WSYM_VOLTAGE_TRANSFORMER,
        CIMDRAW_WSYM_FUSE,
        CIMDRAW_WSYM_JUNCTION
    });

    cimdrawConnectToolListIfFrame(listWidget, frame);
    powerWiringToolList_ = listWidget;
    return listWidget;
}

void CimdrawToolBoxManager::refreshWiringToolIcons()
{
    const QSize iconSize(64, 64);
    for (const SidebarPage& page : pages_)
    {
        QListWidget* listWidget = page.toolList;
        if (!listWidget)
            continue;
        for (int i = 0; i < listWidget->count(); ++i)
        {
            auto* lwItem = dynamic_cast<CimdrawListWidgetItem*>(listWidget->item(i));
            if (!lwItem)
                continue;
            CimdrawTool* tool = lwItem->getTool();
            if (tool)
                lwItem->setIcon(tool->getIcon(iconSize, lwItem->getData()));
        }
    }
}

QListWidget* CimdrawToolBoxManager::createBaseToolListWidget()
{
    CimdrawListWidget* listWidget = new CimdrawListWidget;
    cimdrawSetupToolListWidget(listWidget);
    cimdrawAppendToolItems(listWidget, {
        CIMDRAW_RECT,
        CIMDRAW_ARC,
        CIMDRAW_ROUNDRECT,
        CIMDRAW_ELLIPSE,
        CIMDRAW_TOPOLOGY_NODE,
        CIMDRAW_CONNECT_LINE,
        CIMDRAW_LINE,
        CIMDRAW_POLYLINE,
        CIMDRAW_POLYGON,
        CIMDRAW_BEZIER,
        CIMDRAW_BRACE_CALLOUT,
        CIMDRAW_TEXT
    });

    cimdrawConnectToolListIfFrame(listWidget, frame);
    return listWidget;
}

QListWidget* CimdrawToolBoxManager::createImageToolListWidget()
{
    imageManager = new CimdrawImageManager;
    cimdrawSetupToolListWidget(imageManager->getWidget());
    cimdrawConnectToolListIfFrame(imageManager->getWidget(), frame);
    return imageManager->getWidget();
}

QWidget* CimdrawToolBoxManager::createSidebarPage(const QString& title, QWidget* content, const QString& pageId) const
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
