#ifndef POWERTOPOLOGYSERVICE_H
#define POWERTOPOLOGYSERVICE_H

#include "PowerTopologyTypes.h"

class PowerTopologyService
{
public:
    static QVector<int> deviceNodeIds(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId);
    static QStringList deviceConductorIds(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId);
    static QStringList reachableDevices(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId);
    static QStringList directedReachableDevices(const PowerTopologyAnalysisSnapshot& snapshot, const QString& sourceDeviceId);
    static QStringList connectedComponentDeviceIds(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId);
    static QStringList busbarAttachedDevices(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId);
    static PowerTopologyIslandAnalysis islandAnalysis(const PowerTopologyAnalysisSnapshot& snapshot);
    static PowerTopologyBranchAnalysis branchAnalysis(const PowerTopologyAnalysisSnapshot& snapshot,
                                                      const QString& sourceDeviceId);
    static PowerTopologyLoopAnalysis loopAnalysis(const PowerTopologyAnalysisSnapshot& snapshot);
    static PowerTopologyPath shortestSupplyPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                const QString& sourceDeviceId,
                                                const QString& targetDeviceId);
    static PowerTopologyPath shortestDirectedSupplyPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                        const QString& sourceDeviceId,
                                                        const QString& targetDeviceId);
    static PowerTopologyGroundPath shortestGroundPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                      const QString& sourceDeviceId);
    static PowerTopologyOperationPreview previewSwitchOperation(const PowerTopologyAnalysisSnapshot& beforeSnapshot,
                                                                const PowerTopologyAnalysisSnapshot& afterSnapshot,
                                                                const QString& deviceId,
                                                                int fromSwitchPosition,
                                                                int toSwitchPosition,
                                                                const QString& supplySourceDeviceId,
                                                                const QString& supplyTargetDeviceId,
                                                                const QString& groundSourceDeviceId);
    static PowerTopologyProtectionRangePreview previewProtectionRange(
        const PowerTopologyAnalysisSnapshot& beforeSnapshot,
        const PowerTopologyAnalysisSnapshot& afterSnapshot,
        const QString& protectiveDeviceId,
        int fromSwitchPosition,
        int toSwitchPosition,
        const QString& sourceDeviceId);
    static PowerTopologySwitchChangePreview compareSwitchConnectivity(const PowerTopologyAnalysisSnapshot& beforeSnapshot,
                                                                      const PowerTopologyAnalysisSnapshot& afterSnapshot,
                                                                      const QString& deviceId,
                                                                      int fromSwitchPosition,
                                                                      int toSwitchPosition);
};

#endif // POWERTOPOLOGYSERVICE_H
