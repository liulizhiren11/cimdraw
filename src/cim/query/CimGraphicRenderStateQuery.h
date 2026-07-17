#ifndef CIMGRAPHICRENDERSTATEQUERY_H
#define CIMGRAPHICRENDERSTATEQUERY_H

#include "cim/query/CimGraphicQueryState.h"
#include "cim/query/CimGraphicRenderState.h"

class QString;
class CimdrawScene;
class QGraphicsItem;

class CimGraphicRenderStateQuery
{
public:
    CimGraphicRenderStateSource sourceForItem(QGraphicsItem* item) const;
    CimGraphicRenderStateSource sourceForMrid(const QString& mrid,
                                              CimdrawScene* scene,
                                              const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderStateSource selectedSource(CimdrawScene* scene,
                                               const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderState resultForSource(const CimGraphicRenderStateSource& source,
                                          bool alarmPulse = false) const;
    CimGraphicRenderState resultForItem(QGraphicsItem* item) const;
    CimGraphicRenderState resultForMrid(const QString& mrid,
                                        CimdrawScene* scene,
                                        const CimGraphicQueryState& queryState = {}) const;
    CimGraphicRenderState selectedResult(CimdrawScene* scene,
                                         const CimGraphicQueryState& queryState = {}) const;
};

#endif
