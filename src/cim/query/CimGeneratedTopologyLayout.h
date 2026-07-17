#ifndef CIMGENERATEDTOPOLOGYLAYOUT_H
#define CIMGENERATEDTOPOLOGYLAYOUT_H

#include "cim/query/CimGeneratedTopologyRelationSummary.h"
#include "cim/query/CimGeneratedTopologySourceSummary.h"

#include <QPointF>

QPointF cimGeneratedTopologyNodeLabelPosition(const QPointF& nodePos,
                                              const CimGeneratedTopologySourceSummary& summary);

QPointF cimGeneratedTopologyRelationLabelPosition(const QPointF& startScenePos,
                                                  const QPointF& endScenePos,
                                                  const CimGeneratedTopologyRelationSummary& summary);

#endif
