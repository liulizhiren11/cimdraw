#ifndef CIMGRAPHICOBJECTSUMMARYQUERY_H
#define CIMGRAPHICOBJECTSUMMARYQUERY_H

#include "cim/query/CimGraphicObjectSummary.h"
#include "cim/query/CimGraphicQueryContext.h"
#include "cim/query/CimGraphicQueryState.h"

class CimIdIndex;
class CimdrawScene;
class QGraphicsItem;

class CimGraphicObjectSummaryQuery
{
public:
    QGraphicsItem* sceneItemByMrid(const QString& mrid,
                                   CimdrawScene* scene,
                                   const CimGraphicQueryContext& queryContext) const;
    QGraphicsItem* sceneItemByMrid(const QString& mrid,
                                   CimdrawScene* scene,
                                   const QHash<QString, QGraphicsItem*>& shapeByMrid,
                                   const CimIdIndex* idIndex = nullptr) const;
    QString mridForSceneItem(QGraphicsItem* item,
                             const CimGraphicQueryContext& queryContext) const;
    QString mridForSceneItem(QGraphicsItem* item,
                             const CimIdIndex* idIndex = nullptr) const;
    CimGraphicObjectSummary objectSummaryForSceneItem(
        QGraphicsItem* item,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicObjectSummary objectSummaryForMrid(
        const QString& mrid,
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicObjectSummary selectedObjectSummary(
        CimdrawScene* scene,
        const CimGraphicQueryState& queryState = {}) const;
};

#endif
