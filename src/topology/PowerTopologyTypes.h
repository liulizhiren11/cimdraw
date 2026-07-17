#ifndef POWERTOPOLOGYTYPES_H
#define POWERTOPOLOGYTYPES_H

#include "TopologyTypes.h"

#include <QHash>
#include <QPointF>
#include <QStringList>
#include <QVector>

enum class PowerTopologyRole : int
{
    Busbar = 0,
    Breaker,
    Disconnector,
    GroundSwitch,
    Ground,
    Transformer,
    Reactor,
    Compensation,
    SurgeArrester,
    Measurement,
    Feeder,
    Conductor,
    GenericEquipment,
};

struct PowerTopologyTerminalRecord
{
    QString terminalId;
    QString name;
    QPointF localPos;
    QString directionKey;
};

struct PowerTopologyDeviceRecord
{
    QString deviceId;
    QString className;
    QString displayName;
    PowerTopologyRole role = PowerTopologyRole::GenericEquipment;
    int flowSign = 1;
    bool supportsDirectedFlow = false;
    QVector<PowerTopologyTerminalRecord> terminals;
    QVector<QString> terminalIds;
    QHash<QString, bool> internalConnectivity;
};

inline bool powerTopologyDeviceSupportsDirectedTerminalFlow(const PowerTopologyDeviceRecord& device)
{
    return device.supportsDirectedFlow && device.terminalIds.size() >= 2;
}

inline bool powerTopologyDeviceFlowsForward(const PowerTopologyDeviceRecord& device)
{
    return device.flowSign >= 0;
}

inline QString powerTopologyDeviceForwardFromTerminalId(const PowerTopologyDeviceRecord& device)
{
    if (!powerTopologyDeviceSupportsDirectedTerminalFlow(device))
        return QString();
    return powerTopologyDeviceFlowsForward(device)
        ? device.terminalIds.first()
        : device.terminalIds.last();
}

inline QString powerTopologyDeviceForwardToTerminalId(const PowerTopologyDeviceRecord& device)
{
    if (!powerTopologyDeviceSupportsDirectedTerminalFlow(device))
        return QString();
    return powerTopologyDeviceFlowsForward(device)
        ? device.terminalIds.last()
        : device.terminalIds.first();
}

struct PowerTopologyConductorRecord
{
    QString wireId;
    QString fromDeviceId;
    QString fromTerminalId;
    QString toDeviceId;
    QString toTerminalId;
    QVector<QPointF> routePoints;
};

struct PowerTopologyNodeRecord
{
    int nodeId = -1;
    QVector<CimdrawTopologyTerminalRef> terminals;
    QStringList deviceIds;
};

struct PowerTopologyAnalysisSnapshot
{
    QVector<PowerTopologyDeviceRecord> devices;
    QVector<PowerTopologyConductorRecord> conductors;
    QVector<PowerTopologyNodeRecord> nodes;

    QHash<QString, int> deviceIndex;
    QHash<QString, int> conductorIndex;
    QHash<QString, int> terminalToNodeIndex;
    QHash<QString, QVector<int>> deviceToNodeIds;
    QHash<int, QStringList> nodeIdToDeviceIds;
    QHash<QString, QStringList> deviceToConductorIds;
};

struct PowerTopologyDocumentExport
{
    QVector<PowerTopologyDeviceRecord> devices;
    QVector<PowerTopologyConductorRecord> conductors;
    QVector<PowerTopologyNodeRecord> nodes;
    QPointF viewCenter;
    qreal viewScale = 1.0;
};

struct PowerTopologySwitchChangePreview
{
    QString deviceId;
    int fromSwitchPosition = -1;
    int toSwitchPosition = -1;
    QVector<int> beforeNodeIds;
    QVector<int> afterNodeIds;
    QStringList beforeConnectedDevices;
    QStringList afterConnectedDevices;
    QStringList addedDevices;
    QStringList removedDevices;
    bool topologyChanged = false;
};

struct PowerTopologyIsland
{
    QVector<int> nodeIds;
    QStringList deviceIds;
    QStringList conductorIds;
};

struct PowerTopologyIslandAnalysis
{
    QVector<PowerTopologyIsland> islands;
    QStringList isolatedDeviceIds;
};

struct PowerTopologyPath
{
    QString sourceDeviceId;
    QString targetDeviceId;
    QStringList deviceIds;
    QStringList conductorIds;
    bool found = false;
};

struct PowerTopologyGroundPath
{
    QString sourceDeviceId;
    QString groundDeviceId;
    QStringList deviceIds;
    QStringList conductorIds;
    bool found = false;
};

struct PowerTopologyBranch
{
    QString sourceDeviceId;
    QString leafDeviceId;
    QStringList deviceIds;
    QStringList conductorIds;
};

struct PowerTopologyBranchAnalysis
{
    QString sourceDeviceId;
    QVector<PowerTopologyBranch> branches;
};

struct PowerTopologyLoop
{
    QStringList deviceIds;
    QStringList conductorIds;
};

struct PowerTopologyLoopAnalysis
{
    QVector<PowerTopologyLoop> loops;
};

struct PowerTopologyProtectionRangePreview
{
    QString protectiveDeviceId;
    QString sourceDeviceId;
    int fromSwitchPosition = -1;
    int toSwitchPosition = -1;
    QStringList beforeDeviceIds;
    QStringList afterDeviceIds;
    QStringList removedDeviceIds;
    QStringList addedDeviceIds;
    bool topologyChanged = false;
};

struct PowerTopologyOperationPreview
{
    PowerTopologySwitchChangePreview switchChange;
    PowerTopologyIslandAnalysis beforeIslands;
    PowerTopologyIslandAnalysis afterIslands;
    PowerTopologyBranchAnalysis beforeBranches;
    PowerTopologyBranchAnalysis afterBranches;
    PowerTopologyLoopAnalysis beforeLoops;
    PowerTopologyLoopAnalysis afterLoops;
    PowerTopologyPath beforeSupplyPath;
    PowerTopologyPath afterSupplyPath;
    PowerTopologyGroundPath beforeGroundPath;
    PowerTopologyGroundPath afterGroundPath;
    PowerTopologyProtectionRangePreview protectionRange;
};

#endif // POWERTOPOLOGYTYPES_H
