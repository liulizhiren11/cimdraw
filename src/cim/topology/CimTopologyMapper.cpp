#include "CimTopologyMapper.h"

#include "cim/model/CimModel.h"
#include "cim/model/CimObject.h"
#include "cim/semantic/CimConnectivityNodeView.h"
#include "cim/semantic/CimEquipmentView.h"
#include "cim/semantic/CimSwitchView.h"
#include "cim/semantic/CimTerminalView.h"

#include <algorithm>
#include <QHash>

namespace {

struct TerminalBinding
{
    QString deviceId;
    QString terminalId;
    QString nodeMrid;
};

PowerTopologyRole roleForCimClass(const QString& className)
{
    if (className == QStringLiteral("BusbarSection"))
        return PowerTopologyRole::Busbar;
    if (className == QStringLiteral("Breaker"))
        return PowerTopologyRole::Breaker;
    if (className == QStringLiteral("Disconnector") || className == QStringLiteral("LoadBreakSwitch"))
        return PowerTopologyRole::Disconnector;
    if (className == QStringLiteral("GroundDisconnector"))
        return PowerTopologyRole::GroundSwitch;
    if (className == QStringLiteral("Ground"))
        return PowerTopologyRole::Ground;
    if (className == QStringLiteral("PowerTransformer"))
        return PowerTopologyRole::Transformer;
    if (className == QStringLiteral("SeriesCompensator"))
        return PowerTopologyRole::Reactor;
    if (className == QStringLiteral("LinearShuntCompensator")
        || className == QStringLiteral("ShuntCompensator")
        || className == QStringLiteral("NonlinearShuntCompensator")
        || className == QStringLiteral("StaticVarCompensator")
        || className == QStringLiteral("PetersenCoil"))
    {
        return PowerTopologyRole::Compensation;
    }
    if (className == QStringLiteral("SurgeArrester"))
        return PowerTopologyRole::SurgeArrester;
    if (className == QStringLiteral("CurrentTransformer")
        || className == QStringLiteral("PotentialTransformer")
        || className == QStringLiteral("VoltageTransformer"))
    {
        return PowerTopologyRole::Measurement;
    }
    if (className == QStringLiteral("ACLineSegment"))
        return PowerTopologyRole::Conductor;
    return PowerTopologyRole::GenericEquipment;
}

QString nodeMridForTerminal(const CimObject& terminal, const QHash<QString, const CimObject*>& objectByMrid)
{
    const CimTerminalView terminalView(&terminal);
    const QString topologicalNodeMrid = terminalView.topologicalNodeMrid().trimmed();
    if (!topologicalNodeMrid.isEmpty())
        return topologicalNodeMrid;

    const QString connectivityNodeMrid = terminalView.connectivityNodeMrid().trimmed();
    if (connectivityNodeMrid.isEmpty())
        return QString();

    if (const CimObject* connectivityNode = objectByMrid.value(connectivityNodeMrid, nullptr))
    {
        const CimConnectivityNodeView connectivityNodeView(connectivityNode);
        const QString mappedTopologicalNodeMrid = connectivityNodeView.topologicalNodeMrid().trimmed();
        if (!mappedTopologicalNodeMrid.isEmpty())
            return mappedTopologicalNodeMrid;
    }

    return connectivityNodeMrid;
}

QString fallbackTerminalName(int ordinal)
{
    return QStringLiteral("T%1").arg(ordinal);
}

void connectAllTerminalPairs(PowerTopologyDeviceRecord& record, bool connected)
{
    for (int i = 0; i < record.terminalIds.size(); ++i)
    {
        for (int j = i + 1; j < record.terminalIds.size(); ++j)
        {
            const QString key = record.terminalIds.at(i) + QStringLiteral("|") + record.terminalIds.at(j);
            record.internalConnectivity.insert(key, connected);
        }
    }
}

} // namespace

PowerTopologyAnalysisSnapshot CimTopologyMapper::build(const CimModel& model) const
{
    PowerTopologyAnalysisSnapshot snapshot;

    const QVector<CimObject> objects = model.allObjects();
    QHash<QString, const CimObject*> objectByMrid;
    objectByMrid.reserve(objects.size());
    for (const CimObject& object : objects)
        objectByMrid.insert(object.mrid, &object);

    QHash<QString, QVector<const CimObject*>> terminalsByEquipmentMrid;
    for (const CimObject& object : objects)
    {
        CimTerminalView terminalView(&object);
        if (!terminalView.isTerminal())
            continue;

        const QString equipmentMrid = terminalView.conductingEquipmentMrid().trimmed();
        if (!equipmentMrid.isEmpty())
            terminalsByEquipmentMrid[equipmentMrid].push_back(&object);
    }

    QStringList equipmentMrids = terminalsByEquipmentMrid.keys();
    std::sort(equipmentMrids.begin(), equipmentMrids.end());

    QVector<TerminalBinding> terminalBindings;
    for (const QString& equipmentMrid : equipmentMrids)
    {
        const CimObject* equipment = objectByMrid.value(equipmentMrid, nullptr);
        if (!equipment)
            continue;

        CimEquipmentView equipmentView(equipment);
        PowerTopologyDeviceRecord record;
        record.deviceId = equipment->mrid;
        record.className = equipment->className;
        record.displayName = equipmentView.name().trimmed().isEmpty() ? equipment->className : equipmentView.name();
        record.role = roleForCimClass(equipment->className);

        QVector<const CimObject*> terminals = terminalsByEquipmentMrid.value(equipmentMrid);
        std::sort(terminals.begin(), terminals.end(), [](const CimObject* lhs, const CimObject* rhs) {
            const CimTerminalView left(lhs);
            const CimTerminalView right(rhs);
            const int leftSequence = left.sequenceNumber();
            const int rightSequence = right.sequenceNumber();
            if (leftSequence != rightSequence)
                return leftSequence < rightSequence;
            return lhs->mrid < rhs->mrid;
        });

        int ordinal = 1;
        for (const CimObject* terminal : terminals)
        {
            if (!terminal)
                continue;

            const CimTerminalView terminalView(terminal);
            PowerTopologyTerminalRecord terminalRecord;
            terminalRecord.terminalId = terminal->mrid.trimmed().isEmpty()
                ? QStringLiteral("%1:terminal:%2").arg(record.deviceId).arg(ordinal)
                : terminal->mrid;
            terminalRecord.name = terminalView.name().trimmed().isEmpty()
                ? fallbackTerminalName(ordinal)
                : terminalView.name();
            if (terminalView.sequenceNumber() > 0)
                terminalRecord.directionKey = QStringLiteral("sequence:%1").arg(terminalView.sequenceNumber());

            record.terminals.push_back(terminalRecord);
            record.terminalIds.push_back(terminalRecord.terminalId);

            TerminalBinding binding;
            binding.deviceId = record.deviceId;
            binding.terminalId = terminalRecord.terminalId;
            binding.nodeMrid = nodeMridForTerminal(*terminal, objectByMrid);
            if (binding.nodeMrid.isEmpty())
                binding.nodeMrid = QStringLiteral("synthetic-node:%1").arg(binding.terminalId);
            terminalBindings.push_back(binding);
            ++ordinal;
        }

        bool internallyConnected = true;
        CimSwitchView switchView(equipment);
        if (switchView.isSwitch())
            internallyConnected = !switchView.isOpen();
        connectAllTerminalPairs(record, internallyConnected);

        snapshot.deviceIndex.insert(record.deviceId, snapshot.devices.size());
        snapshot.devices.push_back(record);
    }

    QHash<QString, int> nodeIdByMrid;
    for (const TerminalBinding& binding : terminalBindings)
    {
        int nodeId = nodeIdByMrid.value(binding.nodeMrid, -1);
        if (nodeId < 0)
        {
            nodeId = snapshot.nodes.size();
            nodeIdByMrid.insert(binding.nodeMrid, nodeId);
            PowerTopologyNodeRecord node;
            node.nodeId = nodeId;
            snapshot.nodes.push_back(node);
        }

        snapshot.terminalToNodeIndex.insert(binding.terminalId, nodeId);
        snapshot.nodes[nodeId].terminals.push_back({ binding.deviceId, binding.terminalId });
        if (!snapshot.nodes[nodeId].deviceIds.contains(binding.deviceId))
            snapshot.nodes[nodeId].deviceIds.push_back(binding.deviceId);
        if (!snapshot.deviceToNodeIds[binding.deviceId].contains(nodeId))
            snapshot.deviceToNodeIds[binding.deviceId].push_back(nodeId);
    }

    for (const PowerTopologyNodeRecord& node : snapshot.nodes)
        snapshot.nodeIdToDeviceIds.insert(node.nodeId, node.deviceIds);

    return snapshot;
}
