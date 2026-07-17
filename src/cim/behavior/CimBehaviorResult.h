#ifndef CIMBEHAVIORRESULT_H
#define CIMBEHAVIORRESULT_H

#include <QString>

enum class CimBehaviorTerminalConnectivity : int
{
    Unknown = 0,
    Disconnected = 1,
    Partial = 2,
    Connected = 3,
};

enum class CimBehaviorRunState : int
{
    Unknown = 0,
    Normal = 1,
    Energized = 2,
    Deenergized = 3,
    Alarm = 4,
};

struct CimBehaviorResult
{
    QString objectId;
    bool valid = false;
    CimBehaviorRunState runState = CimBehaviorRunState::Unknown;
    bool available = false;
    bool conductivity = false;
    bool canOperate = false;
    bool energized = false;
    CimBehaviorTerminalConnectivity terminalConnectivity = CimBehaviorTerminalConnectivity::Unknown;
};

#endif
