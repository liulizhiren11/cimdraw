#include "CimdrawDataSourceManager.h"
#include "CimdrawDataSourceBuilder.h"
#include "CimdrawDataSourceConfigLoader.h"
#include "CimdrawIDataSource.h"
#include "CimdrawDataSourceRuntimeController.h"
#include "CimdrawAppConfig.h"

#include <QJsonObject>

#include <QDebug>

CimdrawDataSourceManager::CimdrawDataSourceManager(QObject* parent)
    : QObject(parent)
{
}

CimdrawDataSourceManager::~CimdrawDataSourceManager()
{
    clearSources();
}

void CimdrawDataSourceManager::setSceneResolver(CimdrawSceneResolver resolver)
{
    sceneResolver_ = std::move(resolver);
}

void CimdrawDataSourceManager::clearSources()
{
    CimdrawDataSourceRuntimeController controller;
    controller.clearSources(sources_);
}

bool CimdrawDataSourceManager::loadFromFile(const QString& absolutePath)
{
    lastError_.clear();
    lastConfigPath_ = absolutePath;
    clearSources();

    CimdrawDataSourceConfigLoader loader;
    const CimdrawDataSourceConfigLoader::LoadResult loadResult = loader.loadRootObject(absolutePath);
    if (!loadResult.success)
    {
        lastError_ = loadResult.error;
        if (lastError_ == QLatin1String("config not found"))
            qInfo() << "CimdrawDataSourceManager:" << absolutePath << "missing, no data sources started";
        else
            qWarning() << "CimdrawDataSourceManager: load failed:" << absolutePath << lastError_;
        return false;
    }

    const QJsonObject root = loadResult.root;
    CimdrawAppConfig::applyFromJsonRoot(root);

    CimdrawDataSourceBuilder builder;
    const CimdrawDataSourceBuilder::BuildResult buildResult =
        builder.buildSources(root, sceneResolver_, this);
    if (!buildResult.error.isEmpty())
    {
        lastError_ = buildResult.error;
        qWarning() << "CimdrawDataSourceManager:" << lastError_;
        return false;
    }
    sources_ = buildResult.sources;

    return true;
}

void CimdrawDataSourceManager::startAll()
{
    CimdrawDataSourceRuntimeController controller;
    controller.startAll(sources_);
}

void CimdrawDataSourceManager::stopAll()
{
    CimdrawDataSourceRuntimeController controller;
    controller.stopAll(sources_);
}
