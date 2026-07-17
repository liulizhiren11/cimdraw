#ifndef CIMDRAWSCENETOPOLOGYWORKBENCHCONTROLLER_H
#define CIMDRAWSCENETOPOLOGYWORKBENCHCONTROLLER_H

#include "topology/PowerTopologyTypes.h"

#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class QColor;
class QString;
class CimdrawScene;

class CimdrawSceneTopologyStateController;

class CimdrawSceneTopologyWorkbenchController
{
public:
    void rebuildTopologyIndex(CimdrawScene* scene, CimdrawSceneTopologyStateController& stateController) const;
    TopologyNodeRelationInfo relationNodeInfo(CimdrawScene* scene,
                                              CimdrawSceneTopologyStateController& stateController,
                                              const QString& nodeStableId) const;
    TopologyEdgeMeta relationEdgeMeta(CimdrawScene* scene,
                                      CimdrawSceneTopologyStateController& stateController,
                                      const QString& edgeStableId) const;
    TopologyEdgeMeta resolveRelationEdgeMeta(CimdrawScene* scene,
                                             CimdrawSceneTopologyStateController& stateController,
                                             const TopologyEdgeLookupHint& lookupHint) const;
    QVector<TopologyEdgeMeta> relationEdgeMetas(CimdrawScene* scene,
                                                CimdrawSceneTopologyStateController& stateController) const;
    void highlightTopologyNode(CimdrawSceneTopologyStateController& stateController,
                               const QString& nodeStableId,
                               bool on,
                               const QColor& c) const;
    void highlightTopologyEdge(CimdrawSceneTopologyStateController& stateController,
                               const QString& edgeStableId,
                               bool on,
                               const QColor& c) const;
    void highlightTopologyEdges(CimdrawSceneTopologyStateController& stateController,
                                const QStringList& edgeStableIds,
                                bool on,
                                const QColor& c) const;
    void clearTopologyHighlights(CimdrawSceneTopologyStateController& stateController) const;
    PowerTopologyAnalysisSnapshot buildPowerTopologySnapshot(CimdrawScene* scene,
                                                             CimdrawSceneTopologyStateController& stateController) const;
    PowerTopologyDocumentExport buildPowerTopologyDocumentExport(CimdrawScene* scene,
                                                                 CimdrawSceneTopologyStateController& stateController) const;

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

    bool setWiringData(QHash<QString, QVariant>& wiringData,
                       const QString& key,
                       const QVariant& value) const;
    QVariant wiringData(const QHash<QString, QVariant>& wiringData, const QString& key) const;
};

#endif
