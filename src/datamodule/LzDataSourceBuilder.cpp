#include "LzDataSourceBuilder.h"

#include "LzDataSourceFactory.h"
#include "LzIDataSource.h"

#include <QJsonArray>
#include <QJsonObject>

#include <QDebug>

LzDataSourceBuilder::BuildResult LzDataSourceBuilder::buildSources(const QJsonObject& root,
                                                                   LzSceneResolver resolver,
                                                                   QObject* parent) const
{
    BuildResult result;

    const QJsonValue sourcesValue = root.value(QStringLiteral("sources"));
    if (!sourcesValue.isArray())
    {
        result.error = QStringLiteral("root.sources is not an array");
        return result;
    }

    LzDataSourceFactory factory;
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
            qWarning() << "LzDataSourceBuilder: source entry missing type, skipped";
            continue;
        }

        LzIDataSource* source = factory.createConfiguredSource(typeId, sourceObject, resolver, parent);
        if (!source)
        {
            qWarning() << "LzDataSourceBuilder: create/configure failed for type" << typeId;
            continue;
        }
        result.sources.append(source);
    }

    return result;
}
