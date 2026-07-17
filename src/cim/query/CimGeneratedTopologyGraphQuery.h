#ifndef CIMGENERATEDTOPOLOGYGRAPHQUERY_H
#define CIMGENERATEDTOPOLOGYGRAPHQUERY_H

#include "cim/query/CimGeneratedTopologyGraphSummary.h"

#include <QPair>
#include <QVector>

class CimdrawScene;
class QGraphicsItem;
struct PowerTopologyAnalysisSnapshot;

class CimGeneratedTopologyGraphQuery
{
public:
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
};

#endif
