#include "CgmesPackageImporter.h"

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QSet>

#include "cim/model/CimClassLineage.h"

namespace {

QString localName(const QString& tagName)
{
    const int idx = tagName.indexOf(QLatin1Char(':'));
    return idx >= 0 ? tagName.mid(idx + 1) : tagName;
}

QStringList detectProfilesFromName(const QString& name)
{
    const QString upper = name.toUpper();
    QStringList profiles;
    if (upper.contains(QStringLiteral("EQ")))
        profiles.push_back(QStringLiteral("EQ"));
    if (upper.contains(QStringLiteral("SSH")))
        profiles.push_back(QStringLiteral("SSH"));
    if (upper.contains(QStringLiteral("TP")))
        profiles.push_back(QStringLiteral("TP"));
    if (upper.contains(QStringLiteral("SV")))
        profiles.push_back(QStringLiteral("SV"));
    if (upper.contains(QStringLiteral("DL")))
        profiles.push_back(QStringLiteral("DL"));
    if (upper.contains(QStringLiteral("GL")))
        profiles.push_back(QStringLiteral("GL"));
    if (upper.contains(QStringLiteral("DI")))
        profiles.push_back(QStringLiteral("DI"));
    return profiles;
}

QString textValue(const QDomElement& element)
{
    return element.text().trimmed();
}

bool hasReference(const CimObject& object, const QString& relationName, const QString& targetMrid)
{
    for (const CimReference& reference : object.references)
    {
        if (reference.relationName == relationName && reference.targetMrid == targetMrid)
            return true;
    }
    return false;
}

bool isLikelyCimRdfDocument(const QDomDocument& document)
{
    const QDomElement root = document.documentElement();
    if (root.isNull())
        return false;

    if (localName(root.tagName()) != QStringLiteral("RDF"))
        return false;

    for (QDomElement element = root.firstChildElement(); !element.isNull(); element = element.nextSiblingElement())
    {
        const QString tagName = element.tagName();
        if (tagName.startsWith(QStringLiteral("cim:"), Qt::CaseInsensitive))
            return true;
        if (tagName.startsWith(QStringLiteral("md:"), Qt::CaseInsensitive)
            && localName(tagName) == QStringLiteral("FullModel"))
        {
            return true;
        }
    }
    return false;
}

}

QString CgmesPackageImporter::normalizeProfileUri(const QString& uri)
{
    const QString normalized = uri.trimmed();
    if (normalized.contains(QStringLiteral("Equipment"), Qt::CaseInsensitive))
        return QStringLiteral("EQ");
    if (normalized.contains(QStringLiteral("SteadyStateHypothesis"), Qt::CaseInsensitive))
        return QStringLiteral("SSH");
    if (normalized.contains(QStringLiteral("Topology"), Qt::CaseInsensitive))
        return QStringLiteral("TP");
    if (normalized.contains(QStringLiteral("StateVariables"), Qt::CaseInsensitive))
        return QStringLiteral("SV");
    if (normalized.contains(QStringLiteral("DiagramLayout"), Qt::CaseInsensitive))
        return QStringLiteral("DL");
    if (normalized.contains(QStringLiteral("GeographicalLocation"), Qt::CaseInsensitive))
        return QStringLiteral("GL");
    if (normalized.contains(QStringLiteral("Diagram"), Qt::CaseInsensitive))
        return QStringLiteral("DI");
    return QString();
}

QStringList CgmesPackageImporter::normalizedProfileNames(const QFileInfo& fileInfo, const QDomDocument& document)
{
    QStringList profiles;
    const QDomElement root = document.documentElement();
    for (QDomElement element = root.firstChildElement(); !element.isNull(); element = element.nextSiblingElement())
    {
        if (localName(element.tagName()) != QStringLiteral("FullModel"))
            continue;

        for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        {
            if (localName(child.tagName()) != QStringLiteral("Model.profile"))
                continue;

            const QString profile = normalizeProfileUri(textValue(child));
            if (!profile.isEmpty() && !profiles.contains(profile))
                profiles.push_back(profile);
        }
    }

    if (!profiles.isEmpty())
        return profiles;

    const QStringList detected = detectProfilesFromName(fileInfo.completeBaseName());
    if (!detected.isEmpty())
        return detected;

    return QStringList{QStringLiteral("GENERIC")};
}

QString CgmesPackageImporter::normalizeReferenceValue(const QString& value)
{
    QString normalized = value.trimmed();
    if (normalized.startsWith(QStringLiteral("#")))
        normalized.remove(0, 1);
    return normalized;
}

QString CgmesPackageImporter::detectMrid(const QDomElement& element)
{
    const QString rdfId = normalizeReferenceValue(element.attribute(QStringLiteral("rdf:ID")));
    if (!rdfId.isEmpty())
        return rdfId;

    const QString rdfAbout = normalizeReferenceValue(element.attribute(QStringLiteral("rdf:about")));
    if (!rdfAbout.isEmpty())
        return rdfAbout;

    for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
    {
        if (localName(child.tagName()) == QStringLiteral("IdentifiedObject.mRID"))
        {
            const QString mrid = textValue(child);
            if (!mrid.isEmpty())
                return mrid;
        }
    }
    return QString();
}

bool CgmesPackageImporter::isCompatibleClassHierarchy(const QString& existingClassName,
                                                      const QString& incomingClassName,
                                                      QString* preferredClassName)
{
    if (existingClassName == incomingClassName)
    {
        if (preferredClassName)
            *preferredClassName = existingClassName;
        return true;
    }

    if (CimClassLineage::isA(existingClassName, incomingClassName))
    {
        if (preferredClassName)
            *preferredClassName = existingClassName;
        return true;
    }

    if (CimClassLineage::isA(incomingClassName, existingClassName))
    {
        if (preferredClassName)
            *preferredClassName = incomingClassName;
        return true;
    }

    return false;
}

bool CgmesPackageImporter::parseXmlFile(const QFileInfo& fileInfo,
                                        const QStringList& profileTags,
                                        QHash<QString, ParsedObjectData>& objectMap,
                                        QStringList& logs,
                                        QStringList& warnings,
                                        QStringList& errors)
{
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        errors.push_back(QStringLiteral("无法打开文件: %1").arg(fileInfo.fileName()));
        return false;
    }

    QDomDocument document;
    QString parseError;
    int errorLine = 0;
    int errorColumn = 0;
    if (!document.setContent(&file, &parseError, &errorLine, &errorColumn))
    {
        errors.push_back(QStringLiteral("XML 解析失败 %1 (%2:%3): %4")
                             .arg(fileInfo.fileName())
                             .arg(errorLine)
                             .arg(errorColumn)
                             .arg(parseError));
        return false;
    }

    if (!isLikelyCimRdfDocument(document))
    {
        errors.push_back(QStringLiteral("文件 %1 不是受支持的 CIM/CGMES RDF 文件。")
                             .arg(fileInfo.fileName()));
        return false;
    }

    int importedCount = 0;
    for (QDomElement element = document.documentElement().firstChildElement();
         !element.isNull();
         element = element.nextSiblingElement())
    {
        if (localName(element.tagName()) == QStringLiteral("FullModel"))
            continue;

        CimObject object;
        object.className = localName(element.tagName());
        object.mrid = detectMrid(element);
        object.sourceFile = fileInfo.absoluteFilePath();
        object.sourceNamespace = document.documentElement().namespaceURI().trimmed();

        for (QDomElement child = element.firstChildElement(); !child.isNull(); child = child.nextSiblingElement())
        {
            const QString childName = localName(child.tagName());
            const QString resourceValue = normalizeReferenceValue(child.attribute(QStringLiteral("rdf:resource")));

            if (childName == QStringLiteral("IdentifiedObject.mRID"))
            {
                if (object.mrid.isEmpty())
                    object.mrid = textValue(child);
                continue;
            }

            if (childName == QStringLiteral("IdentifiedObject.name"))
            {
                object.name = textValue(child);
                continue;
            }

            if (!resourceValue.isEmpty())
            {
                object.addReference(childName, resourceValue);
                object.setAttribute(childName, resourceValue);
                continue;
            }

            const QString value = textValue(child);
            if (!value.isEmpty())
                object.setAttribute(childName, value);
        }

        if (!object.isValid())
        {
            errors.push_back(QStringLiteral("文件 %1 中存在无效对象，类名=%2")
                                 .arg(fileInfo.fileName(), object.className));
            continue;
        }

        ParsedObjectData merged = objectMap.value(object.mrid);
        if (!merged.object.mrid.isEmpty() && merged.object.className != object.className)
        {
            QString preferredClassName;
            if (!isCompatibleClassHierarchy(merged.object.className, object.className, &preferredClassName))
            {
                errors.push_back(QStringLiteral("对象 %1 在不同文件中类名不一致：%2 / %3")
                                     .arg(object.mrid, merged.object.className, object.className));
                continue;
            }

            if (preferredClassName != merged.object.className)
            {
                warnings.push_back(QStringLiteral("对象 %1 在不同文件中出现更具体的兼容类型：%2 -> %3，已按更具体类型合并。")
                                       .arg(object.mrid, merged.object.className, preferredClassName));
                merged.object.className = preferredClassName;
            }
        }

        bool duplicateWithinSameProfile = false;
        for (const QString& profileTag : profileTags)
        {
            if (merged.profileTags.contains(profileTag))
            {
                duplicateWithinSameProfile = true;
                warnings.push_back(QStringLiteral("对象 %1 在 Profile %2 中重复定义，已按首次定义合并处理。")
                                       .arg(object.mrid, profileTag));
            }
        }

        if (merged.object.mrid.isEmpty())
        {
            merged.object = object;
        }
        else
        {
            if (!object.name.trimmed().isEmpty())
                merged.object.name = object.name;
            if (!object.sourceFile.trimmed().isEmpty())
                merged.object.sourceFile = object.sourceFile;
            if (!object.sourceNamespace.trimmed().isEmpty())
                merged.object.sourceNamespace = object.sourceNamespace;
            for (auto it = object.attributeMap.constBegin(); it != object.attributeMap.constEnd(); ++it)
                merged.object.attributeMap.insert(it.key(), it.value());
            for (const CimReference& reference : object.references)
                merged.object.references.push_back(reference);
        }

        Q_UNUSED(duplicateWithinSameProfile);
        for (const QString& profileTag : profileTags)
        {
            if (!merged.profileTags.contains(profileTag))
                merged.profileTags.push_back(profileTag);
        }
        merged.object.profileTags = merged.profileTags;
        objectMap.insert(merged.object.mrid, merged);
        ++importedCount;
    }

    if (importedCount == 0)
    {
        logs.push_back(QStringLiteral("文件 %1 未包含可导入的 CIM 对象，已跳过对象创建。")
                           .arg(fileInfo.fileName()));
        return true;
    }

    logs.push_back(QStringLiteral("文件 %1 导入对象 %2 个。").arg(fileInfo.fileName()).arg(importedCount));
    return importedCount > 0;
}

void CgmesPackageImporter::synthesizeInverseTransformerEndReferences(QHash<QString, ParsedObjectData>& objectMap)
{
    const QString relationToTransformer = QStringLiteral("PowerTransformerEnd.PowerTransformer");
    const QString relationToEnd = QStringLiteral("PowerTransformer.PowerTransformerEnd");

    for (auto it = objectMap.constBegin(); it != objectMap.constEnd(); ++it)
    {
        const CimObject& object = it.value().object;
        if (object.className != QStringLiteral("PowerTransformerEnd"))
            continue;

        for (const CimReference& reference : object.references)
        {
            if (reference.relationName != relationToTransformer)
                continue;

            auto transformerIt = objectMap.find(reference.targetMrid);
            if (transformerIt == objectMap.end())
                continue;
            if (transformerIt->object.className != QStringLiteral("PowerTransformer"))
                continue;
            if (hasReference(transformerIt->object, relationToEnd, object.mrid))
                continue;

            transformerIt->object.addReference(relationToEnd, object.mrid);
            if (!transformerIt->profileTags.contains(QStringLiteral("EQ"))
                && object.profileTags.contains(QStringLiteral("EQ")))
            {
                transformerIt->profileTags.push_back(QStringLiteral("EQ"));
                transformerIt->object.profileTags = transformerIt->profileTags;
            }
        }
    }
}

CimImportResult CgmesPackageImporter::importFromPath(const QString& path) const
{
    CimImportResult result;
    const QFileInfo info(path);
    if (!info.exists())
    {
        result.errors.push_back(QStringLiteral("导入路径不存在: %1").arg(path));
        return result;
    }

    QHash<QString, ParsedObjectData> objectMap;
    QSet<QString> profiles;

    auto finalizeResult = [&]() {
        synthesizeInverseTransformerEndReferences(objectMap);
        result.model.clear();
        for (auto it = objectMap.constBegin(); it != objectMap.constEnd(); ++it)
            result.model.addObject(it.value().object);
        result.loadedProfiles = profiles.values();
        result.loadedProfiles.sort();
    };

    if (info.isFile())
    {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            result.errors.push_back(QStringLiteral("无法打开文件: %1").arg(info.fileName()));
            return result;
        }
        QDomDocument document;
        QString parseError;
        int errorLine = 0;
        int errorColumn = 0;
        if (!document.setContent(&file, &parseError, &errorLine, &errorColumn))
        {
            result.errors.push_back(QStringLiteral("XML 解析失败 %1 (%2:%3): %4")
                                        .arg(info.fileName())
                                        .arg(errorLine)
                                        .arg(errorColumn)
                                        .arg(parseError));
            return result;
        }
        const QStringList profileTags = normalizedProfileNames(info, document);
        for (const QString& profileTag : profileTags)
            profiles.insert(profileTag);
        parseXmlFile(info, profileTags, objectMap, result.logs, result.warnings, result.errors);
        finalizeResult();
    }
    else
    {
        const QDir dir(info.absoluteFilePath());
        const QFileInfoList files = dir.entryInfoList(QStringList{QStringLiteral("*.xml"), QStringLiteral("*.rdf")},
                                                      QDir::Files | QDir::NoSymLinks,
                                                      QDir::Name);
        if (files.isEmpty())
        {
            result.errors.push_back(QStringLiteral("目录中未找到 XML/RDF 文件: %1").arg(info.absoluteFilePath()));
            return result;
        }

        for (const QFileInfo& fileInfo : files)
        {
            QFile file(fileInfo.absoluteFilePath());
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                result.errors.push_back(QStringLiteral("无法打开文件: %1").arg(fileInfo.fileName()));
                continue;
            }
            QDomDocument document;
            QString parseError;
            int errorLine = 0;
            int errorColumn = 0;
            if (!document.setContent(&file, &parseError, &errorLine, &errorColumn))
            {
                result.errors.push_back(QStringLiteral("XML 解析失败 %1 (%2:%3): %4")
                                            .arg(fileInfo.fileName())
                                            .arg(errorLine)
                                            .arg(errorColumn)
                                            .arg(parseError));
                continue;
            }
            const QStringList profileTags = normalizedProfileNames(fileInfo, document);
            for (const QString& profileTag : profileTags)
                profiles.insert(profileTag);
            parseXmlFile(fileInfo, profileTags, objectMap, result.logs, result.warnings, result.errors);
        }
        finalizeResult();
    }

    if (result.loadedProfiles.isEmpty())
        result.logs.push_back(QStringLiteral("未识别到明确的 Profile，按通用数据集处理。"));
    else
        result.logs.push_back(QStringLiteral("识别到 Profile: %1").arg(result.loadedProfiles.join(QStringLiteral(", "))));

    if (result.model.objectCount() == 0)
    {
        if (result.errors.isEmpty())
            result.errors.push_back(QStringLiteral("导入完成，但未解析出任何对象。"));
        return result;
    }

    result.logs.push_back(QStringLiteral("已导入 %1 个对象。").arg(result.model.objectCount()));
    result.success = result.errors.isEmpty();
    return result;
}
