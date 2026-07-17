#include "CimdrawDataSourceFactory.h"

#include "CimdrawIDataSource.h"
#include "sources/CimdrawWiringTcpJsonSource.h"

CimdrawIDataSource* CimdrawDataSourceFactory::createConfiguredSource(const QString& typeId,
                                                           const QJsonObject& config,
                                                           CimdrawSceneResolver resolver,
                                                           QObject* parent) const
{
    CimdrawIDataSource* source = nullptr;
    if (typeId == QLatin1String("wiring_tcp_json"))
        source = new CimdrawWiringTcpJsonSource(std::move(resolver), parent);
    if (!source)
        return nullptr;
    if (!source->configure(config))
    {
        delete source;
        return nullptr;
    }
    return source;
}
