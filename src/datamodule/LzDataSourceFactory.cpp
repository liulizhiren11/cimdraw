#include "LzDataSourceFactory.h"

#include "LzIDataSource.h"
#include "sources/LzWiringTcpJsonSource.h"

LzIDataSource* LzDataSourceFactory::createConfiguredSource(const QString& typeId,
                                                           const QJsonObject& config,
                                                           LzSceneResolver resolver,
                                                           QObject* parent) const
{
    LzIDataSource* source = nullptr;
    if (typeId == QLatin1String("wiring_tcp_json"))
        source = new LzWiringTcpJsonSource(std::move(resolver), parent);
    if (!source)
        return nullptr;
    if (!source->configure(config))
    {
        delete source;
        return nullptr;
    }
    return source;
}
