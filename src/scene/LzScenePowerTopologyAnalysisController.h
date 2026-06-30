#ifndef LZSCENEPOWERTOPOLOGYANALYSISCONTROLLER_H
#define LZSCENEPOWERTOPOLOGYANALYSISCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

class QString;
class LzScene;

class LzScenePowerTopologyAnalysisController
{
public:
    QVector<int> powerDeviceNodeIds(LzScene* scene, const QString& deviceId) const;
    QStringList powerDeviceConductorIds(LzScene* scene, const QString& deviceId) const;
    QStringList connectedPowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList busbarAttachedPowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList reachablePowerDevices(LzScene* scene, const QString& deviceId) const;
    QStringList directedReachablePowerDevices(LzScene* scene, const QString& sourceDeviceId) const;

    PowerTopologyIslandAnalysis analyzePowerIslands(LzScene* scene) const;
    PowerTopologyBranchAnalysis analyzePowerBranches(LzScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis analyzePowerLoops(LzScene* scene) const;

    PowerTopologyPath shortestPowerSupplyPath(LzScene* scene,
                                              const QString& sourceDeviceId,
                                              const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedPowerSupplyPath(LzScene* scene,
                                                      const QString& sourceDeviceId,
                                                      const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestPowerGroundPath(LzScene* scene, const QString& sourceDeviceId) const;

    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(LzScene* scene,
                                                                      const QString& deviceId,
                                                                      int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(LzScene* scene,
                                                                    const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview previewPowerSwitchOperation(LzScene* scene,
                                                              const QString& deviceId,
                                                              int toSwitchPosition,
                                                              const QString& supplySourceDeviceId,
                                                              const QString& supplyTargetDeviceId,
                                                              const QString& groundSourceDeviceId) const;
};

#endif
