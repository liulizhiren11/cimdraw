#ifndef CIMTOPOLOGYCOMPATQUERY_H
#define CIMTOPOLOGYCOMPATQUERY_H

#include "topology/TopologyTypes.h"

#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QVector>

class QString;
class CimdrawScene;
struct TopologyEdgeLookupHint;

class CimTopologyCompatQuery
{
public:
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

    TopologyNodeRelationInfo relationNodeInfoValue(CimdrawScene* scene,
                                                   const QString& nodeStableId) const;
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
