#ifndef CIMGRAPHICOBJECTSUMMARY_H
#define CIMGRAPHICOBJECTSUMMARY_H

#include "cim/behavior/CimBehaviorResult.h"
#include "cim/query/CimGraphicRenderState.h"
#include "topology/PowerTopologyTypes.h"
#include "topology/TopologyTypes.h"

#include <QString>

enum class CimGraphicObjectVisualState
{
    Visualized = 0,
    MappableNotInstantiated,
    SemanticOnly,
    LayoutOnly,
    MetadataOnly,
    AbstractOnly,
    UnmappedDevice,
    Uncategorized,
};

struct CimGraphicObjectSummary
{
    QString mrid;
    QString objectId;
    QString displayName;
    QString className;
    QString graphicClassName;
    CimGraphicObjectVisualState visualState = CimGraphicObjectVisualState::Uncategorized;
    bool mappable = false;
    bool visualized = false;
    bool participatesInTopology = false;
    bool isTopologyGraphNode = false;
    bool supportsDirectedFlow = false;
    CimdrawTopologyDomain topologyDomain = CimdrawTopologyDomain::None;
    PowerTopologyRole powerTopologyRole = PowerTopologyRole::GenericEquipment;
    CimGraphicRenderState renderState;
    CimBehaviorResult behaviorResult;
};

inline int cimGraphicObjectSummaryFlowSign(const CimGraphicObjectSummary& summary,
                                           int fallbackFlowSign = 1)
{
    return summary.renderState.valid ? summary.renderState.flowSign : fallbackFlowSign;
}

#endif
