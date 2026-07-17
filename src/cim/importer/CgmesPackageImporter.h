#ifndef CGMESPACKAGEIMPORTER_H
#define CGMESPACKAGEIMPORTER_H

#include "ICimImporter.h"

#include <QHash>
#include <QStringList>

class QFileInfo;
class QDomDocument;
class QDomElement;

class CgmesPackageImporter : public ICimImporter
{
public:
    CimImportResult importFromPath(const QString& path) const override;

private:
    struct ParsedObjectData
    {
        CimObject object;
        QStringList profileTags;
    };

    static QStringList normalizedProfileNames(const QFileInfo& fileInfo, const QDomDocument& document);
    static QString normalizeProfileUri(const QString& uri);
    static QString normalizeReferenceValue(const QString& value);
    static QString detectMrid(const QDomElement& element);
    static bool isCompatibleClassHierarchy(const QString& existingClassName,
                                           const QString& incomingClassName,
                                           QString* preferredClassName);
    static bool parseXmlFile(const QFileInfo& fileInfo,
                             const QStringList& profileTags,
                             QHash<QString, ParsedObjectData>& objectMap,
                             QStringList& logs,
                             QStringList& warnings,
                             QStringList& errors);
    static void synthesizeInverseTransformerEndReferences(QHash<QString, ParsedObjectData>& objectMap);
};

#endif // CGMESPACKAGEIMPORTER_H
