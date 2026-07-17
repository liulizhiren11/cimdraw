#ifndef CIMGRAPHICVISUALSUMMARYQUERY_H
#define CIMGRAPHICVISUALSUMMARYQUERY_H

#include "cim/query/CimGraphicQueryContext.h"
#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimGraphicVisualSummary.h"

class CimIdIndex;
class CimModel;
class QGraphicsItem;

class CimGraphicVisualSummaryQuery
{
public:
    CimGraphicVisualSummary buildSummary(const CimModel& model,
                                         const QSet<QString>& visualizedMrids,
                                         int generatedHelperLineCount = 0) const;
    CimGraphicVisualSummary buildSummary(const CimModel& model,
                                         const CimGraphicQueryState& queryState) const;
    QSet<QString> visualizedMrids(const CimGraphicQueryState& queryState) const;
    QSet<QString> visualizedMrids(const CimGraphicQueryContext& queryContext) const;
    QSet<QString> visualizedMrids(const QHash<QString, QGraphicsItem*>& shapeByMrid,
                                  const CimIdIndex* idIndex = nullptr) const;
};

#endif
