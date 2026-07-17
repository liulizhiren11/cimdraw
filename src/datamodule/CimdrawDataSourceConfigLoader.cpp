#include "CimdrawDataSourceConfigLoader.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>

CimdrawDataSourceConfigLoader::LoadResult CimdrawDataSourceConfigLoader::loadRootObject(
    const QString& absolutePath) const
{
    LoadResult result;

    QFile file(absolutePath);
    if (!file.exists())
    {
        result.error = QStringLiteral("config not found");
        return result;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        result.error = file.errorString();
        return result;
    }

    QJsonParseError parseError{};
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        result.error = parseError.errorString();
        return result;
    }

    result.success = true;
    result.root = document.object();
    return result;
}
