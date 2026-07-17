#ifndef CIMDRAWWIRINGVISUALSTATE_H
#define CIMDRAWWIRINGVISUALSTATE_H

#include "CimdrawWiringTypes.h"

struct CimdrawWiringVisualState
{
    CimdrawWiringRunState runState = CimdrawWiringRunState::Normal;
    int switchPosition = 1;
    int flowSign = 1;
    bool flowLeftToRight = true;
    bool alarmPulse = false;
    bool alarmActive = false;
    bool switchClosed = true;
    bool valid = false;
};

#endif
