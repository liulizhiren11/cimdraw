#include "PowerTopologyService.h"

#include <algorithm>
#include <functional>
#include <QHash>
#include <QQueue>
#include <QSet>

namespace {

QStringList subtractDevices(const QStringList& lhs, const QStringList& rhs)
{
    QStringList diff;
    QSet<QString> rhsSet(rhs.cbegin(), rhs.cend());
    for (const QString& id : lhs)
    {
        if (!rhsSet.contains(id))
            diff.append(id);
    }
    return diff;
}

QStringList sortedStringList(const QSet<QString>& values)
{
    QStringList out(values.cbegin(), values.cend());
    std::sort(out.begin(), out.end());
    return out;
}

struct TerminalEdge
{
    QString nextTerminalId;
    QString deviceId;
    QString conductorId;
};

struct TerminalGraph
{
    QHash<QString, QVector<TerminalEdge>> adjacency;
    QHash<QString, QString> terminalToDeviceId;
};

struct DeviceGraph
{
    QHash<QString, QSet<QString>> adjacency;
    QHash<QString, QHash<QString, QStringList>> conductorsByNeighbor;
};

void appendTerminalEdge(TerminalGraph& graph,
                        const QString& fromTerminalId,
                        const QString& toTerminalId,
                        const QString& deviceId,
                        const QString& conductorId)
{
    graph.adjacency[fromTerminalId].push_back({ toTerminalId, deviceId, conductorId });
}

TerminalGraph buildTerminalGraph(const PowerTopologyAnalysisSnapshot& snapshot, bool directed)
{
    TerminalGraph graph;

    for (const auto& device : snapshot.devices)
    {
        for (const QString& terminalId : device.terminalIds)
            graph.terminalToDeviceId.insert(terminalId, device.deviceId);

        for (auto it = device.internalConnectivity.cbegin(); it != device.internalConnectivity.cend(); ++it)
        {
            if (!it.value())
                continue;
            const QStringList parts = it.key().split(QLatin1Char('|'));
            if (parts.size() != 2)
                continue;
            const QString& a = parts.at(0);
            const QString& b = parts.at(1);
            if (directed && device.supportsDirectedFlow && device.terminalIds.size() >= 2)
            {
                const QString forwardFrom = device.flowSign >= 0
                    ? device.terminalIds.first()
                    : device.terminalIds.last();
                const QString forwardTo = device.flowSign >= 0
                    ? device.terminalIds.last()
                    : device.terminalIds.first();
                if ((a == forwardFrom && b == forwardTo) || (a == forwardTo && b == forwardFrom))
                {
                    appendTerminalEdge(graph, forwardFrom, forwardTo, device.deviceId, QString());
                    continue;
                }
            }
            appendTerminalEdge(graph, a, b, device.deviceId, QString());
            appendTerminalEdge(graph, b, a, device.deviceId, QString());
        }
    }

    for (const auto& conductor : snapshot.conductors)
    {
        appendTerminalEdge(graph, conductor.fromTerminalId, conductor.toTerminalId, conductor.toDeviceId, conductor.wireId);
        appendTerminalEdge(graph, conductor.toTerminalId, conductor.fromTerminalId, conductor.fromDeviceId, conductor.wireId);
    }

    return graph;
}

DeviceGraph buildDeviceGraph(const PowerTopologyAnalysisSnapshot& snapshot, bool directed)
{
    DeviceGraph graph;
    const TerminalGraph terminalGraph = buildTerminalGraph(snapshot, directed);
    for (auto it = terminalGraph.adjacency.cbegin(); it != terminalGraph.adjacency.cend(); ++it)
    {
        const QString fromDeviceId = terminalGraph.terminalToDeviceId.value(it.key());
        if (fromDeviceId.isEmpty())
            continue;
        for (const TerminalEdge& edge : it.value())
        {
            const QString toDeviceId = terminalGraph.terminalToDeviceId.value(edge.nextTerminalId);
            if (toDeviceId.isEmpty() || toDeviceId == fromDeviceId)
                continue;
            graph.adjacency[fromDeviceId].insert(toDeviceId);
            if (!edge.conductorId.isEmpty()
                && !graph.conductorsByNeighbor[fromDeviceId][toDeviceId].contains(edge.conductorId))
            {
                graph.conductorsByNeighbor[fromDeviceId][toDeviceId].append(edge.conductorId);
            }
        }
    }
    return graph;
}

QString canonicalCycleKey(const QStringList& cycle)
{
    if (cycle.isEmpty())
        return QString();

    QStringList normalized = cycle;
    if (normalized.size() > 1 && normalized.first() == normalized.last())
        normalized.removeLast();
    if (normalized.isEmpty())
        return QString();

    auto makeBestRotation = [](const QStringList& values) {
        QString best;
        for (int i = 0; i < values.size(); ++i)
        {
            QStringList rotated;
            for (int j = 0; j < values.size(); ++j)
                rotated.append(values.at((i + j) % values.size()));
            const QString joined = rotated.join(QStringLiteral("->"));
            if (best.isEmpty() || joined < best)
                best = joined;
        }
        return best;
    };

    QStringList reversed = normalized;
    std::reverse(reversed.begin(), reversed.end());
    const QString forward = makeBestRotation(normalized);
    const QString backward = makeBestRotation(reversed);
    return forward < backward ? forward : backward;
}

PowerTopologyPath buildDevicePath(const QString& sourceDeviceId,
                                  const QString& targetDeviceId,
                                  const QStringList& terminalPath,
                                  const QHash<QString, QString>& terminalToDeviceId,
                                  const QHash<QString, QString>& prevConductor)
{
    PowerTopologyPath path;
    path.sourceDeviceId = sourceDeviceId;
    path.targetDeviceId = targetDeviceId;
    if (terminalPath.isEmpty())
        return path;

    QSet<QString> seenDevices;
    for (const QString& terminalId : terminalPath)
    {
        const QString deviceId = terminalToDeviceId.value(terminalId);
        if (!deviceId.isEmpty() && !seenDevices.contains(deviceId))
        {
            seenDevices.insert(deviceId);
            path.deviceIds.append(deviceId);
        }
    }

    for (int i = 1; i < terminalPath.size(); ++i)
    {
        const QString conductorId = prevConductor.value(terminalPath.at(i));
        if (!conductorId.isEmpty())
            path.conductorIds.append(conductorId);
    }

    path.found = !path.deviceIds.isEmpty();
    return path;
}

PowerTopologyGroundPath buildGroundPath(const QString& sourceDeviceId,
                                        const QString& groundDeviceId,
                                        const QStringList& terminalPath,
                                        const QHash<QString, QString>& terminalToDeviceId,
                                        const QHash<QString, QString>& prevConductor)
{
    PowerTopologyGroundPath path;
    path.sourceDeviceId = sourceDeviceId;
    path.groundDeviceId = groundDeviceId;
    if (terminalPath.isEmpty())
        return path;

    QSet<QString> seenDevices;
    for (const QString& terminalId : terminalPath)
    {
        const QString deviceId = terminalToDeviceId.value(terminalId);
        if (!deviceId.isEmpty() && !seenDevices.contains(deviceId))
        {
            seenDevices.insert(deviceId);
            path.deviceIds.append(deviceId);
        }
    }

    for (int i = 1; i < terminalPath.size(); ++i)
    {
        const QString conductorId = prevConductor.value(terminalPath.at(i));
        if (!conductorId.isEmpty())
            path.conductorIds.append(conductorId);
    }

    path.found = !path.deviceIds.isEmpty();
    return path;
}

} // namespace

QVector<int> PowerTopologyService::deviceNodeIds(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId)
{
    return snapshot.deviceToNodeIds.value(deviceId);
}

QStringList PowerTopologyService::deviceConductorIds(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId)
{
    return snapshot.deviceToConductorIds.value(deviceId);
}

QStringList PowerTopologyService::reachableDevices(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId)
{
    QStringList reachable;
    QSet<QString> seen;
    const QVector<int> nodeIds = snapshot.deviceToNodeIds.value(deviceId);
    for (int nodeId : nodeIds)
    {
        for (const QString& candidate : snapshot.nodeIdToDeviceIds.value(nodeId))
        {
            if (seen.contains(candidate))
                continue;
            seen.insert(candidate);
            reachable.append(candidate);
        }
    }
    return reachable;
}

QStringList PowerTopologyService::directedReachableDevices(const PowerTopologyAnalysisSnapshot& snapshot,
                                                           const QString& sourceDeviceId)
{
    QStringList reachable;
    if (sourceDeviceId.isEmpty() || !snapshot.deviceIndex.contains(sourceDeviceId))
        return reachable;

    const int sourceIdx = snapshot.deviceIndex.value(sourceDeviceId, -1);
    if (sourceIdx < 0 || sourceIdx >= snapshot.devices.size())
        return reachable;

    const auto graph = buildTerminalGraph(snapshot, true);
    const auto& sourceDevice = snapshot.devices.at(sourceIdx);
    QQueue<QString> queue;
    QSet<QString> visitedTerminals;
    QSet<QString> seenDevices;

    for (const QString& terminalId : sourceDevice.terminalIds)
    {
        queue.enqueue(terminalId);
        visitedTerminals.insert(terminalId);
    }

    while (!queue.isEmpty())
    {
        const QString cur = queue.dequeue();
        const QString curDeviceId = graph.terminalToDeviceId.value(cur);
        if (!curDeviceId.isEmpty() && !seenDevices.contains(curDeviceId))
        {
            seenDevices.insert(curDeviceId);
            reachable.append(curDeviceId);
        }

        for (const auto& edge : graph.adjacency.value(cur))
        {
            if (visitedTerminals.contains(edge.nextTerminalId))
                continue;
            visitedTerminals.insert(edge.nextTerminalId);
            queue.enqueue(edge.nextTerminalId);
        }
    }

    return reachable;
}

QStringList PowerTopologyService::connectedComponentDeviceIds(const PowerTopologyAnalysisSnapshot& snapshot,
                                                              const QString& deviceId)
{
    return reachableDevices(snapshot, deviceId);
}

QStringList PowerTopologyService::busbarAttachedDevices(const PowerTopologyAnalysisSnapshot& snapshot, const QString& deviceId)
{
    QStringList attached;
    const int idx = snapshot.deviceIndex.value(deviceId, -1);
    if (idx < 0 || idx >= snapshot.devices.size())
        return attached;
    if (snapshot.devices.at(idx).role != PowerTopologyRole::Busbar)
        return attached;
    for (const QString& candidate : reachableDevices(snapshot, deviceId))
    {
        if (candidate != deviceId)
            attached.append(candidate);
    }
    return attached;
}

PowerTopologyIslandAnalysis PowerTopologyService::islandAnalysis(const PowerTopologyAnalysisSnapshot& snapshot)
{
    PowerTopologyIslandAnalysis analysis;
    for (const auto& node : snapshot.nodes)
    {
        if (node.deviceIds.isEmpty())
            continue;

        PowerTopologyIsland island;
        island.nodeIds.push_back(node.nodeId);
        island.deviceIds = node.deviceIds;

        QSet<QString> conductorSet;
        for (const QString& deviceId : node.deviceIds)
        {
            const QStringList conductorIds = snapshot.deviceToConductorIds.value(deviceId);
            for (const QString& conductorId : conductorIds)
            {
                const int conductorIdx = snapshot.conductorIndex.value(conductorId, -1);
                if (conductorIdx < 0 || conductorIdx >= snapshot.conductors.size())
                    continue;
                const auto& conductor = snapshot.conductors.at(conductorIdx);
                if (node.deviceIds.contains(conductor.fromDeviceId)
                    && node.deviceIds.contains(conductor.toDeviceId))
                {
                    conductorSet.insert(conductorId);
                }
            }
        }
        island.conductorIds = sortedStringList(conductorSet);
        analysis.islands.push_back(island);

        if (node.deviceIds.size() == 1)
            analysis.isolatedDeviceIds.append(node.deviceIds.first());
    }

    std::sort(analysis.isolatedDeviceIds.begin(), analysis.isolatedDeviceIds.end());
    return analysis;
}

PowerTopologyBranchAnalysis PowerTopologyService::branchAnalysis(const PowerTopologyAnalysisSnapshot& snapshot,
                                                                 const QString& sourceDeviceId)
{
    PowerTopologyBranchAnalysis analysis;
    analysis.sourceDeviceId = sourceDeviceId;
    if (sourceDeviceId.isEmpty() || !snapshot.deviceIndex.contains(sourceDeviceId))
        return analysis;

    const DeviceGraph graph = buildDeviceGraph(snapshot, true);
    const QStringList reachable = directedReachableDevices(snapshot, sourceDeviceId);
    QSet<QString> reachableSet;
    for (const QString& deviceId : reachable)
        reachableSet.insert(deviceId);

    QStringList leaves;
    for (const QString& deviceId : reachable)
    {
        if (deviceId == sourceDeviceId)
            continue;
        int reachableNeighborCount = 0;
        const QSet<QString> neighbors = graph.adjacency.value(deviceId);
        for (auto it = neighbors.cbegin(); it != neighbors.cend(); ++it)
        {
            if (reachableSet.contains(*it))
                ++reachableNeighborCount;
        }
        if (reachableNeighborCount <= 1)
            leaves.append(deviceId);
    }

    std::sort(leaves.begin(), leaves.end());
    for (const QString& leafDeviceId : leaves)
    {
        const PowerTopologyPath path = shortestDirectedSupplyPath(snapshot, sourceDeviceId, leafDeviceId);
        if (!path.found)
            continue;
        PowerTopologyBranch branch;
        branch.sourceDeviceId = sourceDeviceId;
        branch.leafDeviceId = leafDeviceId;
        branch.deviceIds = path.deviceIds;
        branch.conductorIds = path.conductorIds;
        analysis.branches.push_back(branch);
    }

    return analysis;
}

PowerTopologyLoopAnalysis PowerTopologyService::loopAnalysis(const PowerTopologyAnalysisSnapshot& snapshot)
{
    PowerTopologyLoopAnalysis analysis;
    const DeviceGraph graph = buildDeviceGraph(snapshot, false);
    QSet<QString> seenCycles;

    std::function<void(const QString&, const QString&, QStringList&)> dfs =
        [&](const QString& start, const QString& current, QStringList& path) {
            const QStringList neighbors = sortedStringList(graph.adjacency.value(current));
            for (const QString& next : neighbors)
            {
                if (next == start && path.size() >= 3)
                {
                    QStringList cycle = path;
                    cycle.append(start);
                    const QString key = canonicalCycleKey(cycle);
                    if (seenCycles.contains(key))
                        continue;
                    seenCycles.insert(key);

                    PowerTopologyLoop loop;
                    loop.deviceIds = cycle;
                    for (int i = 0; i + 1 < cycle.size(); ++i)
                    {
                        const QString& a = cycle.at(i);
                        const QString& b = cycle.at(i + 1);
                        const QStringList conductors = graph.conductorsByNeighbor.value(a).value(b);
                        for (const QString& conductorId : conductors)
                        {
                            if (!loop.conductorIds.contains(conductorId))
                                loop.conductorIds.append(conductorId);
                        }
                    }
                    analysis.loops.push_back(loop);
                    continue;
                }

                if (path.contains(next) || next < start)
                    continue;
                path.append(next);
                dfs(start, next, path);
                path.removeLast();
            }
        };

    QSet<QString> deviceSet;
    for (auto it = graph.adjacency.cbegin(); it != graph.adjacency.cend(); ++it)
        deviceSet.insert(it.key());
    const QStringList devices = sortedStringList(deviceSet);
    for (const QString& start : devices)
    {
        QStringList path{ start };
        dfs(start, start, path);
    }

    return analysis;
}

PowerTopologyPath PowerTopologyService::shortestSupplyPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                           const QString& sourceDeviceId,
                                                           const QString& targetDeviceId)
{
    PowerTopologyPath empty;
    empty.sourceDeviceId = sourceDeviceId;
    empty.targetDeviceId = targetDeviceId;

    if (sourceDeviceId.isEmpty() || targetDeviceId.isEmpty())
        return empty;
    if (!snapshot.deviceIndex.contains(sourceDeviceId) || !snapshot.deviceIndex.contains(targetDeviceId))
        return empty;

    const int sourceIdx = snapshot.deviceIndex.value(sourceDeviceId, -1);
    const int targetIdx = snapshot.deviceIndex.value(targetDeviceId, -1);
    if (sourceIdx < 0 || targetIdx < 0)
        return empty;

    const auto graph = buildTerminalGraph(snapshot, false);
    const auto& sourceDevice = snapshot.devices.at(sourceIdx);
    const auto& targetDevice = snapshot.devices.at(targetIdx);
    QSet<QString> targetTerminalSet(targetDevice.terminalIds.cbegin(), targetDevice.terminalIds.cend());
    QQueue<QString> queue;
    QSet<QString> visited;
    QHash<QString, QString> prevTerminal;
    QHash<QString, QString> prevConductor;

    for (const QString& terminalId : sourceDevice.terminalIds)
    {
        queue.enqueue(terminalId);
        visited.insert(terminalId);
        if (targetTerminalSet.contains(terminalId))
        {
            return buildDevicePath(sourceDeviceId, targetDeviceId, QStringList{ terminalId },
                                   graph.terminalToDeviceId, prevConductor);
        }
    }

    while (!queue.isEmpty())
    {
        const QString cur = queue.dequeue();
        for (const auto& edge : graph.adjacency.value(cur))
        {
            if (visited.contains(edge.nextTerminalId))
                continue;
            visited.insert(edge.nextTerminalId);
            prevTerminal.insert(edge.nextTerminalId, cur);
            prevConductor.insert(edge.nextTerminalId, edge.conductorId);

            if (targetTerminalSet.contains(edge.nextTerminalId))
            {
                QStringList terminalPath;
                QString walk = edge.nextTerminalId;
                terminalPath.prepend(walk);
                while (prevTerminal.contains(walk))
                {
                    walk = prevTerminal.value(walk);
                    terminalPath.prepend(walk);
                }
                return buildDevicePath(sourceDeviceId, targetDeviceId, terminalPath,
                                       graph.terminalToDeviceId, prevConductor);
            }
            queue.enqueue(edge.nextTerminalId);
        }
    }

    return empty;
}

PowerTopologyPath PowerTopologyService::shortestDirectedSupplyPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                                   const QString& sourceDeviceId,
                                                                   const QString& targetDeviceId)
{
    PowerTopologyPath empty;
    empty.sourceDeviceId = sourceDeviceId;
    empty.targetDeviceId = targetDeviceId;

    if (sourceDeviceId.isEmpty() || targetDeviceId.isEmpty())
        return empty;
    if (!snapshot.deviceIndex.contains(sourceDeviceId) || !snapshot.deviceIndex.contains(targetDeviceId))
        return empty;

    const int sourceIdx = snapshot.deviceIndex.value(sourceDeviceId, -1);
    const int targetIdx = snapshot.deviceIndex.value(targetDeviceId, -1);
    if (sourceIdx < 0 || targetIdx < 0)
        return empty;

    const auto graph = buildTerminalGraph(snapshot, true);
    const auto& sourceDevice = snapshot.devices.at(sourceIdx);
    const auto& targetDevice = snapshot.devices.at(targetIdx);
    QSet<QString> targetTerminalSet(targetDevice.terminalIds.cbegin(), targetDevice.terminalIds.cend());
    QQueue<QString> queue;
    QSet<QString> visited;
    QHash<QString, QString> prevTerminal;
    QHash<QString, QString> prevConductor;

    for (const QString& terminalId : sourceDevice.terminalIds)
    {
        queue.enqueue(terminalId);
        visited.insert(terminalId);
        if (targetTerminalSet.contains(terminalId))
        {
            return buildDevicePath(sourceDeviceId, targetDeviceId, QStringList{ terminalId },
                                   graph.terminalToDeviceId, prevConductor);
        }
    }

    while (!queue.isEmpty())
    {
        const QString cur = queue.dequeue();
        for (const auto& edge : graph.adjacency.value(cur))
        {
            if (visited.contains(edge.nextTerminalId))
                continue;
            visited.insert(edge.nextTerminalId);
            prevTerminal.insert(edge.nextTerminalId, cur);
            prevConductor.insert(edge.nextTerminalId, edge.conductorId);

            if (targetTerminalSet.contains(edge.nextTerminalId))
            {
                QStringList terminalPath;
                QString walk = edge.nextTerminalId;
                terminalPath.prepend(walk);
                while (prevTerminal.contains(walk))
                {
                    walk = prevTerminal.value(walk);
                    terminalPath.prepend(walk);
                }
                return buildDevicePath(sourceDeviceId, targetDeviceId, terminalPath,
                                       graph.terminalToDeviceId, prevConductor);
            }
            queue.enqueue(edge.nextTerminalId);
        }
    }

    return empty;
}

PowerTopologyGroundPath PowerTopologyService::shortestGroundPath(const PowerTopologyAnalysisSnapshot& snapshot,
                                                                 const QString& sourceDeviceId)
{
    PowerTopologyGroundPath empty;
    empty.sourceDeviceId = sourceDeviceId;

    if (sourceDeviceId.isEmpty() || !snapshot.deviceIndex.contains(sourceDeviceId))
        return empty;

    const int sourceIdx = snapshot.deviceIndex.value(sourceDeviceId, -1);
    if (sourceIdx < 0 || sourceIdx >= snapshot.devices.size())
        return empty;

    QStringList groundTerminalIds;
    QString groundDeviceId;
    for (const auto& device : snapshot.devices)
    {
        if (device.role != PowerTopologyRole::Ground)
            continue;
        if (groundDeviceId.isEmpty())
            groundDeviceId = device.deviceId;
        for (const QString& terminalId : device.terminalIds)
            groundTerminalIds.append(terminalId);
    }
    if (groundTerminalIds.isEmpty())
        return empty;

    const auto graph = buildTerminalGraph(snapshot, false);
    QSet<QString> groundTerminalSet(groundTerminalIds.cbegin(), groundTerminalIds.cend());
    const auto& sourceDevice = snapshot.devices.at(sourceIdx);
    QQueue<QString> queue;
    QSet<QString> visited;
    QHash<QString, QString> prevTerminal;
    QHash<QString, QString> prevConductor;

    for (const QString& terminalId : sourceDevice.terminalIds)
    {
        queue.enqueue(terminalId);
        visited.insert(terminalId);
        if (groundTerminalSet.contains(terminalId))
        {
            return buildGroundPath(sourceDeviceId, graph.terminalToDeviceId.value(terminalId),
                                   QStringList{ terminalId }, graph.terminalToDeviceId, prevConductor);
        }
    }

    while (!queue.isEmpty())
    {
        const QString cur = queue.dequeue();
        for (const auto& edge : graph.adjacency.value(cur))
        {
            if (visited.contains(edge.nextTerminalId))
                continue;
            visited.insert(edge.nextTerminalId);
            prevTerminal.insert(edge.nextTerminalId, cur);
            prevConductor.insert(edge.nextTerminalId, edge.conductorId);

            if (groundTerminalSet.contains(edge.nextTerminalId))
            {
                QStringList terminalPath;
                QString walk = edge.nextTerminalId;
                terminalPath.prepend(walk);
                while (prevTerminal.contains(walk))
                {
                    walk = prevTerminal.value(walk);
                    terminalPath.prepend(walk);
                }
                return buildGroundPath(sourceDeviceId, graph.terminalToDeviceId.value(edge.nextTerminalId),
                                       terminalPath, graph.terminalToDeviceId, prevConductor);
            }
            queue.enqueue(edge.nextTerminalId);
        }
    }

    empty.groundDeviceId = groundDeviceId;
    return empty;
}

PowerTopologyOperationPreview PowerTopologyService::previewSwitchOperation(
    const PowerTopologyAnalysisSnapshot& beforeSnapshot,
    const PowerTopologyAnalysisSnapshot& afterSnapshot,
    const QString& deviceId,
    int fromSwitchPosition,
    int toSwitchPosition,
    const QString& supplySourceDeviceId,
    const QString& supplyTargetDeviceId,
    const QString& groundSourceDeviceId)
{
    PowerTopologyOperationPreview preview;
    preview.switchChange = compareSwitchConnectivity(beforeSnapshot, afterSnapshot, deviceId,
                                                     fromSwitchPosition, toSwitchPosition);
    preview.beforeIslands = islandAnalysis(beforeSnapshot);
    preview.afterIslands = islandAnalysis(afterSnapshot);
    preview.beforeLoops = loopAnalysis(beforeSnapshot);
    preview.afterLoops = loopAnalysis(afterSnapshot);

    if (!supplySourceDeviceId.isEmpty() && !supplyTargetDeviceId.isEmpty())
    {
        preview.beforeSupplyPath = shortestSupplyPath(beforeSnapshot, supplySourceDeviceId, supplyTargetDeviceId);
        preview.afterSupplyPath = shortestSupplyPath(afterSnapshot, supplySourceDeviceId, supplyTargetDeviceId);
    }

    if (!groundSourceDeviceId.isEmpty())
    {
        preview.beforeGroundPath = shortestGroundPath(beforeSnapshot, groundSourceDeviceId);
        preview.afterGroundPath = shortestGroundPath(afterSnapshot, groundSourceDeviceId);
    }

    if (!supplySourceDeviceId.isEmpty())
    {
        preview.beforeBranches = branchAnalysis(beforeSnapshot, supplySourceDeviceId);
        preview.afterBranches = branchAnalysis(afterSnapshot, supplySourceDeviceId);
        preview.protectionRange = previewProtectionRange(beforeSnapshot,
                                                         afterSnapshot,
                                                         deviceId,
                                                         fromSwitchPosition,
                                                         toSwitchPosition,
                                                         supplySourceDeviceId);
    }

    return preview;
}

PowerTopologyProtectionRangePreview PowerTopologyService::previewProtectionRange(
    const PowerTopologyAnalysisSnapshot& beforeSnapshot,
    const PowerTopologyAnalysisSnapshot& afterSnapshot,
    const QString& protectiveDeviceId,
    int fromSwitchPosition,
    int toSwitchPosition,
    const QString& sourceDeviceId)
{
    PowerTopologyProtectionRangePreview preview;
    preview.protectiveDeviceId = protectiveDeviceId;
    preview.sourceDeviceId = sourceDeviceId;
    preview.fromSwitchPosition = fromSwitchPosition;
    preview.toSwitchPosition = toSwitchPosition;
    preview.beforeDeviceIds = directedReachableDevices(beforeSnapshot, sourceDeviceId);
    preview.afterDeviceIds = directedReachableDevices(afterSnapshot, sourceDeviceId);
    preview.removedDeviceIds = subtractDevices(preview.beforeDeviceIds, preview.afterDeviceIds);
    preview.addedDeviceIds = subtractDevices(preview.afterDeviceIds, preview.beforeDeviceIds);
    preview.topologyChanged = preview.beforeDeviceIds != preview.afterDeviceIds;
    return preview;
}

PowerTopologySwitchChangePreview PowerTopologyService::compareSwitchConnectivity(
    const PowerTopologyAnalysisSnapshot& beforeSnapshot,
    const PowerTopologyAnalysisSnapshot& afterSnapshot,
    const QString& deviceId,
    int fromSwitchPosition,
    int toSwitchPosition)
{
    PowerTopologySwitchChangePreview preview;
    preview.deviceId = deviceId;
    preview.fromSwitchPosition = fromSwitchPosition;
    preview.toSwitchPosition = toSwitchPosition;
    preview.beforeNodeIds = deviceNodeIds(beforeSnapshot, deviceId);
    preview.afterNodeIds = deviceNodeIds(afterSnapshot, deviceId);
    preview.beforeConnectedDevices = connectedComponentDeviceIds(beforeSnapshot, deviceId);
    preview.afterConnectedDevices = connectedComponentDeviceIds(afterSnapshot, deviceId);
    preview.addedDevices = subtractDevices(preview.afterConnectedDevices, preview.beforeConnectedDevices);
    preview.removedDevices = subtractDevices(preview.beforeConnectedDevices, preview.afterConnectedDevices);
    preview.topologyChanged = preview.beforeNodeIds != preview.afterNodeIds
        || !preview.addedDevices.isEmpty()
        || !preview.removedDevices.isEmpty();
    return preview;
}
