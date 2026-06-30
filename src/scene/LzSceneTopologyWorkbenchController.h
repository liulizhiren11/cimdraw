#ifndef LZSCENETOPOLOGYWORKBENCHCONTROLLER_H
#define LZSCENETOPOLOGYWORKBENCHCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class QColor;
class QString;
class LzScene;
class TopologyGraphIndex;

class LzSceneTopologyStateController;

class LzSceneTopologyWorkbenchController
{
public:
    void rebuildTopologyIndex(LzScene* scene, LzSceneTopologyStateController& stateController) const;
    const TopologyGraphIndex* topologyIndex(const LzSceneTopologyStateController& stateController) const;
    TopologyGraphIndex* topologyIndex(LzSceneTopologyStateController& stateController) const;
    void highlightTopologyNode(LzSceneTopologyStateController& stateController,
                               const QString& nodeStableId,
                               bool on,
                               const QColor& c) const;
    void highlightTopologyEdge(LzSceneTopologyStateController& stateController,
                               const QString& edgeStableId,
                               bool on,
                               const QColor& c) const;
    void highlightTopologyEdges(LzSceneTopologyStateController& stateController,
                                const QStringList& edgeStableIds,
                                bool on,
                                const QColor& c) const;
    void clearTopologyHighlights(LzSceneTopologyStateController& stateController) const;
    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(LzScene* scene,
                                                             LzSceneTopologyStateController& stateController) const;
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(LzScene* scene,
                                                                 LzSceneTopologyStateController& stateController) const;

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

    QVariantMap buildPowerTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap buildTopologyBindingSnapshot(LzScene* scene) const;
    QVariantMap queryTopology(LzScene* scene, const QVariantMap& request) const;

    bool setWiringData(QHash<QString, QVariant>& wiringData,
                       const QString& key,
                       const QVariant& value) const;
    QVariant wiringData(const QHash<QString, QVariant>& wiringData, const QString& key) const;
};

#endif
