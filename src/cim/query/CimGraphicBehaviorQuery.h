#ifndef CIMGRAPHICBEHAVIORQUERY_H
#define CIMGRAPHICBEHAVIORQUERY_H

#include "cim/behavior/CimBehaviorResult.h"
#include "cim/query/CimGraphicQueryState.h"

class QString;
class CimdrawScene;
class QGraphicsItem;

class CimGraphicBehaviorQuery
{
public:
    CimBehaviorResult resultForItem(QGraphicsItem* item) const;
    CimBehaviorResult resultForMrid(const QString& mrid,
                                    CimdrawScene* scene,
                                    const CimGraphicQueryState& queryState = {}) const;
    CimBehaviorResult selectedResult(CimdrawScene* scene,
                                     const CimGraphicQueryState& queryState = {}) const;
    CimBehaviorResult resultForWiringKey(CimdrawScene* scene,
                                         const QString& wiringDataKey) const;
};

#endif
