#ifndef FRAMEDATASOURCECOORDINATOR_H
#define FRAMEDATASOURCECOORDINATOR_H

#include "datamodule/LzSceneResolver.h"

class LzDataSourceManager;
class QString;

class FrameDataSourceCoordinator
{
public:
    bool initializeManager(LzDataSourceManager* manager,
                           const QString& configPath,
                           LzSceneResolver resolver) const;
};

#endif
