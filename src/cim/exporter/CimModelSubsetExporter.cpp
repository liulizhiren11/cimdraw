#include "CimModelSubsetExporter.h"

#include "cim/mapping/CimIdIndex.h"
#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"

#include <QSet>

namespace {

QStringList uniqueOrdered(const QStringList& values)
{
    QStringList result;
    QSet<QString> seen;
    for (const QString& value : values)
    {
        const QString normalized = value.trimmed();
        if (normalized.isEmpty() || seen.contains(normalized))
            continue;
        seen.insert(normalized);
        result << normalized;
    }
    return result;
}

QString valueToExportString(const QVariant& value)
{
    if (value.typeId() == QMetaType::Bool)
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    return value.toString();
}

void appendTextElement(QDomDocument& document,
                       QDomElement& parent,
                       const QString& tagName,
                       const QString& text)
{
    QDomElement element = document.createElement(tagName);
    element.appendChild(document.createTextNode(text));
    parent.appendChild(element);
}

QString severityKey(CimValidationSeverity severity)
{
    switch (severity)
    {
    case CimValidationSeverity::Error:
        return QStringLiteral("error");
    case CimValidationSeverity::Warning:
        return QStringLiteral("warning");
    case CimValidationSeverity::Info:
    default:
        return QStringLiteral("info");
    }
}

QString issueCodeKey(CimValidationIssueCode code)
{
    switch (code)
    {
    case CimValidationIssueCode::InvalidObject:
        return QStringLiteral("invalidObject");
    case CimValidationIssueCode::MissingReference:
        return QStringLiteral("missingReference");
    case CimValidationIssueCode::DanglingGraphicBinding:
        return QStringLiteral("danglingGraphicBinding");
    case CimValidationIssueCode::MissingGraphicBinding:
        return QStringLiteral("missingGraphicBinding");
    case CimValidationIssueCode::EmptyModel:
    default:
        return QStringLiteral("emptyModel");
    }
}

void appendValidationIssues(QDomDocument& document,
                            QDomElement& root,
                            const CimModelValidationReport& report)
{
    QDomElement validation = document.createElement(QStringLiteral("validation"));
    validation.setAttribute(QStringLiteral("errors"), report.summary.errorCount);
    validation.setAttribute(QStringLiteral("warnings"), report.summary.warningCount);
    validation.setAttribute(QStringLiteral("infos"), report.summary.infoCount);

    for (const CimValidationIssue& issue : report.issues)
    {
        QDomElement issueElement = document.createElement(QStringLiteral("issue"));
        issueElement.setAttribute(QStringLiteral("severity"), severityKey(issue.severity));
        issueElement.setAttribute(QStringLiteral("code"), issueCodeKey(issue.code));
        if (!issue.objectMrid.isEmpty())
            issueElement.setAttribute(QStringLiteral("mrid"), issue.objectMrid);
        if (!issue.className.isEmpty())
            issueElement.setAttribute(QStringLiteral("class"), issue.className);
        if (!issue.relationName.isEmpty())
            issueElement.setAttribute(QStringLiteral("relation"), issue.relationName);
        if (!issue.targetMrid.isEmpty())
            issueElement.setAttribute(QStringLiteral("targetMrid"), issue.targetMrid);
        if (!issue.graphicItemId.isEmpty())
            issueElement.setAttribute(QStringLiteral("graphicItemId"), issue.graphicItemId);
        issueElement.appendChild(document.createTextNode(issue.message));
        validation.appendChild(issueElement);
    }

    root.appendChild(validation);
}

QStringList exportMridsForOptions(const CimModel& model,
                                  const CimModelSubsetExportOptions& options,
                                  QStringList* missingMrids)
{
    QStringList requested = uniqueOrdered(options.mrids);
    if (requested.isEmpty() && options.exportAllWhenMridsEmpty)
    {
        for (const CimObject& object : model.allObjects())
            requested << object.mrid;
    }

    QStringList result;
    QSet<QString> exported;
    for (int i = 0; i < requested.size(); ++i)
    {
        const QString mrid = requested.at(i);
        const CimObject* object = model.objectByMrid(mrid);
        if (!object)
        {
            if (missingMrids)
                *missingMrids << mrid;
            continue;
        }

        if (!exported.contains(mrid))
        {
            exported.insert(mrid);
            result << mrid;
        }

        if (!options.includeReferencedObjects)
            continue;

        for (const CimReference& reference : object->references)
        {
            const QString target = reference.targetMrid.trimmed();
            if (target.isEmpty() || exported.contains(target))
                continue;
            if (!model.contains(target))
                continue;
            exported.insert(target);
            result << target;
            requested << target;
        }
    }

    return result;
}

} // namespace

QString CimModelSubsetExportResult::xmlText(int indent) const
{
    return document.toString(indent);
}

QStringList CimModelSubsetExportResult::reportLines() const
{
    QStringList lines;
    lines << QStringLiteral("导出成功：%1").arg(success ? QStringLiteral("是") : QStringLiteral("否"));
    lines << QStringLiteral("请求对象数：%1").arg(summary.requestedCount);
    lines << QStringLiteral("导出对象数：%1").arg(summary.exportedObjectCount);
    lines << QStringLiteral("缺失对象数：%1").arg(summary.missingObjectCount);
    lines << QStringLiteral("校验错误数：%1").arg(summary.validationErrorCount);
    lines << QStringLiteral("校验警告数：%1").arg(summary.validationWarningCount);
    if (!missingMrids.isEmpty())
        lines << QStringLiteral("缺失对象：%1").arg(missingMrids.join(QStringLiteral(", ")));
    lines << validationReport.messages();
    return lines;
}

CimModelSubsetExportResult exportCimModelSubsetToXml(
    const CimModel& model,
    const CimModelSubsetExportOptions& options,
    const CimIdIndex* graphicIndex)
{
    CimModelSubsetExportResult result;
    result.summary.requestedCount = uniqueOrdered(options.mrids).size();
    if (result.summary.requestedCount == 0 && options.exportAllWhenMridsEmpty)
        result.summary.requestedCount = model.objectCount();

    CimModelValidationOptions validationOptions;
    validationOptions.requireGraphicBindingsForMappableObjects =
        options.requireGraphicBindingsForMappableObjects;
    result.validationReport = validateCimModelStructure(model, graphicIndex, validationOptions);
    result.summary.validationErrorCount = result.validationReport.summary.errorCount;
    result.summary.validationWarningCount = result.validationReport.summary.warningCount;

    const QStringList exportMrids = exportMridsForOptions(model, options, &result.missingMrids);
    result.summary.exportedObjectCount = exportMrids.size();
    result.summary.missingObjectCount = result.missingMrids.size();
    result.success = result.summary.ok();

    QDomDocument document(QStringLiteral("cimSubsetExport"));
    QDomElement root = document.createElement(QStringLiteral("cimSubsetExport"));
    root.setAttribute(QStringLiteral("projection"), QStringLiteral("PowerSystemModel"));
    root.setAttribute(QStringLiteral("success"), result.success ? QStringLiteral("true") : QStringLiteral("false"));
    document.appendChild(root);

    QDomElement summary = document.createElement(QStringLiteral("summary"));
    summary.setAttribute(QStringLiteral("requestedObjects"), result.summary.requestedCount);
    summary.setAttribute(QStringLiteral("exportedObjects"), result.summary.exportedObjectCount);
    summary.setAttribute(QStringLiteral("missingObjects"), result.summary.missingObjectCount);
    summary.setAttribute(QStringLiteral("validationErrors"), result.summary.validationErrorCount);
    summary.setAttribute(QStringLiteral("validationWarnings"), result.summary.validationWarningCount);
    root.appendChild(summary);

    QDomElement missing = document.createElement(QStringLiteral("missingObjects"));
    for (const QString& mrid : result.missingMrids)
    {
        QDomElement item = document.createElement(QStringLiteral("object"));
        item.setAttribute(QStringLiteral("mrid"), mrid);
        missing.appendChild(item);
    }
    root.appendChild(missing);

    QDomElement objectsElement = document.createElement(QStringLiteral("objects"));
    for (const QString& mrid : exportMrids)
    {
        const CimObject* object = model.objectByMrid(mrid);
        if (!object)
            continue;

        QDomElement objectElement = document.createElement(QStringLiteral("object"));
        objectElement.setAttribute(QStringLiteral("mrid"), object->mrid);
        objectElement.setAttribute(QStringLiteral("class"), object->className);
        if (!object->name.isEmpty())
            objectElement.setAttribute(QStringLiteral("name"), object->name);
        if (!object->sourceFile.isEmpty())
            objectElement.setAttribute(QStringLiteral("sourceFile"), object->sourceFile);
        if (!object->sourceNamespace.isEmpty())
            objectElement.setAttribute(QStringLiteral("sourceNamespace"), object->sourceNamespace);
        if (!object->profileTags.isEmpty())
            objectElement.setAttribute(QStringLiteral("profiles"), object->profileTags.join(QStringLiteral(",")));
        if (graphicIndex && graphicIndex->hasBinding(object->mrid))
            objectElement.setAttribute(QStringLiteral("graphicItemId"),
                                       graphicIndex->graphicItemIdForMrid(object->mrid));

        QDomElement attributesElement = document.createElement(QStringLiteral("attributes"));
        QStringList attributeNames = object->attributeMap.keys();
        attributeNames.sort();
        for (const QString& name : attributeNames)
        {
            QDomElement attributeElement = document.createElement(QStringLiteral("attribute"));
            attributeElement.setAttribute(QStringLiteral("name"), name);
            attributeElement.setAttribute(QStringLiteral("type"), object->attributeMap.value(name).typeName());
            attributeElement.appendChild(document.createTextNode(valueToExportString(object->attributeMap.value(name))));
            attributesElement.appendChild(attributeElement);
        }
        objectElement.appendChild(attributesElement);

        QDomElement referencesElement = document.createElement(QStringLiteral("references"));
        for (const CimReference& reference : object->references)
        {
            QDomElement referenceElement = document.createElement(QStringLiteral("reference"));
            referenceElement.setAttribute(QStringLiteral("name"), reference.relationName);
            referenceElement.setAttribute(QStringLiteral("targetMrid"), reference.targetMrid);
            referenceElement.setAttribute(QStringLiteral("missing"),
                                          model.contains(reference.targetMrid) ? QStringLiteral("false")
                                                                              : QStringLiteral("true"));
            referencesElement.appendChild(referenceElement);
        }
        objectElement.appendChild(referencesElement);

        objectsElement.appendChild(objectElement);
    }
    root.appendChild(objectsElement);
    appendValidationIssues(document, root, result.validationReport);

    QDomElement reportElement = document.createElement(QStringLiteral("report"));
    for (const QString& line : result.reportLines())
        appendTextElement(document, reportElement, QStringLiteral("line"), line);
    root.appendChild(reportElement);

    result.document = document;
    return result;
}
