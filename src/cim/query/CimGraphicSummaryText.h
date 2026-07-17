#ifndef CIMGRAPHICSUMMARYTEXT_H
#define CIMGRAPHICSUMMARYTEXT_H

#include "cim/query/CimRelationEdgeSummary.h"
#include "cim/query/CimTopologyObjectSummary.h"
#include "cim/query/CimGeneratedTopologySourceSummary.h"
#include "cim/query/CimGraphicObjectSummary.h"

#include <QStringList>

class QString;

QString cimTopologyRelationTypeLabel(TopologyRelationType type);
QString cimGeneratedTopologyNodeLabel(const CimGeneratedTopologySourceSummary& summary);
QString cimGraphicTopologyDomainLabel(CimdrawTopologyDomain domain);
QString cimBehaviorTerminalConnectivityLabel(CimBehaviorTerminalConnectivity connectivity);
QString cimGraphicObjectVisualStateLabel(CimGraphicObjectVisualState state);
QString cimGraphicPowerRoleLabel(PowerTopologyRole role);
QString cimBehaviorStateLabel(const CimBehaviorResult* behaviorResult);
QString cimGraphicRenderRunStateLabel(CimdrawWiringRunState runState);
QString cimGraphicRenderSwitchPositionLabel(const CimGraphicObjectSummary& objectSummary);
QString cimGraphicRenderFlowDirectionLabel(const CimGraphicObjectSummary& objectSummary);
QString cimGraphicRenderStateLabel(const CimGraphicObjectSummary& objectSummary);
QString cimGraphicObjectStatusLabel(const CimGraphicObjectSummary& objectSummary);
QStringList cimRelationEdgeSummaryLines(const CimRelationEdgeSummary& relationSummary);
QStringList cimTopologyObjectSummaryLines(const CimGraphicObjectSummary& objectSummary,
                                          const CimTopologyObjectSummary& topologySummary);

#endif
