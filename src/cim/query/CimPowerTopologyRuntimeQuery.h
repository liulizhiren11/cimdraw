#ifndef CIMPOWERTOPOLOGYRUNTIMEQUERY_H
#define CIMPOWERTOPOLOGYRUNTIMEQUERY_H

#include "topology/PowerTopologyProjection.h"

class QString;
class CimdrawScene;

class CimPowerTopologyRuntimeQuery
{
public:
    PowerTopologyProjection runtimePowerTopologyProjection(CimdrawScene* scene) const;
    PowerTopologyAnalysisSnapshot runtimePowerTopologySnapshot(CimdrawScene* scene) const;
    PowerTopologyDocumentExport runtimePowerTopologyDocumentExport(CimdrawScene* scene) const;

    QVector<int> powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const;
    QStringList connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const;
    QStringList directedReachablePowerDevices(CimdrawScene* scene, const QString& sourceDeviceId) const;

    PowerTopologyIslandAnalysis analyzePowerIslands(CimdrawScene* scene) const;
    PowerTopologyBranchAnalysis analyzePowerBranches(CimdrawScene* scene,
                                                     const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis analyzePowerLoops(CimdrawScene* scene) const;
    PowerTopologyPath shortestPowerSupplyPath(CimdrawScene* scene,
                                              const QString& sourceDeviceId,
                                              const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                      const QString& sourceDeviceId,
                                                      const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestPowerGroundPath(CimdrawScene* scene,
                                                    const QString& sourceDeviceId) const;
};

#endif
