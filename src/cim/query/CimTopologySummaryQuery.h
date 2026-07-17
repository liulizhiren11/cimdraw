#ifndef CIMTOPOLOGYSUMMARYQUERY_H
#define CIMTOPOLOGYSUMMARYQUERY_H

#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimRelationEdgeSummary.h"
#include "cim/query/CimTopologyHighlightSummary.h"
#include "cim/query/CimTopologyObjectSummary.h"

class QString;
class CimdrawScene;
class QGraphicsItem;

class CimTopologySummaryQuery
{
public:
    CimRelationEdgeSummary relationEdgeSummaryForSceneItem(QGraphicsItem* item) const;
    CimRelationEdgeSummary selectedRelationEdgeSummary(CimdrawScene* scene) const;
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
};

#endif
