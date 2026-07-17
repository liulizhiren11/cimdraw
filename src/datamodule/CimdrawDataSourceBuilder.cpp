#include "CimdrawDataSourceBuilder.h"

#include "CimdrawDataSourceFactory.h"
#include "CimdrawIDataSource.h"

#include <QJsonArray>
#include <QJsonObject>

#include <QDebug>

CimdrawDataSourceBuilder::BuildResult CimdrawDataSourceBuilder::buildSources(const QJsonObject& root,
                                                                   CimdrawSceneResolver resolver,
                                                                   QObject* parent) const
{
    BuildResult result;

    const QJsonValue sourcesValue = root.value(QStringLiteral("sources"));
    if (!sourcesValue.isArray())
    {
        result.error = QStringLiteral("root.sources is not an array");
        return result;
    }

    CimdrawDataSourceFactory factory;
    for (const QJsonValue& entryValue : sourcesValue.toArray())
    {
        if (!entryValue.isObject())
            continue;

        const QJsonObject sourceObject = entryValue.toObject();
        if (!sourceObject.value(QStringLiteral("enabled")).toBool(true))
            continue;

        const QString typeId = sourceObject.value(QStringLiteral("type")).toString();
        if (typeId.isEmpty())
        {
            qWarning() << "CimdrawDataSourceBuilder: source entry missing type, skipped";
            continue;
        }

        CimdrawIDataSource* source = factory.createConfiguredSource(typeId, sourceObject, resolver, parent);
        if (!source)
        {
            qWarning() << "CimdrawDataSourceBuilder: create/configure failed for type" << typeId;
            continue;
        }
        result.sources.append(source);
    }

    return result;
}
