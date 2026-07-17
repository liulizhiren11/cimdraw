#ifndef POWERTOPOLOGYBUILDER_H
#define POWERTOPOLOGYBUILDER_H

#include "PowerTopologyTypes.h"

class CimdrawScene;

class PowerTopologyBuilder
{
public:
    static PowerTopologyAnalysisSnapshot build(CimdrawScene* scene);
    static PowerTopologyAnalysisSnapshot buildWithSwitchOverride(CimdrawScene* scene,
                                                                 const QString& deviceId,
                                                                 int switchPosition);
};

#endif // POWERTOPOLOGYBUILDER_H
