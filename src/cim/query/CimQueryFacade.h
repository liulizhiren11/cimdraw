#ifndef CIMQUERYFACADE_H
#define CIMQUERYFACADE_H

#include "cim/query/CimGeneratedTopologyGraphSummary.h"
#include "cim/behavior/CimBehaviorResult.h"
#include "cim/query/CimGeneratedTopologyRelationSummary.h"
#include "cim/query/CimGeneratedTopologySourceSummary.h"
#include "cim/query/CimGraphicObjectSummary.h"
#include "cim/query/CimGraphicRenderState.h"
#include "cim/query/CimGraphicQueryContext.h"
#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimRelationEdgeSummary.h"
#include "cim/query/CimTopologyHighlightSummary.h"
#include "cim/query/CimTopologyObjectSummary.h"
#include "cim/query/CimGraphicVisualSummary.h"
#include "topology/PowerTopologyProjection.h"
#include "topology/PowerTopologyTypes.h"

#include <QGraphicsItem>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVector>

class QString;
class CimdrawScene;
class CimModel;
struct TopologyEdgeLookupHint;

class CimQueryFacade
{
public:
    CimGraphicVisualSummary buildGraphicVisualSummary(const CimModel& model,
                                                     const QSet<QString>& visualizedMrids,
                                                     int generatedHelperLineCount = 0) const;
    CimGraphicVisualSummary buildGraphicVisualSummary(const CimModel& model,
                                                     const CimGraphicQueryState& queryState) const;
    QSet<QString> visualizedMrids(const CimGraphicQueryState& queryState) const;
    QSet<QString> visualizedMrids(const CimGraphicQueryContext& queryContext) const;
    QGraphicsItem* sceneItemByMrid(const QString& mrid,
                                   CimdrawScene* scene,
                                   const CimGraphicQueryContext& queryContext) const;
    QString mridForSceneItem(QGraphicsItem* item,
                             const CimGraphicQueryContext& queryContext) const;
    CimGraphicObjectSummary graphicObjectSummaryForSceneItem(
        QGraphicsItem* item,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicObjectSummary graphicObjectSummaryForMrid(
        const QString& mrid,
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicObjectSummary selectedGraphicObjectSummary(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderStateSource graphicRenderStateSourceForSceneItem(QGraphicsItem* item) const;
    CimGraphicRenderStateSource graphicRenderStateSourceForMrid(
        const QString& mrid,
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderStateSource selectedGraphicRenderStateSource(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderState graphicRenderStateForSceneItem(QGraphicsItem* item) const;
    CimGraphicRenderState graphicRenderStateForMrid(
        const QString& mrid,
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderState selectedGraphicRenderState(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGeneratedTopologySourceSummary generatedTopologySourceSummaryForSceneItem(
        QGraphicsItem* item,
        const PowerTopologyAnalysisSnapshot* runtimeSnapshot = nullptr) const;
    QVector<QPair<const QGraphicsItem*, CimGeneratedTopologySourceSummary>>
        generatedTopologySourceSummaries(
            CimdrawScene* scene,
            const PowerTopologyAnalysisSnapshot* runtimeSnapshot = nullptr,
            bool ensureNodeIds = false) const;
    CimGeneratedTopologyGraphSummary generatedTopologyGraphSummary(
        CimdrawScene* scene,
        CimdrawTopologyDomain domainFilter = CimdrawTopologyDomain::None) const;
    QVector<CimGeneratedTopologyRelationSummary> generatedTopologyRelationSummaries(
        CimdrawScene* scene,
        const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId) const;
    CimTopologyObjectSummary topologyObjectSummaryForSceneItem(QGraphicsItem* item) const;
    CimTopologyObjectSummary topologyObjectSummaryForMrid(
        const QString& mrid,
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimTopologyObjectSummary selectedTopologyObjectSummary(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimTopologyHighlightSummary topologyHighlightSummaryForSceneItem(QGraphicsItem* item) const;
    QStringList topologySummaryLinesForSceneItem(QGraphicsItem* item) const;
    CimBehaviorResult behaviorResultForSceneItem(QGraphicsItem* item) const;
    CimBehaviorResult behaviorResultForMrid(const QString& mrid,
                                            CimdrawScene* scene,
                                            const CimGraphicQueryState& queryState) const;
    CimBehaviorResult selectedBehaviorResult(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimBehaviorResult behaviorResultForWiringKey(CimdrawScene* scene,
                                                 const QString& wiringDataKey) const;

    QVariantMap topologyQuery(CimdrawScene* scene, const QVariantMap& request) const;
    QVariantMap topologyQuery(CimdrawScene* scene,
                              const QString& domain,
                              const QString& op,
                              const QVariantMap& extra = {}) const;
    QVariant topologyQueryData(CimdrawScene* scene,
                               const QString& domain,
                               const QString& op,
                               const QVariantMap& extra = {}) const;
    QStringList topologyQueryStringList(CimdrawScene* scene,
                                        const QString& domain,
                                        const QString& op,
                                        const QVariantMap& extra = {}) const;
    QVector<int> topologyQueryIntVector(CimdrawScene* scene,
                                        const QString& domain,
                                        const QString& op,
                                        const QVariantMap& extra = {}) const;
    QVariantMap topologyQueryMap(CimdrawScene* scene,
                                 const QString& domain,
                                 const QString& op,
                                 const QVariantMap& extra = {}) const;
    QVariantMap topologyBindingSnapshot(CimdrawScene* scene) const;
    QVariantMap powerTopologyBindingSnapshot(CimdrawScene* scene) const;
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
    TopologyNodeRelationInfo relationNodeInfoValue(CimdrawScene* scene,
                                                   const QString& nodeStableId) const;
    CimRelationEdgeSummary relationEdgeSummaryForSceneItem(QGraphicsItem* item) const;
    CimRelationEdgeSummary selectedRelationEdgeSummary(CimdrawScene* scene) const;
    TopologyEdgeMeta relationEdgeMetaForSceneItem(QGraphicsItem* item) const;
    TopologyEdgeMeta selectedRelationEdgeMeta(CimdrawScene* scene) const;
    TopologyEdgeMeta relationEdgeMeta(CimdrawScene* scene,
                                      const QString& edgeStableId) const;
    QVector<TopologyEdgeMeta> relationEdgeMetas(CimdrawScene* scene) const;
    TopologyEdgeMeta resolveRelationEdgeMeta(CimdrawScene* scene,
                                             const TopologyEdgeLookupHint& lookupHint) const;
    QVariantMap relationNodeInfo(CimdrawScene* scene,
                                 const QString& nodeStableId) const;
    QVariantMap relationBindingSnapshot(CimdrawScene* scene) const;
    QVariantMap relationEdgeInfo(CimdrawScene* scene,
                                 const TopologyEdgeLookupHint& lookupHint) const;
};

#endif
