#include "cim/query/CimQueryFacade.h"

#include "cim/query/CimGeneratedTopologyGraphQuery.h"
#include "cim/query/CimGraphicBehaviorQuery.h"
#include "cim/query/CimGraphicObjectSummaryQuery.h"
#include "cim/query/CimPowerTopologyPreviewQuery.h"
#include "cim/query/CimGraphicRenderStateQuery.h"
#include "cim/query/CimPowerTopologyRuntimeQuery.h"
#include "cim/query/CimTopologyCompatQuery.h"
#include "cim/query/CimTopologySummaryQuery.h"
#include "cim/query/CimGraphicVisualSummaryQuery.h"

CimGraphicVisualSummary CimQueryFacade::buildGraphicVisualSummary(const CimModel& model,
                                                                  const QSet<QString>& visualizedMrids,
                                                                  int generatedHelperLineCount) const
{
    CimGraphicVisualSummaryQuery query;
    return query.buildSummary(model, visualizedMrids, generatedHelperLineCount);
}

CimGraphicVisualSummary CimQueryFacade::buildGraphicVisualSummary(const CimModel& model,
                                                                 const CimGraphicQueryState& queryState) const
{
    CimGraphicVisualSummaryQuery query;
    return query.buildSummary(model, queryState);
}

QSet<QString> CimQueryFacade::visualizedMrids(const CimGraphicQueryState& queryState) const
{
    CimGraphicVisualSummaryQuery query;
    return query.visualizedMrids(queryState);
}

QSet<QString> CimQueryFacade::visualizedMrids(const CimGraphicQueryContext& queryContext) const
{
    CimGraphicVisualSummaryQuery query;
    return query.visualizedMrids(queryContext);
}

QGraphicsItem* CimQueryFacade::sceneItemByMrid(const QString& mrid,
                                               CimdrawScene* scene,
                                               const CimGraphicQueryContext& queryContext) const
{
    CimGraphicObjectSummaryQuery query;
    return query.sceneItemByMrid(mrid, scene, queryContext);
}

QString CimQueryFacade::mridForSceneItem(QGraphicsItem* item,
                                         const CimGraphicQueryContext& queryContext) const
{
    CimGraphicObjectSummaryQuery query;
    return query.mridForSceneItem(item, queryContext);
}

CimGraphicObjectSummary CimQueryFacade::graphicObjectSummaryForSceneItem(
    QGraphicsItem* item,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery query;
    return query.objectSummaryForSceneItem(item, queryState);
}

CimGraphicObjectSummary CimQueryFacade::graphicObjectSummaryForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery query;
    return query.objectSummaryForMrid(mrid, scene, queryState);
}

CimGraphicObjectSummary CimQueryFacade::selectedGraphicObjectSummary(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicObjectSummaryQuery query;
    return query.selectedObjectSummary(scene, queryState);
}

CimGraphicRenderStateSource CimQueryFacade::graphicRenderStateSourceForSceneItem(QGraphicsItem* item) const
{
    CimGraphicRenderStateQuery query;
    return query.sourceForItem(item);
}

CimGraphicRenderStateSource CimQueryFacade::graphicRenderStateSourceForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicRenderStateQuery query;
    return query.sourceForMrid(mrid, scene, queryState);
}

CimGraphicRenderStateSource CimQueryFacade::selectedGraphicRenderStateSource(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicRenderStateQuery query;
    return query.selectedSource(scene, queryState);
}

CimGraphicRenderState CimQueryFacade::graphicRenderStateForSceneItem(QGraphicsItem* item) const
{
    CimGraphicRenderStateQuery query;
    return query.resultForItem(item);
}

CimGraphicRenderState CimQueryFacade::graphicRenderStateForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicRenderStateQuery query;
    return query.resultForMrid(mrid, scene, queryState);
}

CimGraphicRenderState CimQueryFacade::selectedGraphicRenderState(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimGraphicRenderStateQuery query;
    return query.selectedResult(scene, queryState);
}

CimGeneratedTopologySourceSummary CimQueryFacade::generatedTopologySourceSummaryForSceneItem(
    QGraphicsItem* item,
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot) const
{
    CimGeneratedTopologyGraphQuery query;
    return query.generatedTopologySourceSummaryForSceneItem(item, runtimeSnapshot);
}

QVector<QPair<const QGraphicsItem*, CimGeneratedTopologySourceSummary>>
CimQueryFacade::generatedTopologySourceSummaries(
    CimdrawScene* scene,
    const PowerTopologyAnalysisSnapshot* runtimeSnapshot,
    bool ensureNodeIds) const
{
    CimGeneratedTopologyGraphQuery query;
    return query.generatedTopologySourceSummaries(scene, runtimeSnapshot, ensureNodeIds);
}

CimGeneratedTopologyGraphSummary CimQueryFacade::generatedTopologyGraphSummary(
    CimdrawScene* scene,
    CimdrawTopologyDomain domainFilter) const
{
    CimGeneratedTopologyGraphQuery query;
    return query.generatedTopologyGraphSummary(scene, domainFilter);
}

QVector<CimGeneratedTopologyRelationSummary> CimQueryFacade::generatedTopologyRelationSummaries(
    CimdrawScene* scene,
    const QHash<QString, CimGeneratedTopologySourceSummary>& sourceSummariesByNodeId) const
{
    CimGeneratedTopologyGraphQuery query;
    return query.generatedTopologyRelationSummaries(scene, sourceSummariesByNodeId);
}

CimTopologyObjectSummary CimQueryFacade::topologyObjectSummaryForSceneItem(QGraphicsItem* item) const
{
    CimTopologySummaryQuery query;
    return query.topologyObjectSummaryForSceneItem(item);
}

CimTopologyHighlightSummary CimQueryFacade::topologyHighlightSummaryForSceneItem(QGraphicsItem* item) const
{
    CimTopologySummaryQuery query;
    return query.topologyHighlightSummaryForSceneItem(item);
}

QStringList CimQueryFacade::topologySummaryLinesForSceneItem(QGraphicsItem* item) const
{
    CimTopologySummaryQuery query;
    return query.topologySummaryLinesForSceneItem(item);
}

CimTopologyObjectSummary CimQueryFacade::topologyObjectSummaryForMrid(
    const QString& mrid,
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimTopologySummaryQuery query;
    return query.topologyObjectSummaryForMrid(mrid, scene, queryState);
}

CimTopologyObjectSummary CimQueryFacade::selectedTopologyObjectSummary(
    CimdrawScene* scene,
    const CimGraphicQueryState& queryState) const
{
    CimTopologySummaryQuery query;
    return query.selectedTopologyObjectSummary(scene, queryState);
}

CimBehaviorResult CimQueryFacade::behaviorResultForSceneItem(QGraphicsItem* item) const
{
    CimGraphicBehaviorQuery query;
    return query.resultForItem(item);
}

CimBehaviorResult CimQueryFacade::behaviorResultForMrid(const QString& mrid,
                                                        CimdrawScene* scene,
                                                        const CimGraphicQueryState& queryState) const
{
    CimGraphicBehaviorQuery query;
    return query.resultForMrid(mrid, scene, queryState);
}

CimBehaviorResult CimQueryFacade::selectedBehaviorResult(CimdrawScene* scene,
                                                         const CimGraphicQueryState& queryState) const
{
    CimGraphicBehaviorQuery query;
    return query.selectedResult(scene, queryState);
}

CimBehaviorResult CimQueryFacade::behaviorResultForWiringKey(CimdrawScene* scene,
                                                             const QString& wiringDataKey) const
{
    CimGraphicBehaviorQuery query;
    return query.resultForWiringKey(scene, wiringDataKey);
}

QVariantMap CimQueryFacade::topologyQuery(CimdrawScene* scene, const QVariantMap& request) const
{
    CimTopologyCompatQuery query;
    return query.topologyQuery(scene, request);
}

QVariantMap CimQueryFacade::topologyQuery(CimdrawScene* scene,
                                          const QString& domain,
                                          const QString& op,
                                          const QVariantMap& extra) const
{
    CimTopologyCompatQuery query;
    return query.topologyQuery(scene, domain, op, extra);
}

QVariant CimQueryFacade::topologyQueryData(CimdrawScene* scene,
                                           const QString& domain,
                                           const QString& op,
                                           const QVariantMap& extra) const
{
    CimTopologyCompatQuery query;
    return query.topologyQueryData(scene, domain, op, extra);
}

QStringList CimQueryFacade::topologyQueryStringList(CimdrawScene* scene,
                                                    const QString& domain,
                                                    const QString& op,
                                                    const QVariantMap& extra) const
{
    CimTopologyCompatQuery query;
    return query.topologyQueryStringList(scene, domain, op, extra);
}

QVector<int> CimQueryFacade::topologyQueryIntVector(CimdrawScene* scene,
                                                    const QString& domain,
                                                    const QString& op,
                                                    const QVariantMap& extra) const
{
    CimTopologyCompatQuery query;
    return query.topologyQueryIntVector(scene, domain, op, extra);
}

QVariantMap CimQueryFacade::topologyQueryMap(CimdrawScene* scene,
                                             const QString& domain,
                                             const QString& op,
                                             const QVariantMap& extra) const
{
    CimTopologyCompatQuery query;
    return query.topologyQueryMap(scene, domain, op, extra);
}

QVariantMap CimQueryFacade::topologyBindingSnapshot(CimdrawScene* scene) const
{
    CimTopologyCompatQuery query;
    return query.topologyBindingSnapshot(scene);
}

QVariantMap CimQueryFacade::powerTopologyBindingSnapshot(CimdrawScene* scene) const
{
    CimTopologyCompatQuery query;
    return query.powerTopologyBindingSnapshot(scene);
}

PowerTopologyProjection CimQueryFacade::runtimePowerTopologyProjection(CimdrawScene* scene) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.runtimePowerTopologyProjection(scene);
}

PowerTopologyAnalysisSnapshot CimQueryFacade::runtimePowerTopologySnapshot(CimdrawScene* scene) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.runtimePowerTopologySnapshot(scene);
}

PowerTopologyDocumentExport CimQueryFacade::runtimePowerTopologyDocumentExport(CimdrawScene* scene) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.runtimePowerTopologyDocumentExport(scene);
}

QVector<int> CimQueryFacade::powerDeviceNodeIds(CimdrawScene* scene, const QString& deviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.powerDeviceNodeIds(scene, deviceId);
}

QStringList CimQueryFacade::powerDeviceConductorIds(CimdrawScene* scene, const QString& deviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.powerDeviceConductorIds(scene, deviceId);
}

QStringList CimQueryFacade::connectedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.connectedPowerDevices(scene, deviceId);
}

QStringList CimQueryFacade::busbarAttachedPowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.busbarAttachedPowerDevices(scene, deviceId);
}

QStringList CimQueryFacade::reachablePowerDevices(CimdrawScene* scene, const QString& deviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.reachablePowerDevices(scene, deviceId);
}

QStringList CimQueryFacade::directedReachablePowerDevices(CimdrawScene* scene,
                                                          const QString& sourceDeviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.directedReachablePowerDevices(scene, sourceDeviceId);
}

PowerTopologyIslandAnalysis CimQueryFacade::analyzePowerIslands(CimdrawScene* scene) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.analyzePowerIslands(scene);
}

PowerTopologyBranchAnalysis CimQueryFacade::analyzePowerBranches(CimdrawScene* scene,
                                                                 const QString& sourceDeviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.analyzePowerBranches(scene, sourceDeviceId);
}

PowerTopologyLoopAnalysis CimQueryFacade::analyzePowerLoops(CimdrawScene* scene) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.analyzePowerLoops(scene);
}

PowerTopologyPath CimQueryFacade::shortestPowerSupplyPath(CimdrawScene* scene,
                                                          const QString& sourceDeviceId,
                                                          const QString& targetDeviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.shortestPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
}

PowerTopologyPath CimQueryFacade::shortestDirectedPowerSupplyPath(CimdrawScene* scene,
                                                                  const QString& sourceDeviceId,
                                                                  const QString& targetDeviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.shortestDirectedPowerSupplyPath(scene, sourceDeviceId, targetDeviceId);
}

PowerTopologyGroundPath CimQueryFacade::shortestPowerGroundPath(CimdrawScene* scene,
                                                                const QString& sourceDeviceId) const
{
    CimPowerTopologyRuntimeQuery query;
    return query.shortestPowerGroundPath(scene, sourceDeviceId);
}

PowerTopologySwitchChangePreview CimQueryFacade::previewPowerSwitchTopologyChange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition) const
{
    CimPowerTopologyPreviewQuery query;
    return query.previewPowerSwitchTopologyChange(scene, deviceId, toSwitchPosition);
}

PowerTopologyProtectionRangePreview CimQueryFacade::previewPowerProtectionRange(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& sourceDeviceId) const
{
    CimPowerTopologyPreviewQuery query;
    return query.previewPowerProtectionRange(scene, deviceId, toSwitchPosition, sourceDeviceId);
}

PowerTopologyOperationPreview CimQueryFacade::previewPowerSwitchOperation(
    CimdrawScene* scene,
    const QString& deviceId,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId) const
{
    CimPowerTopologyPreviewQuery query;
    return query.previewPowerSwitchOperation(scene,
                                             deviceId,
                                             toSwitchPosition,
                                             supplySourceDeviceId,
                                             supplyTargetDeviceId,
                                             groundSourceDeviceId);
}

TopologyNodeRelationInfo CimQueryFacade::relationNodeInfoValue(CimdrawScene* scene,
                                                               const QString& nodeStableId) const
{
    CimTopologyCompatQuery query;
    return query.relationNodeInfoValue(scene, nodeStableId);
}

CimRelationEdgeSummary CimQueryFacade::relationEdgeSummaryForSceneItem(QGraphicsItem* item) const
{
    CimTopologySummaryQuery query;
    return query.relationEdgeSummaryForSceneItem(item);
}

CimRelationEdgeSummary CimQueryFacade::selectedRelationEdgeSummary(CimdrawScene* scene) const
{
    CimTopologySummaryQuery query;
    return query.selectedRelationEdgeSummary(scene);
}

TopologyEdgeMeta CimQueryFacade::relationEdgeMetaForSceneItem(QGraphicsItem* item) const
{
    return relationEdgeSummaryForSceneItem(item).edgeMeta;
}

TopologyEdgeMeta CimQueryFacade::selectedRelationEdgeMeta(CimdrawScene* scene) const
{
    return selectedRelationEdgeSummary(scene).edgeMeta;
}

TopologyEdgeMeta CimQueryFacade::relationEdgeMeta(CimdrawScene* scene,
                                                  const QString& edgeStableId) const
{
    CimTopologyCompatQuery query;
    return query.relationEdgeMeta(scene, edgeStableId);
}

QVector<TopologyEdgeMeta> CimQueryFacade::relationEdgeMetas(CimdrawScene* scene) const
{
    CimTopologyCompatQuery query;
    return query.relationEdgeMetas(scene);
}

TopologyEdgeMeta CimQueryFacade::resolveRelationEdgeMeta(CimdrawScene* scene,
                                                         const TopologyEdgeLookupHint& lookupHint) const
{
    CimTopologyCompatQuery query;
    return query.resolveRelationEdgeMeta(scene, lookupHint);
}

QVariantMap CimQueryFacade::relationNodeInfo(CimdrawScene* scene,
                                             const QString& nodeStableId) const
{
    CimTopologyCompatQuery query;
    return query.relationNodeInfo(scene, nodeStableId);
}

QVariantMap CimQueryFacade::relationBindingSnapshot(CimdrawScene* scene) const
{
    CimTopologyCompatQuery query;
    return query.relationBindingSnapshot(scene);
}

QVariantMap CimQueryFacade::relationEdgeInfo(CimdrawScene* scene,
                                             const TopologyEdgeLookupHint& lookupHint) const
{
    CimTopologyCompatQuery query;
    return query.relationEdgeInfo(scene, lookupHint);
}
