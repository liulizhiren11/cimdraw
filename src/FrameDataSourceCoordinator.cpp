#include "FrameDataSourceCoordinator.h"

#include "datamodule/CimdrawDataSourceManager.h"

bool FrameDataSourceCoordinator::initializeManager(CimdrawDataSourceManager* manager,
                                                   const QString& configPath,
                                                   CimdrawSceneResolver resolver) const
{
    if (!manager)
        return false;

    manager->setSceneResolver(std::move(resolver));
    manager->loadFromFile(configPath);
    manager->startAll();
    return true;
}
