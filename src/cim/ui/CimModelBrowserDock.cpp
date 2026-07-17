#include "CimModelBrowserDock.h"
#include "cim/query/CimGraphicSummaryText.h"

#include <algorithm>

#include <QBrush>
#include <QColor>
#include <QSignalBlocker>
#include <QHeaderView>
#include <QLineEdit>
#include <QSet>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QString objectDisplayLabel(const CimObject& object)
{
    const QString name = object.name.trimmed();
    return name.isEmpty() ? object.mrid : name;
}

QBrush visualStateBrush(CimGraphicObjectVisualState state)
{
    switch (state)
    {
    case CimGraphicObjectVisualState::Visualized:
        return QBrush(QColor(0x1f, 0x6f, 0x43));
    case CimGraphicObjectVisualState::MappableNotInstantiated:
        return QBrush(QColor(0xb4, 0x53, 0x09));
    case CimGraphicObjectVisualState::SemanticOnly:
        return QBrush(QColor(0x60, 0x6c, 0x76));
    case CimGraphicObjectVisualState::LayoutOnly:
        return QBrush(QColor(0x25, 0x61, 0x9a));
    case CimGraphicObjectVisualState::MetadataOnly:
        return QBrush(QColor(0x64, 0x57, 0x75));
    case CimGraphicObjectVisualState::AbstractOnly:
        return QBrush(QColor(0x7a, 0x7a, 0x7a));
    case CimGraphicObjectVisualState::UnmappedDevice:
        return QBrush(QColor(0xc2, 0x41, 0x0c));
    case CimGraphicObjectVisualState::Uncategorized:
        return QBrush(QColor(0xb4, 0x23, 0x18));
    }
    return QBrush();
}

double coveragePercent(int numerator, int denominator)
{
    if (denominator <= 0)
        return 100.0;
    return static_cast<double>(numerator) * 100.0 / static_cast<double>(denominator);
}

QString buildSummaryText(const QStringList& profiles,
                         const QStringList& logs,
                         const QStringList& warnings,
                         const QStringList& errors,
                         const CimGraphicVisualSummary& visualSummary,
                         const PowerTopologyProjection& topologyProjection)
{
    const PowerTopologyAnalysisSnapshot& topologySnapshot = topologyProjection.snapshot();
    QStringList lines;
    lines << QObject::tr("对象总数：%1").arg(visualSummary.totalObjects);
    lines << QObject::tr("已识别 Profile：%1").arg(
        profiles.isEmpty() ? QObject::tr("未识别") : profiles.join(QStringLiteral(", ")));
    lines << QObject::tr("应可视化设备数：%1").arg(visualSummary.mappableObjects);
    lines << QObject::tr("已可视化设备数：%1").arg(visualSummary.visualizedObjects);
    lines << QObject::tr("未可视化设备数：%1").arg(visualSummary.pendingObjects);
    lines << QObject::tr("可视化覆盖率：%1%")
                 .arg(QString::number(coveragePercent(visualSummary.visualizedObjects, visualSummary.mappableObjects), 'f', 1));
    lines << QObject::tr("其他未映射设备数：%1").arg(visualSummary.unmappedDeviceObjects);
    if (!visualSummary.unmappedDeviceClasses.isEmpty())
    {
        QStringList classNames = visualSummary.unmappedDeviceClasses.values();
        std::sort(classNames.begin(), classNames.end());
        lines << QObject::tr("其他未映射设备类型：%1").arg(classNames.join(QStringLiteral(", ")));
    }
    lines << QObject::tr("非图元对象数：%1（语义 %2 / 布局 %3 / 元数据 %4 / 抽象层 %5）")
                 .arg(visualSummary.semanticObjects + visualSummary.layoutObjects + visualSummary.metadataObjects + visualSummary.abstractObjects)
                 .arg(visualSummary.semanticObjects)
                 .arg(visualSummary.layoutObjects)
                 .arg(visualSummary.metadataObjects)
                 .arg(visualSummary.abstractObjects);
    lines << QObject::tr("标准导体对象数（ACLineSegment）：%1，其中已图元化 %2")
                 .arg(visualSummary.acLineSegmentObjects)
                 .arg(visualSummary.visualizedAcLineSegments);
    lines << QObject::tr("自动辅助连线数：%1").arg(visualSummary.generatedHelperLineCount);
    if (!visualSummary.behaviorResultsByMrid.isEmpty())
    {
        lines << QObject::tr("行为可用图元数：%1").arg(visualSummary.behaviorAvailableObjects);
        lines << QObject::tr("导通图元数：%1").arg(visualSummary.conductiveObjects);
        lines << QObject::tr("可操作图元数：%1").arg(visualSummary.operableObjects);
        lines << QObject::tr("带电图元数：%1").arg(visualSummary.energizedObjects);
    }
    lines << QObject::tr("说明：画布中的连线数量会包含自动辅助连线，因此不等同于 ACLineSegment 对象数。");
    lines << QObject::tr("其他未归类对象数：%1").arg(visualSummary.unmappedObjects);
    if (!visualSummary.unmappedOtherClasses.isEmpty())
    {
        QStringList classNames = visualSummary.unmappedOtherClasses.values();
        std::sort(classNames.begin(), classNames.end());
        lines << QObject::tr("其他未归类对象类型：%1").arg(classNames.join(QStringLiteral(", ")));
    }

    lines << QStringLiteral("\u6A21\u578B\u62D3\u6251\u5FEB\u7167\uFF1A\u8BBE\u5907 %1 / \u8282\u70B9 %2 / \u5BFC\u4F53 %3")
                 .arg(topologySnapshot.devices.size())
                 .arg(topologySnapshot.nodes.size())
                 .arg(topologySnapshot.conductors.size());
    lines << QObject::tr("拓扑投影来源：%1").arg(topologyProjection.sourceLabel().trimmed().isEmpty()
                                               ? QObject::tr("未定义")
                                               : topologyProjection.sourceLabel());
    if (!topologyProjection.truthBoundaryText().trimmed().isEmpty())
        lines << QObject::tr("真源边界：%1").arg(topologyProjection.truthBoundaryText());

    if (!logs.isEmpty())
    {
        lines << QString();
        lines << QObject::tr("导入日志：");
        for (const QString& log : logs)
            lines << QStringLiteral(" - %1").arg(log);
    }

    if (!warnings.isEmpty())
    {
        lines << QString();
        lines << QObject::tr("导入告警：");
        for (const QString& warning : warnings)
            lines << QStringLiteral(" - %1").arg(warning);
    }

    if (!errors.isEmpty())
    {
        lines << QString();
        lines << QObject::tr("导入错误：");
        for (const QString& error : errors)
            lines << QStringLiteral(" - %1").arg(error);
    }

    return lines.join(QLatin1Char('\n'));
}

}

CimModelBrowserDock::CimModelBrowserDock(QWidget* parent)
    : QDockWidget(parent)
{
    setObjectName(QStringLiteral("cimModelBrowserDock"));
    setWindowTitle(tr("CIM 模型"));

    auto* container = new QWidget(this);
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    filterEdit_ = new QLineEdit(container);
    filterEdit_->setPlaceholderText(tr("搜索类名、名称、mRID 或状态"));
    layout->addWidget(filterEdit_);

    treeWidget_ = new QTreeWidget(container);
    treeWidget_->setColumnCount(4);
    treeWidget_->setHeaderLabels({tr("对象"), tr("类名"), tr("mRID"), tr("状态")});
    treeWidget_->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeWidget_->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeWidget_->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    treeWidget_->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    treeWidget_->setAlternatingRowColors(true);
    layout->addWidget(treeWidget_, 1);

    summaryTextEdit_ = new QTextEdit(container);
    summaryTextEdit_->setReadOnly(true);
    summaryTextEdit_->setMinimumHeight(140);
    layout->addWidget(summaryTextEdit_);

    setWidget(container);

    connect(filterEdit_, &QLineEdit::textChanged, this, &CimModelBrowserDock::onFilterTextChanged);
    connect(treeWidget_, &QTreeWidget::currentItemChanged, this, &CimModelBrowserDock::onCurrentItemChanged);
    connect(treeWidget_, &QTreeWidget::itemActivated, this, &CimModelBrowserDock::onItemActivated);
}

void CimModelBrowserDock::setImportResult(const CimModel& model,
                                          const QStringList& loadedProfiles,
                                          const QStringList& logs,
                                          const QStringList& warnings,
                                          const QStringList& errors,
                                          const CimGraphicVisualSummary& visualSummary,
                                          const PowerTopologyProjection& topologyProjection)
{
    rebuildTree(model, visualSummary);
    summaryTextEdit_->setPlainText(buildSummaryText(loadedProfiles,
                                                   logs,
                                                   warnings,
                                                   errors,
                                                   visualSummary,
                                                   topologyProjection));
    onFilterTextChanged(filterEdit_->text());
}

void CimModelBrowserDock::selectObjectByMrid(const QString& mrid)
{
    if (mrid.trimmed().isEmpty())
        return;

    QTreeWidgetItem* item = mridItemMap_.value(mrid);
    if (!item)
        return;
    if (treeWidget_->currentItem() == item)
        return;

    QTreeWidgetItem* parent = item->parent();
    while (parent)
    {
        parent->setExpanded(true);
        parent = parent->parent();
    }

    syncingSelection_ = true;
    QSignalBlocker blocker(treeWidget_);
    treeWidget_->setCurrentItem(item);
    treeWidget_->scrollToItem(item);
    syncingSelection_ = false;
}

void CimModelBrowserDock::onFilterTextChanged(const QString& text)
{
    const QString filterText = text.trimmed().toLower();
    for (int i = 0; i < treeWidget_->topLevelItemCount(); ++i)
        updateItemVisibility(treeWidget_->topLevelItem(i), filterText);
}

void CimModelBrowserDock::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    if (syncingSelection_)
        return;
    const QString mrid = mridForItem(current);
    if (!mrid.isEmpty())
        emit objectActivated(mrid);
}

void CimModelBrowserDock::onItemActivated(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if (syncingSelection_)
        return;
    const QString mrid = mridForItem(item);
    if (!mrid.isEmpty())
        emit objectActivated(mrid);
}

void CimModelBrowserDock::rebuildTree(const CimModel& model, const CimGraphicVisualSummary& visualSummary)
{
    treeWidget_->clear();
    mridItemMap_.clear();

    QHash<QString, QTreeWidgetItem*> classItems;
    QHash<QString, QHash<int, int>> stateCountsByClass;
    const QVector<CimObject> objects = model.allObjects();
    for (const CimObject& object : objects)
    {
        QTreeWidgetItem* classItem = classItems.value(object.className);
        if (!classItem)
        {
            classItem = new QTreeWidgetItem(treeWidget_);
            classItem->setText(0, object.className);
            classItem->setText(1, tr("类分组"));
            classItems.insert(object.className, classItem);
        }

        auto* objectItem = new QTreeWidgetItem(classItem);
        const auto objectSummaryIt = visualSummary.objectSummariesByMrid.constFind(object.mrid);
        CimGraphicObjectSummary fallbackSummary;
        fallbackSummary.mrid = object.mrid;
        fallbackSummary.visualState = CimGraphicObjectVisualState::Uncategorized;
        const CimGraphicObjectSummary objectSummary =
            objectSummaryIt == visualSummary.objectSummariesByMrid.constEnd()
            ? fallbackSummary
            : objectSummaryIt.value();
        const CimGraphicObjectVisualState state = objectSummary.visualState;
        objectItem->setText(0, objectDisplayLabel(object));
        objectItem->setText(1, object.className);
        objectItem->setText(2, object.mrid);
        objectItem->setText(3, cimGraphicObjectStatusLabel(objectSummary));
        objectItem->setForeground(3, visualStateBrush(state));
        objectItem->setData(0, Qt::UserRole, object.mrid);
        mridItemMap_.insert(object.mrid, objectItem);
        stateCountsByClass[object.className][static_cast<int>(state)] += 1;
    }

    for (auto it = classItems.begin(); it != classItems.end(); ++it)
    {
        QTreeWidgetItem* classItem = it.value();
        const QHash<int, int> counts = stateCountsByClass.value(it.key());
        const int visualizedCount = counts.value(static_cast<int>(CimGraphicObjectVisualState::Visualized));
        const int pendingCount = counts.value(static_cast<int>(CimGraphicObjectVisualState::MappableNotInstantiated));
        const int mappableCount = visualizedCount + pendingCount;
        if (mappableCount > 0)
        {
            classItem->setText(3, tr("%1/%2 已可视化").arg(visualizedCount).arg(mappableCount));
            continue;
        }

        if (counts.size() == 1 && classItem->childCount() > 0)
        {
            const CimGraphicObjectVisualState onlyState =
                static_cast<CimGraphicObjectVisualState>(counts.constBegin().key());
            classItem->setText(3, tr("%1（%2 项）")
                                      .arg(cimGraphicObjectVisualStateLabel(onlyState))
                                      .arg(classItem->childCount()));
            classItem->setForeground(3, visualStateBrush(onlyState));
        }
        else
        {
            classItem->setText(3, tr("%1 项").arg(classItem->childCount()));
        }
    }

    treeWidget_->expandAll();
}

bool CimModelBrowserDock::updateItemVisibility(QTreeWidgetItem* item, const QString& filterText)
{
    if (!item)
        return false;

    bool childVisible = false;
    for (int i = 0; i < item->childCount(); ++i)
    {
        if (updateItemVisibility(item->child(i), filterText))
            childVisible = true;
    }

    const QString joined = QStringLiteral("%1 %2 %3 %4")
                               .arg(item->text(0), item->text(1), item->text(2), item->text(3))
                               .toLower();
    const bool selfVisible = filterText.isEmpty() || joined.contains(filterText);
    const bool visible = selfVisible || childVisible;
    item->setHidden(!visible);
    return visible;
}

QString CimModelBrowserDock::mridForItem(QTreeWidgetItem* item) const
{
    if (!item)
        return QString();
    return item->data(0, Qt::UserRole).toString();
}
