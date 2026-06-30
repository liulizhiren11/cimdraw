#ifndef POWERTOPOLOGYBUILDER_H
#define POWERTOPOLOGYBUILDER_H

#include "PowerTopologyTypes.h"

class LzScene;

class PowerTopologyBuilder
{
public:
    static PowerTopologyAnalysisSnapshot build(LzScene* scene);
    static PowerTopologyAnalysisSnapshot buildWithSwitchOverride(LzScene* scene,
                                                                 const QString& deviceId,
                                                                 int switchPosition);
};

#endif // POWERTOPOLOGYBUILDER_H
