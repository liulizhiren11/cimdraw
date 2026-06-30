#include "CimModelBrowserDock.h"

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

#include "cim/mapping/CimGraphicMapper.h"
#include "cim/model/CimClassLineage.h"

namespace {

enum class CimObjectVisualState
{
    Visualized = 0,
    MappableNotInstantiated,
    SemanticOnly,
    LayoutOnly,
    MetadataOnly,
    AbstractOnly,
    UnmappedDevice,
    Uncategorized,
};

QString objectDisplayLabel(const CimObject& object)
{
    const QString name = object.name.trimmed();
    return name.isEmpty() ? object.mrid : name;
}

bool cimSemanticClass(const QString& className)
{
    return className == QStringLiteral("Terminal")
        || className == QStringLiteral("ACDCTerminal")
        || className == QStringLiteral("PowerTransformerEnd")
        || className == QStringLiteral("SvPowerFlow")
        || className == QStringLiteral("SvVoltage")
        || className == QStringLiteral("ConnectivityNode")
        || className == QStringLiteral("TopologicalNode");
}

bool cimLayoutClass(const QString& className)
{
    return className == QStringLiteral("Diagram")
        || className == QStringLiteral("DiagramObject")
        || className == QStringLiteral("DiagramObjectPoint")
        || className == QStringLiteral("CoordinateSystem");
}

bool cimMetadataContainerClass(const QString& className)
{
    return className == QStringLiteral("BaseVoltage")
        || className == QStringLiteral("Name")
        || className == QStringLiteral("NameType")
        || className == QStringLiteral("VoltageLevel")
        || className == QStringLiteral("Bay")
        || className == QStringLiteral("Substation")
        || className == QStringLiteral("SubGeographicalRegion")
        || className == QStringLiteral("GeographicalRegion")
        || className == QStringLiteral("OperationalLimit")
        || className == QStringLiteral("OperationalLimitSet")
        || className == QStringLiteral("OperationalLimitType")
        || className == QStringLiteral("VoltageLimit")
        || className == QStringLiteral("CurrentLimit");
}

bool cimAbstractCompatibilityClass(const QString& className)
{
    return className == QStringLiteral("IdentifiedObject")
        || className == QStringLiteral("Equipment")
        || className == QStringLiteral("ConductingEquipment")
        || className == QStringLiteral("Conductor")
        || className == QStringLiteral("PowerSystemResource")
        || className == QStringLiteral("ConnectivityNodeContainer")
        || className == QStringLiteral("Switch")
        || className == QStringLiteral("ProtectedSwitch")
        || className == QStringLiteral("RotatingMachine")
        || className == QStringLiteral("TransformerEnd")
        || className == QStringLiteral("EquipmentContainer");
}

QString visualStateLabel(CimObjectVisualState state)
{
    switch (state)
    {
    case CimObjectVisualState::Visualized:
        return QObject::tr("已可视化");
    case CimObjectVisualState::MappableNotInstantiated:
        return QObject::tr("应显示未实例化");
    case CimObjectVisualState::SemanticOnly:
        return QObject::tr("语义对象");
    case CimObjectVisualState::LayoutOnly:
        return QObject::tr("布局对象");
    case CimObjectVisualState::MetadataOnly:
        return QObject::tr("元数据/容器对象");
    case CimObjectVisualState::AbstractOnly:
        return QObject::tr("抽象层对象");
    case CimObjectVisualState::UnmappedDevice:
        return QObject::tr("设备未映射");
    case CimObjectVisualState::Uncategorized:
        return QObject::tr("未归类对象");
    }
    return QObject::tr("未分类对象");
}

QBrush visualStateBrush(CimObjectVisualState state)
{
    switch (state)
    {
    case CimObjectVisualState::Visualized:
        return QBrush(QColor(0x1f, 0x6f, 0x43));
    case CimObjectVisualState::MappableNotInstantiated:
        return QBrush(QColor(0xb4, 0x53, 0x09));
    case CimObjectVisualState::SemanticOnly:
        return QBrush(QColor(0x60, 0x6c, 0x76));
    case CimObjectVisualState::LayoutOnly:
        return QBrush(QColor(0x25, 0x61, 0x9a));
    case CimObjectVisualState::MetadataOnly:
        return QBrush(QColor(0x64, 0x57, 0x75));
    case CimObjectVisualState::AbstractOnly:
        return QBrush(QColor(0x7a, 0x7a, 0x7a));
    case CimObjectVisualState::UnmappedDevice:
        return QBrush(QColor(0xc2, 0x41, 0x0c));
    case CimObjectVisualState::Uncategorized:
        return QBrush(QColor(0xb4, 0x23, 0x18));
    }
    return QBrush();
}

struct CimVisualCoverage
{
    int totalObjects = 0;
    int mappableObjects = 0;
    int visualizedObjects = 0;
    int pendingObjects = 0;
    int unmappedDeviceObjects = 0;
    int semanticObjects = 0;
    int layoutObjects = 0;
    int metadataObjects = 0;
    int abstractObjects = 0;
    int unmappedObjects = 0;
    QSet<QString> unmappedDeviceClasses;
    QSet<QString> unmappedOtherClasses;
};

struct CimLineVisualSummary
{
    int acLineSegmentObjects = 0;
    int visualizedAcLineSegments = 0;
    int generatedHelperLines = 0;
};

double coveragePercent(int numerator, int denominator)
{
    if (denominator <= 0)
        return 100.0;
    return static_cast<double>(numerator) * 100.0 / static_cast<double>(denominator);
}

bool isObjectVisualized(const QString& mrid, const QSet<QString>& visualizedMrids)
{
    return !mrid.trimmed().isEmpty() && visualizedMrids.contains(mrid);
}

bool looksLikeEquipmentClass(const QString& className)
{
    return CimClassLineage::isA(className, QStringLiteral("ConductingEquipment"))
        || CimClassLineage::isA(className, QStringLiteral("Equipment"));
}

CimLineVisualSummary buildLineSummary(const QVector<CimObject>& objects,
                                      const QSet<QString>& visualizedMrids,
                                      int generatedHelperLineCount)
{
    CimLineVisualSummary summary;
    summary.generatedHelperLines = generatedHelperLineCount;
    for (const CimObject& object : objects)
    {
        if (object.className != QStringLiteral("ACLineSegment"))
            continue;

        ++summary.acLineSegmentObjects;
        if (isObjectVisualized(object.mrid, visualizedMrids))
            ++summary.visualizedAcLineSegments;
    }
    return summary;
}

CimObjectVisualState classifyObjectVisualState(const CimObject& object,
                                               const QSet<QString>& visualizedMrids,
                                               const CimGraphicMapper& mapper)
{
    if (mapper.canMap(object))
    {
        return isObjectVisualized(object.mrid, visualizedMrids)
            ? CimObjectVisualState::Visualized
            : CimObjectVisualState::MappableNotInstantiated;
    }

    if (cimSemanticClass(object.className))
        return CimObjectVisualState::SemanticOnly;
    if (cimLayoutClass(object.className))
        return CimObjectVisualState::LayoutOnly;
    if (cimMetadataContainerClass(object.className))
        return CimObjectVisualState::MetadataOnly;
    if (cimAbstractCompatibilityClass(object.className))
        return CimObjectVisualState::AbstractOnly;
    return looksLikeEquipmentClass(object.className)
        ? CimObjectVisualState::UnmappedDevice
        : CimObjectVisualState::Uncategorized;
}

CimVisualCoverage buildCoverage(const QVector<CimObject>& objects,
                                const QSet<QString>& visualizedMrids)
{
    CimVisualCoverage coverage;
    CimGraphicMapper mapper;
    coverage.totalObjects = objects.size();

    for (const CimObject& object : objects)
    {
        const CimObjectVisualState state = classifyObjectVisualState(object, visualizedMrids, mapper);
        switch (state)
        {
        case CimObjectVisualState::Visualized:
            ++coverage.mappableObjects;
            ++coverage.visualizedObjects;
            break;
        case CimObjectVisualState::MappableNotInstantiated:
            ++coverage.mappableObjects;
            ++coverage.pendingObjects;
            break;
        case CimObjectVisualState::SemanticOnly:
            ++coverage.semanticObjects;
            break;
        case CimObjectVisualState::LayoutOnly:
            ++coverage.layoutObjects;
            break;
        case CimObjectVisualState::MetadataOnly:
            ++coverage.metadataObjects;
            break;
        case CimObjectVisualState::AbstractOnly:
            ++coverage.abstractObjects;
            break;
        case CimObjectVisualState::UnmappedDevice:
            ++coverage.unmappedDeviceObjects;
            coverage.unmappedDeviceClasses.insert(object.className);
            break;
        case CimObjectVisualState::Uncategorized:
            ++coverage.unmappedObjects;
            coverage.unmappedOtherClasses.insert(object.className);
            break;
        }
    }

    return coverage;
}

QString buildSummaryText(const QStringList& profiles,
                         const QStringList& logs,
                         const QStringList& warnings,
                         const QStringList& errors,
                         const QVector<CimObject>& objects,
                         const QSet<QString>& visualizedMrids,
                         int generatedHelperLineCount)
{
    const CimVisualCoverage coverage = buildCoverage(objects, visualizedMrids);
    const CimLineVisualSummary lineSummary = buildLineSummary(objects, visualizedMrids, generatedHelperLineCount);
    QStringList lines;
    lines << QObject::tr("对象总数：%1").arg(coverage.totalObjects);
    lines << QObject::tr("已识别 Profile：%1").arg(
        profiles.isEmpty() ? QObject::tr("未识别") : profiles.join(QStringLiteral(", ")));
    lines << QObject::tr("应可视化设备数：%1").arg(coverage.mappableObjects);
    lines << QObject::tr("已可视化设备数：%1").arg(coverage.visualizedObjects);
    lines << QObject::tr("未可视化设备数：%1").arg(coverage.pendingObjects);
    lines << QObject::tr("可视化覆盖率：%1%")
                 .arg(QString::number(coveragePercent(coverage.visualizedObjects, coverage.mappableObjects), 'f', 1));
    lines << QObject::tr("其他未映射设备数：%1").arg(coverage.unmappedDeviceObjects);
    if (!coverage.unmappedDeviceClasses.isEmpty())
    {
        QStringList classNames = coverage.unmappedDeviceClasses.values();
        std::sort(classNames.begin(), classNames.end());
        lines << QObject::tr("其他未映射设备类型：%1").arg(classNames.join(QStringLiteral(", ")));
    }
    lines << QObject::tr("非图元对象数：%1（语义 %2 / 布局 %3 / 元数据 %4 / 抽象层 %5）")
                 .arg(coverage.semanticObjects + coverage.layoutObjects + coverage.metadataObjects + coverage.abstractObjects)
                 .arg(coverage.semanticObjects)
                 .arg(coverage.layoutObjects)
                 .arg(coverage.metadataObjects)
                 .arg(coverage.abstractObjects);
    lines << QObject::tr("标准导体对象数（ACLineSegment）：%1，其中已图元化 %2")
                 .arg(lineSummary.acLineSegmentObjects)
                 .arg(lineSummary.visualizedAcLineSegments);
    lines << QObject::tr("自动辅助连线数：%1").arg(lineSummary.generatedHelperLines);
    lines << QObject::tr("说明：画布中的连线数量会包含自动辅助连线，因此不等同于 ACLineSegment 对象数。");
    lines << QObject::tr("其他未归类对象数：%1").arg(coverage.unmappedObjects);
    if (!coverage.unmappedOtherClasses.isEmpty())
    {
        QStringList classNames = coverage.unmappedOtherClasses.values();
        std::sort(classNames.begin(), classNames.end());
        lines << QObject::tr("其他未归类对象类型：%1").arg(classNames.join(QStringLiteral(", ")));
    }

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
                                          const QSet<QString>& visualizedMrids)
{
    setImportResult(model,
                    loadedProfiles,
                    logs,
                    warnings,
                    errors,
                    visualizedMrids,
                    0);
}

void CimModelBrowserDock::setImportResult(const CimModel& model,
                                          const QStringList& loadedProfiles,
                                          const QStringList& logs,
                                          const QStringList& warnings,
                                          const QStringList& errors,
                                          const QSet<QString>& visualizedMrids,
                                          int generatedHelperLineCount)
{
    rebuildTree(model, visualizedMrids);
    summaryTextEdit_->setPlainText(buildSummaryText(loadedProfiles,
                                                   logs,
                                                   warnings,
                                                   errors,
                                                   model.allObjects(),
                                                   visualizedMrids,
                                                   generatedHelperLineCount));
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

void CimModelBrowserDock::rebuildTree(const CimModel& model, const QSet<QString>& visualizedMrids)
{
    treeWidget_->clear();
    mridItemMap_.clear();

    QHash<QString, QTreeWidgetItem*> classItems;
    QHash<QString, QHash<int, int>> stateCountsByClass;
    CimGraphicMapper mapper;
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
        const CimObjectVisualState state = classifyObjectVisualState(object, visualizedMrids, mapper);
        objectItem->setText(0, objectDisplayLabel(object));
        objectItem->setText(1, object.className);
        objectItem->setText(2, object.mrid);
        objectItem->setText(3, visualStateLabel(state));
        objectItem->setForeground(3, visualStateBrush(state));
        objectItem->setData(0, Qt::UserRole, object.mrid);
        mridItemMap_.insert(object.mrid, objectItem);
        stateCountsByClass[object.className][static_cast<int>(state)] += 1;
    }

    for (auto it = classItems.begin(); it != classItems.end(); ++it)
    {
        QTreeWidgetItem* classItem = it.value();
        const QHash<int, int> counts = stateCountsByClass.value(it.key());
        const int visualizedCount = counts.value(static_cast<int>(CimObjectVisualState::Visualized));
        const int pendingCount = counts.value(static_cast<int>(CimObjectVisualState::MappableNotInstantiated));
        const int mappableCount = visualizedCount + pendingCount;
        if (mappableCount > 0)
        {
            classItem->setText(3, tr("%1/%2 已可视化").arg(visualizedCount).arg(mappableCount));
            continue;
        }

        if (counts.size() == 1 && classItem->childCount() > 0)
        {
            const CimObjectVisualState onlyState = static_cast<CimObjectVisualState>(counts.constBegin().key());
            classItem->setText(3, tr("%1（%2 项）").arg(visualStateLabel(onlyState)).arg(classItem->childCount()));
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
