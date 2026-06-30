#include "FrameDataSourceCoordinator.h"

#include "datamodule/LzDataSourceManager.h"

bool FrameDataSourceCoordinator::initializeManager(LzDataSourceManager* manager,
                                                   const QString& configPath,
                                                   LzSceneResolver resolver) const
{
    if (!manager)
        return false;

    manager->setSceneResolver(std::move(resolver));
    manager->loadFromFile(configPath);
    manager->startAll();
    return true;
}
