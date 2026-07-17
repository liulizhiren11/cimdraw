#ifndef CIMDRAWSCENEPOWERTOPOLOGYANALYSISCONTROLLER_H
#define CIMDRAWSCENEPOWERTOPOLOGYANALYSISCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

class QString;
class CimdrawScene;

class CimdrawScenePowerTopologyAnalysisController
{
public:
    QVector<int> powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList directedReachablePowerDevices(CimdrawScene* scene, const QString& sourceDeviceId) const;

    PowerTopologyIslandAnalysis analyzePowerIslands(CimdrawScene* scene) const;
    PowerTopologyBranchAnalysis analyzePowerBranches(CimdrawScene* scene, const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis analyzePowerLoops(CimdrawScene* scene) const;

    PowerTopologyPath shortestPowerSupplyPath(CimdrawScene* scene,
                                              const QString& sourceDeviceId,
                                              const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                      const QString& sourceDeviceId,
                                                      const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestPowerGroundPath(CimdrawScene* scene, const QString& sourceDeviceId) const;

    PowerTopologySwitchChangePreview previewPowerSwitchTopologyChange(CimdrawScene* scene,
                                                                      const QString& deviceId,
                                                                      int toSwitchPosition) const;
    PowerTopologyProtectionRangePreview previewPowerProtectionRange(CimdrawScene* scene,
                                                                    const QString& deviceId,
                                                                    int toSwitchPosition,
                                                                    const QString& sourceDeviceId) const;
    PowerTopologyOperationPreview previewPowerSwitchOperation(CimdrawScene* scene,
                                                              const QString& deviceId,
                                                              int toSwitchPosition,
                                                              const QString& supplySourceDeviceId,
                                                              const QString& supplyTargetDeviceId,
                                                              const QString& groundSourceDeviceId) const;
};

#endif
