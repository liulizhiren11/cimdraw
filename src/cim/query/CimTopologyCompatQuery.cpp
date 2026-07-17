#include "cim/query/CimTopologyCompatQuery.h"

#include "CimdrawScene.h"
#include "scene/CimdrawSceneTopologyQueryController.h"
#include "scene/CimdrawSceneTopologyStateController.h"
#include "scene/CimdrawSceneTopologyWorkbenchController.h"

namespace {

bool buildRelationWorkbenchContext(CimdrawScene* scene,
                                   CimdrawSceneTopologyWorkbenchController* workbenchController,
                                   CimdrawSceneTopologyStateController* stateController)
{
    if (!scene || !workbenchController || !stateController)
        return false;
    workbenchController->rebuildTopologyIndex(scene, *stateController);
    return true;
}

} // namespace

QVariantMap CimTopologyCompatQuery::topologyQuery(CimdrawScene* scene, const QVariantMap& request) const
{
    if (!scene)
        return {};

    CimdrawSceneTopologyQueryController controller;
    return controller.queryTopology(scene, request);
}

QVariantMap CimTopologyCompatQuery::topologyQuery(CimdrawScene* scene,
                                                  const QString& domain,
                                                  const QString& op,
                                                  const QVariantMap& extra) const
{
    QVariantMap request = extra;
    request.insert(QStringLiteral("domain"), domain);
    request.insert(QStringLiteral("op"), op);
    return topologyQuery(scene, request);
}

QVariant CimTopologyCompatQuery::topologyQueryData(CimdrawScene* scene,
                                                   const QString& domain,
                                                   const QString& op,
                                                   const QVariantMap& extra) const
{
    const QVariantMap result = topologyQuery(scene, domain, op, extra);
    if (!result.value(QStringLiteral("ok")).toBool())
        return {};
    return result.value(QStringLiteral("data"));
}

QStringList CimTopologyCompatQuery::topologyQueryStringList(CimdrawScene* scene,
                                                            const QString& domain,
                                                            const QString& op,
                                                            const QVariantMap& extra) const
{
    return topologyQueryData(scene, domain, op, extra).toStringList();
}

QVector<int> CimTopologyCompatQuery::topologyQueryIntVector(CimdrawScene* scene,
                                                            const QString& domain,
                                                            const QString& op,
                                                            const QVariantMap& extra) const
{
    const QVariant data = topologyQueryData(scene, domain, op, extra);
    const QVector<int> values = data.value<QVector<int>>();
    if (!values.isEmpty())
        return values;

    const QVariantList list = data.toList();
    QVector<int> result;
    result.reserve(list.size());
    for (const QVariant& entry : list)
        result.push_back(entry.toInt());
    return result;
}

QVariantMap CimTopologyCompatQuery::topologyQueryMap(CimdrawScene* scene,
                                                     const QString& domain,
                                                     const QString& op,
                                                     const QVariantMap& extra) const
{
    return topologyQueryData(scene, domain, op, extra).toMap();
}

QVariantMap CimTopologyCompatQuery::topologyBindingSnapshot(CimdrawScene* scene) const
{
    return topologyQueryMap(scene,
                            QStringLiteral("all"),
                            QStringLiteral("bindingSnapshot"));
}

QVariantMap CimTopologyCompatQuery::powerTopologyBindingSnapshot(CimdrawScene* scene) const
{
    return topologyQueryMap(scene,
                            QStringLiteral("power"),
                            QStringLiteral("bindingSnapshot"));
}

TopologyNodeRelationInfo CimTopologyCompatQuery::relationNodeInfoValue(CimdrawScene* scene,
                                                                       const QString& nodeStableId) const
{
    if (!scene || nodeStableId.trimmed().isEmpty())
        return {};

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return {};
    return workbenchController.relationNodeInfo(scene, stateController, nodeStableId);
}

TopologyEdgeMeta CimTopologyCompatQuery::relationEdgeMeta(CimdrawScene* scene,
                                                          const QString& edgeStableId) const
{
    if (!scene || edgeStableId.trimmed().isEmpty())
        return {};

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return {};
    return workbenchController.relationEdgeMeta(scene, stateController, edgeStableId);
}

QVector<TopologyEdgeMeta> CimTopologyCompatQuery::relationEdgeMetas(CimdrawScene* scene) const
{
    if (!scene)
        return {};

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return {};
    return workbenchController.relationEdgeMetas(scene, stateController);
}

TopologyEdgeMeta CimTopologyCompatQuery::resolveRelationEdgeMeta(
    CimdrawScene* scene,
    const TopologyEdgeLookupHint& lookupHint) const
{
    if (!scene || !lookupHint.hasLookupFields())
        return {};

    CimdrawSceneTopologyWorkbenchController workbenchController;
    CimdrawSceneTopologyStateController stateController;
    if (!buildRelationWorkbenchContext(scene, &workbenchController, &stateController))
        return {};
    return workbenchController.resolveRelationEdgeMeta(scene, stateController, lookupHint);
}

QVariantMap CimTopologyCompatQuery::relationNodeInfo(CimdrawScene* scene,
                                                     const QString& nodeStableId) const
{
    if (!scene || nodeStableId.trimmed().isEmpty())
        return {};

    return topologyQueryMap(scene,
                            QStringLiteral("relation"),
                            QStringLiteral("nodeInfo"),
                            {{QStringLiteral("nodeId"), nodeStableId}});
}

QVariantMap CimTopologyCompatQuery::relationBindingSnapshot(CimdrawScene* scene) const
{
    return topologyQueryMap(scene,
                            QStringLiteral("relation"),
                            QStringLiteral("bindingSnapshot"));
}

QVariantMap CimTopologyCompatQuery::relationEdgeInfo(CimdrawScene* scene,
                                                     const TopologyEdgeLookupHint& lookupHint) const
{
    if (!scene || !lookupHint.hasLookupFields())
        return {};

    QVariantMap request;
    if (!lookupHint.edgeStableId.isEmpty())
        request.insert(QStringLiteral("edgeId"), lookupHint.edgeStableId);
    if (!lookupHint.startNodeStableId.isEmpty())
        request.insert(QStringLiteral("startNodeId"), lookupHint.startNodeStableId);
    if (!lookupHint.endNodeStableId.isEmpty())
        request.insert(QStringLiteral("endNodeId"), lookupHint.endNodeStableId);
    if (!lookupHint.startPortKey.isEmpty())
        request.insert(QStringLiteral("startPortKey"), lookupHint.startPortKey);
    if (!lookupHint.endPortKey.isEmpty())
        request.insert(QStringLiteral("endPortKey"), lookupHint.endPortKey);
    if (lookupHint.relationTypeSpecified)
        request.insert(QStringLiteral("relationType"), static_cast<int>(lookupHint.relationType));

    return topologyQueryMap(scene,
                            QStringLiteral("relation"),
                            QStringLiteral("resolveEdgeInfo"),
                            request);
}
