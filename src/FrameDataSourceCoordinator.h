#ifndef FRAMEDATASOURCECOORDINATOR_H
#define FRAMEDATASOURCECOORDINATOR_H

#include "datamodule/CimdrawSceneResolver.h"

class CimdrawDataSourceManager;
class QString;

class FrameDataSourceCoordinator
{
public:
    bool initializeManager(CimdrawDataSourceManager* manager,
                           const QString& configPath,
                           CimdrawSceneResolver resolver) const;
};

#endif
