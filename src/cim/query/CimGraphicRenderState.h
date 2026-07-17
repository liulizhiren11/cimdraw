#ifndef CIMGRAPHICRENDERSTATE_H
#define CIMGRAPHICRENDERSTATE_H

#include "cim/behavior/CimBehaviorResult.h"
#include "wiring/base/CimdrawWiringVisualState.h"

using CimGraphicRenderState = CimdrawWiringVisualState;

struct CimGraphicRenderStateSource
{
    bool valid = false;
    CimdrawWiringRunState runtimeRunState = CimdrawWiringRunState::Normal;
    int manualSwitchPosition = 1;
    int flowSign = 1;
    CimBehaviorResult behaviorResult;
    bool behaviorDrivesSwitchPosition = false;
};

inline CimdrawWiringRunState cimGraphicRunStateFromSource(const CimGraphicRenderStateSource& source)
{
    if (!source.behaviorResult.valid)
        return source.runtimeRunState;
    switch (source.behaviorResult.runState)
    {
    case CimBehaviorRunState::Normal:
        return CimdrawWiringRunState::Normal;
    case CimBehaviorRunState::Energized:
        return CimdrawWiringRunState::Energized;
    case CimBehaviorRunState::Deenergized:
        return CimdrawWiringRunState::Deenergized;
    case CimBehaviorRunState::Alarm:
        return CimdrawWiringRunState::Alarm;
    case CimBehaviorRunState::Unknown:
    default:
        break;
    }
    if (source.behaviorResult.energized)
        return CimdrawWiringRunState::Energized;
    if (source.behaviorResult.available || source.behaviorResult.conductivity
        || source.behaviorResult.terminalConnectivity != CimBehaviorTerminalConnectivity::Unknown)
        return CimdrawWiringRunState::Deenergized;
    return CimdrawWiringRunState::Normal;
}

inline CimBehaviorRunState cimBehaviorRunStateFromGraphicRunState(CimdrawWiringRunState runState)
{
    switch (runState)
    {
    case CimdrawWiringRunState::Energized:
        return CimBehaviorRunState::Energized;
    case CimdrawWiringRunState::Deenergized:
        return CimBehaviorRunState::Deenergized;
    case CimdrawWiringRunState::Alarm:
        return CimBehaviorRunState::Alarm;
    case CimdrawWiringRunState::Normal:
    default:
        return CimBehaviorRunState::Normal;
    }
}

inline int cimGraphicSwitchPositionFromSource(const CimGraphicRenderStateSource& source)
{
    if (source.behaviorResult.valid && source.behaviorDrivesSwitchPosition)
        return source.behaviorResult.conductivity ? 1 : 0;
    return source.manualSwitchPosition;
}

inline int cimGraphicFlowSignFromSource(const CimGraphicRenderStateSource& source)
{
    return source.flowSign < 0 ? -1 : 1;
}

inline bool cimGraphicFlowLeftToRightFromSign(int flowSign)
{
    return flowSign >= 0;
}

inline bool cimGraphicRenderStateFlowLeftToRight(const CimGraphicRenderState& state)
{
    return state.flowLeftToRight;
}

inline bool cimGraphicAlarmActive(CimdrawWiringRunState runState, bool alarmPulse)
{
    return runState == CimdrawWiringRunState::Alarm && alarmPulse;
}

inline bool cimGraphicRenderStateAlarmActive(const CimGraphicRenderState& state)
{
    return state.alarmActive;
}

inline bool cimGraphicUsesAlarmAccent(CimdrawWiringRunState runState)
{
    return runState == CimdrawWiringRunState::Alarm;
}

inline bool cimGraphicRenderStateUsesAlarmAccent(const CimGraphicRenderState& state)
{
    return cimGraphicUsesAlarmAccent(state.runState);
}

inline bool cimGraphicRenderStateEnergized(const CimGraphicRenderState& state)
{
    return state.runState == CimdrawWiringRunState::Energized;
}

inline qreal cimGraphicAlarmPenWidth(CimdrawWiringRunState runState,
                                     bool alarmPulse,
                                     qreal normalWidth = 2.0,
                                     qreal alarmWidth = 3.0)
{
    return cimGraphicAlarmActive(runState, alarmPulse) ? alarmWidth : normalWidth;
}

inline qreal cimGraphicRenderStateAlarmPenWidth(const CimGraphicRenderState& state,
                                                qreal normalWidth = 2.0,
                                                qreal alarmWidth = 3.0)
{
    return cimGraphicAlarmPenWidth(state.runState, state.alarmPulse, normalWidth, alarmWidth);
}

inline bool cimGraphicSwitchClosedFromPosition(int switchPosition)
{
    return switchPosition != 0;
}

inline bool cimGraphicRenderStateSwitchClosed(const CimGraphicRenderState& state,
                                              bool deenergizedOpensSwitch = false)
{
    return state.switchClosed
        && (!deenergizedOpensSwitch || state.runState != CimdrawWiringRunState::Deenergized);
}

inline bool cimGraphicRenderStateSwitchOpen(const CimGraphicRenderState& state,
                                            bool deenergizedOpensSwitch = false)
{
    return !cimGraphicRenderStateSwitchClosed(state, deenergizedOpensSwitch);
}

inline CimGraphicRenderState cimGraphicRenderStateFromSource(const CimGraphicRenderStateSource& source,
                                                            bool alarmPulse = false)
{
    CimGraphicRenderState state;
    if (!source.valid)
        return state;
    state.valid = true;
    state.runState = cimGraphicRunStateFromSource(source);
    state.switchPosition = cimGraphicSwitchPositionFromSource(source);
    state.flowSign = cimGraphicFlowSignFromSource(source);
    state.flowLeftToRight = cimGraphicFlowLeftToRightFromSign(state.flowSign);
    state.alarmPulse = alarmPulse;
    state.alarmActive = cimGraphicAlarmActive(state.runState, state.alarmPulse);
    state.switchClosed = cimGraphicSwitchClosedFromPosition(state.switchPosition);
    return state;
}

#endif
