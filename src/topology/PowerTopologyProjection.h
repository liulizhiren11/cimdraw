#ifndef POWERTOPOLOGYPROJECTION_H
#define POWERTOPOLOGYPROJECTION_H

#include "PowerTopologyTypes.h"

#include <QString>

class CimModel;
class CimdrawScene;

enum class PowerTopologyProjectionSourceKind : int
{
    ImportedModel = 0,
    RuntimeScene,
};

class PowerTopologyProjection
{
public:
    PowerTopologyProjection() = default;

    static PowerTopologyProjection fromImportedModel(const CimModel& model);
    static PowerTopologyProjection fromImportedSnapshot(const PowerTopologyAnalysisSnapshot& snapshot);
    static PowerTopologyProjection fromRuntimeScene(CimdrawScene* scene);
    static PowerTopologyProjection fromRuntimeSnapshot(const PowerTopologyAnalysisSnapshot& snapshot);

    bool isValid() const;
    PowerTopologyProjectionSourceKind sourceKind() const;
    QString sourceKindKey() const;
    QString sourceLabel() const;
    QString truthBoundaryText() const;

    const PowerTopologyAnalysisSnapshot& snapshot() const;

    QVector<int> deviceNodeIds(const QString& deviceId) const;
    QStringList deviceConductorIds(const QString& deviceId) const;
    QStringList connectedComponentDeviceIds(const QString& deviceId) const;
    QStringList busbarAttachedDevices(const QString& deviceId) const;
    QStringList reachableDevices(const QString& deviceId) const;
    QStringList directedReachableDevices(const QString& sourceDeviceId) const;

    PowerTopologyIslandAnalysis islandAnalysis() const;
    PowerTopologyBranchAnalysis branchAnalysis(const QString& sourceDeviceId) const;
    PowerTopologyLoopAnalysis loopAnalysis() const;
    PowerTopologyPath shortestSupplyPath(const QString& sourceDeviceId, const QString& targetDeviceId) const;
    PowerTopologyPath shortestDirectedSupplyPath(const QString& sourceDeviceId,
                                                 const QString& targetDeviceId) const;
    PowerTopologyGroundPath shortestGroundPath(const QString& sourceDeviceId) const;

private:
    PowerTopologyProjection(PowerTopologyProjectionSourceKind sourceKind,
                            const PowerTopologyAnalysisSnapshot& snapshot);

    PowerTopologyProjectionSourceKind sourceKind_ = PowerTopologyProjectionSourceKind::ImportedModel;
    PowerTopologyAnalysisSnapshot snapshot_;
};

#endif // POWERTOPOLOGYPROJECTION_H
