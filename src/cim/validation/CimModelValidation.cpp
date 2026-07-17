#include "CimModelValidation.h"

#include "cim/mapping/CimGraphicMapper.h"
#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"

namespace {

void addIssue(CimModelValidationReport& report,
              CimValidationSeverity severity,
              CimValidationIssueCode code,
              const QString& objectMrid,
              const QString& className,
              const QString& relationName,
              const QString& targetMrid,
              const QString& graphicItemId,
              const QString& message)
{
    CimValidationIssue issue;
    issue.severity = severity;
    issue.code = code;
    issue.objectMrid = objectMrid;
    issue.className = className;
    issue.relationName = relationName;
    issue.targetMrid = targetMrid;
    issue.graphicItemId = graphicItemId;
    issue.message = message;
    report.issues.push_back(issue);

    switch (severity)
    {
    case CimValidationSeverity::Error:
        ++report.summary.errorCount;
        break;
    case CimValidationSeverity::Warning:
        ++report.summary.warningCount;
        break;
    case CimValidationSeverity::Info:
    default:
        ++report.summary.infoCount;
        break;
    }
}

QString severityLabel(CimValidationSeverity severity)
{
    switch (severity)
    {
    case CimValidationSeverity::Error:
        return QStringLiteral("错误");
    case CimValidationSeverity::Warning:
        return QStringLiteral("警告");
    case CimValidationSeverity::Info:
    default:
        return QStringLiteral("信息");
    }
}

} // namespace

QStringList CimModelValidationReport::messages() const
{
    QStringList result;
    result.reserve(issues.size());
    for (const CimValidationIssue& issue : issues)
    {
        const QString prefix = issue.objectMrid.isEmpty()
                                   ? severityLabel(issue.severity)
                                   : QStringLiteral("%1[%2]").arg(severityLabel(issue.severity), issue.objectMrid);
        result << QStringLiteral("%1: %2").arg(prefix, issue.message);
    }
    return result;
}

CimModelValidationReport validateCimModelStructure(
    const CimModel& model,
    const CimIdIndex* graphicIndex,
    const CimModelValidationOptions& options)
{
    CimModelValidationReport report;
    report.summary.objectCount = model.objectCount();
    report.summary.referenceCount = model.referenceCount();
    report.summary.graphicBindingCount = graphicIndex ? graphicIndex->bindingCount() : 0;

    if (options.reportEmptyModel && model.objectCount() == 0)
    {
        addIssue(report,
                 CimValidationSeverity::Warning,
                 CimValidationIssueCode::EmptyModel,
                 {},
                 {},
                 {},
                 {},
                 {},
                 QStringLiteral("CIM 模型为空，无法形成有效 Projection。"));
    }

    CimGraphicMapper mapper;
    const QVector<CimObject> objects = model.allObjects();
    for (const CimObject& object : objects)
    {
        if (!object.isValid())
        {
            addIssue(report,
                     CimValidationSeverity::Error,
                     CimValidationIssueCode::InvalidObject,
                     object.mrid,
                     object.className,
                     {},
                     {},
                     {},
                     QStringLiteral("对象缺少 mRID 或 className。"));
            continue;
        }

        const bool mappable = mapper.canMap(object);
        if (mappable)
        {
            ++report.summary.mappableObjectCount;
            if (options.requireGraphicBindingsForMappableObjects
                && graphicIndex
                && !graphicIndex->hasBinding(object.mrid))
            {
                ++report.summary.missingGraphicBindingCount;
                addIssue(report,
                         CimValidationSeverity::Warning,
                         CimValidationIssueCode::MissingGraphicBinding,
                         object.mrid,
                         object.className,
                         {},
                         {},
                         {},
                         QStringLiteral("可映射对象缺少图元绑定。"));
            }
        }

        for (const CimReference& reference : object.references)
        {
            if (!model.contains(reference.targetMrid))
            {
                addIssue(report,
                         CimValidationSeverity::Error,
                         CimValidationIssueCode::MissingReference,
                         object.mrid,
                         object.className,
                         reference.relationName,
                         reference.targetMrid,
                         {},
                         QStringLiteral("引用目标不存在：%1 -> %2。")
                             .arg(reference.relationName, reference.targetMrid));
            }
        }
    }

    if (graphicIndex)
    {
        const QStringList mrids = graphicIndex->mrids();
        for (const QString& mrid : mrids)
        {
            if (model.contains(mrid))
                continue;
            ++report.summary.danglingGraphicBindingCount;
            addIssue(report,
                     CimValidationSeverity::Error,
                     CimValidationIssueCode::DanglingGraphicBinding,
                     mrid,
                     {},
                     {},
                     {},
                     graphicIndex->graphicItemIdForMrid(mrid),
                     QStringLiteral("图元绑定指向不存在的 CIM 对象。"));
        }
    }

    return report;
}
