#include "CimImportController.h"

#include <QFileInfo>
#include <QObject>

#include "cim/importer/CgmesPackageImporter.h"

CimImportController::ExecuteResult CimImportController::executeImport(const QString& path) const
{
    ExecuteResult result;
    if (path.trimmed().isEmpty())
        return result;

    QFileInfo importTarget(path);
    QString resolvedPath = path;
    if (importTarget.exists() && importTarget.isFile())
    {
        const QString suffix = importTarget.suffix().trimmed().toLower();
        if (suffix != QStringLiteral("xml") && suffix != QStringLiteral("rdf"))
        {
            result.validationError = QObject::tr("请选择包含 CIM/CGMES 数据的目录，或选择 xml/rdf 文件。");
            return result;
        }
        resolvedPath = importTarget.absolutePath();
    }

    CgmesPackageImporter importer;
    result.importResult = importer.importFromPath(resolvedPath);
    result.accepted = true;
    result.resolvedPath = resolvedPath;
    return result;
}
