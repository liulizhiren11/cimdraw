#include "PowerTopologyBuilder.h"

#include "LzScene.h"
#include "Item/LzConnectLine.h"
#include "Item/LzConnectPoint.h"
#include "Item/LzHandle.h"
#include "cim/ui/CimSceneBuilder.h"
#include "item/TmpBase.h"
#include "topology/ILzTopologyParticipant.h"
#include "wiring/base/LzWiringItemBase.h"

#include <QGraphicsItem>
#include <QHash>
#include <QSet>
#include <QRegularExpression>

namespace {

class StringUnionFind
{
public:
    void add(const QString& id)
    {
        if (!parent_.contains(id))
            parent_.insert(id, id);
    }

    QString find(const QString& id)
    {
        const auto it = parent_.find(id);
        if (it == parent_.end())
            return QString();
        QString cur = it.value();
        while (cur != parent_.value(cur))
            cur = parent_.value(cur);
        QString node = id;
        while (node != cur)
        {
            const QString next = parent_.value(node);
            parent_[node] = cur;
            node = next;
        }
        return cur;
    }

    void unite(const QString& a, const QString& b)
    {
        add(a);
        add(b);
        const QString ra = find(a);
        const QString rb = find(b);
        if (!ra.isEmpty() && !rb.isEmpty() && ra != rb)
            parent_[rb] = ra;
    }

private:
    QHash<QString, QString> parent_;
};

QVector<LzConnectPoint*> connectPointsForItem(QGraphicsItem* item)
{
    QVector<LzConnectPoint*> ports;
    if (!item)
        return ports;
    for (QGraphicsItem* child : item->childItems())
    {
        if (auto* port = qgraphicsitem_cast<LzConnectPoint*>(child))
            ports.push_back(port);
    }
    return ports;
}

QString portKeyForIndex(const LzConnectPoint* port, int index)
{
    if (!port)
        return QStringLiteral("port:%1").arg(index);
    return QStringLiteral("dir:%1:%2").arg(static_cast<int>(port->getDirection())).arg(index);
}

QString fallbackPortTag(const LzConnectPoint* port, int index)
{
    const QString base = [port]() -> QString {
        if (!port)
            return QStringLiteral("port");
        switch (port->getDirection())
        {
        case LEFT_DIRECTION:
            return QStringLiteral("left");
        case RIGHT_DIRECTION:
            return QStringLiteral("right");
        case TOP_DIRECTION:
            return QStringLiteral("top");
        case BOTTOM_DIRECTION:
            return QStringLiteral("bottom");
        default:
            return QStringLiteral("port");
        }
    }();
    return QStringLiteral("%1%2").arg(base).arg(index + 1);
}

QString normalizePortToken(QString text, const QString& fallback)
{
    text = text.trimmed();
    if (text.isEmpty())
        text = fallback;
    text.replace(QRegularExpression(QStringLiteral("[^A-Za-z0-9_]+")), QStringLiteral("_"));
    text.replace(QRegularExpression(QStringLiteral("_+")), QStringLiteral("_"));
    text.remove(QRegularExpression(QStringLiteral("^_+|_+$")));
    return text.isEmpty() ? fallback : text;
}

QString portTokenForTopology(const LzConnectPoint* port, int index)
{
    const QString fallback = fallbackPortTag(port, index);
    if (!port)
        return fallback;
    if (!port->portTag().trimmed().isEmpty())
        return normalizePortToken(port->portTag(), fallback);
    return normalizePortToken(portKeyForIndex(port, index), fallback);
}

QString portDisplayNameForTopology(const LzConnectPoint* port, int index)
{
    if (port && !port->portTag().trimmed().isEmpty())
        return port->portTag().trimmed();
    return fallbackPortTag(port, index);
}

TmpShape* asTopologyShape(QGraphicsItem* item)
{
    if (!item || item->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool()
        || lzIsHandle(item) || item->type() == LzConnectPoint::Type || item->type() == LzConnectLine::Type)
        return nullptr;
    auto* shape = qgraphicsitem_cast<TmpShape*>(item);
    const auto* participant = lzTopologyParticipantForItem(shape);
    if (!shape || !participant || !participant->participatesInTopology()
        || participant->topologyDomain() != LzTopologyDomain::PowerSystem)
        return nullptr;
    return shape;
}

PowerTopologyRole roleForWiringItem(const LzWiringItemBase* item)
{
    if (!item)
        return PowerTopologyRole::GenericEquipment;
    return item->powerTopologyRole();
}

QString displayNameForShape(TmpShape* shape)
{
    if (!shape)
        return QString();
    const QString name = shape->displayName().trimmed();
    if (!name.isEmpty())
        return name;
    return shape->className();
}

int effectiveDeviceSwitchPosition(const LzWiringItemBase* item,
                                  PowerTopologyRole role,
                                  const QString& overrideDeviceId = QString(),
                                  int overrideSwitchPosition = -1)
{
    if (!item)
        return 0;
    switch (role)
    {
    case PowerTopologyRole::Breaker:
    case PowerTopologyRole::Disconnector:
    case PowerTopologyRole::GroundSwitch:
    {
        const QString currentDeviceId = const_cast<LzWiringItemBase*>(item)->ensureTopologyNodeStableId();
        if (!overrideDeviceId.isEmpty() && currentDeviceId == overrideDeviceId && overrideSwitchPosition >= 0)
            return qBound(0, overrideSwitchPosition, 1);
        return item->switchPosition();
    }
    default:
        return 1;
    }
}

} // namespace

static PowerTopologyAnalysisSnapshot buildImpl(LzScene* scene,
                                               const QString& overrideDeviceId,
                                               int overrideSwitchPosition)
{
    PowerTopologyAnalysisSnapshot snapshot;
    if (!scene)
        return snapshot;

    QHash<const QGraphicsItem*, QString> itemToDeviceId;
    QHash<const LzConnectPoint*, QString> portToTerminalId;
    StringUnionFind uf;
    QStringList allTerminalIds;

    for (QGraphicsItem* item : scene->items(Qt::AscendingOrder))
    {
        TmpShape* shape = asTopologyShape(item);
        if (!shape)
            continue;

        const QVector<LzConnectPoint*> ports = connectPointsForItem(shape);
        if (ports.isEmpty())
            continue;

        PowerTopologyDeviceRecord record;
        record.deviceId = shape->ensureTopologyNodeStableId();
        record.className = shape->className();
        record.displayName = displayNameForShape(shape);

        auto* wiringItem = dynamic_cast<LzWiringItemBase*>(shape);
        const PowerTopologyRole role = roleForWiringItem(wiringItem);
        record.role = role;
        if (wiringItem)
        {
            record.flowSign = wiringItem->flowSign();
            record.supportsDirectedFlow = wiringItem->supportsFlowDirectionAction();
        }
        const int effectiveSwitchPosition = effectiveDeviceSwitchPosition(
            wiringItem, role, overrideDeviceId, overrideSwitchPosition);

        for (int i = 0; i < ports.size(); ++i)
        {
            LzConnectPoint* port = ports.at(i);
            if (!port)
                continue;
            const QString portKey = portTokenForTopology(port, i);
            PowerTopologyTerminalRecord terminal;
            terminal.terminalId = record.deviceId + QStringLiteral(":") + portKey;
            terminal.name = portDisplayNameForTopology(port, i);
            terminal.localPos = port->connectionCenterLocal();
            terminal.directionKey = QStringLiteral("dir:%1").arg(static_cast<int>(port->getDirection()));
            record.terminals.push_back(terminal);
            record.terminalIds.push_back(terminal.terminalId);
            portToTerminalId.insert(port, terminal.terminalId);
            uf.add(terminal.terminalId);
            allTerminalIds.push_back(terminal.terminalId);
        }

        if (wiringItem)
        {
            const QHash<QString, bool> internalConnectivity =
                wiringItem->powerTopologyInternalConnectivity(record.terminalIds, effectiveSwitchPosition);
            for (auto it = internalConnectivity.constBegin(); it != internalConnectivity.constEnd(); ++it)
            {
                record.internalConnectivity.insert(it.key(), it.value());
                if (!it.value())
                    continue;
                const QStringList parts = it.key().split(QLatin1Char('|'));
                if (parts.size() != 2)
                    continue;
                uf.unite(parts.at(0), parts.at(1));
            }
        }

        snapshot.deviceIndex.insert(record.deviceId, snapshot.devices.size());
        snapshot.devices.push_back(record);
        itemToDeviceId.insert(shape, record.deviceId);
    }

    for (QGraphicsItem* item : scene->items(Qt::AscendingOrder))
    {
        auto* line = qgraphicsitem_cast<LzConnectLine*>(item);
        if (!line)
            continue;
        if (line->data(CimSceneBuilder::CimGeneratedVisualHelperDataKey).toBool())
            continue;

        auto* startShape = qgraphicsitem_cast<TmpShape*>(line->getStartItem());
        auto* endShape = qgraphicsitem_cast<TmpShape*>(line->getEndItem());
        const auto startDevIt = itemToDeviceId.constFind(startShape);
        const auto endDevIt = itemToDeviceId.constFind(endShape);
        if (startDevIt == itemToDeviceId.cend() || endDevIt == itemToDeviceId.cend())
            continue;

        LzConnectPoint* startPort = line->startConnectPort();
        LzConnectPoint* endPort = line->endConnectPort();
        if (!startPort || !endPort)
            continue;
        const auto startTermIt = portToTerminalId.constFind(startPort);
        const auto endTermIt = portToTerminalId.constFind(endPort);
        if (startTermIt == portToTerminalId.cend() || endTermIt == portToTerminalId.cend())
            continue;

        PowerTopologyConductorRecord conductor;
        conductor.wireId = line->ensureTopologyEdgeStableId();
        conductor.fromDeviceId = startDevIt.value();
        conductor.fromTerminalId = startTermIt.value();
        conductor.toDeviceId = endDevIt.value();
        conductor.toTerminalId = endTermIt.value();
        conductor.routePoints = line->pathInSceneCoords();
        snapshot.conductorIndex.insert(conductor.wireId, snapshot.conductors.size());
        snapshot.conductors.push_back(conductor);
        snapshot.deviceToConductorIds[conductor.fromDeviceId].append(conductor.wireId);
        snapshot.deviceToConductorIds[conductor.toDeviceId].append(conductor.wireId);
        uf.unite(conductor.fromTerminalId, conductor.toTerminalId);
    }

    QHash<QString, int> rootToNodeId;
    for (const QString& terminalId : allTerminalIds)
    {
        const QString root = uf.find(terminalId);
        if (root.isEmpty())
            continue;
        int nodeId = rootToNodeId.value(root, -1);
        if (nodeId < 0)
        {
            nodeId = snapshot.nodes.size();
            rootToNodeId.insert(root, nodeId);
            PowerTopologyNodeRecord node;
            node.nodeId = nodeId;
            snapshot.nodes.push_back(node);
        }
        snapshot.terminalToNodeIndex.insert(terminalId, nodeId);
        const int sep = terminalId.indexOf(QLatin1Char(':'));
        if (sep <= 0)
            continue;
        const QString deviceId = terminalId.left(sep);
        snapshot.nodes[nodeId].terminals.push_back({ deviceId, terminalId });
        if (!snapshot.nodes[nodeId].deviceIds.contains(deviceId))
            snapshot.nodes[nodeId].deviceIds.append(deviceId);
        if (!snapshot.deviceToNodeIds[deviceId].contains(nodeId))
            snapshot.deviceToNodeIds[deviceId].push_back(nodeId);
    }

    for (auto it = snapshot.nodes.cbegin(); it != snapshot.nodes.cend(); ++it)
        snapshot.nodeIdToDeviceIds.insert(it->nodeId, it->deviceIds);

    return snapshot;
}

PowerTopologyAnalysisSnapshot PowerTopologyBuilder::build(LzScene* scene)
{
    return buildImpl(scene, QString(), -1);
}

PowerTopologyAnalysisSnapshot PowerTopologyBuilder::buildWithSwitchOverride(LzScene* scene,
                                                                            const QString& deviceId,
                                                                            int switchPosition)
{
    return buildImpl(scene, deviceId, switchPosition);
}
