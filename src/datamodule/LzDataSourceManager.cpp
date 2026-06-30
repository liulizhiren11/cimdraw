#include "LzDataSourceManager.h"
#include "LzDataSourceBuilder.h"
#include "LzDataSourceConfigLoader.h"
#include "LzIDataSource.h"
#include "LzDataSourceRuntimeController.h"
#include "LzAppConfig.h"

#include <QJsonObject>

#include <QDebug>

LzDataSourceManager::LzDataSourceManager(QObject* parent)
    : QObject(parent)
{
}

LzDataSourceManager::~LzDataSourceManager()
{
    clearSources();
}

void LzDataSourceManager::setSceneResolver(LzSceneResolver resolver)
{
    sceneResolver_ = std::move(resolver);
}

void LzDataSourceManager::clearSources()
{
    LzDataSourceRuntimeController controller;
    controller.clearSources(sources_);
}

bool LzDataSourceManager::loadFromFile(const QString& absolutePath)
{
    lastError_.clear();
    lastConfigPath_ = absolutePath;
    clearSources();

    LzDataSourceConfigLoader loader;
    const LzDataSourceConfigLoader::LoadResult loadResult = loader.loadRootObject(absolutePath);
    if (!loadResult.success)
    {
        lastError_ = loadResult.error;
        if (lastError_ == QLatin1String("config not found"))
            qInfo() << "LzDataSourceManager:" << absolutePath << "missing, no data sources started";
        else
            qWarning() << "LzDataSourceManager: load failed:" << absolutePath << lastError_;
        return false;
    }

    const QJsonObject root = loadResult.root;
    LzAppConfig::applyFromJsonRoot(root);

    LzDataSourceBuilder builder;
    const LzDataSourceBuilder::BuildResult buildResult =
        builder.buildSources(root, sceneResolver_, this);
    if (!buildResult.error.isEmpty())
    {
        lastError_ = buildResult.error;
        qWarning() << "LzDataSourceManager:" << lastError_;
        return false;
    }
    sources_ = buildResult.sources;

    return true;
}

void LzDataSourceManager::startAll()
{
    LzDataSourceRuntimeController controller;
    controller.startAll(sources_);
}

void LzDataSourceManager::stopAll()
{
    LzDataSourceRuntimeController controller;
    controller.stopAll(sources_);
}
